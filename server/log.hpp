#pragma once

#include <boost/format.hpp>
#include <boost/system/error_code.hpp>

#include <ctime>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace math::server::log {

namespace details {

inline std::thread::id get_tid() { return std::this_thread::get_id(); }

inline std::string get_timestamp() {
    const auto tt = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&tt), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

inline void log(const std::string& msg) {
    std::clog << get_timestamp() << " | " << get_tid() << " | " << msg << '\n';
}

}

template <typename... Args>
inline void log(const std::string_view& fmt, Args&&... args) {
    details::log(boost::str((boost::format(fmt.data()) % ... % args)));
}

template <typename... Args>
inline void error(const std::string_view& fmt, Args&&... args) {
    details::log(boost::str((boost::format(fmt.data()) % ... % args)));
}

inline void error(const boost::system::error_code& ec) {
    details::log(ec.message());
}

}
