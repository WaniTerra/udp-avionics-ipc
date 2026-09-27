#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "protocol.h"

#define PORT 8080
#define IP "127.0.0.1"

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Sock not created");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(IP);

    TelemetryPacket packet;
    packet.target_x = 0.0f;
    packet.target_y = 0.0f;

    srand(time(NULL));

    printf("Sensor started...\n");

    while (1)
    {
        packet.header = 0xAABB;

        packet.target_x += 1.5f;
        packet.target_y += 0.8f;

        packet.checksum = calculate_checksum((uint8_t *)&packet, sizeof(TelemetryPacket) - 1);

        int noise_probability = rand() % 100;
        if (noise_probability < 10)
        {
            packet.checksum = rand() % 256;
            printf("[NOISE] Corrupted Checksum! -> X: %.2f, Y: %.2f\n", packet.target_x, packet.target_y);
        }
        else if (noise_probability >= 10 && noise_probability < 15)
        {
            packet.header = 0xDEAD;
            printf("[NOISE] Corrupted Header! -> X: %.2f, Y: %.2f\n", packet.target_x, packet.target_y);
        }
        else
        {
            printf("Sent -> X: %.2f, Y: %.2f, CHK: 0x%X\n", packet.target_x, packet.target_y, packet.checksum);
        }

        sendto(sockfd, &packet, sizeof(TelemetryPacket), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));

        usleep(100000);
    }

    close(sockfd);
    return 0;
}