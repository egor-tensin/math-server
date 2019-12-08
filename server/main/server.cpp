#include "server.hpp"
#include "session.hpp"
#include "session_manager.hpp"
#include "settings.hpp"

#include "../common/error.hpp"
#include "../common/log.hpp"

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <cstddef>

#include <exception>
#include <thread>
#include <vector>

namespace math::server {
namespace {

boost::asio::ip::tcp::endpoint make_endpoint(unsigned short port) {
    return {boost::asio::ip::tcp::v4(), port};
}

void configure_acceptor(boost::asio::ip::tcp::acceptor& acceptor, unsigned short port) {
    try {
        const auto endpoint = make_endpoint(port);
        acceptor.open(endpoint.protocol());
        acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();
    } catch (const boost::system::system_error& e) {
        throw Error{e.what()};
    }
}

}

Server::Server(const Settings& settings)
    : Server{settings.m_port, settings.m_threads}
{ }

Server::Server(unsigned short port, std::size_t threads)
    : m_numof_threads{threads}
    , m_signals{m_io_context}
    , m_acceptor{m_io_context} {

    wait_for_signal();
    configure_acceptor(m_acceptor, port);

    accept();
}

void Server::run() {
    std::vector<std::thread> threads{m_numof_threads};
    for (std::size_t i = 0; i < m_numof_threads; ++i) {
        threads[i] = std::thread{[this] () { m_io_context.run(); }};
    }

    for (std::size_t i = 0; i < m_numof_threads; ++i) {
        threads[i].join();
    }
}

void Server::wait_for_signal() {
    try {
        m_signals.add(SIGINT);
        m_signals.add(SIGTERM);

        m_signals.async_wait([this] (const boost::system::error_code& ec, int signo) {
            handle_signal(ec, signo);
        });
    } catch (const boost::system::system_error& e) {
        throw Error{e.what()};
    }
}

void Server::handle_signal(const boost::system::error_code& ec, int signo) {
    if (ec) {
        log::error("%1%: %2%", __func__, ec.message());
    }

    log::log("Caught signal %1%", signo);

    try {
        m_acceptor.close();
        m_session_mgr.stop_all();
    } catch (const std::exception& e) {
        log::error(e.what());
    }
}

void Server::accept() {
    const auto session = m_session_mgr.make_session(m_io_context);
    m_acceptor.async_accept(session->socket(),
        [session, this] (const boost::system::error_code& ec) {
            handle_accept(session, ec);
        });
}

void Server::handle_accept(SessionPtr session, const boost::system::error_code& ec) {
    if (ec) {
        log::error("%1%: %2%", __func__, ec.message());
        return;
    }

    m_session_mgr.start(session);
    accept();
}

}
