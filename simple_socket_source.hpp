#ifndef SIMPLE_SOCKET_SOURCE_HPP_INCLUDED
#define SIMPLE_SOCKET_SOURCE_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "socket_type.hpp"
#include "task_type.hpp"
#include <boost/capy/read.hpp>
#include <boost/capy/buffers.hpp>
#include <system_error>
#include <cstddef>

class simple_socket_source
{
private:

    socket_type sock_;

public:

    explicit simple_socket_source( socket_type&& sock ): sock_( std::move(sock) )
    {
    }

    task_type read( void* p, std::size_t n )
    {
        auto [ec, m] = co_await boost::capy::read( sock_, boost::capy::mutable_buffer( p, n ) );
        if( ec ) throw std::system_error( ec, "simple_socket_source::read" );
    }

    void shutdown()
    {
        sock_.shutdown( socket_type::shutdown_receive );
    }
};

#endif // #ifndef SIMPLE_SOCKET_SOURCE_HPP_INCLUDED
