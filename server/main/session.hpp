#pragma once

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <cstddef>

#include <memory>
#include <string>

namespace math::server {

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(SessionManager& mgr, boost::asio::io_context& io_context);

    boost::asio::ip::tcp::socket& socket();

    void start();
    void stop();

private:
    void close();

    void read();
    void write(const std::string&);

    void handle_read(const boost::system::error_code&, std::size_t);
    void handle_write(const boost::system::error_code&, std::size_t);

    std::string consume_input(std::size_t);

    SessionManager& m_session_mgr;

    boost::asio::io_context::strand m_strand;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
};

}
