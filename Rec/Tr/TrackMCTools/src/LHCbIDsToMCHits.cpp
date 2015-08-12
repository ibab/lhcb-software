// GaudiKernel
#include "GaudiKernel/IIncidentSvc.h"

#include "LHCbIDsToMCHits.h"

#include "Event/MCHit.h"
#include "Event/Track.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"
#include "Event/VPCluster.h"
#include "Event/OTTime.h"
#include "Event/MuonCoord.h"
#include "Event/FTCluster.h"


DECLARE_TOOL_FACTORY( LHCbIDsToMCHits )
using namespace LHCb;

LHCbIDsToMCHits::LHCbIDsToMCHits(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent),
  m_itLinks(0,0,""),
  m_ttLinks(0,0,""),
  m_utLinks(0,0,""),
  m_otLinks(0,0,""),
  m_veloLinks(0,0,""),
  m_vPLinks(0,0,""),
  m_muonLinks(0,0,""),
  m_ftLinks(0,0,""),
  m_configuredOT(false),
  m_configuredIT(false),
  m_configuredTT(false),
  m_configuredUT(false),
  m_configuredVelo(false),
  m_configuredVP(false),
  m_configuredMuon(false),
  m_configuredFT(false),
  m_endString("2MCHits") {

  // constructer
  declareInterface<ILHCbIDsToMCHits>(this);
}

LHCbIDsToMCHits::~LHCbIDsToMCHits(){
  // destructer
}

StatusCode LHCbIDsToMCHits::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  incSvc()->addListener(this, IncidentType::BeginEvent);

  return StatusCode::SUCCESS;
}


StatusCode LHCbIDsToMCHits::link(LHCbIDs::const_iterator& start, 
                                 LHCbIDs::const_iterator& stop, LinkMap& output) const{

 

  for ( LHCbIDs::const_iterator iter = start ; iter != stop ; ++iter) {
    link(*iter,output);
  } // iter

  return StatusCode::SUCCESS;
}

StatusCode LHCbIDsToMCHits::link(const Track& aTrack, LinkMap& output) const{
  LHCbIDs::const_iterator start = aTrack.lhcbIDs().begin();
  LHCbIDs::const_iterator stop = aTrack.lhcbIDs().end();
  return link(start,stop,output);
}

StatusCode LHCbIDsToMCHits::link(const LHCbID& id, LinkMap& output) const{

 // switch statement from hell
 const unsigned int type = id.detectorType();
 switch(type){
 case LHCbID::IT:
   linkIT(id,output);
   break;
 case LHCbID::TT:
   linkTT(id,output);
   break;
 case LHCbID::UT:
   linkUT(id,output);
   break;
 case LHCbID::Velo:
   linkVelo(id,output);
   break;
 case LHCbID::VP:
   linkVP(id,output);
   break;
 case LHCbID::OT:   
   linkOT(id,output);
   break;
 case LHCbID::Muon:   
   linkMuon(id,output);
   break;
 case LHCbID::FT:
   linkFT(id,output);   
   break;
   

 default:
   Warning("Unknown type !", StatusCode::SUCCESS, 10);
   break;
 }
 
 return StatusCode::SUCCESS;
}

void LHCbIDsToMCHits::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { 
    m_configuredOT = false; 
    m_configuredIT = false;
    m_configuredTT = false;
    m_configuredUT = false;
    m_configuredFT = false;
    m_configuredVelo = false;
    m_configuredVP = false;
    m_configuredMuon = false;
  }
}

void LHCbIDsToMCHits::linkIT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredIT){
    m_configuredIT = true;
    m_itLinks = STLinks( evtSvc(), msgSvc(),LHCb::STClusterLocation::ITClusters+m_endString );
    if (m_itLinks.notFound()) {
      throw GaudiException("no itLinker", "LHCbIDsToMCHits" ,
                           StatusCode::FAILURE);
    }
  }
  linkToDetTruth(lhcbid.stID(),m_itLinks, output);
 
}

void LHCbIDsToMCHits::linkTT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredTT){
    m_configuredTT = true;
    m_ttLinks = STLinks( evtSvc(), msgSvc(),LHCb::STClusterLocation::TTClusters+m_endString );
    if (m_ttLinks.notFound()) {
      throw GaudiException("no ttLinker", "LHCbIDsToMCHits" ,
                           StatusCode::FAILURE);
    }
  }
  linkToDetTruth(lhcbid.stID(),m_ttLinks, output);
 
}


void LHCbIDsToMCHits::linkUT(const LHCbID& lhcbid, LinkMap& output) const
{
  if (!m_configuredUT){   
    m_configuredUT = true;
    m_utLinks = STLinks( evtSvc(), msgSvc(),LHCb::STClusterLocation::UTClusters+m_endString );
    if (m_utLinks.notFound()) {
      throw GaudiException("no UTLinker", "LHCbIDsToMCHits" ,
                           StatusCode::FAILURE);
    }
  }
  linkToDetTruth(lhcbid.stID(),m_utLinks, output);

}


void LHCbIDsToMCHits::linkOT(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredOT){
    m_configuredOT = true;
    m_otLinks = OTLinks( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default+m_endString );
    if (m_otLinks.notFound()) {
      throw GaudiException("no otLinker", "LHCbIDsToMCHits" ,
                           StatusCode::FAILURE);
    }
  }
  linkToDetTruth(lhcbid.otID(),m_otLinks, output);
}

void LHCbIDsToMCHits::linkVelo(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredVelo){
    m_configuredVelo = true;
    m_veloLinks = VeloLinks( evtSvc(), msgSvc(),LHCb::VeloClusterLocation::Default+m_endString);
    if (m_veloLinks.notFound()) {
      throw GaudiException("no veloLinker", "LHCbIDsToMCHits" ,
                           StatusCode::FAILURE);
    }
  }
  linkToDetTruth(lhcbid.veloID(),m_veloLinks, output);
 
}

void LHCbIDsToMCHits::linkVP(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredVP){
    m_configuredVP = true;
    m_vPLinks = VPLinks( evtSvc(), msgSvc(),LHCb::VPClusterLocation::Default+m_endString);
    if (m_vPLinks.notFound()) {
      throw GaudiException("no vPLinker", "LHCbIDsToMCHits" ,
                           StatusCode::FAILURE);
    }
  }
  linkToDetTruth(lhcbid.vpID(),m_vPLinks, output);
 
}

void LHCbIDsToMCHits::linkFT(const LHCbID& lhcbid, LinkMap& output) const
{
  if (!m_configuredFT){   
    m_configuredFT = true;
    m_ftLinks = FTLinks( evtSvc(), msgSvc(),LHCb::FTClusterLocation::Default+m_endString);
    if (m_ftLinks.notFound()) {
      throw GaudiException("no FTLinker", "LHCbIDsToMCHits" ,
                           StatusCode::FAILURE);
    } 
  }
  linkToDetTruth(lhcbid.ftID(),m_ftLinks, output);
}




void LHCbIDsToMCHits::linkMuon(const LHCbID& lhcbid, LinkMap& output) const{

  if (!m_configuredMuon){
    m_configuredMuon = true;
    m_muonLinks = MuonLinks( evtSvc(), msgSvc(),LHCb::MuonCoordLocation::MuonCoords+m_endString);
    if (m_muonLinks.notFound()) {
      throw GaudiException("no MuonLinker", "LHCbIDsToMCHits" ,
                           StatusCode::FAILURE);
    }
  }
  linkToDetTruth(lhcbid.muonID(),m_muonLinks, output);
}



