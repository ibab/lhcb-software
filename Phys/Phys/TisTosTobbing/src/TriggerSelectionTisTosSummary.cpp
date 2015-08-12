// $Id: TriggerSelectionTisTosSummary.cpp,v 1.22 2010-07-21 21:22:17 tskwarni Exp $
//#define LOCDEBUG
// Include files 
#include <algorithm>
#include <sstream>
#include "boost/algorithm/string/replace.hpp"

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "Kernel/Particle2UnsignedInts.h"


// local
#include "TriggerSelectionTisTosSummary.h"

#include "Event/HltDecReports.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerSelectionTisTosSummary
//
// 2010-10-18 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerSelectionTisTosSummary )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerSelectionTisTosSummary::TriggerSelectionTisTosSummary( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : ParticleTisTos ( type, name , parent )
  , m_hltDecReports(0)
  , m_hltDecReportsL0(0)
{
  declareInterface<ITriggerSelectionTisTos>(this);

  declareProperty("HltDecReportsLocation",
                  m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default); 
  declareProperty("L0HltDecReportsLocation",
                  m_HltDecReportsLocationL0 = "HltLikeL0/DecReports" );
 
  m_cached_SelectionNames.reserve(500);
  m_cached_tisTosTob.reserve(500);
 
}


//=============================================================================
// Destructor
//=============================================================================
TriggerSelectionTisTosSummary::~TriggerSelectionTisTosSummary() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerSelectionTisTosSummary::initialize() {
  StatusCode sc = ParticleTisTos::initialize(); // must be executed first
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
  
  
  return StatusCode::SUCCESS;

}


void TriggerSelectionTisTosSummary::handle(const Incident& ) 
{
  m_hltDecReports=0;
  m_hltDecReportsL0=0;

  setOfflineInput( );

  m_newEvent=true;  
  
}


// ------------------------------------------------------------------------------------
void TriggerSelectionTisTosSummary::getHltSummary()
{
  if( !m_hltDecReports ){
    if( exist<HltDecReports>(m_HltDecReportsLocation) ){    
      m_hltDecReports = get<HltDecReports>(m_HltDecReportsLocation);
    } else {
      Error( " No HltDecReports at "+m_HltDecReportsLocation.value(), StatusCode::FAILURE, 2 ).setChecked();
      m_hltDecReports =0;
    }    
  }  
  if( !m_hltDecReportsL0 ){
    if( exist<HltDecReports>(m_HltDecReportsLocationL0) ){    
      m_hltDecReportsL0 = get<HltDecReports>(m_HltDecReportsLocationL0);
    } else {
      // Error( " No HltDecReports at "+m_HltDecReportsLocationL0.value(), StatusCode::FAILURE, 2 ).setChecked();
      m_hltDecReportsL0 =0;
    }    
  }  
}

//=============================================================================
// -------------- offline inputs -----------------------------------------------
//=============================================================================

// erase previous input ---------------------------------------------------------
void TriggerSelectionTisTosSummary::setOfflineInput( )
{
  clearCache();
}
   

//    data object signal  ---------------------------------------------------------------
bool TriggerSelectionTisTosSummary::addParticleTisTosSummary( const LHCb::Particle & object )
{
  // try global map at RootInTES location
  DaVinci::Map::Particle2UnsignedInts* p2tisTosSummary(0);
  if( exist< DaVinci::Map::Particle2UnsignedInts >("Particle2TisTos") ){
    p2tisTosSummary = get< DaVinci::Map::Particle2UnsignedInts >("Particle2TisTos");    
  } else {
    const DataObject* container = object.parent(); 
    if( !container ){
      Error("Particle passed as OfflineInput not in a container - no TisTossing", StatusCode::FAILURE ,10).setChecked();
      return false;   
    }
    IRegistry* registry =  container->registry();
    if( !registry ){
      Error("Particle passed as OfflineInput is in a container which is not on TES - no TisTossing", 
            StatusCode::FAILURE ,10).setChecked();
      return false;   
    }
    std::string path = registry->identifier();
    // local map
    boost::replace_last(path,"/Particles","/Particle2TisTos");
    if( exist< DaVinci::Map::Particle2UnsignedInts >(path,false) ){
      p2tisTosSummary = get< DaVinci::Map::Particle2UnsignedInts >(path,false);
    } else {
      // global map at stream location (find first "/" past "/Event/")
      std::size_t ipos = path.find("/",7);            
      if( ipos != std::string::npos ){              
        std::string pathg = path.substr(0,ipos)+"/Particle2TisTos";
        if( exist< DaVinci::Map::Particle2UnsignedInts >(pathg,false) ){
          p2tisTosSummary = get< DaVinci::Map::Particle2UnsignedInts >(pathg,false);
        } else {
          Error("No /Particle2TisTos for the Particle passed as OfflineInput - no TisTossing", StatusCode::FAILURE ,10).setChecked();
          return false;   
        }
      }
    }
  }
  if( !p2tisTosSummary ){
    Error("No /Particle2TisTos for the Particle passed as OfflineInput - no TisTossing", StatusCode::FAILURE ,10).setChecked();
    return false;   
  }
  std::vector<unsigned int> tisTosSummary = (*p2tisTosSummary)[&object];
  return addTisTosSummary(  tisTosSummary );
}


//    data object signal  ---------------------------------------------------------------
bool TriggerSelectionTisTosSummary::addTisTosSummary(std::vector<unsigned int> & tisTosSummary)
{
  // add to existing TisTosSummary
  if( m_cached_tisTosTob.size() ){
    if( tisTosSummary.size() != m_cached_tisTosTob.size() ){
      Error( " TisTosSummary size different from previous TisTosSummary size ", StatusCode::FAILURE, 10 ).setChecked();	
      return false;
    } 
    std::vector<unsigned int>::iterator iResult=tisTosSummary.begin();
    for( std::vector<unsigned int>::iterator iPrevResult=m_cached_tisTosTob.begin();iPrevResult!=m_cached_tisTosTob.end();
         ++iPrevResult,++iResult){
      unsigned int & resultToAdd = *iResult;
      unsigned int & resultPrev = *iPrevResult;
      TisTosTob updatedResult(0);
      updatedResult.set_decision( (resultPrev & kDecision) != 0 );
      // this is not going to give accurate results (some TOS and TIS may be missed, but if set not to true not wrong)
      updatedResult.set_tos( ( ( resultPrev & kTOS ) | (resultToAdd & kTOS ) ) != 0 ); 
      updatedResult.set_tis( ( ( resultPrev & kTIS ) & (resultToAdd & kTIS ) ) != 0 ); 
      updatedResult.set_tps( ( ( ( resultPrev & kTPS ) | (resultToAdd & kTPS ) ) != 0 ) && ( !updatedResult.tos() ) );
      resultPrev = updatedResult.value();
    }
    return true;
  } else {
    // create cached results
    // get trigger names from HltDecReports and HltDecReportsL0
    if( !m_hltDecReports ) getHltSummary();
    if( !m_hltDecReports ){
      Error( " No HltDecReports available - cannot interprete results. No TisTossing possible. ", 
             StatusCode::FAILURE, 10 ).setChecked();	
      return false;
    }
    std::vector<std::string> triggerNames = m_hltDecReports->decisionNames();
    if( tisTosSummary.size() < triggerNames.size() ){
      Error( " TisTosSummary size < HltDecReports size ", StatusCode::FAILURE, 10 ).setChecked();	
      return false;
    } 
    std::vector<unsigned int>::iterator iResult=tisTosSummary.begin();
    for(std::vector<std::string>::const_iterator iDecName=triggerNames.begin();iDecName!=triggerNames.end();++iDecName,++iResult){
      m_cached_SelectionNames.push_back(*iDecName);
      m_cached_tisTosTob.push_back(*iResult);
    }
    if( tisTosSummary.size() > triggerNames.size() ){
      if( !m_hltDecReportsL0 ){
	Warning( " TisTosSummary size > HltDecReports size but no L0 HltDecReports ", StatusCode::SUCCESS, 10 ).setChecked();	
	return true;
      } 
      std::vector<std::string> triggerNamesL0 = m_hltDecReportsL0->decisionNames();
      if( tisTosSummary.size() != ( triggerNames.size() + triggerNamesL0.size() ) ){
	Warning( " TisTosSummary size != HltDecReports + L0 size ", StatusCode::SUCCESS, 10 ).setChecked();	
	return true;
      } 
      for( std::vector<std::string>::const_iterator iDecName=triggerNamesL0.begin();iDecName!=triggerNamesL0.end();
           ++iDecName,++iResult){
	m_cached_SelectionNames.push_back(*iDecName);
	m_cached_tisTosTob.push_back(*iResult);
      }
    }
  }
  return true;
}



//    hit list input ---------------------------------------------------------------
void TriggerSelectionTisTosSummary::addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist )
{
  // try to pass lhcbids as TisTosSummary 
  std::vector<unsigned int> tisTosSummary;
  for(std::vector<LHCb::LHCbID>::const_iterator i=hitlist.begin();i!=hitlist.end();++i){
    unsigned int result = i->lhcbID();
    // not a legal TisTos result value?
    if( result > 15 ){
      Error("Cannot handle LHCbID Offline input - no TisTossing",StatusCode::SUCCESS,10).setChecked();
      clearCache();  
    }
    tisTosSummary.push_back( result );
  }
  if( addTisTosSummary(  tisTosSummary ) )return;
  Error("Cannot handle LHCbID Offline input - no TisTossing",StatusCode::SUCCESS,10).setChecked();
  clearCache();  
}

//    Track input ---------------------------------------------------------------
void TriggerSelectionTisTosSummary::addToOfflineInput( const LHCb::Track &  )
{
  Error("Cannot handle Track input - no TisTossing",StatusCode::SUCCESS,10).setChecked();
  clearCache();
}

//    Proto-particle input -----------------------------------------------------------------------
void TriggerSelectionTisTosSummary::addToOfflineInput( const LHCb::ProtoParticle &  )
{
  Error("Cannot handle ProtoParticle input - no TisTossing",StatusCode::SUCCESS,10).setChecked();
  clearCache();
}

//    Particle input -----------------------------------------------------------------------
void TriggerSelectionTisTosSummary::addToOfflineInput( const LHCb::Particle & particle )
{
  if( !addParticleTisTosSummary( particle ) )clearCache();
}


//=============================================================================
// -------------- outputs:
//=============================================================================


// single complete Trigger Selection TisTos  (define Offline Input before calling)
unsigned int TriggerSelectionTisTosSummary::tisTosSelection( const std::string & selectionName ){
  unsigned int result=0;
  if( findInCache( selectionName, result ) )return result;
  return result;
}

// analysisReport for Selection (define Offline Input before calling)
std::string TriggerSelectionTisTosSummary::analysisReportSelection( const std::string & selectionName ){
  std::ostringstream report;
  report << offset() << " Selection " + selectionName + " ";  

  unsigned int result=0;

  getHltSummary();

  bool decision(false);  
  // get decision from HltDecReports if can find it
  if( m_hltDecReports ){
    const HltDecReport* rep=m_hltDecReports->decReport( selectionName );
    if( rep ){
      decision = rep->decision();
      report << " HltDecReport decision=" << decision;      
    }    
  }
  //  if( decision )result |= kDecision;  

  if( !findInCache( selectionName, result ) ){
    report << " not found in TisTosSummary " << std::endl; 
    return report.str();
  }

  // if( decision )result |= kDecision;  
  report << std::endl;
  //++m_reportDepth;
  //report << analysisReport( *sel );  
  //--m_reportDepth;
  TisTosTob res( result );
  report << offset()  
         << " Selection " + selectionName + " TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() 
         << " decision= " << res.decision() << " Cached result " << std::endl;
  return report.str();
}

// fast check for TOS
bool TriggerSelectionTisTosSummary::tosSelection(const std::string & selectionName ){
  unsigned int result=0;
  if( !findInCache( selectionName, result ) )return false;
  if( result & kTOS )return true;
  return false;
}

// fast check for TIS
bool TriggerSelectionTisTosSummary::tisSelection(const std::string & selectionName ){
  unsigned int result=0;
  if( !findInCache( selectionName, result ) )return false;
  if( result & kTIS )return true;
  return false;
}

// fast check for TOS
bool TriggerSelectionTisTosSummary::tusSelection(const std::string & selectionName ){
  unsigned int result=0;
  if( !findInCache( selectionName, result ) )return false;
  if( result & kTUS )return true;
  return false;
}



// ------------ auxiliary output:  list of LHCbIDs corresponding to present offline input
std::vector<LHCb::LHCbID> TriggerSelectionTisTosSummary::offlineLHCbIDs() 
{
  // return TisTosSummary instead!
  std::vector<LHCb::LHCbID> fakeLHCbIDs;
  for( std::vector<unsigned int>::iterator iResult=m_cached_tisTosTob.begin();iResult!=m_cached_tisTosTob.end();++iResult){
    fakeLHCbIDs.push_back( LHCb::LHCbID( *iResult ) );
  }
  return fakeLHCbIDs;
}

// ------------  additional functionality:  lists of object summaries for tracks/vertices/particles from trigger selections
//               satisfying TIS, TOS requirements

std::vector<const LHCb::HltObjectSummary*> TriggerSelectionTisTosSummary::hltSelectionObjectSummaries(
                                                                                          const std::string & ,
                                                                                                unsigned int ,
                                                                                                unsigned int ,
                                                                                                unsigned int )
{
  // This is a dummy version since no such info available
  std::vector<const LHCb::HltObjectSummary*> matchedObjectSummaries;
  return matchedObjectSummaries;
}

