// $Id: SerializeStl.h,v 1.3 2002-04-05 10:27:18 roiser Exp $
/*
    Small header file to serialize vectors and lists
    to standard Gaudi StreamBuffer objects
*/
#ifndef LHCBKERNEL_SERIALIZESTL_H
#define LHCBKERNEL_SERIALIZESTL_H

#include "GaudiKernel/StreamBuffer.h"
#include <vector>
#include <list>
#include <utility>

static const int NUMBERS_PER_LINE = 6;

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

// Output operator (ASCII)
template <class T> inline
std::ostream& operator<< ( std::ostream& s, const std::vector<T>& v ) 
{
  int cnt = 0;
  for(std::vector<T>::const_iterator i=v.begin(); i!=v.end(); i++, cnt++) {
    s << "[" << cnt << "]="; 
    s.width(12); 
    s << (*i) << " ";
    if ( (++cnt)%NUMBERS_PER_LINE == 0 )  s << endmsg;
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

// Output operator (ASCII)
template <class T> inline
std::ostream& operator<< ( std::ostream& s, const std::list<T>& l ) 
{
  int cnt = 0;
  for ( std::list<T>::const_iterator i = l.begin(); i != l.end(); i++ )  {
    s << "[" << cnt << "]="; 
    s.width(12); 
    s << (*i) << " ";
    if ( (++cnt)%NUMBERS_PER_LINE == 0 )  s << endmsg;
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
  




#endif LHCBKERNEL_SERIALIZESTL_H
