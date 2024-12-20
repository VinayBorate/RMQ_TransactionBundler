let me tell you what i have done in previous assignment :


A) Earlier sender was serializing and saving this in shared memory a writing logs in "sender_log.txt" :

Here is what we are printing on console :

Transaction Type: 1, Creation Time: 75201910, Mobile Number: 1453811710, IP Address: 124.201.6.190, Personal Info: {Name: Bob Brown, Email: Bob.Brown@example.com}, Checksum: 349350755, Time Taken: 196 μs


Transaction Type: 2, Creation Time: 75201861, Mobile Number: 1534569757, IP Address: 72.80.192.161, Personal Info: {Name: Jane Smith, Email: Jane.Smith@example.com}, Checksum: 781696522, Time Taken: 38 μs


Transaction Type: 3, Creation Time: 75201779, Mobile Number: 2969755205, IP Address: 192.155.175.170, Personal Info: {Name: Bob Brown, Email: Bob.Brown@example.com}, Checksum: 2815490948, Time Taken: 33 μs


Transaction Type: 4, Creation Time: 75201412, Mobile Number: 2806696052, IP Address: 219.119.20.45, Personal Info: {Name: John Doe, Email: John.Doe@example.com}, Checksum: 1693693616, Time Taken: 49 μs


Transaction Type: 5, Creation Time: 75201455, Mobile Number: 3143923340, IP Address: 122.138.29.187, Personal Info: {Name: Alice Johnson, Email: Alice.Johnson@example.com}, Checksum: 2242474485, Time Taken: 54 μs


All Messages written to shared memory Except Time..
Total time for all transactions: 370 microseconds
Average time per transaction: 74 microseconds

B)receiver will start reading the data from shared memory and vaildate it by CRC checksum of Boost and save logs in "receiver_log.txt"

Here is what we are printing on console :

Opening shared memory...
Vector size: 5

Messages from shared memory:
Transaction Type: 1, Creation Time: 75201910, Mobile Number: 1453811710, IP Address: 124.201.6.190, Personal Info: {

Email: Bob.Brown@example.com, Name: Bob Brown, }, Checksum: 349350755, Time: 268μs
Transaction Type: 2, Creation Time: 75201861, Mobile Number: 1534569757, IP Address: 72.80.192.161, Personal Info: {

Email: Jane.Smith@example.com, Name: Jane Smith, }, Checksum: 781696522, Time: 54μs
Transaction Type: 3, Creation Time: 75201779, Mobile Number: 2969755205, IP Address: 192.155.175.170, Personal Info: {

Email: Bob.Brown@example.com, Name: Bob Brown, }, Checksum: 2815490948, Time: 30μs
Transaction Type: 4, Creation Time: 75201412, Mobile Number: 2806696052, IP Address: 219.119.20.45, Personal Info: {

Email: John.Doe@example.com, Name: John Doe, }, Checksum: 1693693616, Time: 32μs
Transaction Type: 5, Creation Time: 75201455, Mobile Number: 3143923340, IP Address: 122.138.29.187, Personal Info: {

Email: Alice.Johnson@example.com, Name: Alice Johnson, }, Checksum: 2242474485, Time: 61μs
Messages read from shared memory.
Total time for reading all transactions: 445 microseconds
Average time per transaction: 89 microseconds


=========================================================================================================
So now lets move to our current asssignmnet problem :
Create bundle of messages of the deserialised class and send it to rmq in the following format
Header - TTY, Creation Time, Mobile number, IP, PII
Value  -  1, 12343, 9876543210, 10.15.72.36, PII as json string

Condition when these bundles will be sent to RMQ are controlled by bundle size or bundle timeout.
Configurable part - Bundle size, time out of bundle, local dumping with relevant file name for debugging.
The data sent will be in the csv file which can be observed in excel.

===========================================================================================================


Do you need my sender reciver and UserMessages_Data.h CODE SO THAT WE CAN MOVE MORE PRECISLEY IN EACH STEP wITH EXPANATION AND CODE ?

/////gave the code of all files 

===========================================================================================================
===========================================================================================================
===========================================================================================================
===========================================================================================================
===========================================================================================================
===========================================================================================================
===========================================================================================================
===========================================================================================================
COPILET RESPONSE :
===========================================================================================================
===========================================================================================================
===========================================================================================================
===========================================================================================================
===========================================================================================================

STEP A) 

### Step 1: Modify `UserMessages_Data.h` to Include Serialization for RMQ

**AIM:** 
We need to modify the `UserMessages_Data` structure to include a method for serializing the data into a JSON string format. This will help us send the data to RabbitMQ (RMQ) in the required format.

**Code:**

1. **Include the JSON library:** We'll use the nlohmann/json library for JSON serialization. Make sure to add this library to your project if it's not already included.

2. **Add a method to serialize the data to JSON:** We'll add a method in `UserMessages_Data` to convert the data to a JSON string.

Here's the updated `UserMessages_Data.h` file:

```cpp
#ifndef USER_MESSAGES_DATA_H
#define USER_MESSAGES_DATA_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <iostream>
#include <cstdint> // Include this header for std::uint32_t
#include <nlohmann/json.hpp> // Include JSON library

// Define a namespace alias for ease of use
namespace bip = boost::interprocess;

// Define the shared memory allocator
typedef bip::allocator<char, bip::managed_shared_memory::segment_manager> ShmemCharAllocator;
typedef bip::basic_string<char, std::char_traits<char>, ShmemCharAllocator> ShmemString;

// Define ShmemMap to represent a map in shared memory
typedef bip::allocator<std::pair<const ShmemString, ShmemString>, bip::managed_shared_memory::segment_manager> ShmemMapAllocator;
typedef bip::map<ShmemString, ShmemString, std::less<ShmemString>, ShmemMapAllocator> ShmemMap;

struct UserMessages_Data {
    int transaction_type;
    int creation_time;
    long long int mobile_number;
    ShmemString ip_address;
    ShmemMap personal_identifier_info;
    std::uint32_t checksum; // Add this field

    // Constructor for shared memory usage
    UserMessages_Data(const ShmemCharAllocator& alloc)
        : ip_address(alloc), personal_identifier_info(alloc) {}

    // Serialization function
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & boost::serialization::make_nvp("transaction_type", transaction_type);
        ar & boost::serialization::make_nvp("creation_time", creation_time);
        ar & boost::serialization::make_nvp("mobile_number", mobile_number);
        ar & boost::serialization::make_nvp("ip_address", ip_address);
        ar & boost::serialization::make_nvp("personal_identifier_info", personal_identifier_info);
        ar & boost::serialization::make_nvp("checksum", checksum); // Serialize checksum
    }

    // Method to convert the data to a JSON string
    std::string to_json() const {
        nlohmann::json j;
        j["transaction_type"] = transaction_type;
        j["creation_time"] = creation_time;
        j["mobile_number"] = mobile_number;
        j["ip_address"] = ip_address.c_str();
        
        nlohmann::json pii_json;
        for (const auto& entry : personal_identifier_info) {
            pii_json[entry.first.c_str()] = entry.second.c_str();
        }
        j["personal_identifier_info"] = pii_json;
        j["checksum"] = checksum;

        return j.dump();
    }
};

// Custom operator<< for ShmemString
inline std::ostream& operator<<(std::ostream& os, const ShmemString& str) {
    os << str.c_str();
    return os;
}

// Custom operator<< for std::pair containing ShmemString
inline std::ostream& operator<<(std::ostream& os, const std::pair<const ShmemString, ShmemString>& p) {
    os << p.first << ": " << p.second;
    return os;
}

#endif // USER_MESSAGES_DATA_H
```

**Explanation:**

- **Include JSON library:** We include the `nlohmann/json.hpp` header for JSON serialization.
- **Add `to_json` method:** This method converts the `UserMessages_Data` object into a JSON string. It uses the `nlohmann::json` library to create a JSON object and populate it with the data from the `UserMessages_Data` object.

With this step completed, we can now serialize our data into JSON format, which is essential for sending it to RabbitMQ.

Shall we move on to the next step?


===========================================================================================================

STEP B)


### Step 2: Modify `sender.cpp` to Bundle Messages and Send to RMQ

**AIM:**
We need to modify the `sender.cpp` file to create bundles of messages and send them to RabbitMQ (RMQ) based on configurable conditions such as bundle size or timeout. We'll also add functionality to dump the data locally for debugging.

**Code:**

1. **Include RabbitMQ library:** Ensure you have the RabbitMQ C++ client library installed and include it in your project.

2. **Modify `sender.cpp` to bundle messages:** We'll add logic to bundle messages and send them to RMQ.

Here's the updated `sender.cpp` file:

```cpp
#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/crc.hpp> // Include the CRC header
#include "UserMessages_Data.h"
#include "../../common/include/logger.h"
#include "../../common/include/config.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp> // Include JSON library
#include <amqpcpp.h> // Include RabbitMQ library
#include <amqpcpp/libboostasio.h> // Include Boost.Asio handler for RabbitMQ

// Define a namespace alias for ease of use
namespace bip = boost::interprocess;

// Function to generate a random 10-digit mobile number
long long int generate_random_mobile_number() {
    return 1000000000LL + rand() % 9000000000LL;
}

// Function to generate a random IP address
std::string generate_random_ip_address() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

// Function to generate a random creation time in HHMMSSMilliseconds format
int generate_random_creation_time() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int milliseconds = rand() % 1000;
    return (ltm->tm_hour * 10000000) + (ltm->tm_min * 100000) + (ltm->tm_sec * 1000) + milliseconds;
}

// Function to generate a random name
std::string generate_random_name() {
    const std::vector<std::string> names = {"John Doe", "Jane Smith", "Alice Johnson", "Bob Brown", "Charlie Davis"};
    return names[rand() % names.size()];
}

// Function to generate a random email
std::string generate_random_email(const std::string& name) {
    std::string email = name;
    std::replace(email.begin(), email.end(), ' ', '.');
    email += "@example.com";
    return email;
}

// Function to compute CRC checksum
std::uint32_t compute_crc(const UserMessages_Data& msg) {
    boost::crc_32_type crc;
    crc.process_bytes(&msg.transaction_type, sizeof(msg.transaction_type));
    crc.process_bytes(&msg.creation_time, sizeof(msg.creation_time));
    crc.process_bytes(&msg.mobile_number, sizeof(msg.mobile_number));
    crc.process_bytes(msg.ip_address.c_str(), msg.ip_address.size());

    for (const auto& entry : msg.personal_identifier_info) {
        crc.process_bytes(entry.first.c_str(), entry.first.size());
        crc.process_bytes(entry.second.c_str(), entry.second.size());
    }

    return crc.checksum();
}

void log_transaction_times(const std::vector<std::chrono::microseconds>& times) {
    auto total_time = std::chrono::microseconds::zero();
    for (const auto& time : times) {
        total_time += time;
    }
    auto avg_time = total_time / times.size();

    std::cout << "Total time for all transactions: " << total_time.count() << " microseconds" << std::endl;
    std::cout << "Average time per transaction: " << avg_time.count() << " microseconds" << std::endl;

    spdlog::info("Total time for all transactions: {} microseconds", total_time.count());
    spdlog::info("Average time per transaction: {} microseconds", avg_time.count());
}

void send_to_rmq(const std::vector<UserMessages_Data>& bundle) {
    // Convert bundle to JSON
    nlohmann::json j;
    for (const auto& msg : bundle) {
        j.push_back(nlohmann::json::parse(msg.to_json()));
    }
    std::string json_str = j.dump();

    // Setup RabbitMQ connection and channel
    boost::asio::io_service io_service;
    AMQP::LibBoostAsioHandler handler(io_service);
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://guest:guest@localhost/"));
    AMQP::TcpChannel channel(&connection);

    // Declare a queue
    channel.declareQueue("message_queue");

    // Publish the message
    channel.publish("", "message_queue", json_str);

    // Run the I/O service
    io_service.run();
}

int main() {
    loadConfig();
    LOGGER_LEVEL_SETTT = getConfigValue("LOGGER_LEVEL_SETTT");
    int DATA_BULK_COUNTT = std::stoi(getConfigValue("DATA_BULK_COUNT"));
    NAME_SHARED_MEMORY = getConfigValue("NAME_SHARED_MEMORY");
    const char* name_of_shared_memeory = NAME_SHARED_MEMORY.c_str();
    std::string size_memeory = getConfigValue("SHARED_MEMORY_SIZE");
    std::size_t size_of_shared_memeory = static_cast<std::size_t>(std::stoull(size_memeory));
    int BUNDLE_SIZE = std::stoi(getConfigValue("BUNDLE_SIZE"));
    int BUNDLE_TIMEOUT = std::stoi(getConfigValue("BUNDLE_TIMEOUT"));

    srand(time(0)); // Seed the random number generator

    setup_logger("sender_logger", "sender_log.txt", LOGGER_LEVEL_SETTT);

    try {
        // Remove shared memory on start
        bip::shared_memory_object::remove(name_of_shared_memeory);

        // Create shared memory
        bip::managed_shared_memory segment(bip::create_only, name_of_shared_memeory, size_of_shared_memeory);

        // Define shared memory allocator and vector
        typedef bip::allocator<UserMessages_Data, bip::managed_shared_memory::segment_manager> ShmemAllocator;
        typedef boost::container::vector<UserMessages_Data, ShmemAllocator> ShmemVector;

        ShmemAllocator alloc_inst(segment.get_segment_manager());
        ShmemVector* messages = segment.construct<ShmemVector>("Messages")(alloc_inst);

        std::vector<std::chrono::microseconds> transaction_times;
        std::vector<UserMessages_Data> bundle;
        auto bundle_start_time = std::chrono::high_resolution_clock::now();

        // Fill the vector with random data
        for (int i = 1; i <= DATA_BULK_COUNTT; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();

            UserMessages_Data msg(alloc_inst);
            msg.transaction_type = i;
            msg.creation_time = generate_random_creation_time();
            msg.mobile_number = generate_random_mobile_number();
            msg.ip_address = ShmemString(generate_random_ip_address().c_str(), alloc_inst);

            // Generate random personal info
            std::string name = generate_random_name();
            std::string email = generate_random_email(name);
            msg.personal_identifier_info.insert(std::make_pair(ShmemString("Name", alloc_inst), ShmemString(name.c_str(), alloc_inst)));
            msg.personal_identifier_info.insert(std::make_pair(ShmemString("Email", alloc_inst), ShmemString(email.c_str(), alloc_inst)));

            // Compute CRC checksum
            std::uint32_t checksum = compute_crc(msg);
            msg.checksum = checksum;

            messages->push_back(msg);
            bundle.push_back(msg);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            transaction_times.push_back(duration);

            std::cout << "Transaction Type: " << msg.transaction_type
                      << ", Creation Time: " << msg.creation_time
                      << ", Mobile Number: " << msg.mobile_number
                      << ", IP Address: " << msg.ip_address
                      << ", Personal Info: {Name: " << name << ", Email: " << email << "}"
                      << ", Checksum: " << checksum
                      << ", Time Taken: " << duration.count() << " μs" << std::endl << std::endl << std::endl;

            spdlog::info("Transaction Type: {}, Creation Time: {}, Mobile Number: {}, IP Address: {}, Personal Info: {{Name: {}, Email: {}}}, Checksum: {}, Time Taken: {} μs",
                         msg.transaction_type, msg.creation_time, msg.mobile_number, msg.ip_address.c_str(), name, email, checksum, duration.count());

            // Check if bundle size or timeout condition is met
            auto bundle_current_time = std::chrono::high_resolution_clock::now();
            auto bundle_duration = std::chrono::duration_cast<std::chrono::seconds>(bundle_current_time - bundle_start_time).count();

            if (bundle.size() >= BUNDLE_SIZE || bundle_duration >= BUNDLE_TIMEOUT) {
                send_to_rmq(bundle);
                bundle.clear();
                bundle_start_time = std::chrono::high_resolution_clock::now();
            }
        }


===========================================================================================================
### Step 3: Modify `receiver.cpp` to Read Bundles and Validate

**AIM:**
We need to modify the `receiver.cpp` file to read the bundles of messages from shared memory, validate them using the CRC checksum, and log the results.

**Code:**

1. **Modify `receiver.cpp` to read bundles:** We'll add logic to read the bundles of messages from shared memory and validate them.

Here's the updated `receiver.cpp` file:

```cpp
#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/crc.hpp> // Include the CRC header
#include "UserMessages_Data.h"
#include "../../common/include/logger.h"
#include "../../common/include/config.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <chrono>
#include <thread>
#include <vector>

// Define a namespace alias for ease of use
namespace bip = boost::interprocess;

// Function to compute CRC checksum
std::uint32_t compute_crc(const UserMessages_Data& msg) {
    boost::crc_32_type crc;
    crc.process_bytes(&msg.transaction_type, sizeof(msg.transaction_type));
    crc.process_bytes(&msg.creation_time, sizeof(msg.creation_time));
    crc.process_bytes(&msg.mobile_number, sizeof(msg.mobile_number));
    crc.process_bytes(msg.ip_address.c_str(), msg.ip_address.size());

    for (const auto& entry : msg.personal_identifier_info) {
        crc.process_bytes(entry.first.c_str(), entry.first.size());
        crc.process_bytes(entry.second.c_str(), entry.second.size());
    }

    return crc.checksum();
}

void log_read_times(const std::vector<std::chrono::microseconds>& times) {
    auto total_time = std::chrono::microseconds::zero();
    for (const auto& time : times) {
        total_time += time;
    }
    auto avg_time = total_time / times.size();

    std::cout << "Total time for reading all transactions: " << total_time.count() << " microseconds" << std::endl;
    std::cout << "Average time per transaction: " << avg_time.count() << " microseconds" << std::endl;

    spdlog::info("Total time for reading all transactions: {} microseconds", total_time.count());
    spdlog::info("Average time per transaction: {} microseconds", avg_time.count());
}

int main() {
    loadConfig();
    LOGGER_LEVEL_SETTT = getConfigValue("LOGGER_LEVEL_SETTT");
    setup_logger("receiver_logger", "receiver_log.txt", LOGGER_LEVEL_SETTT);
    NAME_SHARED_MEMORY = getConfigValue("NAME_SHARED_MEMORY");
    const char* shared_memeory_name = NAME_SHARED_MEMORY.c_str();

    try {
        std::cout << "Opening shared memory..." << std::endl;
        spdlog::info("Opening shared memory...");

        // Open the shared memory
        bip::managed_shared_memory segment(bip::open_only, shared_memeory_name);

        // Find the vector using the same name
        typedef bip::allocator<UserMessages_Data, bip::managed_shared_memory::segment_manager> ShmemAllocator;
        typedef boost::container::vector<UserMessages_Data, ShmemAllocator> ShmemVector;

        std::pair<ShmemVector*, bip::managed_shared_memory::size_type> result = segment.find<ShmemVector>("Messages");
        ShmemVector* messages = result.first;

        if (!messages) {
            std::cerr << "No messages found in shared memory." << std::endl;
            spdlog::error("No messages found in shared memory.");
            return 1;
        }

        std::cout << "Vector size: " << messages->size() << std::endl;
        spdlog::info("Vector size: {}", messages->size());

        std::vector<std::chrono::microseconds> read_times;

        // Access and print vector elements using iterators
        std::cout << "\nMessages from shared memory:\n";
        spdlog::info("Messages from shared memory:");
        for (const auto& msg : *messages) {
            auto start_time = std::chrono::high_resolution_clock::now();

            // Compute CRC checksum
            std::uint32_t received_checksum = compute_crc(msg);

            // Verify checksum
            if (received_checksum != msg.checksum) {
                std::cerr << "Checksum mismatch for message with Transaction Type: " << msg.transaction_type << std::endl;
                spdlog::error("Checksum mismatch for message with Transaction Type: {}", msg.transaction_type);
                continue;
            }

            std::cout << "Transaction Type: " << msg.transaction_type
                      << ", Creation Time: " << msg.creation_time
                      << ", Mobile Number: " << msg.mobile_number
                      << ", IP Address: " << msg.ip_address
                      << ", Personal Info: {" << std::endl << std::endl;

            spdlog::info("Transaction Type: {}, Creation Time: {}, Mobile Number: {}, IP Address: {}, Personal Info: {{",
                         msg.transaction_type, msg.creation_time, msg.mobile_number, msg.ip_address.c_str());

            for (const auto& entry : msg.personal_identifier_info) {
                std::cout << entry.first << ": " << entry.second << ", ";
                spdlog::info("{}: {}, ", entry.first.c_str(), entry.second.c_str());
            }

            std::cout << "}, Checksum: " << received_checksum << ", Time: ";
            spdlog::info("}}, Checksum: {}, Time: ", received_checksum);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            read_times.push_back(duration);

            std::cout << duration.count() << "μs" << std::endl;
            spdlog::info("{} microseconds", duration.count());
        }

        std::cout << "Messages read from shared memory." << std::endl;
        spdlog::info("Messages read from shared memory.");

        // Create a thread to log read times
        std::thread log_thread(log_read_times, std::ref(read_times));
        log_thread.join();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        spdlog::error("Error: {}", e.what());
    }

    return 0;
}
```

**Explanation:**

- **Compute CRC checksum:** We use the `compute_crc` function to compute the CRC checksum for each message.
- **Log read times:** The `log_read_times` function logs the total and average time taken to read all transactions.
- **Read and validate messages:** We read the messages from shared memory, compute the CRC checksum, and validate it against the stored checksum. If the checksum matches, we log the message details.

With this step completed, we can now read and validate the bundles of messages from shared memory.

Shall we move on to the next step?
===========================================================================================================
### Step 4: Configure CMakeLists.txt for RabbitMQ and JSON Libraries

**AIM:**
We need to update the `CMakeLists.txt` file to include the RabbitMQ and JSON libraries. This will ensure that our project can compile and link with these dependencies.

**Code:**

1. **Add the JSON library:** Ensure the `nlohmann/json` library is included in your project.
2. **Add the RabbitMQ library:** Ensure the RabbitMQ C++ client library is included in your project.

Here's the updated `CMakeLists.txt` file:

```cmake
cmake_minimum_required(VERSION 3.10)
project(SharedMemoryProject)

set(CMAKE_CXX_STANDARD 17)  # Ensure C++17 is enabled

# Add local Boost and spdlog headers
include_directories(${CMAKE_SOURCE_DIR}/lib/Boost_Headers)
include_directories(${CMAKE_SOURCE_DIR}/lib/spdlog)

# Include directories for headers
include_directories(
    lib
    common/include
    lib/spdlog
    lib/json/include  # Add JSON library include directory
    /usr/local/include/amqpcpp  # Add RabbitMQ library include directory
)

# Add local Boost and spdlog libraries
link_directories(${CMAKE_SOURCE_DIR}/lib/Boost_libraries)
link_directories(${CMAKE_SOURCE_DIR}/lib/spdlog_libraries)
link_directories(/usr/local/lib)  # Add RabbitMQ library directory

# Include the directory where UserMessages_Data.h is located
include_directories(${CMAKE_SOURCE_DIR}/messageclass/src)

# Add the sender and receiver executables
add_executable(sender sender/src/sender.cpp common/src/logger.cpp common/src/config.cpp)
add_executable(receiver receiver/src/receiver.cpp common/src/logger.cpp common/src/config.cpp)

# Link Boost, spdlog, JSON, and RabbitMQ libraries
target_link_libraries(sender boost_system boost_filesystem spdlog nlohmann_json amqpcpp)
target_link_libraries(receiver boost_system boost_filesystem spdlog nlohmann_json amqpcpp)
```

**Explanation:**

- **Include JSON library:** We add the include directory for the `nlohmann/json` library.
- **Include RabbitMQ library:** We add the include directory for the RabbitMQ C++ client library.
- **Link libraries:** We link the `nlohmann_json` and `amqpcpp` libraries to the `sender` and `receiver` executables.

With this step completed, our project is now configured to use the RabbitMQ and JSON libraries.

Shall we move on to the next step?
===========================================================================================================
### Step 5: Implement Local Dumping for Debugging

**AIM:**
We need to add functionality to dump the data locally for debugging purposes. This involves writing the bundled messages to a CSV file that can be observed in Excel.

**Code:**

1. **Add a function to write bundles to a CSV file:** We'll create a function that writes the bundled messages to a CSV file.

2. **Modify `sender.cpp` to call this function:** We'll call this function whenever a bundle is sent to RabbitMQ.

Here's the updated `sender.cpp` file with the local dumping functionality:

```cpp
#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/crc.hpp> // Include the CRC header
#include "UserMessages_Data.h"
#include "../../common/include/logger.h"
#include "../../common/include/config.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp> // Include JSON library
#include <amqpcpp.h> // Include RabbitMQ library
#include <amqpcpp/libboostasio.h> // Include Boost.Asio handler for RabbitMQ
#include <fstream> // Include file stream for CSV dumping

// Define a namespace alias for ease of use
namespace bip = boost::interprocess;

// Function to generate a random 10-digit mobile number
long long int generate_random_mobile_number() {
    return 1000000000LL + rand() % 9000000000LL;
}

// Function to generate a random IP address
std::string generate_random_ip_address() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

// Function to generate a random creation time in HHMMSSMilliseconds format
int generate_random_creation_time() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int milliseconds = rand() % 1000;
    return (ltm->tm_hour * 10000000) + (ltm->tm_min * 100000) + (ltm->tm_sec * 1000) + milliseconds;
}

// Function to generate a random name
std::string generate_random_name() {
    const std::vector<std::string> names = {"John Doe", "Jane Smith", "Alice Johnson", "Bob Brown", "Charlie Davis"};
    return names[rand() % names.size()];
}

// Function to generate a random email
std::string generate_random_email(const std::string& name) {
    std::string email = name;
    std::replace(email.begin(), email.end(), ' ', '.');
    email += "@example.com";
    return email;
}

// Function to compute CRC checksum
std::uint32_t compute_crc(const UserMessages_Data& msg) {
    boost::crc_32_type crc;
    crc.process_bytes(&msg.transaction_type, sizeof(msg.transaction_type));
    crc.process_bytes(&msg.creation_time, sizeof(msg.creation_time));
    crc.process_bytes(&msg.mobile_number, sizeof(msg.mobile_number));
    crc.process_bytes(msg.ip_address.c_str(), msg.ip_address.size());

    for (const auto& entry : msg.personal_identifier_info) {
        crc.process_bytes(entry.first.c_str(), entry.first.size());
        crc.process_bytes(entry.second.c_str(), entry.second.size());
    }

    return crc.checksum();
}

void log_transaction_times(const std::vector<std::chrono::microseconds>& times) {
    auto total_time = std::chrono::microseconds::zero();
    for (const auto& time : times) {
        total_time += time;
    }
    auto avg_time = total_time / times.size();

    std::cout << "Total time for all transactions: " << total_time.count() << " microseconds" << std::endl;
    std::cout << "Average time per transaction: " << avg_time.count() << " microseconds" << std::endl;

    spdlog::info("Total time for all transactions: {} microseconds", total_time.count());
    spdlog::info("Average time per transaction: {} microseconds", avg_time.count());
}

void send_to_rmq(const std::vector<UserMessages_Data>& bundle) {
    // Convert bundle to JSON
    nlohmann::json j;
    for (const auto& msg : bundle) {
        j.push_back(nlohmann::json::parse(msg.to_json()));
    }
    std::string json_str = j.dump();

    // Setup RabbitMQ connection and channel
    boost::asio::io_service io_service;
    AMQP::LibBoostAsioHandler handler(io_service);
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://guest:guest@localhost/"));
    AMQP::TcpChannel channel(&connection);

    // Declare a queue
    channel.declareQueue("message_queue");

    // Publish the message
    channel.publish("", "message_queue", json_str);

    // Run the I/O service
    io_service.run();
}

void dump_to_csv(const std::vector<UserMessages_Data>& bundle, const std::string& filename) {
    std::ofstream file(filename);

    // Write CSV header
    file << "Transaction Type,Creation Time,Mobile Number,IP Address,Personal Info,Checksum\n";

    // Write CSV rows
    for (const auto& msg : bundle) {
        file << msg.transaction_type << ","
             << msg.creation_time << ","
             << msg.mobile_number << ","
             << msg.ip_address.c_str() << ","
             << "{";
        for (const auto& entry : msg.personal_identifier_info) {
            file << entry.first.c_str() << ": " << entry.second.c_str() << ", ";
        }
        file << "},"
             << msg.checksum << "\n";
    }

    file.close();
}

int main() {
    loadConfig();
    LOGGER_LEVEL_SETTT = getConfigValue("LOGGER_LEVEL_SETTT");
    int DATA_BULK_COUNTT = std::stoi(getConfigValue("DATA_BULK_COUNT"));
    NAME_SHARED_MEMORY = getConfigValue("NAME_SHARED_MEMORY");
    const char* name_of_shared_memeory = NAME_SHARED_MEMORY.c_str();
    std::string size_memeory = getConfigValue("SHARED_MEMORY_SIZE");
    std::size_t size_of_shared_memeory = static_cast<std::size_t>(std::stoull(size_memeory));
    int BUNDLE_SIZE = std::stoi(getConfigValue("BUNDLE_SIZE"));
    int BUNDLE_TIMEOUT = std::stoi(getConfigValue("BUNDLE_TIMEOUT"));

    srand(time(0)); // Seed the random number generator

    setup_logger("sender_logger", "sender_log.txt", LOGGER_LEVEL_SETTT);

    try {
        // Remove shared memory on start
        bip::shared_memory_object::remove(name_of_shared_memeory);

        // Create shared memory
        bip::managed_shared_memory segment(bip::create_only, name_of_shared_memeory, size_of_shared_memeory);

        // Define shared memory allocator and vector
        typedef bip::allocator<UserMessages_Data, bip::managed_shared_memory::segment_manager> ShmemAllocator;
        typedef boost::container::vector<UserMessages_Data, ShmemAllocator> ShmemVector;

        ShmemAllocator alloc_inst(segment.get_segment_manager());
        ShmemVector* messages = segment.construct<ShmemVector>("Messages")(alloc_inst);

        std::vector<std::chrono::microseconds> transaction_times;
        std::vector<UserMessages_Data> bundle;
        auto bundle_start_time = std::chrono::high_resolution_clock::now();

        // Fill the vector with random data
        for (int i = 1; i <= DATA_BULK_COUNTT; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();

            UserMessages_Data msg(alloc_inst);
            msg.transaction_type = i;
            msg.creation_time = generate_random_creation_time();
            msg.mobile_number = generate_random_mobile_number();
            msg.ip_address = ShmemString(generate_random_ip_address().c_str(), alloc_inst);

            // Generate random personal info
            std::string name = generate_random_name();
            std::string email = generate_random_email(name);
            msg.personal_identifier_info.insert(std::make_pair(ShmemString("Name", alloc_inst), ShmemString(name.c_str(), alloc_inst)));
            msg.personal_identifier_info.insert(std::make_pair(ShmemString("Email", alloc_inst), ShmemString(email.c_str(), alloc_inst)));

            // Compute CRC checksum
            std::uint32_t checksum = compute_crc(msg);
            msg.checksum = checksum;

            messages->push_back(msg);
            bundle.push_back(msg);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            transaction_times.push_back(duration);

            std::cout << "Transaction Type: " << msg.transaction_type
                      << ", Creation Time: " << msg.creation_time
                      << ", Mobile Number: " << msg.mobile_number
                      << ", IP Address: " << msg.ip_address
                      << ", Personal Info: {Name: " << name << ", Email: " << email << "}"
                      << ", Checksum: " << checksum
                      << ", Time Taken: " << duration.count() << " μs" << std::endl << std::endl << std::endl;

            spdlog::info("Transaction Type: {}, Creation Time: {}, Mobile Number: {}, IP Address: {}, Personal Info: {{Name: {}, Email: {}}}, Checksum: {}, Time Taken: {} μs",
                         msg.transaction_type, msg.creation_time, msg.mobile_number, msg.ip_address.c_str(), name, email, checksum, duration.count());

            // Check if bundle size or timeout condition is met
            auto bundle_current_time = std::chrono::high_resolution_clock::now();
            auto bundle_duration = std::chrono::duration_cast<std::chrono::seconds>(bundle_current_time - bundle_start_time).count();

            if (bundle.size() >= BUNDLE_SIZE || bundle_duration >= BUNDLE_TIMEOUT) {
                // Dump the bundle to a CSV file for debugging
                std::string filename = "bundle_" + std::to_string(bundle_start_time.time_since_epoch().count()) + ".csv";
                dump_to_csv(bundle, filename);

                // Send the bundle to RabbitMQ
                send_to_rmq(bundle);

                // Clear the bundle and reset the timer
                bundle.clear();
                bundle_start_time = std::chrono::high_resolution_clock::now();
            }
        }

        std::cout << "All Messages written to shared memory Except Time.." << std::endl;
        spdlog::info("All Messages written to shared memory Except Time..");

        // Create a thread to log transaction times
        std::thread log_thread(log_transaction_times, std::ref(transaction_times));
        log_thread.join();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        spdlog::error("Error: {}", e.what());
    }

    return 0;
}
Explanation:

Dump to CSV: The dump_to_csv function writes the bundled messages to a CSV file. The filename is generated using the bundle start time to ensure uniqueness.
Check bundle conditions: We check if the bundle size or timeout condition is met. If either condition is met, we dump the bundle to a CSV file and send it to RabbitMQ.
Clear bundle: After sending the bundle, we clear it and reset the timer.
With this step completed, we have added the functionality to dump the data locally for debugging purposes.

Shall we move on to the final step?
===========================================================================================================

===========================================================================================================


```s

```
===========================================================================================================
### Step 6: Test and Validate the Implementation

**AIM:**
We need to test and validate the entire implementation to ensure that the sender correctly bundles and sends messages to RabbitMQ, and the receiver correctly reads and validates these messages.

**Steps:**

1. **Build the project:** Use CMake to build the project and ensure there are no compilation errors.
2. **Run the sender:** Execute the sender to generate, bundle, and send messages to RabbitMQ.
3. **Run the receiver:** Execute the receiver to read and validate the messages from shared memory.
4. **Verify the output:** Check the logs and the CSV files to ensure the messages are correctly processed.

**Code:**

1. **Build the project:**

```sh
mkdir -p build
cd build
cmake ..
make
```

2. **Run the sender:**

```sh
./sender
```

3. **Run the receiver:**

```sh
./receiver
```

4. **Verify the output:**
   - Check the `sender_log.txt` and `receiver_log.txt` files for any errors or warnings.
   - Open the generated CSV files to verify the bundled messages.
   - Ensure the messages are correctly sent to RabbitMQ and read by the receiver.

**Explanation:**

- **Build the project:** We use CMake to configure and build the project.
- **Run the sender and receiver:** We execute the sender and receiver to test the entire workflow.
- **Verify the output:** We check the logs and CSV files to ensure everything is working as expected.

With this step completed, we have successfully implemented and tested the assignment. If you encounter any issues during testing, feel free to share the details, and we can troubleshoot them together.

===========================================================================================================
Great! Let's go through the testing process step by step.

 Step 1: Build the Project

1. **Navigate to the project directory:**

```sh
cd /path/to/your/project
```

2. **Create a build directory and navigate into it:**

```sh
mkdir -p build
cd build
```

3. **Run CMake to configure the project:**

```sh
cmake ..
```

4. **Build the project:**

```sh
make
```

Step 2: Run the Sender

1. **Execute the sender to generate, bundle, and send messages to RabbitMQ:**

```sh
./sender
```
 Step 3: Run the Receiver

1. **Execute the receiver to read and validate the messages from shared memory:**

```sh
./receiver
```
 Step 4: Verify the Output

1. **Check the logs:**
   - Open `sender_log.txt` and `receiver_log.txt` to ensure there are no errors or warnings.

2. **Check the CSV files:**
   - Open the generated CSV files (e.g., `bundle_<timestamp>.csv`) to verify the bundled messages.

3. **Verify RabbitMQ:**
   - Ensure the messages are correctly sent to RabbitMQ and read by the receiver.

If you encounter any issues during testing, please share the details, and we can troubleshoot them together. Let's start with building the project and running the sender. Let me know if you face any challenges or if everything works smoothly!
===========================================================================================================
### Step 7: Troubleshooting and Validation

**AIM:**
If you encounter any issues during testing, we need to troubleshoot and validate the implementation to ensure everything works as expected.

**Steps:**

1. **Check for Compilation Errors:**
   - If there are any errors during the build process, review the error messages and ensure all dependencies are correctly installed and linked.

2. **Verify Configuration:**
   - Ensure the configuration values in your `values.config` file are correctly set, especially for `LOGGER_LEVEL_SETTT`, `DATA_BULK_COUNT`, `NAME_SHARED_MEMORY`, `SHARED_MEMORY_SIZE`, `BUNDLE_SIZE`, and `BUNDLE_TIMEOUT`.

3. **Check RabbitMQ Connection:**
   - Ensure RabbitMQ is running and accessible at the specified address (`amqp://guest:guest@localhost/`).
   - Verify that the RabbitMQ queue (`message_queue`) is correctly declared and messages are being published.

4. **Review Logs:**
   - Check `sender_log.txt` and `receiver_log.txt` for any errors or warnings that might indicate issues with message generation, bundling, sending, or receiving.

5. **Validate CSV Files:**
   - Open the generated CSV files to ensure the messages are correctly formatted and contain the expected data.

6. **Debugging:**
   - If there are issues with message validation, add additional logging to track the flow of data and identify where the problem might be occurring.

**Example Troubleshooting Steps:**

1. **Compilation Error:**
   - If you encounter a compilation error related to missing libraries, ensure the paths to the JSON and RabbitMQ libraries are correctly set in `CMakeLists.txt`.

2. **RabbitMQ Connection Issue:**
   - If the sender cannot connect to RabbitMQ, verify that RabbitMQ is running and accessible. You can use the RabbitMQ management interface to check the status of the server and queues.

3. **Checksum Mismatch:**
   - If the receiver reports a checksum mismatch, ensure the `compute_crc` function is correctly implemented and that the data is not being modified between serialization and deserialization.

By following these steps, you should be able to identify and resolve any issues that arise during testing. If you need further assistance, feel free to share the specific error messages or issues you're encountering, and we can troubleshoot them together.

Would you like to proceed with testing, or do you have any specific issues you'd like to address?
===========================================================================================================
### Step 8: Final Validation and Review

**AIM:**
Ensure that the entire implementation is working as expected by performing a final validation and review of the system.

**Steps:**

1. **Final Run of Sender and Receiver:**
   - Execute the sender and receiver one more time to ensure everything is functioning correctly.

2. **Review Logs and Outputs:**
   - Check the `sender_log.txt` and `receiver_log.txt` files for any final errors or warnings.
   - Verify the contents of the generated CSV files to ensure the data is correctly formatted and complete.

3. **RabbitMQ Verification:**
   - Use the RabbitMQ management interface to verify that messages are being correctly published to and consumed from the queue.

4. **Performance Metrics:**
   - Review the total and average transaction times logged by both the sender and receiver to ensure they meet the expected performance criteria.

5. **Documentation:**
   - Ensure that all code is well-documented, and any configuration files are clearly explained.
   - Update the `Readme.md` file with instructions on how to build, run, and test the project.

**Example Commands:**

1. **Run the sender:**

```sh
./sender
```

2. **Run the receiver:**

```sh
./receiver
```

3. **Check logs:**

```sh
cat sender_log.txt
cat receiver_log.txt
```

4. **Verify CSV files:**

```sh
ls -l *.csv
cat bundle_<timestamp>.csv
```

5. **RabbitMQ Management Interface:**
   - Access the RabbitMQ management interface at `http://localhost:15672/` (default credentials: guest/guest).
   - Check the `message_queue` to ensure messages are being published and consumed.

By following these steps, you should be able to confirm that the entire system is working as intended. If you encounter any issues, please share the details, and we can troubleshoot them together.

Would you like to proceed with the final validation, or do you have any specific questions or concerns?
===========================================================================================================
======================================================================================================================================================================================================================

Sure! Here's a detailed flowchart that outlines the entire process, including the key functions and logic used in the sender and receiver components.

### Flowchart Overview

```plaintext
+---------------------+
| Start               |
+---------------------+
          |
          v
+---------------------+
| Load Configuration  |
| - loadConfig()      |
| - getConfigValue()  |
+---------------------+
          |
          v
+---------------------+
| Setup Logger        |
| - setup_logger()    |
+---------------------+
          |
          v
+---------------------+
| Create Shared Memory|
| - bip::managed_shared_memory |
+---------------------+
          |
          v
+---------------------+
| Generate Messages   |
| - generate_random_* |
| - compute_crc()     |
+---------------------+
          |
          v
+---------------------+
| Bundle Messages     |
| - Check bundle size |
| - Check bundle timeout |
+---------------------+
          |
          v
+---------------------+
| Dump to CSV         |
| - dump_to_csv()     |
+---------------------+
          |
          v
+---------------------+
| Send to RabbitMQ    |
| - send_to_rmq()     |
+---------------------+
          |
          v
+---------------------+
| Log Transaction Times |
| - log_transaction_times() |
+---------------------+
          |
          v
+---------------------+
| Receiver: Open Shared Memory |
| - bip::managed_shared_memory |
+---------------------+
          |
          v
+---------------------+
| Read Messages       |
| - compute_crc()     |
+---------------------+
          |
          v
+---------------------+
| Validate Messages   |
| - Check CRC checksum |
+---------------------+
          |
          v
+---------------------+
| Log Read Times      |
| - log_read_times()  |
+---------------------+
          |
          v
+---------------------+
| End                 |
+---------------------+
```

### Detailed Flow

1. **Start:**
   - The process begins with loading the configuration settings using `loadConfig()` and `getConfigValue()` functions.

2. **Setup Logger:**
   - The logger is set up using the `setup_logger()` function to log messages to `sender_log.txt` and `receiver_log.txt`.

3. **Create Shared Memory:**
   - Shared memory is created using `bip::managed_shared_memory` to store the messages.

4. **Generate Messages:**
   - Messages are generated with random data using functions like `generate_random_mobile_number()`, `generate_random_ip_address()`, `generate_random_creation_time()`, `generate_random_name()`, and `generate_random_email()`.
   - CRC checksum for each message is computed using `compute_crc()`.

5. **Bundle Messages:**
   - Messages are bundled based on the configured bundle size and timeout conditions.

6. **Dump to CSV:**
   - Bundled messages are dumped to a CSV file for debugging using `dump_to_csv()`.

7. **Send to RabbitMQ:**
   - Bundled messages are sent to RabbitMQ using `send_to_rmq()`.

8. **Log Transaction Times:**
   - The total and average transaction times are logged using `log_transaction_times()`.

9. **Receiver: Open Shared Memory:**
   - The receiver opens the shared memory using `bip::managed_shared_memory`.

10. **Read Messages:**
    - Messages are read from shared memory, and CRC checksum is computed using `compute_crc()`.

11. **Validate Messages:**
    - Messages are validated by checking the CRC checksum.

12. **Log Read Times:**
    - The total and average read times are logged using `log_read_times()`.

13. **End:**
    - The process ends after logging the read times.

This flowchart and detailed explanation should help you understand the entire process and the key functions involved. If you have any questions or need further clarification, feel free to ask!            
===========================================================================================================
======================================================================================================================================================================================================================

*******************DETAILED FLOW CHART***************************************************************************


Here's a detailed flowchart and explanation focusing on the bundling process, sending to RabbitMQ (RMQ), and the configurable parts such as bundle size, timeout, and local dumping.

### Flowchart Overview

```plaintext
+---------------------+
|       Sender        |
+---------------------+
          |
          v
+---------------------+
| Load Configuration  |
| - Bundle size       |
| - Bundle timeout    |
| - Logger level      |
+---------------------+
          |
          v
+---------------------+
| Setup Logger        |
+---------------------+
          |
          v
+---------------------+
| Create Shared Memory|
+---------------------+
          |
          v
+---------------------+
| Generate Messages   |
| - Random data       |
| - Compute CRC       |
+---------------------+
          |
          v
+---------------------+
| Add to Bundle       |
| - Check size        |
| - Check timeout     |
+---------------------+
          |
          v
+---------------------+       +---------------------+
| Bundle Full or      |  Yes  | Dump to CSV         |
| Timeout Reached?    +------>+ - Filename with     |
|                     |       |   timestamp         |
+---------------------+       +---------------------+
          |                       |
          | No                    v
          |            +---------------------+
          |            | Send to RabbitMQ    |
          |            | - Convert to JSON   |
          |            | - Publish to queue  |
          |            +---------------------+
          |                       |
          v                       v
+---------------------+       +---------------------+
| Continue Generating |       | Clear Bundle        |
| Messages            |       +---------------------+
+---------------------+                 |
          |                             v
          |                  +---------------------+
          |                  | Log Transaction     |
          |                  | Times               |
          |                  +---------------------+
          v                             |
+---------------------+                 v
| End                 |       +---------------------+
+---------------------+       |         End         |
                              +---------------------+
```

```plaintext
+---------------------+
|      Receiver       |
+---------------------+
          |
          v
+---------------------+
| Load Configuration  |
| - Logger level      |
+---------------------+
          |
          v
+---------------------+
| Setup Logger        |
+---------------------+
          |
          v
+---------------------+
| Open Shared Memory  |
+---------------------+
          |
          v
+---------------------+
| Read Messages       |
| - Compute CRC       |
+---------------------+
          |
          v
+---------------------+
| Validate Messages   |
| - Check CRC         |
+---------------------+
          |
          v
+---------------------+
| Log Read Times      |
+---------------------+
          |
          v
+---------------------+
| End                 |
+---------------------+
```

### Detailed Flow

#### Sender

1. **Load Configuration:**
   - Load settings such as bundle size, bundle timeout, and logger level from the configuration file.

2. **Setup Logger:**
   - Initialize the logger to log messages to `sender_log.txt`.

3. **Create Shared Memory:**
   - Create shared memory using Boost Interprocess to store messages.

4. **Generate Messages:**
   - Generate random messages with fields like transaction type, creation time, mobile number, IP address, and personal info.
   - Compute the CRC checksum for each message to ensure data integrity.

5. **Add to Bundle:**
   - Add each generated message to a bundle.
   - Check if the bundle size or timeout condition is met.

6. **Bundle Full or Timeout Reached:**
   - If the bundle size or timeout condition is met:
     - **Dump to CSV:** Write the bundled messages to a CSV file with a filename that includes a timestamp for debugging purposes.
     - **Send to RabbitMQ:** Convert the bundle to a JSON string and publish it to the RabbitMQ queue.
     - **Clear Bundle:** Clear the bundle and reset the timer.

7. **Log Transaction Times:**
   - Log the total and average transaction times.

8. **End:**
   - Complete the sender process.

#### Receiver

1. **Load Configuration:**
   - Load settings such as logger level from the configuration file.

2. **Setup Logger:**
   - Initialize the logger to log messages to `receiver_log.txt`.

3. **Open Shared Memory:**
   - Open the shared memory created by the sender.

4. **Read Messages:**
   - Read messages from shared memory.
   - Compute the CRC checksum for each message to validate data integrity.

5. **Validate Messages:**
   - Validate messages by comparing the computed checksum with the stored checksum.

6. **Log Read Times:**
   - Log the total and average read times.

7. **End:**
   - Complete the receiver process.

This flowchart and detailed explanation should help you understand the entire process, focusing on the bundling, sending to RabbitMQ, and the configurable parts. If you have any questions or need further clarification, feel free to ask!


===========================================================================================================
======================================================================================================================================================================================================================
               ************************* BUNDLE ??*******************************************8
A "bundle" in this context refers to a group of messages that are collected together before being sent to RabbitMQ (RMQ). Instead of sending each message individually, we gather multiple messages into a single bundle and then send that bundle as a single unit. This can help improve efficiency and reduce the overhead of sending messages.

### Detailed Explanation of Bundling Process

1. **Generate Messages:**
   - Messages are generated with random data, including fields like transaction type, creation time, mobile number, IP address, and personal info.
   - Each message is assigned a CRC checksum to ensure data integrity.

2. **Add to Bundle:**
   - Each generated message is added to a bundle (a collection of messages).
   - The bundle is checked against two conditions:
     - **Bundle Size:** The maximum number of messages that can be included in a bundle.
     - **Bundle Timeout:** The maximum time allowed before the bundle must be sent, regardless of its size.

3. **Bundle Full or Timeout Reached:**
   - If the bundle size reaches the configured limit or the timeout is reached, the bundle is processed:
     - **Dump to CSV:** The bundle is written to a CSV file for debugging purposes. The filename includes a timestamp to ensure uniqueness.
     - **Send to RabbitMQ:** The bundle is converted to a JSON string and sent to RabbitMQ as a single message.

4. **Clear Bundle:**
   - After sending, the bundle is cleared, and the process starts again with a new bundle.

### Flowchart of Bundling and Sending Process

```plaintext
+---------------------+
| Generate Messages   |
| - Random data       |
| - Compute CRC       |
+---------------------+
          |
          v
+---------------------+
| Add to Bundle       |
| - Check size        |
| - Check timeout     |
+---------------------+
          |
          v
+---------------------+       +---------------------+
| Bundle Full or      |  Yes  | Dump to CSV         |
| Timeout Reached?    +------>+ - Filename with     |
|                     |       |   timestamp         |
+---------------------+       +---------------------+
          |                       |
          | No                    v
          |            +---------------------+
          |            | Send to RabbitMQ    |
          |            | - Convert to JSON   |
          |            | - Publish to queue  |
          |            +---------------------+
          |                       |
          v                       v
+---------------------+       +---------------------+
| Continue Generating |       | Clear Bundle        |
| Messages            |       +---------------------+
+---------------------+                 |
          |                             v
          |                  +---------------------+
          |                  | Log Transaction     |
          |                  | Times               |
          |                  +---------------------+
          v                             |
+---------------------+                 v
| End                 |       +---------------------+
+---------------------+       |         End         |
                              +---------------------+
```

### Configurable Parts

1. **Bundle Size:**
   - The maximum number of messages that can be included in a bundle before it is sent to RabbitMQ.

2. **Bundle Timeout:**
   - The maximum time allowed before the bundle must be sent, regardless of its size.

3. **Local Dumping:**
   - Bundles are written to a CSV file for debugging purposes. The filename includes a timestamp to ensure uniqueness.

### Receiver Process

The receiver reads the messages from shared memory, validates them using the CRC checksum, and logs the results. This ensures that the messages have not been tampered with and maintains data integrity.

If you have any more questions or need further clarification, feel free to ask!

===================================================================================================================================================================
===========================================================================================================================================================================
===============================================================================================================================================================================
                Let see where are my configurable things 


                Sure! Here are the parts of the `sender.cpp` code where the configurable parameters for bundle size, bundle timeout, and local dumping with relevant file name for debugging are set and used:

### Configurable Parts in `sender.cpp`

1. **Bundle Size and Timeout Configuration:**

```cpp
int BUNDLE_SIZE = std::stoi(getConfigValue("BUNDLE_SIZE"));
int BUNDLE_TIMEOUT = std::stoi(getConfigValue("BUNDLE_TIMEOUT"));
```

These lines load the bundle size and bundle timeout values from the configuration file.

2. **Checking Bundle Size and Timeout:**

```cpp
// Check if bundle size or timeout condition is met
auto bundle_current_time = std::chrono::high_resolution_clock::now();
auto bundle_duration = std::chrono::duration_cast<std::chrono::seconds>(bundle_current_time - bundle_start_time).count();

if (bundle.size() >= BUNDLE_SIZE || bundle_duration >= BUNDLE_TIMEOUT) {
    // Dump the bundle to a CSV file for debugging
    std::string filename = "bundle_" + std::to_string(bundle_start_time.time_since_epoch().count()) + ".csv";
    dump_to_csv(bundle, filename);

    // Send the bundle to RabbitMQ
    send_to_rmq(bundle);

    // Clear the bundle and reset the timer
    bundle.clear();
    bundle_start_time = std::chrono::high_resolution_clock::now();
}
```

This block of code checks if the bundle size or timeout condition is met. If either condition is met, it dumps the bundle to a CSV file and sends it to RabbitMQ.

3. **Local Dumping with Relevant File Name:**

```cpp
void dump_to_csv(const std::vector<UserMessages_Data>& bundle, const std::string& filename) {
    std::ofstream file(filename);

    // Write CSV header
    file << "Transaction Type,Creation Time,Mobile Number,IP Address,Personal Info,Checksum\n";

    // Write CSV rows
    for (const auto& msg : bundle) {
        file << msg.transaction_type << ","
             << msg.creation_time << ","
             << msg.mobile_number << ","
             << msg.ip_address.c_str() << ","
             << "{";
        for (const auto& entry : msg.personal_identifier_info) {
            file << entry.first.c_str() << ": " << entry.second.c_str() << ", ";
        }
        file << "},"
             << msg.checksum << "\n";
    }

    file.close();
}
```

The `dump_to_csv` function writes the bundled messages to a CSV file. The filename includes a timestamp to ensure uniqueness.

These are the key parts of the code where the configurable parameters for bundle size, bundle timeout, and local dumping are set and used. If you have any more questions or need further clarification, feel free to ask!