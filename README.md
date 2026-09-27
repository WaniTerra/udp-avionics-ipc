# Avionics Radar Interface (UDP IPC)

This project was developed for **educational purposes** to explore Inter-Process Communication (IPC) using UDP sockets, custom data protocols, data validation, and real-time graphical rendering using the SDL2 library.

The system consists of a simulated avionics sensor node that continuously generates target coordinates and transmits them over a network. A ground radar display application receives these packets, validates their integrity, logs the flight data, and renders the target on a graphical radar screen.

## 🚀 Features

*   **UDP Socket Communication:** Fast, non-blocking asynchronous data transmission between the sensor and radar processes via `127.0.0.1:8080`.
*   **Custom Packet Protocol & Validation:** Data is packed into a custom `TelemetryPacket` struct containing a specific header (`0xAABB`) and an XOR-based checksum.
*   **Noise Simulation & Error Handling:** The sensor node intentionally injects noise (corrupted headers or checksums) at a random probability. The radar successfully detects and discards these compromised packets, preventing graphical glitches.
*   **Real-Time Visualization (SDL2):** Utilizes the Simple DirectMedia Layer (SDL2) library to render a dynamic 2D radar interface, plotting the incoming target `(X, Y)` coordinates in real-time.
*   **Persistent Data Logging:** Validated telemetry frames are automatically appended to a `flight_data.csv` file for post-flight analysis.

## 📂 File Structure

The core components of the project are:

*   **`sensor_node.c`**: The UDP client that simulates flight telemetry, calculates checksums, randomly injects noise, and transmits the packets.
*   **`radar_display.c`**: The UDP server and graphical interface. It polls for packets, verifies the headers/checksums, logs data to a CSV, and updates the SDL2 canvas.
*   **`protocol.h`**: The shared header defining the `TelemetryPacket` structure (byte-aligned using `#pragma pack(1)`) and the checksum algorithm.
*   **`Makefile`**: The build automation script.

## 🛠️ Build and Run Instructions

### Prerequisites
To compile the graphical interface, you must have the SDL2 library installed on your Linux system.
*   **Debian/Ubuntu:** `sudo apt-get install libsdl2-dev`
*   **Fedora/RHEL:** `sudo dnf install SDL2-devel`

### 1. Compile the Project
Navigate to the project directory and execute:
```bash
make
```
*This command will compile and link both the `sensor` and `radar` binaries.*

### 2. Start the Sensor (Transmitter)
In your terminal, launch the sensor node:
```bash
./sensor
```
*The sensor will begin broadcasting telemetry and periodically indicate when it injects [NOISE] into the data stream.*

### 3. Launch the Radar Interface (Receiver)
Open a **new** terminal window (or split the current one) and start the graphical ground station:
```bash
./radar
```

## 📝 Simulation Scenario

When both programs are running, the sensor simulates an aircraft moving across a coordinate plane. It packages the `X` and `Y` floats along with the calculated checksum. 

The radar display opens an 800x600 window. As packets arrive, the radar verifies them. If the header is wrong or the checksum fails (due to the simulated noise), the radar prints an error to the console and ignores the data. Valid coordinates are plotted as a red dot on the green-lined radar grid and simultaneously logged into `flight_data.csv` for tracking.
