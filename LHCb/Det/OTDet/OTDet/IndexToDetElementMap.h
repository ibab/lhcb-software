// $Id: IndexToDetElementMap.h,v 1.2 2007-06-12 22:49:54 janos Exp $
#ifndef OTDET_INDEXTODETELEMENTMAP_H 
#define OTDET_INDEXTODETELEMENTMAP_H 1

// from STD
#include <vector>

/** @class IndexToDetElementMap IndexToDetElementMap.h OTDet/IndexToDetElementMap.h
 *  
 *  Small mapping utility to map an index to a pointer of an detector element. 
 *  Returns NULL if index does not point to a detector element. 
 *
 *  @author Wouter Hulsbergen
 *  @date   2007-05-30
 */

namespace OT {
  template < class T , size_t initialsize=0 , size_t offset=0 >
  class IndexToDetElementMap {
 
  public:
    /// Constructor
    IndexToDetElementMap():m_contents(initialsize, 0) { };
      
    /// Return size
    size_t size() const { return m_contents.size(); };

    /// Const operator [] 
    const T* operator[](size_t index) const {
      index-=offset;
      return index < size() ? m_contents[index] : 0;
    };
    
    /// Non const operator []
    T* operator[](size_t index) { 
      index-=offset;
      return index < size() ? m_contents[index] : 0;
    };
    
    /// Insert method
    void insert(size_t index, T* object) {
      index-=offset;
      if (index >= m_contents.size()) m_contents.resize(index + 1, 0);
      m_contents[index] = object;
    };
    
    /// Const method to return contens
    const std::vector<T*> contents() const {
      return m_contents;  
    };
    
    /// Non const method to return contens
    std::vector<T*> contents() {
      return m_contents;
    };
    
    /// clear vector
    void clear() { m_contents.clear(); };

    /// reserve
    void reserve(size_t size) { m_contents.reserve(size); };
    
  private:
    std::vector<T*> m_contents;
  } ;
}

#endif // OTDET_INDEXTODETELEMENTMAP_H
