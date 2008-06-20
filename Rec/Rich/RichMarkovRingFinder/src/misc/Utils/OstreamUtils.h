// $Id: OstreamUtils.h,v 1.1 2008-06-20 09:54:23 jonrob Exp $
#ifndef GENERICRINGFINDER_OSTREAMUTILS_H 
#define GENERICRINGFINDER_OSTREAMUTILS_H 1

#include <vector>
#include <map>
#include <iostream>

// Specialization ostream operator for std::vector<T>
template <class T>
inline std::ostream & operator << ( std::ostream & os, const std::vector<T> & v ) 
{
  os << "[ ";
  if ( !v.empty() )
  {
    typename std::vector<T>::const_iterator i = v.begin();
    os << *i; 
    ++i;
    for ( ; i != v.end(); ++i ) os << " " << *i;
  }
  return os << " ]";
}

// Specialization ostream operator for std::map<T1,T2>
template <class T1, class T2>
inline std::ostream & operator << ( std::ostream & os, const std::map<T1,T2> & m ) 
{
  os << "[ ";
  if ( !m.empty() )
  {
    typename std::map<T1,T2>::const_iterator i = m.begin();
    os << (*i).first << "=" << (*i).second; 
    ++i;
    for ( ; i != m.end(); ++i ) os << " " << (*i).first << "=" << (*i).second;
  }
  return os << " ]";
}

#endif // GENERICRINGFINDER_OSTREAMUTILS_H
