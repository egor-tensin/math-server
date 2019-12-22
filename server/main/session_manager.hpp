// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include <boost/asio.hpp>

#include <mutex>
#include <memory>
#include <unordered_set>

namespace math::server {

class Session;
using SessionPtr = std::shared_ptr<Session>;

class SessionManager {
public:
    SessionManager() = default;

    SessionPtr make_session(boost::asio::io_context&);

    void start(const SessionPtr&);
    void stop(const SessionPtr&);

    void stop_all();

private:
    std::mutex m_mtx;
    std::unordered_set<SessionPtr> m_sessions;
};

}
