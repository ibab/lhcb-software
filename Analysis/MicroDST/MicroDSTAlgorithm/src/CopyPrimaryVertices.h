// $Id: CopyPrimaryVertices.h,v 1.4 2007-10-24 15:38:35 jpalac Exp $
#ifndef COPYPRIMARYVERTICES_H 
#define COPYPRIMARYVERTICES_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"
#include "Event/RecVertex.h"

/** @class CopyPrimaryVertices CopyPrimaryVertices.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyPrimaryVertices : public CopyAndStoreData {
public: 
  /// Standard constructor
  CopyPrimaryVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyPrimaryVertices( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /**
   * Functor to custom-clone an LHCb::Vertex object.
   * Does something what BasicItemCloner does, plus
   * something special with the SmartRefVector<LHCb::Track>
   *
   * @author Juan Palacios juanch@nikhef.nl
   * @date 16-10-2007
   */
  struct PVCloner
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
      std::cout << "PVContainer PV clone has " << tracks.size() 
                << " tracks" << std::endl;
      typedef SmartRefVector<LHCb::Track>::const_iterator tk_iterator;
      for (tk_iterator iTrack = tracks.begin(); 
           iTrack != tracks.end();
           iTrack++) {
        pv->addToTracks(*iTrack);      
      }
    }
    
  };

private:

  typedef LHCb::RecVertex::Container Vertices;
  typedef MicroDST::BasicItemCloner<LHCb::RecVertex> PVCloneFunctor;
  typedef MicroDST::CloneKeyedContainerItem<LHCb::RecVertex, PVCloneFunctor> BasicPVCloner;
  

private:

  bool m_storeTracks;

};
#endif // COPYPRIMARYVERTICES_H
