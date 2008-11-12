#ifndef STRINGKEY_H
#define STRINGKEY_H 1
#include <cassert>
#include <boost/operators.hpp>
#include "GaudiKernel/Property.h"

class stringKey : public boost::totally_ordered<stringKey> {

public:

    stringKey(const std::string& str) : m_str(str), m_hash( hash(str) ) 
    { m_str.declareUpdateHandler( &stringKey::updateHandler,this); }
    stringKey(const stringKey& key) : m_str(key.m_str), m_hash(key.m_hash) 
    { m_str.declareUpdateHandler( &stringKey::updateHandler,this); }
    stringKey& operator=(const stringKey& rhs) {
        m_str=rhs.m_str;
        m_hash=rhs.m_hash;
        m_str.declareUpdateHandler( &stringKey::updateHandler,this);
        return *this;
    }

    const std::string& str() const { return m_str.value();}
    bool operator==(const stringKey& rhs) const ;
    bool operator<(const stringKey& rhs) const ;
    std::ostream& print(std::ostream& o ) const ;

    StringProperty& property() { return m_str; }
    void updateHandler(Property& prop);

    // transitional functionality -- to be deleted when no longer needed...
    stringKey() : m_hash( 0 )
    { m_str.declareUpdateHandler( &stringKey::updateHandler,this); }
    bool empty() const { return str().empty(); }

private:
    bool valid() const { return m_hash!=0 && !str().empty() && hash(str())==m_hash; }
    static size_t hash(const std::string& s);


    StringProperty m_str;
    // note: NEVER use m_hash for anything stored in files, as it is NOT 
    //       guaranteed that the hashing scheme will remain the same 
    //       during the lifetime of LHCb.
    // note: the only reason m_hash exists is to increase the speed
    //       of searching for keys in (sorted) lists,maps,...
    // note: hence you do NOT get access to it... period.
    size_t m_hash;
};

inline std::ostream& operator<< (std::ostream& os, const stringKey& k) 
{ return k.print(os); }


inline bool stringKey::operator==(const stringKey& rhs) const 
{ assert(valid()); return m_hash==rhs.m_hash && str() == rhs.str();}

inline bool stringKey::operator<(const stringKey& rhs) const 
{ assert(valid());
  return ( m_hash == rhs.m_hash ) ? ( str() < rhs.str() )
                                  : ( m_hash < rhs.m_hash ); 
}

inline std::ostream& stringKey::print(std::ostream & o ) const 
{ assert(valid()); 
 return  o << "stringKey(" << str() << ")";
}
/// add string and the key value 
inline std::string operator+
( const std::string& s , 
  const stringKey&   k ) { return s + k.str() ; }
/// add string and the key value 
inline std::string operator+
( const stringKey&   k , 
  const std::string& s ) { return s + k ; }

#endif
