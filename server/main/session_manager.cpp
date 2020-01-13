// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#include "session_manager.hpp"

#include "../common/log.hpp"
#include "session.hpp"

#include <memory>
#include <mutex>

namespace math::server {

SessionPtr SessionManager::make_session(boost::asio::io_context& io_context) {
    return std::make_shared<Session>(*this, io_context);
}

void SessionManager::start(const SessionPtr& session) {
    std::lock_guard<std::mutex> lck{m_mtx};
    m_sessions.emplace(session);
    session->start();
}

void SessionManager::stop(const SessionPtr& session) {
    std::lock_guard<std::mutex> lck{m_mtx};
    const auto removed = m_sessions.erase(session) > 0;
    if (removed) {
        session->stop();
    }
}

void SessionManager::stop_all() {
    std::lock_guard<std::mutex> lck{m_mtx};
    log::log("Closing the remaining %1% session(s)...", m_sessions.size());
    for (const auto& session : m_sessions) {
        session->stop();
    }
    m_sessions.clear();
}

} // namespace math::server
