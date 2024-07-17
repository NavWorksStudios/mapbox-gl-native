//
//  nav_stringid.hpp
//
//  Created by BernieZhao on 2024/7/17.
//

#pragma once

#include <string>

namespace nav {

class stringid : public std::string {
    mutable size_t _hash = 0;
    
public:
    stringid() = default;
    explicit stringid(const std::string& id) : std::string(id) { }
    
    size_t hash() const {
        return _hash ? _hash : _hash = std::hash<std::string>()(*this);
    }
    
    void operator = (const std::string& id) { (std::string&) *this = id; _hash = 0; }
    bool operator == (const stringid& other) const { return hash() == other.hash(); }
};

}

template <> struct std::hash<nav::stringid> {
    std::size_t operator()(const nav::stringid& id) const {
        return id.hash();
    }
};
