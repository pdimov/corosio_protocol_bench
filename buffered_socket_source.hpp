#ifndef BUFFERED_SOCKET_SOURCE_HPP_INCLUDED
#define BUFFERED_SOCKET_SOURCE_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "socket_type.hpp"
#include "task_type.hpp"
#include <boost/capy/read.hpp>
#include <boost/capy/buffers.hpp>
#include <system_error>
#include <cstddef>
#include <cstring>

class buffered_socket_source
{
private:

    socket_type sock_;

    static constexpr int N = 1024;

    unsigned char buffer_[ N ];
    std::size_t first_ = 0, last_ = 0;

public:

    explicit buffered_socket_source( socket_type&& sock ): sock_( std::move(sock) )
    {
    }

    ~buffered_socket_source()
    {
        sock_.shutdown( socket_type::shutdown_receive );
    }

    buffered_socket_source( buffered_socket_source&& rhs ) = default;

    task_type read( void* p, std::size_t n )
    {
        if( n == 0 ) co_return;

        if( first_ < last_ )
        {
            std::size_t m = std::min( n, last_ - first_ );

            std::memcpy( p, buffer_ + first_, m );

            first_ += m;
            n -= m;

            p = static_cast<unsigned char*>( p ) + m;
        }

        if( n == 0 ) co_return;

        using boost::capy::mutable_buffer;

        mutable_buffer seq[] = { mutable_buffer( p, n ), mutable_buffer( buffer_, N ) };

        auto [ec, q] = co_await boost::capy::read( sock_, seq );

        if( q < n ) // unexpected eof
        {
            first_ = 0;
            last_ = 0;

            throw std::system_error( ec, "buffered_socket_source::read" );
        }

        q -= n;

        first_ = 0;
        last_ = q;

        if( ec && ec != boost::capy::cond::eof ) throw std::system_error( ec, "buffered_socket_source::read" );
    }
};

#endif // #ifndef BUFFERED_SOCKET_SOURCE_HPP_INCLUDED
