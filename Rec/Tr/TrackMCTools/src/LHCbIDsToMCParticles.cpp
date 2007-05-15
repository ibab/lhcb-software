// $Id: LHCbIDsToMCParticles.cpp,v 1.1 2007-05-15 08:34:15 mneedham Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "LHCbIDsToMCParticles.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"
#include "Event/OTTime.h"

//static const ToolFactory<LHCbIDsToMCParticles>  s_factory;
//const IToolFactory& LHCbIDsToMCParticlesFactory = s_factory;
DECLARE_TOOL_FACTORY( LHCbIDsToMCParticles );
using namespace LHCb;

LHCbIDsToMCParticles::LHCbIDsToMCParticles(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent),
  m_itLinks(0,0,""),
  m_ttLinks(0,0,""),
  m_otLinks(0,0,""),
  m_veloLinks(0,0,""),
  m_configured(false) {

  // constructer
  declareInterface<ILHCbIDsToMCParticles>(this);
};

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
    link(*iter,output);
  } // iter

  return StatusCode::SUCCESS;
}

StatusCode LHCbIDsToMCParticles::link(const Track& aTrack, LinkMap& output) const{
  LHCbIDs::const_iterator start = aTrack.lhcbIDs().begin();
  LHCbIDs::const_iterator stop = aTrack.lhcbIDs().end();
  return link(start,stop,output);
}

StatusCode LHCbIDsToMCParticles::link(const LHCbID& id, LinkMap& output) const{

 if (!m_configured) initEvent();

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
 default:
   Warning("Unknown type !", StatusCode::SUCCESS, 1);
   break;
 }
 
 return StatusCode::SUCCESS;
}

void LHCbIDsToMCParticles::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { m_configured = false; }
}

void LHCbIDsToMCParticles::initEvent() const{

  m_configured = true;

  m_itLinks = STLinks( evtSvc(), msgSvc(),LHCb::STClusterLocation::ITClusters );
  m_ttLinks = STLinks( evtSvc(), msgSvc(),LHCb::STClusterLocation::TTClusters );
  m_otLinks = OTLinks( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );
  m_veloLinks = VeloLinks( evtSvc(), msgSvc(),LHCb::VeloClusterLocation::Default );

}

void LHCbIDsToMCParticles::linkIT(const LHCbID& lhcbid, LinkMap& output) const{

  linkToDetTruth(lhcbid.stID(),m_itLinks, output);
 
}

void LHCbIDsToMCParticles::linkTT(const LHCbID& lhcbid, LinkMap& output) const{

  linkToDetTruth(lhcbid.stID(),m_ttLinks, output);
 
}

void LHCbIDsToMCParticles::linkOT(const LHCbID& lhcbid, LinkMap& output) const{

  linkToDetTruth(lhcbid.otID(),m_otLinks, output);
 
}

void LHCbIDsToMCParticles::linkVelo(const LHCbID& lhcbid, LinkMap& output) const{

  linkToDetTruth(lhcbid.veloID(),m_veloLinks, output);
 
}
