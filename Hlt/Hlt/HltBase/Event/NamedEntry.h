#ifndef NAMEDENTRY_H
#define NAMEDENTRY_H

#include <iosfwd>
#include <string>
#include <utility>
#include "GaudiKernel/INamedInterface.h"

namespace Hlt {

class NamedEntry {
    // it is assumed that any INamedInterface instance outlives an 
    // object whose lifetime is at most an event...
    std::pair<const INamedInterface*,std::string> m_entry;
public:
    NamedEntry(const INamedInterface& arg)
        : m_entry( &arg, std::string() ) {}
    NamedEntry(std::string arg)  
        : m_entry( 0, arg )  { }
        // : m_entry{ nullptr, std::move(arg) } {} // wait unil gccxml is retired...
    const std::string& str() const {
        return m_entry.first ? m_entry.first->name() : m_entry.second ;
    }
    // could implement a dereference operator which returns str()???

    // forward various std::string functions, so this can be used as a drop in
    // replacement for a string...
    typedef std::string::size_type size_type;
    bool operator==(const std::string& rhs) const { return str()==rhs; }
    size_type size() const { return str().size(); }
    size_type length() const { return str().length(); }
    size_type find(const std::string& s, size_type pos = 0 ) const { return str().find(s,pos); }
    size_type find(const char* s, size_type pos = 0 ) const { return str().find(s,pos); }
    size_type find(const char* s, size_type pos, size_type count ) const { return str().find(s,pos,count); }
    size_type find(char ch, size_type pos = 0) const { return str().find(ch,pos); }
};

std::ostream& operator<<(std::ostream& os, const NamedEntry& e) ;

}


#endif
