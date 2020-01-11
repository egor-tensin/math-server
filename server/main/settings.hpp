// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <cstddef>

#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace math::server {

struct Settings {
    static constexpr unsigned short DEFAULT_PORT = 18000;

    static std::size_t default_threads() { return std::thread::hardware_concurrency(); }

    unsigned short m_port;
    std::size_t m_threads;

    bool exit_with_usage() const { return m_vm.count("help"); }

    boost::program_options::variables_map m_vm;
};

class SettingsParser {
public:
    explicit SettingsParser(const std::string& argv0)
        : m_prog_name{extract_filename(argv0)}
    {
        m_visible.add_options()("help,h", "show this message and exit");
        m_visible.add_options()(
            "port,p",
            boost::program_options::value(&m_settings.m_port)->default_value(Settings::DEFAULT_PORT),
            "server port number");
        m_visible.add_options()(
            "threads,n",
            boost::program_options::value(&m_settings.m_threads)->default_value(Settings::default_threads()),
            "number of threads");
    }

    static const char* get_short_description() {
        return "[-h|--help] [-p|--port] [-n|--threads]";
    }

    Settings parse(int argc, char* argv[]) {
        boost::program_options::store(
            boost::program_options::command_line_parser{argc, argv}
                .options(m_visible)
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

    boost::program_options::options_description m_visible;

    Settings m_settings;

    friend std::ostream& operator<<(std::ostream& os, const SettingsParser& parser) {
        os << "usage: " << parser.m_prog_name << ' ' << get_short_description() << '\n';
        os << parser.m_visible;
        return os;
    }
};

}
