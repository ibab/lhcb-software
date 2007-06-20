// $Id: HltInit.h,v 1.4 2007-06-20 12:17:38 hernando Exp $
#ifndef HLTDATASTOREINIT_H 
#define HLTDATASTOREINIT_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/IHltDataStore.h"
#include "HltBase/HltContainers.h"
#include "PatTools/PatDataStore.h"

/** @class HltInit HltInit.h
 *  
 *  functionality:
 *         task to be done at the begining of the HLT sequence
 *         clean the HltDataStore collections
 *         create the HltSummary (empty) for this event
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-25
 */
class HltInit : public GaudiAlgorithm {
public:

  typedef std::map< std::string, Hlt::TrackContainer > MapTracks;
  typedef std::map< std::string, Hlt::VertexContainer > MapVertices;
  

  /// Standard constructor
  HltInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  IDataManagerSvc* m_hltSvc;

  std::string m_summaryName;

  PatDataStore* m_patStore;

  MapTracks* m_tracks;
  MapVertices* m_vertices;

private:

};
#endif // HLTDATASTOREINIT_H
