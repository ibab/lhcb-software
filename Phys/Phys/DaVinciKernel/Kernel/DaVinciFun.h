// $Id: DaVinciFun.h,v 1.1 2009-02-27 16:47:41 jpalac Exp $
#ifndef KERNEL_DAVINCIFUN_H 
#define KERNEL_DAVINCIFUN_H 1

// Include files

/** @namespace DaVinci Kernel/DaVinciFun.h
 *  
 * Collection of free functions used widely in DVAlgorithm 
 * and PhysDesktop
 *
 *
 *  @author Juan PALACIOS
 *  @date   2009-02-27
 */
namespace DaVinci {

  ///  Is in TES
  template <class T>
  inline bool inTES(const T* P) {
    return ( 0!=P && 0!=P->parent()) ;
  }

  /**
   *
   * delete contents of container of newed pointers unless they are
   * also in the TES.
   * T must be a const_iterator defined, begin(), end() and clear() methods,
   * and holding pointers.
   *
   * @param container STL container of ContainedObject pointers
   * @return number of elements that are also in the TES.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 10/02/2009
   *
   **/
  template <class T>
  unsigned int safeContainerClear(T& container) 
  {
    if ( container.empty() ) return 0;
    int iCount(0);
    for (typename T::const_iterator iObj = container.begin();
         iObj != container.end(); ++iObj) {
      if( DaVinci::inTES(*iObj) ) {
        ++iCount;
      } else {
        delete *iObj;
      }
      
    }
    container.clear();
    return iCount;
  }

  /// Find the position of an LHCb::VertexBase inside a range
  inline Particle2Vertex::Range::iterator findPos(const Particle2Vertex::Range& range, 
                                                  const LHCb::VertexBase* to )
  {
    for (Particle2Vertex::Range::iterator r = range.begin(); 
         range.end()!=r; 
         ++r) {
      if ( to == r->to() ) { return r; }  
    }
    return range.end();
  }

  const LHCb::VertexBase* bestVertexBase(const Particle2Vertex::Range& range) 
  {
    if (range.empty()) return 0;
    return range.back().to();
  }

  const LHCb::RecVertex* bestRecVertex(const Particle2Vertex::Range& range) 
  {
    if (range.empty()) return 0;
    return dynamic_cast<const LHCb::RecVertex*>( range.back().to() );
  }

}
#endif // KERNEL_DAVINCIFUN_H
