// $Id: HltInit.h,v 1.3 2007-02-05 09:09:08 hernando Exp $
#ifndef HLTDATASTOREINIT_H 
#define HLTDATASTOREINIT_H 1

// Include files
// from Gaudi
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

  std::string m_summaryName;

  PatDataStore* m_patStore;

  MapTracks* m_tracks;
  MapVertices* m_vertices;

private:

};
#endif // HLTDATASTOREINIT_H
