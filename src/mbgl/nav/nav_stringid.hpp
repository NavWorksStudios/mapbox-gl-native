//
//  nav_stringid.hpp
//
//  Created by BernieZhao on 2024/7/17.
//

#pragma once

#include <string>

namespace nav {

class stringid {
    std::string _id;
    size_t _hash;
    
public:
    stringid() { }
    stringid(const std::string& id) : _id(id), _hash(std::hash<std::string>()(_id)) { }

    inline size_t hash() const { return _hash; }
    inline const std::string& get() const { return _id; }
    inline operator const std::string& () const { return _id; }
    
    bool operator == (const std::string& id) const { return _id == id; }
    bool operator != (const std::string& id) const { return _id != id; }
    bool operator == (const stringid& other) const { return hash() == other.hash(); }
    bool operator != (const stringid& other) const { return hash() != other.hash(); }
    bool operator < (const stringid& other) const { return hash() < other.hash(); }
};

}

template <> struct std::hash<nav::stringid> {
    size_t operator () (const nav::stringid& id) const { return id.hash(); }
};
