#ifndef CONFIGTREENODEALIAS_H
#define CONFIGTREENODEALIAS_H 1

#include "HltBase/ConfigTreeNode.h"
#include <string>
#include <iostream>
#include "boost/lexical_cast.hpp"



class ConfigTreeNodeAlias {
public:
    typedef ConfigTreeNode::digest_type  digest_type;

    class alias_type {
    public:
        //@TODO: add/enforce rules: TCK/xxx, TOPLEVEL/xxx, TAG/xxx
       alias_type() {}
       alias_type(const std::string& s) : m_alias(s) { }
       const std::string& str() const { return m_alias; }
       alias_type  operator/ (const std::string& x) { return alias_type( *this ) /= x ; }
       alias_type& operator/=(const std::string& x) { m_alias += "/" ; m_alias+=x; return *this; }
       alias_type& operator=(const std::string& x)  { m_alias = x ; return *this; }
       std::string major() const; // the part upto (not includng) the first '/'
       bool valid() const;
       bool invalid() const { return !valid(); }
       bool operator==(const alias_type& rhs) const { return m_alias == rhs.m_alias; }
       //@TODO: add hashing to speed up <...
       bool operator<(const alias_type& rhs) const { return m_alias < rhs.m_alias; } 
       std::ostream& print(std::ostream& os) const;
       std::istream& read(std::istream& is);
    private:
       std::string m_alias;
    };
     
    ConfigTreeNodeAlias() : m_ref(digest_type::createInvalid()) {}

    // protect top of alias namespace...
    static ConfigTreeNodeAlias createTopLevel(const ConfigTreeNode& top) 
    { digest_type d=top.digest(); return ConfigTreeNodeAlias(d,alias_type("TOPLEVEL") / d.str()); }
    static ConfigTreeNodeAlias createTCK(const ConfigTreeNode& top, unsigned int tck) 
    { return ConfigTreeNodeAlias(top.digest(),alias_type("TCK") / boost::lexical_cast<std::string>(tck)); }
    static ConfigTreeNodeAlias createTag(const ConfigTreeNode& top, const std::string& tag) 
    { return ConfigTreeNodeAlias(top.digest(),alias_type("TAG") / tag); }

    std::ostream& print(std::ostream& os) const;
    std::istream& read(std::istream& is);

    bool operator!=(const ConfigTreeNodeAlias& rhs) const { return !operator==(rhs); }
    bool operator==(const ConfigTreeNodeAlias& rhs) const { return m_ref == rhs.m_ref && m_alias == rhs.m_alias; }

    const digest_type& ref()   const { return m_ref; }   // value
    const alias_type&  alias() const { return m_alias; } // key

private:

    ConfigTreeNodeAlias(const digest_type& ref, const alias_type& alias)
      : m_ref(ref), m_alias(alias) { }

    digest_type     m_ref;
    alias_type      m_alias;
};

std::ostream& operator<<(std::ostream& os, const ConfigTreeNodeAlias& x);
std::istream& operator>>(std::istream& is, ConfigTreeNodeAlias& x);
std::ostream& operator<<(std::ostream& os, const ConfigTreeNodeAlias::alias_type& x);
std::istream& operator>>(std::ostream& os, ConfigTreeNodeAlias::alias_type& x);

#endif
