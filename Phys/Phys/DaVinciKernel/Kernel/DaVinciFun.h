// $Id: DaVinciFun.h,v 1.9 2009-11-05 10:19:04 jpalac Exp $
#ifndef KERNEL_DAVINCIFUN_H 
#define KERNEL_DAVINCIFUN_H 1

// Include files
// from ROOT
#include "Math/Boost.h"
#include "Kernel/Particle2Vertex.h"
#include "Event/RecVertex.h"
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

  const LHCb::VertexBase* bestVertexBase(const Particle2Vertex::Table::Range& range);

  const LHCb::RecVertex* bestRecVertex(const Particle2Vertex::Table::Range& range);

  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    /** set the input particles for some component
     *  @param component the component inself
     *  @param input the intut data 
     *  @return status code 
     * 
     *  - 300 : invalid component 
     *  - 301 : no valid ISetInputParticles interface 
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-07-11
     */
    StatusCode setInput 
    ( IInterface*                        component , 
      const LHCb::Particle::ConstVector& input     ) ;
    // ========================================================================
  } // end of namespace Utils 
  // ==========================================================================

} // namespace DaVinci

#endif // KERNEL_DAVINCIFUN_H
