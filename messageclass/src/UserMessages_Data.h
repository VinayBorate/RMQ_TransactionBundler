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