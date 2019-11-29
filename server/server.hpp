#pragma once

#include "session_manager.hpp"
#include "settings.hpp"

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace math::server {

class Server {
public:
    Server(const Settings& settings);
    Server(unsigned port, unsigned threads);

    void run();

private:
    void wait_for_signal();
    void handle_signal(const boost::system::error_code&, int);

    void accept();
    void handle_accept(SessionPtr session, const boost::system::error_code& ec);

    const unsigned m_numof_threads;

    boost::asio::io_context m_io_context;
    boost::asio::signal_set m_signals;
    boost::asio::ip::tcp::acceptor m_acceptor;

    SessionManager m_session_mgr;
};

}
