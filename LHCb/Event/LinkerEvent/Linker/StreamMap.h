// $Id: StreamMap.h,v 1.1.1.1 2004-01-08 12:24:33 ocallot Exp $
#ifndef EVENT_STREAMMAP_H 
#define EVENT_STREAMMAP_H 1

// Output serialize a map of items

template <class T, class Q> inline 
StreamBuffer& operator << (StreamBuffer& s, const std::map<T,Q>& v)  {
  s << v.size();
  for ( typename std::map<T,Q>::const_iterator i = v.begin(); i != v.end(); 
        i++ )  {
    s << (*i).first << (*i).second;
  }
  return s;
}

// Input serialize a map of items

template <class T, class Q> inline
StreamBuffer& operator >> (StreamBuffer& s, std::map<T,Q>& v)  {
  long i, len;
  s >> len;
  v.clear();
  for ( i = 0; i < len; i++ )  {
    T    temp;
    s >> temp;
    Q    temp2;
    s >> temp2;
    v[temp] = temp2;
  }
  return s;
}

template <class T, class Q> inline
std::ostream& operator << (std::ostream& s, const std::map<T,Q>& v)  {
  return s;
}

 
#endif // EVENT_STREAMMAP_H
