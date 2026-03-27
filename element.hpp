#ifndef ELEMENT_HPP_INCLUDED
#define ELEMENT_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/describe/class.hpp>
#include <string>
#include <vector>

struct element
{
    std::string key_;
    float value_;
    std::vector<unsigned char> padding_;
};

BOOST_DESCRIBE_STRUCT(element, (), (key_, value_, padding_))

#endif // #ifndef ELEMENT_HPP_INCLUDED
