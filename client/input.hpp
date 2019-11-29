#pragma once

#include "error.hpp"

#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace math::client::input {

class Error : public client::Error {
public:
    explicit Error(const std::string& what)
        : client::Error{"input error: " + what} {
    }
};

class Reader {
public:
    using InputHandler = std::function<bool (const std::string&)>;

    virtual ~Reader() = default;

    virtual bool for_each_input(const InputHandler& process) const = 0;
};

using ReaderPtr = std::unique_ptr<input::Reader>;

class FileReader : public Reader {
public:
    explicit FileReader(const std::string& path)
        : m_path{path}
    { }

    bool for_each_input(const InputHandler& process) const override {
        return enum_lines(process);
    }

private:
    bool enum_lines(const InputHandler& process) const {
        std::ifstream ifs;
        ifs.exceptions(std::ifstream::badbit);

        try {
            ifs.open(m_path);
            if (!ifs.is_open()) {
                throw Error{"couldn't open file: " + m_path};
            }

            for (std::string line; std::getline(ifs, line);) {
                if (!process(line)) {
                    return false;
                }
            }
        } catch (const std::exception& e) {
            throw Error{e.what()};
        }

        return true;
    }

    const std::string m_path;
};

class MultiFileReader : public Reader {
public:
    explicit MultiFileReader(const std::vector<std::string>& paths)
        : m_paths{paths}
    { }

    bool for_each_input(const InputHandler& process) const override {
        for (const auto& path : m_paths) {
            const FileReader reader{path};
            if (!reader.for_each_input(process)) {
                return false;
            }
        }
        return true;
    }

private:
    const std::vector<std::string> m_paths;
};

inline input::ReaderPtr make_file_reader(const std::string& path) {
    return std::make_unique<input::FileReader>(path);
}

inline input::ReaderPtr make_file_reader(const std::vector<std::string>& paths) {
    return std::make_unique<input::MultiFileReader>(paths);
}

class StringReader : public Reader {
public:
    explicit StringReader(const std::string& input)
        : m_input{input}
    { }

    bool for_each_input(const InputHandler& process) const override {
        return process(m_input);
    }

private:
    const std::string m_input;
};

inline input::ReaderPtr make_string_reader(const std::string& input) {
    return std::make_unique<input::StringReader>(input);
}

class ConsoleReader : public Reader {
public:
    ConsoleReader() = default;

    bool for_each_input(const InputHandler& process) const override {
        std::string line;
        while (read_line(line)) {
            if (!process(line)) {
                return false;
            }
        }
        return true;
    }

private:
    static bool read_line(std::string& dest) {
        return static_cast<bool>(std::getline(std::cin, dest));
    }
};

inline input::ReaderPtr make_console_reader() {
    return std::make_unique<input::ConsoleReader>();
}

}
