// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "transport.hpp"

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace math::client {

struct Settings {
    std::string m_input;
    std::string m_host;
    std::string m_port;
    std::vector<std::string> m_files;

    bool exit_with_usage() const { return m_vm.count("help"); }

    bool input_from_string() const {
        return m_vm.count("command");
    }

    bool input_from_files() const {
        return !input_from_string() && !m_files.empty();
    }

    bool input_from_console() const {
        return !input_from_string() && !input_from_files();
    }

    boost::program_options::variables_map m_vm;
};

class SettingsParser {
public:
    explicit SettingsParser(const std::string& argv0)
        : m_prog_name{extract_filename(argv0)}
    {
        m_visible.add_options()("help,h", "show this message and exit");
        m_visible.add_options()(
            "command,c",
            boost::program_options::value(&m_settings.m_input),
            "evaluate the argument expression and exit");
        m_visible.add_options()(
            "host,H",
            boost::program_options::value(&m_settings.m_host)->default_value("localhost"),
            "server host address");
        m_visible.add_options()(
            "port,p",
            boost::program_options::value(&m_settings.m_port)->default_value(NetworkTransport::DEFAULT_PORT),
            "server port number");
        m_hidden.add_options()(
            "files",
            boost::program_options::value<std::vector<std::string>>(&m_settings.m_files),
            "shouldn't be visible");
        m_positional.add("files", -1);
    }

    static const char* get_short_description() {
        return "[-h|--help] [-c|--command arg] [-H|--host] [-p|--port] [file...]";
    }

    Settings parse(int argc, char* argv[]) {
        boost::program_options::options_description all;
        all.add(m_hidden).add(m_visible);
        boost::program_options::store(
            boost::program_options::command_line_parser{argc, argv}
                .options(all)
                .positional(m_positional)
                .run(),
            m_settings.m_vm);
        if (m_settings.exit_with_usage()) {
            return m_settings;
        }
        boost::program_options::notify(m_settings.m_vm);
        return m_settings;
    }

    void usage() const {
        std::cout << *this;
    }

    void usage_error(const std::exception& e) const {
        std::cerr << "usage error: " << e.what() << '\n';
        std::cerr << *this;
    }

private:
    static std::string extract_filename(const std::string& path) {
        return boost::filesystem::path{path}.filename().string();
    }

    const std::string m_prog_name;

    boost::program_options::options_description m_hidden;
    boost::program_options::options_description m_visible;
    boost::program_options::positional_options_description m_positional;

    Settings m_settings;

    friend std::ostream& operator<<(std::ostream& os, const SettingsParser& parser) {
        os << "usage: " << parser.m_prog_name << ' ' << get_short_description() << '\n';
        os << parser.m_visible;
        return os;
    }
};

}
