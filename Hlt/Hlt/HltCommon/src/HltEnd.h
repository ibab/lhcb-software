// $Id: HltEnd.h,v 1.3 2006-10-24 09:44:03 hernando Exp $
#ifndef HLT_HLTEND_H 
#define HLT_HLTEND_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "PatTools/PatDataStore.h"
#include "HltBase/IHltDataStore.h"
#include "HltBase/HltContainers.h"
#include "Event/HltSummary.h"

/** @class HltEnd HltEnd.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-25
 */
class HltEnd : public GaudiAlgorithm {
public:

  typedef std::map< std::string, Hlt::TrackContainer > MapTracks;
  typedef std::map< std::string, Hlt::VertexContainer > MapVertices;
  

  /// Standard constructor
  HltEnd( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltEnd( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void fromPatToHlt();

  void saveAll();

  void saveSummary();
  
  void saveTracks(const std::string& name, const Hlt::TrackContainer& cont);

  void saveVertices(const std::string& name, const Hlt::VertexContainer& cont);

protected:

  bool m_onTES;
  bool m_patToHlt;

  std::string m_summaryName;
  LHCb::HltSummary* m_summary;

  std::string m_summaryTracksName;
  std::string m_summaryVerticesName;

  Hlt::TrackContainer* m_summaryTracks;
  Hlt::VertexContainer* m_summaryVertices;

protected:

  IHltDataStore* m_store;
  PatDataStore* m_patstore;

  MapTracks* m_tracks;
  MapVertices* m_vertices;

};
#endif // HLTDATASTOREINIT_H
