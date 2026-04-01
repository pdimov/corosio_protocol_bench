# corosio_protocol_bench

This repository contains a benchmark that sends a large C++ data structure
(`std::vector<element>`, where the vector size is sufficiently big,)
over TCP/IP using asynchronous I/O.
([Boost.Corosio](https://develop.corosio.cpp.al/corosio/index.html) is used as the asynchronous I/O library.)

The C++ structure is serialized into bytes by using a custom binary protocol. This is intended to model real world applications.

The protocol serializes fundamental C++ types (characters, integers, floating point) as a sequence of their underlying storage bytes.
(This doesn't take into account endianness and other factors such as `sizeof(size_t)` differences, but is good enough for both this benchmark and many practical uses.)

Sequences such as `std::vector` and `std::string` are serialized by first serializing `size()`, then each of their elements, in order.

Structures are serialized by serializing each member, in order.

The `element` type used in the benchmark is defined in `element.hpp` as:

```
struct point
{
    float x, y, z;
};

struct element
{
    int index_;
    std::string key_;
    std::vector<point> value_;
};
```

The protocol is implemented in `proto_write.hpp` (serialization) and `proto_read.hpp` (deserialization). The implementations
are generic with respect to the structure types; reflection (emulated via [Boost.Describe](https://boost.org/libs/describe)) is used to enumerate the members
and to implement (de)serialization.

In order for the (de)serialization code to be independent of the underlying I/O mechanism, the functions take a `Source` (for
reading) and a `Sink` (for writing). These source/sink types have `read` and `write` functions that implement byte I/O and
return awaitable types (so that they can be `co_await`-ed.)

The `Source` concept can be specified in pseudocode as

```
struct Source
{
    __awaitable__ read( void* p, std::size_t n );
};
```

and the `Sink` concept is

```
struct Sink
{
    __awaitable__ write( void const* p, std::size_t n );
    __awaitable__ flush();
};
```

Three concrete source/sink implementations are tested: simple, buffered, and immediate.

The simple source/sink types just forward `read` and `write` to the underlying I/O library
(by using the
[`boost::capy::read`](https://develop.capy.cpp.al/capy/reference/boost/capy/read-0a.html)
and
[`boost::capy::write`](https://develop.capy.cpp.al/capy/reference/boost/capy/write.html)
algorithms, respectively.)

They are very easy to implement, but performance suffers because of the need to issue
many short read/write syscalls.

```
-- void bench(const std::vector<element> &) [Source = simple_socket_source, Sink = simple_socket_sink]:
  writer_task: n=100000 hash=1a7bc9acb714e4a0
  reader_task: n=100000 computed=1a7bc9acb714e4a0 received=1a7bc9acb714e4a0
-- 27989 ms
```

The buffered sink accumulates the written bytes into a local buffer, issuing a syscall
to flush it only when it becomes full; the buffered source similarly reads large chunks
from the input socket and then serves the short reads from the local buffer until it's
exhausted.

This results in dramatically fewer syscalls and in correspondingly dramatic performance
improvement.

```
-- void bench(const std::vector<element> &) [Source = buffered_socket_source, Sink = buffered_socket_sink]:
  writer_task: n=100000 hash=1a7bc9acb714e4a0
  reader_task: n=100000 computed=1a7bc9acb714e4a0 received=1a7bc9acb714e4a0
-- 436 ms
```

Finally, the immediate source/sink types improve upon the corresponding buffered variants
by employing _immediate completion_. When the bytes to be read are already available in the
local buffer (in the source case), or when the local buffer has enough space for the written bytes
(in the sink case), the `await_ready` function of the returned awaitable returns `true`, which
allows `co_await` to complete immediately, without suspending the caller coroutine.

This results in a noticeable gain over the buffered case.

```
-- void bench(const std::vector<element> &) [Source = immediate_socket_source, Sink = immediate_socket_sink]:
  writer_task: n=100000 hash=1a7bc9acb714e4a0
  reader_task: n=100000 computed=1a7bc9acb714e4a0 received=1a7bc9acb714e4a0
-- 309 ms
```

(The quoted times are for Clang on Windows. Other platforms differ substantially in the
absolute times, but the relative rankings of the three cases above remain.)
