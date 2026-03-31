#ifndef IMMEDIATE_SOCKET_SOURCE_HPP_INCLUDED
#define IMMEDIATE_SOCKET_SOURCE_HPP_INCLUDED

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
#include <cstdio>

class immediate_socket_source
{
private:

    socket_type sock_;

    static constexpr int N = 1024;

    unsigned char buffer_[ N ];
    std::size_t first_ = 0, last_ = 0;

private:

    struct read_awaitable
    {
        immediate_socket_source* this_;

        void* p_;
        std::size_t n_;

        task_type inner_;

        bool await_ready() noexcept
        {
            if( n_ == 0 ) return true;

            if( this_->first_ < this_->last_ )
            {
                std::size_t m = std::min( n_, this_->last_ - this_->first_ );

                std::memcpy( p_, this_->buffer_ + this_->first_, m );

                this_->first_ += m;
                n_ -= m;

                p_ = static_cast<unsigned char*>( p_ ) + m;
            }

            if( n_ == 0 ) return true;

            inner_ = this_->read_impl( p_, n_ );

            return inner_.await_ready();
        }

        std::coroutine_handle<> await_suspend( std::coroutine_handle<> h, boost::capy::io_env const* env ) noexcept
        {
            return inner_.await_suspend( h, env );
        }

        void await_resume() noexcept
        {
            return inner_.await_resume();
        }
    };

    task_type read_impl( void* p, std::size_t n )
    {
        using boost::capy::mutable_buffer;

        mutable_buffer seq[] = { mutable_buffer( p, n ), mutable_buffer( buffer_, N ) };

        auto [ec, q] = co_await boost::capy::read( sock_, seq );

        if( q < n ) // unexpected eof
        {
            first_ = 0;
            last_ = 0;

            std::printf( "immediate_socket_source::read: unexpected eof, n=%zu q=%zu\n", n, q );

            throw std::system_error( ec, "immediate_socket_source::read" );
        }

        q -= n;

        first_ = 0;
        last_ = q;

        if( ec && ec != boost::capy::cond::eof )
        {
            std::printf( "immediate_socket_source::read: read error, ec=%s:%d\n", ec.category().name(), ec.value() );

            throw std::system_error( ec, "immediate_socket_source::read" );
        }
    }

    task_type noop()
    {
        co_return;
    }

public:

    explicit immediate_socket_source( socket_type&& sock ): sock_( std::move(sock) )
    {
    }

    ~immediate_socket_source()
    {
        sock_.shutdown( socket_type::shutdown_receive );
    }

    immediate_socket_source( immediate_socket_source&& rhs ) = default;

    auto read( void* p, std::size_t n )
    {
        return read_awaitable{ this, p, n, noop() };
    }
};

#endif // #ifndef IMMEDIATE_SOCKET_SOURCE_HPP_INCLUDED
