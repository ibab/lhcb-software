// $Id: SerializeStl.h,v 1.11 2007-03-26 11:18:48 jonrob Exp $
/*
  Small header file to serialize vectors, lists, pairs, maps
*/
#ifndef LHCBKERNEL_SERIALIZESTL_H
#define LHCBKERNEL_SERIALIZESTL_H

#include <ostream>
#include <vector>
#include <list>
#include <map>
#include <utility>

namespace LHCb
{
  static const int NUMBERS_PER_LINE = 6;

  /// Output serialize a vector
  template <class T> inline
  std::ostream& operator<< ( std::ostream& s, const std::vector<T>& v )
  {
    int cnt = 0;
    for(typename std::vector<T>::const_iterator i=v.begin(); i!=v.end(); ++i) {
      s << "[" << cnt << "]=";
      s.width(12);
      s << (*i) << " ";
      if ((++cnt)%NUMBERS_PER_LINE == 0)  s << std::endl;
    }
    return s;
  }

  /// Output serialize a list
  template <class T> inline
  std::ostream& operator<< ( std::ostream& s, const std::list<T>& l )
  {
    int cnt = 0;
    for (typename std::list<T>::const_iterator i = l.begin(); i != l.end(); ++i) {
      s << "[" << cnt << "]=";
      s.width(12);
      s << (*i) << " ";
      if ( (++cnt)%NUMBERS_PER_LINE == 0 )  s << std::endl;
    }
    return s;
  }

  /// Output serialize a pair
  template <class T1, class T2> inline
  std::ostream& operator<< ( std::ostream& s, const std::pair<T1,T2>& p )
  {
    s << "[" << p.first << ", " << p.second << "]";
    return s;
  }

  /// Output serialize a std::map
  template <class T1, class T2> inline
  std::ostream& operator << ( std::ostream& s, const std::map<T1,T2>& m )
  {
    for ( typename std::map<T1,T2>::const_iterator i = m.begin();
          i != m.end(); ++i ) {
      s << "[" << i->first << ", " << i->second << "]";
    }
    return s;
  }

} // namespace LHCb

#endif //LHCBKERNEL_SERIALIZESTL_H
