#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/crc.hpp> // CRC header
#include "UserMessages_Data.h"
#include "../../common/include/logger.h"
#include "../../common/include/config.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <chrono>
#include <thread>
#include <vector>
#include <sstream>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <rabbitmq-c/framing.h>
#include "../../lib/nlohmann_Json_Header/nlohmann/json.hpp" // Include the JSON library
#include <fstream>

namespace bip = boost::interprocess;
using json = nlohmann::json; // Alias for the JSON library

// Compute CRC checksum
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

void send_to_rabbitmq(const std::vector<UserMessages_Data>& messages, const std::string& queue_name, const std::string& max_bundle_size_allowed) {
    // RabbitMQ connection setup
    amqp_connection_state_t conn = amqp_new_connection();
    amqp_socket_t *socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        std::cerr << "Creating TCP socket failed" << std::endl;
        spdlog::error("Creating TCP socket failed");
        return;
    }

    int status = amqp_socket_open(socket, "localhost", 5672);
    if (status) {
        std::cerr << "Opening TCP socket failed" << std::endl;
        spdlog::error("Opening TCP socket failed");
        return;
    }

    amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
    amqp_channel_open(conn, 1);
    amqp_get_rpc_reply(conn);

    // Declare the queue
    amqp_queue_declare(conn, 1, amqp_cstring_bytes(queue_name.c_str()), 0, 0, 0, 1, amqp_empty_table);
    amqp_get_rpc_reply(conn);

    std::string bundle_body = "Transaction Type,Creation Time,Mobile Number,IP Address,Personal Info\n";
    size_t bundle_size = bundle_body.size();
    const size_t max_bundle_size = static_cast<std::size_t>(std::stoull(max_bundle_size_allowed)); // 1 KB

    for (const auto& msg : messages) {
        json personal_info_json;
        for (const auto& entry : msg.personal_identifier_info) {
            personal_info_json[entry.first.c_str()] = entry.second.c_str();
        }
        std::string personal_info_str = personal_info_json.dump();

        std::string message_body = std::to_string(msg.transaction_type) + "," +
                                   std::to_string(msg.creation_time) + "," +
                                   std::to_string(msg.mobile_number) + "," +
                                   msg.ip_address.c_str() + "," +
                                   personal_info_str + "\n";

        bundle_body += message_body;
        bundle_size += message_body.size();

        // Print the bundle size after each transaction
        std::cout << message_body << "Bundle Size: " << bundle_size << " bytes" << std::endl;
        spdlog::info("{}Bundle Size: {} bytes", message_body, bundle_size);

        if (bundle_size >= max_bundle_size) {
            amqp_bytes_t message_bytes;
            message_bytes.len = bundle_body.size();
            message_bytes.bytes = (void*)bundle_body.c_str();

            // Log and print the bundle size before sending
            std::cout << "Sending bundle to RabbitMQ, size: " << bundle_size << " bytes" << std::endl;
            spdlog::info("Sending bundle to RabbitMQ, size: {} bytes", bundle_size);

            int publish_status = amqp_basic_publish(conn, 1, amqp_cstring_bytes(""), amqp_cstring_bytes(queue_name.c_str()),
                                                    0, 0, NULL, message_bytes);
            if (publish_status != AMQP_STATUS_OK) {
                std::cerr << "Failed to publish bundle" << std::endl;
                spdlog::error("Failed to publish bundle");
            } else {
                std::cout << "Bundle published successfully" << std::endl;
                spdlog::info("Bundle published successfully");
            }

            bundle_body = "Transaction Type,Creation Time,Mobile Number,IP Address,Personal Info\n";
            bundle_size = bundle_body.size();
        }
    }

    if (bundle_size > strlen("Transaction Type,Creation Time,Mobile Number,IP Address,Personal Info\n")) {
        amqp_bytes_t message_bytes;
        message_bytes.len = bundle_body.size();
        message_bytes.bytes = (void*)bundle_body.c_str();

        // Log and print the final bundle size before sending
        std::cout << "Sending final bundle to RabbitMQ, size: " << bundle_size << " bytes" << std::endl;
        spdlog::info("Sending final bundle to RabbitMQ, size: {} bytes", bundle_size);

        int publish_status = amqp_basic_publish(conn, 1, amqp_cstring_bytes(""), amqp_cstring_bytes(queue_name.c_str()),
                                                0, 0, NULL, message_bytes);
        if (publish_status != AMQP_STATUS_OK) {
            std::cerr << "Failed to publish final bundle" << std::endl;
            spdlog::error("Failed to publish final bundle");
        } else {
            std::cout << "Final bundle published successfully" << std::endl;
            spdlog::info("Final bundle published successfully");
        }
    }

    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
}

void read_and_log_messages(bip::managed_shared_memory& segment, std::vector<std::chrono::microseconds>& read_times, size_t& last_processed_index, std::vector<UserMessages_Data>& message_bundle) {
    typedef bip::allocator<UserMessages_Data, bip::managed_shared_memory::segment_manager> ShmemAllocator;
    typedef boost::container::vector<UserMessages_Data, ShmemAllocator> ShmemVector;

    std::pair<ShmemVector*, bip::managed_shared_memory::size_type> result = segment.find<ShmemVector>("Messages");
    ShmemVector* messages = result.first;

    if (!messages) {
        std::cerr << "No messages found in shared memory." << std::endl;
        spdlog::error("No messages found in shared memory.");
        return;
    }

    if (last_processed_index < messages->size()) {
        for (size_t i = last_processed_index; i < messages->size(); ++i) {
            const auto& msg = (*messages)[i];
            auto start_time = std::chrono::high_resolution_clock::now();

            std::uint32_t received_checksum = compute_crc(msg);
            if (received_checksum != msg.checksum) {
                std::cerr << "Checksum mismatch for message with Transaction Type: " << msg.transaction_type << std::endl;
                spdlog::error("Checksum mismatch for message with Transaction Type: {}", msg.transaction_type);
                continue;
            }

            message_bundle.push_back(msg);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            read_times.push_back(duration);
        }

        last_processed_index = messages->size();
    }
}

bool DEBUGGING_MODE = false;

void dump_to_csv(const std::vector<UserMessages_Data>& messages, const std::string& filename) {
    std::ofstream csv_file(filename, std::ios::app); // Open in append mode
    if (csv_file.tellp() == 0) {
        csv_file << "Transaction Type,Creation Time,Mobile Number,IP Address,Personal Info\n";
    }

    for (const auto& msg : messages) {
        json personal_info_json;
        for (const auto& entry : msg.personal_identifier_info) {
            personal_info_json[entry.first.c_str()] = entry.second.c_str();
        }
        std::string personal_info_str = personal_info_json.dump();
        // Remove the comma between Email and Name
        size_t pos = personal_info_str.find("\",\"");
        if (pos != std::string::npos) {
            personal_info_str.replace(pos, 3, "\" \"");
        }

        csv_file << msg.transaction_type << ","
                 << msg.creation_time << ","
                 << msg.mobile_number << ","
                 << msg.ip_address.c_str() << ","
                 << "\"" << personal_info_str << "\"\n"; // Enclose personal_info_str in quotes

        // Log and print the data being written to the CSV file
        std::cout << "Writing to CSV: " << msg.transaction_type << ", "
                  << msg.creation_time << ", "
                  << msg.mobile_number << ", "
                  << msg.ip_address.c_str() << ", "
                  << personal_info_str << std::endl;
        spdlog::info("Writing to CSV: {}, {}, {}, {}, {}",
                     msg.transaction_type, msg.creation_time, msg.mobile_number, msg.ip_address.c_str(), personal_info_str);
    }

    csv_file.close();
}

int main() {
    loadConfig();
    LOGGER_LEVEL_SETTT = getConfigValue("LOGGER_LEVEL_SETTT");
    setup_logger("receiver_logger", "receiver_log.txt", LOGGER_LEVEL_SETTT);
    NAME_SHARED_MEMORY = getConfigValue("NAME_SHARED_MEMORY");
    const char* shared_memeory_name = NAME_SHARED_MEMORY.c_str();
    DEBUGGING_MODE = getConfigValue("DEBUGGING_MODE") == "true";
    std::string bundle_size = getConfigValue("BUNDLE_SIZE");
   
    try {
        bip::managed_shared_memory segment(bip::open_only, shared_memeory_name);

        std::vector<std::chrono::microseconds> read_times;
        size_t last_processed_index = 0;
        std::vector<UserMessages_Data> message_bundle;
        std::string filename = getConfigValue("DUMP_CSV_FILE_NAME");
        std::string queue_name = getConfigValue("NAME_RABBIT_MQ");
        while (true) {
            read_and_log_messages(segment, read_times, last_processed_index, message_bundle);

            if (message_bundle.size() > 0) {
                send_to_rabbitmq(message_bundle, queue_name, bundle_size);
                if (DEBUGGING_MODE) {
                    dump_to_csv(message_bundle, filename);
                }
                message_bundle.clear();
            }

            int time_log = std::stoi(getConfigValue("LOG_TIMER_RECEIVER"));
            std::this_thread::sleep_for(std::chrono::seconds(time_log));
            std::thread log_thread(log_read_times, std::ref(read_times));
            log_thread.join();

            std::cout << "Waiting for new data in shared memory..." << std::endl;
            spdlog::info("Waiting for new data in shared memory...");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        spdlog::error("Error: {}", e.what());
    }

    return 0;
}