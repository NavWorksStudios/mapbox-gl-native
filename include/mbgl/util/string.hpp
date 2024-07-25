#pragma once

#include <string>
#include <cstdlib>
#include <type_traits>

// Polyfill needed by Qt when building for Android with GCC
#if defined(__ANDROID__) && defined(__GLIBCXX__)

namespace std {

inline int stoi(const std::string &str)
{
    return atoi(str.c_str());
}

inline float stof(const std::string &str) {
    return static_cast<float>(atof(str.c_str()));
}

} // namespace std

#endif

namespace mbgl {
namespace util {

inline std::string toString(int8_t number) { return std::to_string(number); }
inline std::string toString(uint8_t number) { return std::to_string(number); }
inline std::string toString(int16_t number) { return std::to_string(number); }
inline std::string toString(uint16_t number) { return std::to_string(number); }
inline std::string toString(int32_t number) { return std::to_string(number); }
inline std::string toString(uint32_t number) { return std::to_string(number); }
inline std::string toString(int64_t number) { return std::to_string(number); }
inline std::string toString(uint64_t number) { return std::to_string(number); }

template <typename = std::enable_if<!std::is_same<int32_t, long>::value>>
inline std::string toString(long t) {
    return std::to_string(static_cast<int32_t>(t));
}

template <typename = std::enable_if<!std::is_same<int64_t, long long>::value>>
inline std::string toString(long long t) {
    return std::to_string(static_cast<int64_t>(t));
}

template <typename = std::enable_if<!std::is_same<uint32_t, unsigned long>::value>>
inline std::string toString(unsigned long t) {
    return std::to_string(static_cast<uint32_t>(t));
}

template <typename = std::enable_if<!std::is_same<uint64_t, unsigned long long>::value>>
inline std::string toString(unsigned long long t) {
    return std::to_string(static_cast<uint64_t>(t));
}

inline std::string toString(float number) { return std::to_string(number); }
inline std::string toString(double number) { return std::to_string(number); }

std::string toString(const std::exception_ptr &);

template <class T>
std::string toString(T) = delete;

std::string toHex(uint32_t);
std::string toHex(uint64_t);

inline float stof(const std::string& str) {
    return std::stof(str);
}

} // namespace util
} // namespace mbgl

// Android's libstdc++ doesn't have std::to_string()
#if defined(__ANDROID__) && defined(__GLIBCXX__)

namespace std {

template <typename T>
inline std::string to_string(T value) {
    return mbgl::util::toString(value);
}

} // namespace std

#endif
