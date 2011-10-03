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
   linkIT(id,output);
   break;
 case LHCbID::TT:
   linkTT(id,output);
   break;
 case LHCbID::Velo:
   linkVelo(id,output);
   break;
 case LHCbID::OT:   
   linkOT(id,output);
   break;
 case LHCbID::Muon:
   linkMuon(id,output);
 default:
   Warning("Unknown type !", StatusCode::SUCCESS, 1);
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

void LHCbIDsToMCParticles::linkIT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredIT){
    m_configuredIT = true;
    m_itLinks = STLinks(evtSvc(), msgSvc(),
                        LHCb::STClusterLocation::ITClusters );
    if (m_itLinks.notFound()) {
      throw GaudiException("no ITLinker", "LHCbIDsToMCParticles" ,
                          StatusCode::FAILURE);
    }
  }
  linkToDetTruth(lhcbid.stID(),m_itLinks, output);
}

void LHCbIDsToMCParticles::linkTT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredTT){
    m_configuredTT = true;
    m_ttLinks = STLinks(evtSvc(), msgSvc(),
                        LHCb::STClusterLocation::TTClusters);
    if (m_ttLinks.notFound()) {
      throw GaudiException("no TTLinker", "LHCbIDsToMCParticles" ,
                           StatusCode::FAILURE);
    }
  }  
  linkToDetTruth(lhcbid.stID(),m_ttLinks, output); 
}

void LHCbIDsToMCParticles::linkOT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredOT){
    m_configuredOT = true;
    m_otLinks = OTLinks( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );
    if (m_otLinks.notFound()) {
      throw GaudiException("no OTLinker", "LHCbIDsToMCParticles" ,
                           StatusCode::FAILURE);
    }
  }  
  linkToDetTruth(lhcbid.otID(),m_otLinks, output);
}

void LHCbIDsToMCParticles::linkVelo(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredVelo){
    m_configuredVelo = true;
    m_veloLinks = VeloLinks(evtSvc(), msgSvc(),
                            LHCb::VeloClusterLocation::Default);
    if (m_veloLinks.notFound()) {
      throw GaudiException("no VeloLinker", "LHCbIDsToMCParticles" ,
                           StatusCode::FAILURE);
    }
  }  
  linkToDetTruth(lhcbid.veloID(),m_veloLinks, output); 
}

void LHCbIDsToMCParticles::linkMuon(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredMuon){
    m_configuredMuon = true;
    m_muonLinks = MuonLinks( evtSvc(), msgSvc(),LHCb::MuonCoordLocation::MuonCoords);
    if (m_muonLinks.notFound()) {
      throw GaudiException("no MuonLinker", "LHCbIDsToMCParticles" ,
                           StatusCode::FAILURE);
    }
  }  
  linkToDetTruth(lhcbid.muonID(),m_muonLinks, output);
}

