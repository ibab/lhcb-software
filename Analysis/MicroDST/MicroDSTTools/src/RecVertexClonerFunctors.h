// $Id: RecVertexClonerFunctors.h,v 1.1.1.1 2007-12-10 09:32:24 jpalac Exp $
#ifndef RECVERTEXCLONERFUNCTORS_H 
#define RECVERTEXCLONERFUNCTORS_H 1

// Include files
#include <MicroDST/Functors.hpp>
#include <Event/RecVertex.h>
#include <Event/Track.h>
/** @namespace MicroDST RecVertexClonerFunctors.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
namespace MicroDST {

  typedef BasicItemCloner<LHCb::RecVertex> BasicRecVertexCloner;

  /**
   * Functor to custom-clone an LHCb::Vertex object.
   * Does something what BasicItemCloner does, plus
   * take the SmartRefVector<LHCb::Track> and copy only SmartRefs
   * to the tracks.
   *
   * @author Juan Palacios juanch@nikhef.nl
   * @date 16-10-2007
   */
  struct RecVertexClonerShallowTracks
  {
  public:
    static LHCb::RecVertex* clone(const LHCb::RecVertex* pv)
    {
      LHCb::RecVertex* item = pv->clone();
      const SmartRefVector< LHCb::Track >& tracks = pv->tracks();
      storeVertexTracks(item, tracks);
      return item;
    }
  private:
    static void storeVertexTracks(LHCb::RecVertex* pv, 
                                  const SmartRefVector<LHCb::Track>& tracks) 
    {
      pv->clearTracks();
      std::cout << "RecVertexClonerShallowTracks PV clone has " 
                << tracks.size() 
                << " tracks" << std::endl;
      typedef SmartRefVector<LHCb::Track>::const_iterator tk_iterator;
      for (tk_iterator iTrack = tracks.begin(); 
           iTrack != tracks.end();
           iTrack++) {
        pv->addToTracks(*iTrack);      
      }
    }
    
  };

}
#endif // RECVERTEXCLONERFUNCTORS_H
