# Sensor Monitoring System

This repository contains the final project for the Operating Systems course in the Bachelor of Electronics/ICT program. The project involves creating a Sensor Monitoring System, which consists of client-side sensor nodes measuring room temperature and a central server-side sensor-gateway that acquires all sensor data from the sensor nodes.

## Overview

The sensor gateway is comprised of a main process and a log process. The main process runs three threads at startup: the connection manager, the data manager, and the storage manager thread. A shared data structure (`sbuffer`) is used for communication between all threads. This project aims to ensure thread-safe access to shared data and implement the required functionality for each component of the sensor gateway.

## Functionalities

1. **Main Process and Log Process**
   - The log process is started as a child process of the main process.
2. **Threads in Main Process**
   - The main process runs three threads: connection manager, data manager, and storage manager.
3. **Shared Data Structure**
   - Utilize the `sbuffer` module for communication between threads.
   - Ensure thread-safe read/write/update access to shared data.
4. **Connection Manager**
   - Listens on a TCP socket for incoming connection requests from sensor nodes.
   - Each client-side node has a dedicated thread for processing incoming data.
5. **Data Manager**
   - Implements server intelligence, reading sensor measurements from shared memory.
   - Reads room-sensor mapping from a text file.
   - Calculates running average temperature and makes decisions based on thresholds.
6. **Storage Manager**
   - Inserts sensor measurements into a CSV file (`data.csv`).
   - Creates a new `data.csv` file at startup.
7. **Log Process**
   - Receives log events from the main process using a pipe.
   - Writes log events to a log file (`gateway.log`).
   - Supports various log events from different components of the system.

## Setup

1. Clone the repository: `git clone <repository_url>`
2. Compile the project: `make`
3. Run the server: `./server <port_number>`

## Usage

- Modify `room_sensor.map` to update room-sensor mappings.
- Adjust timeout settings in the Makefile if necessary.
- View log events in `gateway.log` for monitoring system activities.
