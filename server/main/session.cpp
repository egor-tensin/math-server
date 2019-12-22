// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#include "session.hpp"
#include "session_manager.hpp"

#include "../common/error.hpp"
#include "../common/log.hpp"
#include "../parser/parser.hpp"

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <cstddef>

#include <exception>
#include <string>
#include <utility>

namespace math::server {
namespace {

std::string reply_to_string(double result) {
    return boost::lexical_cast<std::string>(result);
}

std::string calc_reply(const std::string& input) {
    std::string reply;
    try {
        reply = reply_to_string(Parser{input}.exec());
    } catch (const std::exception& e) {
        reply = e.what();
    }
    return reply;
}

}

Session::Session(SessionManager& mgr, boost::asio::io_context& io_context)
    : m_session_mgr{mgr}, m_strand{io_context}, m_socket{io_context}
{ }

boost::asio::ip::tcp::socket& Session::socket() {
    return m_socket;
}

void Session::start() {
    read();
}

void Session::stop() {
    close();
}

void Session::close() {
    try {
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        m_socket.close();
    } catch (const boost::system::system_error& e) {
        throw Error{e.what()};
    }
}

void Session::read() {
    const auto self = shared_from_this();

    // Stop at LF
    boost::asio::async_read_until(m_socket, m_buffer, '\n', boost::asio::bind_executor(m_strand,
        [this, self] (const boost::system::error_code& ec, std::size_t bytes) {
            handle_read(ec, bytes);
        }));
}

void Session::handle_read(const boost::system::error_code& ec, std::size_t bytes) {
    if (ec) {
        log::error("%1%: %2%", __func__, ec.message());
        m_session_mgr.stop(shared_from_this());
        return;
    }

    write(calc_reply(consume_input(bytes)));
}

std::string Session::consume_input(std::size_t bytes) {
    const auto data = boost::asio::buffer_cast<const char*>(m_buffer.data());
    const std::string input{data, bytes - 1};
    m_buffer.consume(bytes);
    return input;
}

void Session::write(const std::string& output) {
    const auto self = shared_from_this();

    std::ostream os(&m_buffer);
    // Include CR (so that Windows' telnet client works)
    os << output << "\r\n";

    boost::asio::async_write(m_socket, m_buffer, boost::asio::bind_executor(m_strand,
        [this, self] (const boost::system::error_code& ec, std::size_t bytes) {
            handle_write(ec, bytes);
        }));
}

void Session::handle_write(const boost::system::error_code& ec, std::size_t bytes) {
    if (ec) {
        log::error("%1%: %2%", __func__, ec.message());
        m_session_mgr.stop(shared_from_this());
        return;
    }

    read();
}

}
