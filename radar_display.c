#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "protocol.h"
#include <SDL2/SDL.h>

#define PORT 8080
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Sock not created");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Station is listening... Port: %d\n", PORT);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Avionics Radar Interface", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TelemetryPacket recv_packet;
    socklen_t len = sizeof(cliaddr);
    int running = 1;
    SDL_Event event;

    float current_x = -1.0f;
    float current_y = -1.0f;

    FILE *log_file = fopen("flight_data.csv", "a");
    if (log_file == NULL)
    {
        perror("File creation failed");
    }
    else
    {
        fprintf(log_file, "TARGET_X,TARGET_Y,CHECKSUM\n");
        fflush(log_file);
    }

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }

        int n = recvfrom(sockfd, &recv_packet, sizeof(TelemetryPacket), MSG_DONTWAIT, (struct sockaddr *)&cliaddr, &len);

        if (n == sizeof(TelemetryPacket))
        {
            if (recv_packet.header == 0xAABB)
            {
                uint8_t expected_checksum = calculate_checksum((uint8_t *)&recv_packet, sizeof(TelemetryPacket) - 1);

                if (expected_checksum == recv_packet.checksum)
                {
                    current_x = recv_packet.target_x;
                    current_y = recv_packet.target_y;

                    if (log_file != NULL)
                    {
                        fprintf(log_file, "%.2f,%.2f,0x%X\n", current_x, current_y, recv_packet.checksum);
                        fflush(log_file);
                    }
                }
                else
                {
                    printf("ERROR: DATA Compromised\n");
                }
            }
            else
            {
                printf("ERROR: WRONG HEADER!\n");
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(renderer, WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT / 2);

        if (current_x >= 0 && current_y >= 0)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect target_rect = {(int)current_x, (int)current_y, 6, 6};
            SDL_RenderFillRect(renderer, &target_rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (log_file != NULL)
    {
        fclose(log_file);
    }

    close(sockfd);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}