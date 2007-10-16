// $Id: CopyPrimaryVertices.h,v 1.1 2007-10-16 14:07:30 jpalac Exp $
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
   * Depending on the state of storeTracks(), 
   * either performs standard clone or does something
   * special with the SmartRefVector<LHCb::Track>
   *
   * @author Juan Palacios juanch@nikhef.nl
   * @date 16-10-2007
   */
  struct PVCloner 
  {
  public:

    explicit PVCloner(const bool& storeTracks)
      : m_storeTracks(storeTracks)
    {
      std::cout << "In PVCloner constructor!" << std::endl;
    }

    void storeTracks(const bool& storeTracks) 
    {
      m_storeTracks=storeTracks;
    }

    LHCb::RecVertex* clone(const LHCb::RecVertex* pv) const
    {
      LHCb::RecVertex* item = pv->clone();
      const SmartRefVector< LHCb::Track >& tracks = pv->tracks();
      if (m_storeTracks) storeVertexTracks(item, tracks);
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
  
  private:
    bool m_storeTracks;
    
  };

private:

  typedef LHCb::RecVertex::Container Vertices;
  typedef CopyAndStoreData::ContainerCloner<Vertices, PVCloner > PVContainerCloner;

  inline const bool storeTracks() const 
  {
    return m_storeTracks;
  }

  inline const PVContainerCloner& containerCloner() const 
  {
    return m_cloner;
  }

  inline PVContainerCloner& containerCloner()
  {
    return m_cloner;
  }

private:

  bool m_storeTracks;
  PVContainerCloner m_cloner;

};
#endif // COPYPRIMARYVERTICES_H
