#include "GaudiKernel/ToolFactory.h"
#include "RelInfoMuonIsolation.h"
#include "Kernel/RelatedInfoNamed.h"
#include "MuonID/IMuonIDTool.h"
#include "Event/MuonPID.h"
#include "MuonDet/DeMuonDetector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoMuonIsolation
//
// 2015-11-25 : Giacomo Graziani
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RelInfoMuonIsolation )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoMuonIsolation::RelInfoMuonIsolation( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);

}

StatusCode RelInfoMuonIsolation::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_muIDtool  = tool<IMuonIDTool>("MuonIDPlusTool", this);

  DeMuonDetector* mudet=getDet<DeMuonDetector>(DeMuonLocation::Default);
  m_nStations = mudet->stations();

  return StatusCode::SUCCESS;
}

StatusCode RelInfoMuonIsolation::calculateRelatedInfo( const LHCb::Particle* ,
                                                       const LHCb::Particle* P) {
  // check that we have a track
  if ( !P ) return StatusCode::FAILURE;
  if ( !P->isBasicParticle() ) return StatusCode::SUCCESS;
  const LHCb::ProtoParticle* protop = P->proto();
  if (!protop) return StatusCode::SUCCESS;
  const LHCb::Track* track = protop->track();
  if (!track) return StatusCode::SUCCESS;

  LHCb::MuonPID* pid= m_muIDtool->getMuonID(track);
  const LHCb::Track *mTrack= (pid ? pid->muonTrack() : NULL);
  

  m_map.clear();
  int firstStation= (m_nStations < 5) ? 0 : 1; // upgrade ready
  for (int station = firstStation ; station < m_nStations; station++) {
    float isoMu = (float) m_muIDtool->muonIDPropertyD(track, "iso", station);
    int M = station - firstStation +2; // must be 2 to 5
    debug()<< "iso variable for M"<<M<<" is "<<isoMu<< endmsg;
    switch (M) {
    case 2:
      m_map.insert( std::make_pair(RelatedInfoNamed::ISOMUM2, isoMu) );
      break;
    case 3:
      m_map.insert( std::make_pair(RelatedInfoNamed::ISOMUM3, isoMu) );
      break;
    case 4:
      m_map.insert( std::make_pair(RelatedInfoNamed::ISOMUM4, isoMu) );
      break;
    case 5:
      m_map.insert( std::make_pair(RelatedInfoNamed::ISOMUM5, isoMu) );
      break;
    }
  }
  if(mTrack)
    debug() << "GGDEBUG chi2 is "<<mTrack->chi2()<<"/"<<  mTrack->nDoF()<<endmsg;
  m_map.insert( std::make_pair(RelatedInfoNamed::MUONCORRCHI2PERDOF, (float) (mTrack ? mTrack->chi2PerDoF() : -1.)) );
                
  return StatusCode::SUCCESS;
}

LHCb::RelatedInfoMap* RelInfoMuonIsolation::getInfo(void) {
  return &m_map;
}
