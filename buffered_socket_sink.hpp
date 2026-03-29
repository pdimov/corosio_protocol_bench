#ifndef BUFFERED_SOCKET_SINK_HPP_INCLUDED
#define BUFFERED_SOCKET_SINK_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "socket_type.hpp"
#include "task_type.hpp"
#include <boost/capy/write.hpp>
#include <boost/capy/buffers.hpp>
#include <system_error>
#include <cstddef>
#include <cstring>

class buffered_socket_sink
{
private:

    socket_type sock_;

    static constexpr int N = 1024;

    unsigned char buffer_[ N ];
    std::size_t m_ = 0;

public:

    explicit buffered_socket_sink( socket_type&& sock ): sock_( std::move(sock) )
    {
    }

    ~buffered_socket_sink()
    {
        sock_.shutdown( socket_type::shutdown_send );
    }

    buffered_socket_sink( buffered_socket_sink&& rhs ) = default;

    task_type write( void const* p, std::size_t n )
    {
        if( m_ + n <= N )
        {
            std::memcpy( buffer_ + m_, p, n );
            m_ += n;
            co_return;
        }

        using boost::capy::const_buffer;

        const_buffer seq[] = { const_buffer( buffer_, m_ ), const_buffer( p, n ) };

        auto [ec, q] = co_await boost::capy::write( sock_, seq );

        m_ = 0;

        if( ec ) throw std::system_error( ec, "buffered_socket_sink::write" );
    }

    task_type flush()
    {
        using boost::capy::const_buffer;

        auto [ec, q] = co_await boost::capy::write( sock_, const_buffer( buffer_, m_ ) );

        m_ = 0;

        if( ec ) throw std::system_error( ec, "buffered_socket_sink::flush" );
    }
};

#endif // #ifndef BUFFERED_SOCKET_SINK_HPP_INCLUDED
