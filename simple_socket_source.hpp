#ifndef SIMPLE_SOCKET_SOURCE_HPP_INCLUDED
#define SIMPLE_SOCKET_SOURCE_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/corosio/tcp_socket.hpp>
#include <boost/capy/task.hpp>
#include <boost/capy/read.hpp>
#include <boost/capy/buffers.hpp>
#include <system_error>
#include <cstddef>

class simple_socket_source
{
private:

    boost::corosio::tcp_socket sock_;

public:

    explicit simple_socket_source( boost::corosio::tcp_socket&& sock ): sock_( std::move(sock) )
    {
    }

    boost::capy::task<void> read( void* p, std::size_t n )
    {
        auto [ec, m] = co_await boost::capy::read( sock_, boost::capy::mutable_buffer( p, n ) );
        if( ec ) throw std::system_error( ec, "simple_socket_source::read" );
    }

    void shutdown()
    {
        sock_.shutdown( boost::corosio::tcp_socket::shutdown_receive );
    }
};

#endif // #ifndef SIMPLE_SOCKET_SOURCE_HPP_INCLUDED
