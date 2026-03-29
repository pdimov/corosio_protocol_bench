#ifndef IMMEDIATE_SOCKET_SINK_HPP_INCLUDED
#define IMMEDIATE_SOCKET_SINK_HPP_INCLUDED

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

class immediate_socket_sink
{
private:

    socket_type sock_;

    static constexpr int N = 1024;

    unsigned char buffer_[ N ];
    std::size_t m_ = 0;

private:

    struct write_awaitable
    {
        immediate_socket_sink* this_;

        void const* p_;
        std::size_t n_;

        task_type inner_;

        bool await_ready() const noexcept
        {
            if( this_->m_ + n_ <= N )
            {
                std::memcpy( this_->buffer_ + this_->m_, p_, n_ );
                this_->m_ += n_;
                return true;
            }

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

    task_type write_impl( void const* p, std::size_t n )
    {
        using boost::capy::const_buffer;

        const_buffer seq[] = { const_buffer( buffer_, m_ ), const_buffer( p, n ) };

        auto [ec, q] = co_await boost::capy::write( sock_, seq );

        m_ = 0;

        if( ec ) throw std::system_error( ec, "immediate_socket_sink::write" );
    }

public:

    explicit immediate_socket_sink( socket_type&& sock ): sock_( std::move(sock) )
    {
    }

    ~immediate_socket_sink()
    {
        sock_.shutdown( socket_type::shutdown_send );
    }

    immediate_socket_sink( immediate_socket_sink&& rhs ) = default;

    auto write( void const* p, std::size_t n )
    {
        return write_awaitable{ this, p, n, write_impl( p, n ) };
    }

    task_type flush()
    {
        using boost::capy::const_buffer;

        auto [ec, q] = co_await boost::capy::write( sock_, const_buffer( buffer_, m_ ) );

        m_ = 0;

        if( ec ) throw std::system_error( ec, "immediate_socket_sink::flush" );
    }
};

#endif // #ifndef IMMEDIATE_SOCKET_SINK_HPP_INCLUDED
