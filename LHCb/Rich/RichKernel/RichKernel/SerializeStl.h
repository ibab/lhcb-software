// $Id: SerializeStl.h,v 1.3 2003-06-30 14:17:04 jonrob Exp $
#ifndef RICHKERNEL_SERIALIZESTL_H 
#define RICHKERNEL_SERIALIZESTL_H 1

#include <map>
#include "Kernel/SerializeStl.h"

// Add methods for std::map

/// Output ostream a std::map
template <class T1, class T2> inline
std::ostream& operator << ( std::ostream& s, const std::map<T1,T2>& m )
{
  for ( typename std::map<T1,T2>::const_iterator i = m.begin(); 
        i != m.end(); 
        i++ ) {
    s << "[" << i->first << ", " << i->second << "]";
  }
  return s;
}

/// Output serialize an std::map
template <class T1, class T2> inline
StreamBuffer& operator << ( StreamBuffer& s, const std::map<T1,T2>& m ) 
{
  s << m.size();
  for ( typename std::map<T1,T2>::const_iterator i = m.begin(); 
        i != m.end(); 
        i++ ) {
    s << i->first << i->second;
  }
  return s;
}

/// Input serialize an std::map
template <class T1, class T2> inline
StreamBuffer& operator >> ( StreamBuffer& s, std::map<T1,T2>& m ) 
{
  long length;
  s >> length;
  for ( int i = 0; i < length; i++ ) {
    T1 key; s >> key;
    T2 value; s >> value;
    m[key] = value;
  }
  return s;
}

#endif // RICHKERNEL_SERIALIZESTL_H 

