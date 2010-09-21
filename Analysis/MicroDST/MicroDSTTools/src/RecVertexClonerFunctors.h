// $Id: RecVertexClonerFunctors.h,v 1.3 2010-08-11 12:50:50 jpalac Exp $
#ifndef RECVERTEXCLONERFUNCTORS_H 
#define RECVERTEXCLONERFUNCTORS_H 1

// Include files
#include <MicroDST/Functors.hpp>
#include <Event/RecVertex.h>
#include <Event/Track.h>
#include <functional>
/** @namespace MicroDST RecVertexClonerFunctors.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
namespace MicroDST {

  typedef BasicItemCloner<LHCb::RecVertex> BasicRecVertexCloner;


  /**
   * Functor that simply calls the target() method
   * of an object. Necessary for copying of SmartRefs.
   *
   * @author Juan Palacios palacios@physik.uzh.ch
   * 
   * Example:
   *
   * @code
   *
   * const SmartRefVector< T >& smartRefs = x->refs();
   * std::for_each(smartRefs.begin(), smartRefs.end(), CallTarget<T>());
   * T* y = x->clone();
   *
   * @endcode
   *
   *
   */

  template <class T>
  struct DeReference : public std::unary_function<const SmartRef<T>&,void>
  {
    inline void operator()(const SmartRef<T>& obj) { obj.target(); }
  };

  /**
   * Functor to custom-clone an LHCb::RecVertex object.
   * Takes care that the RecVertex's track SmartRefs are
   * de-referenced before cloning the action. This ensures
   * that they are valid.
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @date 16-10-2007
   */
  struct RecVertexClonerShallowTracks
  {
    typedef LHCb::RecVertex Type;
  public:
    static LHCb::RecVertex* clone(const LHCb::RecVertex* pv)
    {
      const SmartRefVector< LHCb::Track >& tracks = pv->tracks();
      std::for_each(tracks.begin(), tracks.end(), DeReference< LHCb::Track >());
      LHCb::RecVertex* item = pv->clone();
      return item;
    }
    
  };

}
#endif // RECVERTEXCLONERFUNCTORS_H
