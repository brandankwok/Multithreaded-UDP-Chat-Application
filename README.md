# s-talk: Multi-threaded UDP Chat Application

Created for CMPT 300: Operating Systems Assignment 2 at SFU, `s-talk` is a Linux terminal-based chat application enabling real-time messaging between two users over a network using UDP sockets. Built in C with multi-threading, this project emphasizes concurrency, inter-thread communication, and socket programming. The program uses four primary threads for keyboard input, message sending, message receiving, and screen output each handling specific operations and communicating through message queues for efficient inter-thread coordination.

## Features

- **Concurrent, Multi-threaded Architecture**:
  - Simultaneous input/output using four threads:
    - `keyboard`: Captures user input and adds the message to `senderList`.
    - `sender`: Takes messages off `senderList` and sends the messages to the specified remote host via UDP.
    - `receiver`: Receives messages from the remote source and adds the messages to `receiverList`.
    - `screen`: Takes messages off `receiverList` and displays the received messages on the terminal.
  - Message Queues using two linked lists:
    - `senderList`: used for outgoing messages, shared between `keyboard` and `sender` threads
    - `receiverList`: used for for incoming messages, shared between `receiver` and `screen` threads
- **Synchronization**: Uses mutexes and condition variables for smooth, thread-safe data sharing.
- **UDP Sockets**: Facilitates low-latency message exchange between endpoints by binding to user-specified port numbers.

## Compilation and Execution

1. **Compile**:
   ```bash
   make all
2. **Run**:
   ```bash
   ./s-talk <your_port_num> <destination_host_name> <destination_port_num>

## Example Usage

To start a conversation between two users, run `s-talk` on both (Linux or Unix) systems with matching port information:

On User A's machine:
```bash
./s-talk 1234 userB_hostname 5678
```
On User B's machine:
```bash
./s-talk 5678 userA_hostname 1234
```
Each user can then type messages, which will be sent and displayed on each other’s screen until either user types "!" to exit.

## Authors
- [Brandan Kwok](https://github.com/brandankwok)
- [Sriyathavan Srisathanantham](https://github.com/Sriyathavan) 
