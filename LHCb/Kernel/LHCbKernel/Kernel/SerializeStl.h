// $Id: SerializeStl.h,v 1.1 2002-03-19 08:13:09 cattanem Exp $
/*
    Small header file to serialize vectors and lists
    to standard Gaudi StreamBuffer objects
*/
#ifndef LHCBKERNEL_SERIALIZESTL_H
#define LHCBKERNEL_SERIALIZESTL_H

#include "GaudiKernel/StreamBuffer.h"
#include <vector>
#include <list>

// Output serialize a vector of items
template <class T> inline 
StreamBuffer& operator << (StreamBuffer& s, const std::vector<T>& v)  {
  s << v.size();
  for ( std::vector<T>::const_iterator i = v.begin(); i != v.end(); i++ )  {
    s << (*i);
  }
  return s;
}

// Input serialize a vector of items
template <class T> inline
StreamBuffer& operator >> (StreamBuffer& s, std::vector<T>& v)  {
  long i, len;
  s >> len;
  v.clear();
  for ( i = 0; i < len; i++ )  {
    T    temp;
    s >> temp;
    v.push_back(temp);
  }
  return s;
}

// Output serialize a list of items
template <class T> inline 
StreamBuffer& operator << (StreamBuffer& s, const std::list<T>& l)  {
  s << l.size();
  for ( std::list<T>::const_iterator i = l.begin(); i != l.end(); i++ )  {
    s << (*i);
  }
  return s;
}

// Input serialize a list of items
template <class T> inline
StreamBuffer& operator >> (StreamBuffer& s, std::list<T>& l)  {
  long i, len;
  s >> len;
  l.clear();
  for ( i = 0; i < len; i++ )  {
    T    temp;
    s >> temp;
    l.push_back(temp);
  }
  return s;
}

#endif LHCBKERNEL_SERIALIZESTL_H
