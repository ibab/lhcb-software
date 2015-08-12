// $Id: TriggerSelectionTisTos.cpp,v 1.22 2010-07-21 21:22:17 tskwarni Exp $
//#define LOCDEBUG
// Include files
#include <algorithm>
#include <sstream>
#include "boost/algorithm/string/replace.hpp"

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"

#include "GaudiKernel/IRegistry.h"
#include "Kernel/Particle2LHCbIDs.h"

// local
#include "TriggerSelectionTisTos.h"

#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerSelectionTisTos
//
// 2007-08-06 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerSelectionTisTos )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerSelectionTisTos::TriggerSelectionTisTos( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : ParticleTisTos ( type, name , parent )
  , m_hltDecReports(0)
  , m_hltSelReports(0)
{
  declareInterface<ITriggerSelectionTisTos>(this);

  declareProperty("HltDecReportsLocation",
                  m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
  declareProperty("HltSelReportsLocation",
                  m_HltSelReportsLocation = LHCb::HltSelReportsLocation::Default);

  // values: 0=no; 1=yes but try normal particle analysis if not found; 2=yes exclusively;
  // this was deafult until 11/13/11:  declareProperty("UseParticle2LHCbIDsMap", m_useParticle2LHCbIDs = 0 );
  declareProperty("UseParticle2LHCbIDsMap", m_useParticle2LHCbIDs = 1 );

  declareProperty("Particle2LHCbIDsMapLocation",m_Particle2LHCbIDsLocation = ""  );

  m_cached_SelectionNames.reserve(500);
  m_cached_tisTosTob.reserve(500);

}


//=============================================================================
// Destructor
//=============================================================================
TriggerSelectionTisTos::~TriggerSelectionTisTos() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerSelectionTisTos::initialize() {
  const StatusCode sc = ParticleTisTos::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  IIncidentSvc*                incidentSvc(0);
  if (!service( "IncidentSvc", incidentSvc).isSuccess()) return StatusCode::FAILURE;
  // add listener to be triggered by first BeginEvent
  bool rethrow = false; bool oneShot = false; long priority = 0;
  incidentSvc->addListener(this,IncidentType::BeginEvent,priority,rethrow,oneShot);
  incidentSvc->release();


  m_newEvent =true;

  setOfflineInput();


  return sc;

}


void TriggerSelectionTisTos::handle(const Incident& )
{
  m_hltDecReports=0;
  m_hltSelReports=0;

  setOfflineInput( );

  m_newEvent=true;

}


// ------------------------------------------------------------------------------------
void TriggerSelectionTisTos::getHltSummary()
{
  if( !m_hltDecReports ){
    if( exist<HltDecReports>(m_HltDecReportsLocation, false) ){
      m_hltDecReports = get<HltDecReports>(m_HltDecReportsLocation, false);
    } else if( exist<HltDecReports>(m_HltDecReportsLocation) ){
      m_hltDecReports = get<HltDecReports>(m_HltDecReportsLocation);
    } else {
      Error( " No HltDecReports at "+m_HltDecReportsLocation.value(), StatusCode::FAILURE, 2 ).setChecked();
      m_hltDecReports =0;
    }
  }
  if( !m_hltSelReports ){
    if( exist<HltSelReports>(m_HltSelReportsLocation, false) ){
      m_hltSelReports = get<HltSelReports>(m_HltSelReportsLocation, false);
    } else if( exist<HltSelReports>(m_HltSelReportsLocation) ){
      m_hltSelReports = get<HltSelReports>(m_HltSelReportsLocation);
    } else {
      Error( " No HltSelReports at "+m_HltSelReportsLocation.value(), StatusCode::FAILURE, 2 ).setChecked();
      m_hltSelReports =0;
    }
  }
}

//=============================================================================
// -------------- offline inputs -----------------------------------------------
//=============================================================================

// erase previous input ---------------------------------------------------------
void TriggerSelectionTisTos::setOfflineInput( )
{
  setSignal();  clearCache();
}


//    hit list input ---------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist )
{
  if( addToSignal( hitlist ) )clearCache();
}

//    Track input ---------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::Track & track )
{
  if( addToSignal( track ) )clearCache();
}

//    Proto-particle input -----------------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::ProtoParticle & protoParticle )
{
  if( addToSignal( protoParticle ) )clearCache();
}

//    Particle input -----------------------------------------------------------------------
void TriggerSelectionTisTos::addToOfflineInput( const LHCb::Particle & particle )
{
  if( !m_useParticle2LHCbIDs ){
#ifdef LOCDEBUG
    info() << " Not using Particle2LHCbIDs map " << endmsg;
#endif
    if( addToSignal( particle ) )clearCache();
  } else {
    DaVinci::Map::Particle2LHCbIDs* p2lhcbids(0);
    // global map at RootInTES location
#ifdef LOCDEBUG
    info() << " Looking for Particle2LHCbIDs map with RootInTES at: " 
           << m_Particle2LHCbIDsLocation.value()+"Particle2LHCbIDMap" << endmsg;
#endif
    if( exist< DaVinci::Map::Particle2LHCbIDs >(m_Particle2LHCbIDsLocation.value()+"Particle2LHCbIDMap") ){
      p2lhcbids = get< DaVinci::Map::Particle2LHCbIDs >(m_Particle2LHCbIDsLocation.value()+"Particle2LHCbIDMap");
#ifdef LOCDEBUG
      info() << " ... got it " << p2lhcbids << endmsg;
#endif
    } else {
      const DataObject* container = particle.parent();
      if( container ){
        IRegistry* registry =  container->registry();
        if( registry ){
          std::string path = registry->identifier();
          // local map at particle location
          boost::replace_last(path,"/Particles","/Particle2LHCbIDMap");
#ifdef LOCDEBUG
          info() << " Looking for Particle2LHCbIDs map without RootInTES at: " << path <<  endmsg;
#endif
          if( exist< DaVinci::Map::Particle2LHCbIDs >(path,false) ){
            p2lhcbids = get< DaVinci::Map::Particle2LHCbIDs >(path,false);
#ifdef LOCDEBUG
            info() << " ... got it " << p2lhcbids << endmsg;
#endif
          } else {
            // global map at stream location (find first "/" past "/Event/")
            std::size_t ipos = path.find("/",7);
            if( ipos != std::string::npos ){
              std::string pathg = path.substr(0,ipos)+"/Particle2LHCbIDMap";
#ifdef LOCDEBUG
          info() << " Looking for Particle2LHCbIDs map without RootInTES at: " << pathg <<  endmsg;
#endif
              if( exist< DaVinci::Map::Particle2LHCbIDs >(pathg,false) ){
                p2lhcbids = get< DaVinci::Map::Particle2LHCbIDs >(pathg,false);
#ifdef LOCDEBUG
            info() << " ... got it " << p2lhcbids << endmsg;
#endif
              }
            }
            if( ! p2lhcbids ){
              // loop backgrounds in path and try at every level
              std::string pathgo =path;
              ipos = pathgo.find_last_of("/");
              while(  ipos != std::string::npos ){
                pathgo = pathgo.substr(0,ipos);
                std::string pathtry= pathgo + "/Particle2LHCbIDMap";
#ifdef LOCDEBUG
          info() << " Looking for Particle2LHCbIDs map without RootInTES at: " << pathtry <<  endmsg;
#endif
                if( exist< DaVinci::Map::Particle2LHCbIDs >(pathtry,false) ){
                  p2lhcbids = get< DaVinci::Map::Particle2LHCbIDs >(pathtry,false);
#ifdef LOCDEBUG
            info() << " ... got it " << p2lhcbids << endmsg;
#endif
                  break;
                }
                ipos = pathgo.find_last_of("/");
              }
            }
          }
        }
      }
    }
    if( p2lhcbids ){
      //      p2lhcbids->update(0); // temporary hack. Framework should do this automatically but isn't.
      const std::vector<LHCb::LHCbID> & hits = (*p2lhcbids)[&particle];
#ifdef LOCDEBUG
      info() << " Particle2LHCbIDs map leads to the lhcbid hit vector of size " << hits.size() << endmsg;
#endif
      if ( !hits.empty() ){
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " addToOfflineSignal Particle via Particle2LHCbIDs map " << endmsg;
        addToOfflineInput( hits );
        return;
      }
    }
#ifdef LOCDEBUG
      info() << " Particle2LHCbIDs map method did not succeed " << endmsg;
#endif
    const std::vector<const LHCb::Particle*> & daughters = particle.daughtersVector();
    if ( !daughters.empty() ){
      for(std::vector<const LHCb::Particle*>::const_iterator p = daughters.begin(); p!=daughters.end(); ++p){
        if(*p){
          const LHCb::Particle & part = *(*p);
          if ( msgLevel(MSG::VERBOSE) )
            verbose() << " addToOfflineSignal Particle via daughter " << endmsg;
#ifdef LOCDEBUG
      info() << "    adding hits via daughter " << endmsg;
#endif
          addToOfflineInput( part );
        }
      }
    } else if( m_useParticle2LHCbIDs == 1 ){
#ifdef LOCDEBUG
      info() << "     will try reco objects instead " << endmsg;
#endif
      if( addToSignal( particle ) )clearCache();
    } else if( m_useParticle2LHCbIDs == 2 ){
#ifdef LOCDEBUG
      info() << "     failed to use the map and not allowed to use reco objects " << endmsg;
#endif
      Error(" No /Particle2LHCbIDMap available for Particle with no daughters nor for mother",
            StatusCode::FAILURE, 10 ).setChecked();
      clearCache();
    }
  }
}


//=============================================================================
// -------------- outputs:
//=============================================================================


// single complete Trigger Selection TisTos  (define Offline Input before calling)
unsigned int TriggerSelectionTisTos::tisTosSelection( const std::string & selectionName ){
  unsigned int result=0;
  if( findInCache( selectionName, result ) )return result;

  getHltSummary();

  bool decision(false);
  // get decision from HltDecReports if can find it
  bool decFound(false);
  if( m_hltDecReports ){
    const HltDecReport* rep=m_hltDecReports->decReport( selectionName );
    if( rep ){
      decision = rep->decision();
      decFound = true;
    }
  }
  if( decision )result |= kDecision;

  if( !m_hltSelReports ){ storeInCache(selectionName,result); return result;}
  const LHCb::HltObjectSummary* sel =   m_hltSelReports->selReport( selectionName );
  if( !sel ){ storeInCache(selectionName,result); return result;}
  // presence in HltSelReports indicates that selection was successful
  // it is possible that decision was not stored in HltDecReports if this was an intermediate step, thus
  //   if not found in HltDecReports, set it here
  // do not overwrite decisions from HltDecReports, since positive selection could have been killed by postscale
  if( !decFound )decision=true;

  // classify the decision
  result = tisTos( *sel );
  if( decision )result |= kDecision;
  storeInCache(selectionName,result);
  return result;
}

// analysisReport for Selection (define Offline Input before calling)
std::string TriggerSelectionTisTos::analysisReportSelection( const std::string & selectionName ){
  std::ostringstream report;
  report << offset() << " Selection " + selectionName + " ";

  unsigned int result=0;

  getHltSummary();

  bool decision(false);
  // get decision from HltDecReports if can find it
  bool decFound(false);
  if( m_hltDecReports ){
    const HltDecReport* rep=m_hltDecReports->decReport( selectionName );
    if( rep ){
      decision = rep->decision();
      decFound = true;
      report << " HltDecReport decision=" << decision;
    }
  }
  if( decision )result |= kDecision;

  if( !m_hltSelReports ){ report << "HltSelReports missing " << std::endl; return report.str();}
  const LHCb::HltObjectSummary* sel =   m_hltSelReports->selReport( selectionName );
  if( !sel ){ report << "not in HltSelReports" << std::endl; return report.str();}
  // presence in HltSelReports indicates that selection was successful
  // it is possible that decision was not stored in HltDecReports if this was an intermediate step, thus
  //   if not found in HltDecReports, set it here
  // do not overwrite decisions from HltDecReports, since positive selection could have been killed by postscale
  if( !decFound )decision=true;

  // classify the decision
  result = tisTos( *sel );
  if( decision )result |= kDecision;
  report << std::endl;
  ++m_reportDepth;
  report << analysisReport( *sel );
  --m_reportDepth;
  TisTosTob res( result );
  report << offset()
         << " Selection " + selectionName + " TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps()
         << " decision= " << res.decision() << std::endl;
  return report.str();
}

// fast check for TOS
bool TriggerSelectionTisTos::tosSelection(const std::string & selectionName ){
  getHltSummary();
  if( !m_hltSelReports )return false;
  const LHCb::HltObjectSummary* sel =   m_hltSelReports->selReport( selectionName );
  if( !sel )return false;
  return tos( *sel );
}

// fast check for TIS
bool TriggerSelectionTisTos::tisSelection(const std::string & selectionName ){
  getHltSummary();
  if( !m_hltSelReports )return false;
  const LHCb::HltObjectSummary* sel =   m_hltSelReports->selReport( selectionName );
  if( !sel )return false;
  return tis( *sel );
}

// fast check for TUS
bool TriggerSelectionTisTos::tusSelection(const std::string & selectionName ){
  getHltSummary();
  if( !m_hltSelReports )return false;
  const LHCb::HltObjectSummary* sel =   m_hltSelReports->selReport( selectionName );
  if( !sel )return false;
  return tus( *sel );
}


// ------------ auxiliary output:  list of LHCbIDs corresponding to present offline input
std::vector<LHCb::LHCbID> TriggerSelectionTisTos::offlineLHCbIDs()
{
  return signal();
}

// ------------  additional functionality:  lists of object summaries for tracks/vertices/particles from trigger selections
//               satisfying TIS, TOS requirements

std::vector<const LHCb::HltObjectSummary*>
TriggerSelectionTisTos::hltSelectionObjectSummaries( const std::string & selectionName,
                                                     unsigned int tisRequirement,
                                                     unsigned int tosRequirement,
                                                     unsigned int tpsRequirement )
{
  std::vector<const LHCb::HltObjectSummary*> matchedObjectSummaries;

  getHltSummary();
  if( !m_hltSelReports )return matchedObjectSummaries;
  const LHCb::HltObjectSummary* sel =   m_hltSelReports->selReport( selectionName );
  if( !sel )return matchedObjectSummaries;

  const SmartRefVector< LHCb::HltObjectSummary > & sub = sel->substructure();
  for( SmartRefVector< LHCb::HltObjectSummary >::const_iterator ihos=sub.begin();ihos!=sub.end();++ihos){
    const LHCb::HltObjectSummary* hos = ihos->target();
    unsigned int result = tisTos( *hos );
    bool tis,tos,tps;
    tis = result & kTIS;
    tos = result & kTOS;
    tps = result & kTPS;
    if(    ((tisRequirement>=kAnything)||(tis==tisRequirement))
           && ((tosRequirement>=kAnything)||(tos==tosRequirement))
           && ((tpsRequirement>=kAnything)||(tps==tpsRequirement))){
      matchedObjectSummaries.push_back( hos );
    }
  }
  return matchedObjectSummaries;
}

