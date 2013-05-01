// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "LHCbIDsToMCParticles.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"
#include "Event/OTTime.h"
#include "Event/MuonCoord.h"

DECLARE_TOOL_FACTORY( LHCbIDsToMCParticles )
using namespace LHCb;

LHCbIDsToMCParticles::LHCbIDsToMCParticles(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent),
  m_itLinks(0,0,""),
  m_ttLinks(0,0,""),
  m_otLinks(0,0,""),
  m_veloLinks(0,0,""),
  m_muonLinks(0,0,""),
  m_configuredOT(false),
  m_configuredIT(false),
  m_configuredTT(false),
  m_configuredVelo(false),
  m_configuredMuon(false){

  // constructer
  declareInterface<ILHCbIDsToMCParticles>(this);
}

LHCbIDsToMCParticles::~LHCbIDsToMCParticles(){
  // destructer
}

StatusCode LHCbIDsToMCParticles::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  incSvc()->addListener(this, IncidentType::BeginEvent);

  return StatusCode::SUCCESS;
}


StatusCode LHCbIDsToMCParticles::link(LHCbIDs::const_iterator& start, 
                                      LHCbIDs::const_iterator& stop, LinkMap& output) const{

  for ( LHCbIDs::const_iterator iter = start ; iter != stop ; ++iter) {
    StatusCode sc = link(*iter,output);
    if (sc.isFailure() ){
      return sc;
    }
  } // iter

  return StatusCode::SUCCESS;
}

StatusCode LHCbIDsToMCParticles::link(const Track& aTrack, LinkMap& output) const{
  LHCbIDs::const_iterator start = aTrack.lhcbIDs().begin();
  LHCbIDs::const_iterator stop = aTrack.lhcbIDs().end();
  return link(start,stop,output);
}

StatusCode LHCbIDsToMCParticles::link(const LHCbID& id, LinkMap& output) const{

 // switch statement from hell
 const unsigned int type = id.detectorType();
 switch(type){
 case LHCbID::IT:
   return linkIT(id,output);
   break;
 case LHCbID::TT:
   return linkTT(id,output);
   break;
 case LHCbID::Velo:
   return linkVelo(id,output);
   break;
 case LHCbID::OT:   
   return linkOT(id,output);
   break;
 case LHCbID::Muon:
   return linkMuon(id,output);
   break;
 default:
   return Warning("Unknown type !", StatusCode::SUCCESS, 1);
   break;
 }
 
 return StatusCode::SUCCESS;
}

void LHCbIDsToMCParticles::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) {
    m_configuredOT = false; 
    m_configuredIT = false;
    m_configuredTT = false;
    m_configuredVelo = false;
    m_configuredMuon = false;
  }
}

StatusCode LHCbIDsToMCParticles::linkIT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredIT){
    m_configuredIT = true;
    m_itLinks = STLinks(evtSvc(), msgSvc(),
                        LHCb::STClusterLocation::ITClusters );
    if (m_itLinks.notFound()) {
      return Error("no ITLinker",StatusCode::FAILURE,10);
    }
  }
  linkToDetTruth(lhcbid.stID(),m_itLinks, output);
  return StatusCode::SUCCESS;
}

StatusCode LHCbIDsToMCParticles::linkTT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredTT){
    m_configuredTT = true;
    m_ttLinks = STLinks(evtSvc(), msgSvc(),
                        LHCb::STClusterLocation::TTClusters);
    if (m_ttLinks.notFound()) {
      return Error("no TTLinker",StatusCode::FAILURE,10);
    }
  }  
  linkToDetTruth(lhcbid.stID(),m_ttLinks, output); 
  return StatusCode::SUCCESS;
}

StatusCode LHCbIDsToMCParticles::linkOT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredOT){
    m_configuredOT = true;
    m_otLinks = OTLinks( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );
    if (m_otLinks.notFound()) {
      return Error("no OTLinker",StatusCode::FAILURE,10);
    }
  }  
  linkToDetTruth(lhcbid.otID(),m_otLinks, output);
  return StatusCode::SUCCESS;
}

StatusCode LHCbIDsToMCParticles::linkVelo(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredVelo){
    m_configuredVelo = true;
    m_veloLinks = VeloLinks(evtSvc(), msgSvc(),
                            LHCb::VeloClusterLocation::Default);
    if (m_veloLinks.notFound()) {
      return Error("no VeloLinker",StatusCode::FAILURE,10);
    }
  }  
  linkToDetTruth(lhcbid.veloID(),m_veloLinks, output); 
  return StatusCode::SUCCESS;
}

StatusCode LHCbIDsToMCParticles::linkMuon(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredMuon){
    m_configuredMuon = true;
    m_muonLinks = MuonLinks( evtSvc(), msgSvc(),LHCb::MuonCoordLocation::MuonCoords);
    if (m_muonLinks.notFound()) {
      return Error("no MuonLinker",StatusCode::FAILURE,10);
    }
  }  
  linkToDetTruth(lhcbid.muonID(),m_muonLinks, output);
  return StatusCode::SUCCESS;
}

