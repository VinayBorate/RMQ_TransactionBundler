```markdown
# RMQ_TransactionBundler

## Project Description
The **RMQ_TransactionBundler** project is designed to handle the serialization and deserialization of transaction messages, ensuring data integrity and configurability. This project facilitates the bundling of deserialized messages and their subsequent transmission to RabbitMQ (RMQ) in a structured format. It includes configurable parameters for bundle size, timeout, and local dumping for debugging purposes.

## Technology and Frameworks Used
- **C++17**: The core programming language used for the project.
- **Boost Libraries**: Used for shared memory management, containers, and CRC computation.
- **RabbitMQ**: For message queuing and handling.
- **nlohmann/json**: For JSON serialization and deserialization.
- **spdlog**: For logging.
- **CMake**: For build configuration.

## Project Structure

```
.
├── CMakeLists.txt
├── README.md
├── build
│   ├── dumppp.csv
│   ├── log
│   │   ├── receiver_log.txt
│   │   └── sender_log.txt
│   ├── receiver
│   └── sender
├── common
│   ├── include
│   │   ├── config.h
│   │   └── logger.h
│   └── src
│       ├── config.cpp
│       └── logger.cpp
├── config
│   └── values.config
├── lib
│   ├── Boost_Headers
│   ├── Boost_libraries
│   ├── nlohmann_Json_Header
│   ├── rabbitmq_Headers
│   ├── rabbitmq_Libraries
│   ├── spdlog
│   └── spdlog_libraries
├── messageclass
│   └── src
│       └── UserMessages_Data.h
├── receiver
│   └── src
│       └── receiver.cpp
└── sender
    └── src
        └── sender.cpp
```

## Project Images
Include relevant images or diagrams here to illustrate the project structure, data flow, or any other important aspects.

## Steps to Run the Project

### Clone the Repository:
```bash
git clone <repository-url>
cd <repository-directory>
```

### Build the Project:
```bash
mkdir build
cd build
cmake ..
make
```

### Configure the Project:
Edit the `config/values.config` file to set the desired configuration values.

### Run the Sender:
```bash
./sender
```

### Run the Receiver:
```bash
./receiver
```

## Technical Details

### Functions and Their Purpose

#### Sender.cpp
- **generaterandommobile_number()**: Generates a random 10-digit mobile number.
- **generaterandomip_address()**: Generates a random IP address.
- **generaterandomcreation_time()**: Generates a random creation time in `HHMMSSMilliseconds` format.
- **generaterandomname()**: Generates a random name from a predefined list.
- **generaterandomemail(const std::string& name)**: Generates a random email based on the provided name.
- **computecrc(const UserMessagesData& msg)**: Computes the CRC checksum for a given message.
- **logtransactiontimes(const std::vector& times)**: Logs the total and average time taken for transactions.
- **main()**: The main function that initializes the shared memory, generates random messages, computes checksums, and logs transaction details.

#### Receiver.cpp
- **computecrc(const UserMessagesData& msg)**: Computes the CRC checksum for a given message.
- **logreadtimes(const std::vector& times)**: Logs the total and average time taken for reading transactions.
- **sendtorabbitmq(const std::vector& messages, const std::string& queuename, const std::string& maxbundlesizeallowed)**: Sends bundled messages to RabbitMQ.
- **readandlogmessages(bip::managedsharedmemory& segment, std::vector& readtimes, sizet& lastprocessedindex, std::vector& messagebundle)**: Reads messages from shared memory, verifies checksums, and logs read times.
- **dumptocsv(const std::vector& messages, const std::string& filename)**: Dumps messages to a CSV file for debugging purposes.
- **main()**: The main function that initializes the shared memory, reads messages, sends bundles to RabbitMQ, and logs read times.

## Goals and What I Learned

### Goals:
- To implement a robust system for serializing and deserializing transaction messages.
- To ensure data integrity through CRC checksums.
- To bundle and send messages to RabbitMQ efficiently.
- To provide configurable parameters for flexibility and debugging.

### What I Learned:
- How to use **Boost libraries** for shared memory management and CRC computation.
- How to integrate **RabbitMQ** for message queuing.
- How to use **nlohmann/json** for JSON serialization and deserialization.
- How to implement **logging** using **spdlog**.
- How to configure and build a C++ project using **CMake**.
```

### Notes:
- You can add actual images or diagrams for **Project Images** where needed.
- Replace `<repository-url>` with the actual URL of your GitHub repository.
