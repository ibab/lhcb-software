#ifndef ALIGNSOLVTOOLS_ALFILEIO_H
#define ALIGNSOLVTOOLS_ALFILEIO_H

#include <ostream>
#include <istream>
#include <vector>
#include <map>

namespace Al
{
  namespace FileIO
  {
    // Be careful: you NEVER want to put these in global scope

    // IO for a generic simple object
    template<class T>
    std::ostream& operator<<(std::ostream& file, const T& object) 
    {
      file.write( reinterpret_cast<const char*>(&object), sizeof(object) ) ;
      return file ;
    }
    template<class T>
    std::istream& operator>>(std::istream& file, T& object) 
    {
      file.read( reinterpret_cast<char*>(&object), sizeof(object) ) ;
      return file ;
    }
    
    // IO for a map
    template<class T, class S>
    std::ostream& operator<<(std::ostream& file, const std::map<T,S>& amap) 
    {
      size_t n = amap.size() ;
      file << n ;
      for( typename std::map<T,S>::const_iterator it = amap.begin() ;
           it != amap.end(); ++it)
        file << it->first << it->second ;
      return file ;
    }
    template<class T, class S>
    std::istream& operator>>(std::istream& file, std::map<T,S>& amap) 
    {
      size_t n ;
      file >> n ;
      T key ;
      S obj ;
      for(size_t i=0; i<n ; ++i) {
        file >> key >> obj ;
        amap[key] = obj ; // could be done more efficienctly
      }
      return file ;
    }

    // IO for a vector
    template<class T>
    std::ostream& operator<<(std::ostream& file, const std::vector<T>& v) 
    {
      size_t n = v.size() ;
      file << n ;
      for( typename std::vector<T>::const_iterator it = v.begin() ;
           it != v.end(); ++it) file << *it ;
      return file ;
    }
    template<class T>
    std::istream& operator>>(std::istream& file, std::vector<T>& v) 
    {
      size_t n ;
      file >> n ;
      v.resize(n) ;
      for(size_t i=0; i<n ; ++i) file >> v[i] ;
      return file ;
    }
  }
}

#endif
