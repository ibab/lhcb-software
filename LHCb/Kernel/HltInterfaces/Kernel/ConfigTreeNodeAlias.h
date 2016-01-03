#ifndef CONFIGTREENODEALIAS_H
#define CONFIGTREENODEALIAS_H 1

#include "Kernel/ConfigTreeNode.h"
#include <string>
#include <iostream>
#include "boost/format.hpp"

class ConfigTreeNodeAlias final {
public:
    typedef ConfigTreeNode::digest_type  digest_type;

    class alias_type final {
    public:
        //@TODO: add/enforce rules: TCK/xxx, TOPLEVEL/xxx, TAG/xxx
       alias_type() = default;
       alias_type(std::string s) : m_alias(std::move(s)) { }
       const std::string& str() const { return m_alias; }
       alias_type  operator/ (const std::string& x) const { return alias_type( *this ) /= x ; }
       alias_type& operator/=(const std::string& x) { m_alias += "/" ; m_alias+=x; return *this; }
       alias_type& operator=(const std::string& x)  { m_alias = x ; return *this; }
       std::string major() const; // the part upto (not including) the first '/'
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
     
    ConfigTreeNodeAlias() = default;

    // protect top of alias namespace...
    static ConfigTreeNodeAlias createTopLevel(const std::string& release, const std::string& runType, const ConfigTreeNode& top) 
    { 
        if (runType.empty()) return ConfigTreeNodeAlias(); 
        digest_type d=top.digest(); return ConfigTreeNodeAlias(d,alias_type("TOPLEVEL") / release / runType / d.str()); 
    }
    static ConfigTreeNodeAlias createTCK(const ConfigTreeNode& top, unsigned int tck) 
    { return ConfigTreeNodeAlias(top.digest(),alias_type("TCK") / boost::str(boost::format("0x%08x") %tck )); }
    static ConfigTreeNodeAlias createTag(const ConfigTreeNode& top, const std::string& tag) 
    { return ConfigTreeNodeAlias(top.digest(),alias_type("TAG") / tag); }

    std::ostream& print(std::ostream& os) const;
    std::istream& read(std::istream& is);

    bool operator!=(const ConfigTreeNodeAlias& rhs) const { return !operator==(rhs); }
    bool operator==(const ConfigTreeNodeAlias& rhs) const { return m_ref == rhs.m_ref && m_alias == rhs.m_alias; }

    const digest_type& ref()   const { return m_ref; }   // value
    const alias_type&  alias() const { return m_alias; } // key

private:

    ConfigTreeNodeAlias(const digest_type& ref, const alias_type& alias);
    void invalidate(const std::string& reason);

    digest_type     m_ref = digest_type::createInvalid();
    alias_type      m_alias;
};

inline std::ostream& operator<<(std::ostream& os, const ConfigTreeNodeAlias& x) { return x.print(os); }
inline std::istream& operator>>(std::istream& is, ConfigTreeNodeAlias& x) { return x.read(is); }
inline std::ostream& operator<<(std::ostream& os, const ConfigTreeNodeAlias::alias_type& x) { return x.print(os); }
inline std::istream& operator>>(std::istream& is, ConfigTreeNodeAlias::alias_type& x) { return x.read(is); }

#endif
