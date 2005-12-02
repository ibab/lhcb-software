// $Id: SerializeStl.h,v 1.9 2005-12-02 16:28:07 cattanem Exp $
/*
    Small header file to serialize vectors, lists, pairs, maps
    to standard Gaudi StreamBuffer objects
*/
#ifndef LHCBKERNEL_SERIALIZESTL_H
#define LHCBKERNEL_SERIALIZESTL_H

#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
#include <vector>
#include <list>
#include <map>
#include <utility>

namespace LHCb 
{
static const int NUMBERS_PER_LINE = 6;

// Output operator (ASCII)
template <class T> inline
std::ostream& operator<< ( std::ostream& s, const std::vector<T>& v ) 
{
  int cnt = 0;
  for(typename std::vector<T>::const_iterator i=v.begin();
      i!=v.end();
      i++, cnt++) {
    s << "[" << cnt << "]="; 
    s.width(12); 
    s << (*i) << " ";
    if ((++cnt)%NUMBERS_PER_LINE == 0)  s << std::endl;
  }
  return s;
}

// Output operator (ASCII)
template <class T> inline
std::ostream& operator<< ( std::ostream& s, const std::list<T>& l ) 
{
  int cnt = 0;
  for (typename std::list<T>::const_iterator i = l.begin(); i != l.end(); i++) {
    s << "[" << cnt << "]="; 
    s.width(12); 
    s << (*i) << " ";
    if ( (++cnt)%NUMBERS_PER_LINE == 0 )  s << std::endl;
  }
  return s;
}
  
// Output serialize a pair of items
template <class T1, class T2> inline 
StreamBuffer& operator << (StreamBuffer& s, const std::pair<T1,T2>& p)  {
  s << p.first << p.second;
  return s;
}

// Input serialize a pair of items
template <class T1, class T2> inline
StreamBuffer& operator >> (StreamBuffer& s, std::pair<T1,T2>& p)  {
  T1 temp1;
  T2 temp2;
  s >> temp1;
  s >> temp2;
  p = std::make_pair(temp1, temp2);
  return s;
}

// Output operator (ASCII)
template <class T1, class T2> inline
std::ostream& operator<< ( std::ostream& s, const std::pair<T1,T2>& p ) 
{
  s << "[" << p.first << ", " << p.second << "]";
  return s;
}

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
} // namespace LHCb
  
#endif //LHCBKERNEL_SERIALIZESTL_H
