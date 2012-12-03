#ifndef PRLHCBID2MCPARTICLE_H 
#define PRLHCBID2MCPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"

#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/IOTHitCreator.h"


/** @class PrLHCbID2MCParticle PrLHCbID2MCParticle.h
 *  A clone of PatLHCbIDs2MCParticle, to be used with upgraded detectors.
 *
 *  @author Victor Coco (Original PatLHCbIDs2MCParticle by Olivier Callot)
 *  @date   2010-03-22
 */

class PrLHCbID2MCParticle : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrLHCbID2MCParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrLHCbID2MCParticle( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  void linkAll( LHCb::LHCbID myId, int firstId, int size = 1 );  // link all particles to the specified id

private:
  std::string m_targetName;
  std::vector<const LHCb::MCParticle*> m_partList;
  ToolHandle<Tf::IOTHitCreator>    m_otHitCreator;
  LinkerWithKey<LHCb::MCParticle>* m_linker;
  LinkedTo<LHCb::MCParticle>*      m_detectorLink;
  bool m_debug;
  bool m_otReady;
  std::vector<std::string>         m_stClusterNames;
  std::vector<std::string>         m_stLiteClusterNames;
};
#endif // PRLHCBID2MCPARTICLE_H
