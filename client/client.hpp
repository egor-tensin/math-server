#pragma once

#include "input.hpp"
#include "settings.hpp"
#include "transport.hpp"

#include <iostream>
#include <string>
#include <utility>

namespace math::client {

class Client {
public:
    explicit Client(const Settings& settings)
        : Client{make_input_reader(settings), make_transport(settings)}
    { }

    Client(input::ReaderPtr&& input_reader, TransportPtr&& transport)
        : m_input_reader{std::move(input_reader)}
        , m_transport{std::move(transport)}
    { }

    void run() {
        m_input_reader->for_each_input([this] (const std::string& input) {
            m_transport->send_query(input, [] (const std::string& reply) {
                std::cout << reply << '\n';
            });
            return true;
        });
    }

private:
    static input::ReaderPtr make_input_reader(const Settings& settings) {
        if (settings.input_from_string()) {
            return input::make_string_reader(settings.m_input);
        }
        if (settings.input_from_files()) {
            return input::make_file_reader(settings.m_files);
        }
        return input::make_console_reader();
    }

    static TransportPtr make_transport(const Settings& settings) {
        return make_blocking_network_transport(settings.m_host, settings.m_port);
    }

    const input::ReaderPtr m_input_reader;
    TransportPtr m_transport;
};

}
