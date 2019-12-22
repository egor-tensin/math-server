// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "error.hpp"

#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>

#include <functional>
#include <memory>
#include <string>

namespace math::client {
namespace transport {

class Error : public client::Error {
public:
    explicit Error(const std::string& msg)
        : client::Error{"transport error: " + msg}
    { }
};

}

class Transport {
public:
    virtual ~Transport() = default;

    using ProcessResult = std::function<void (const std::string&)>;

    virtual void send_query(const std::string&, const ProcessResult&) = 0;
};

using TransportPtr = std::unique_ptr<Transport>;

class NetworkTransport : public Transport {
public:
    static constexpr auto DEFAULT_PORT = "18000";

    NetworkTransport(const std::string& host, const std::string& port)
        : m_host{host}, m_port{port}
    { }

protected:
    const std::string m_host;
    const std::string m_port;
};

class BlockingNetworkTransport : public NetworkTransport {
public:
    BlockingNetworkTransport(const std::string &host, const std::string& port)
        : NetworkTransport{host, port}, m_socket{m_io_context} {
        try {
            connect();
        } catch (const boost::system::system_error& e) {
            throw transport::Error{e.what()};
        }
    }

    void send_query(const std::string& query, const ProcessResult& on_reply) override {
        std::string reply;
        try {
            reply = send_query(query);
        } catch (const boost::system::system_error& e) {
            throw transport::Error{e.what()};
        }
        on_reply(reply);
    }

private:
    void connect() {
        boost::asio::ip::tcp::resolver resolver{m_io_context};
        boost::asio::connect(m_socket, resolver.resolve(m_host, m_port));
    }

    std::string send_query(const std::string& query) {
        write(query);
        return read_line();
    }

    void write(std::string input) {
        input += '\n';
        boost::asio::write(m_socket, boost::asio::const_buffer{input.c_str(), input.size()});
    }

    std::string read_line() {
        const auto bytes = boost::asio::read_until(m_socket, m_buffer, "\r\n");
        const auto data = boost::asio::buffer_cast<const char*>(m_buffer.data());
        const std::string result{data, bytes - 2}; // Skip \r\n
        m_buffer.consume(bytes);
        return result;
    }

    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
};

inline TransportPtr make_blocking_network_transport(
    const std::string& host, const std::string& port) {

    return std::make_unique<BlockingNetworkTransport>(host, port);
}

}
