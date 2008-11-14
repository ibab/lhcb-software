// $Id: TriggerTisTosHS.cpp,v 1.1 2008-11-14 06:55:39 tskwarni Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TriggerTisTosHS.h"

#include "HltBase/HltUtils.h"
#include "Event/HltSummary.h"
#include "Event/Particle.h"


using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerTisTosHS
//
// 2007-08-20 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerTisTosHS );

const std::vector< std::string > TriggerTisTosHS::m_empty_selections = std::vector< std::string >();

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerTisTosHS::TriggerTisTosHS( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TriggerSelectionTisTosHS ( type, name , parent )
{
  declareInterface<ITriggerTisTos>(this);

  m_hltANSvc = 0;
}
//=============================================================================
// Destructor
//=============================================================================
TriggerTisTosHS::~TriggerTisTosHS() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerTisTosHS::initialize() {
  StatusCode sc = TriggerSelectionTisTosHS::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_hltANSvc = svc<IANSvc>("HltANNSvc");

  setOfflineInput();
  setTriggerInput();
 
  return StatusCode::SUCCESS;
}

//=============================================================================


void TriggerTisTosHS::getTriggerNames()
{
  // done before ?
  if( m_triggerNames.size() !=0 ){ return; }

  // use HltANNSvc to get Hlt1, then Hlt2 names
  std::vector<std::string> selIDs = m_hltANSvc->keys("Hlt1SelectionID");
  for(std::vector<std::string>::const_iterator i =
          selIDs.begin(); i!=selIDs.end(); ++i) {
    if( find( m_triggerNames.begin(), m_triggerNames.end(), *i )
            == m_triggerNames.end() ){
          m_triggerNames.push_back(*i);
    }
  }
  {    
  std::vector<std::string> selIDs2 = m_hltANSvc->keys("Hlt2SelectionID");
  for(std::vector<std::string>::const_iterator i =
          selIDs2.begin(); i!=selIDs2.end(); ++i) {
    if( find( m_triggerNames.begin(), m_triggerNames.end(), *i )
            == m_triggerNames.end() ){
          m_triggerNames.push_back(*i);
    }
  }
  }
  
  if( m_triggerNames.size()==0 ){
    Error( "No known trigger names found" , StatusCode::FAILURE, 50 );
  }
  
}
void TriggerTisTosHS::refreshTriggerStructure()
{
  m_triggerNames.clear();  
}

void TriggerTisTosHS::setTriggerInput()
{
  m_triggerInput_Selections.clear();
}

void TriggerTisTosHS::addToTriggerInput( const std::string & selectionNameWithWildChar)
{
  unsigned int sizeAtEntrance( m_triggerInput_Selections.size() );
  getTriggerNames();
  for( std::vector< std::string >::const_iterator inpt=m_triggerNames.begin();inpt!=m_triggerNames.end();++inpt){
    if( wildcmp( selectionNameWithWildChar.c_str(), inpt->c_str()) ){ 
      if( find( m_triggerInput_Selections.begin(), m_triggerInput_Selections.end(), *inpt ) 
          == m_triggerInput_Selections.end() ){
        m_triggerInput_Selections.push_back(*inpt);
      }
    }
  }
  if( m_triggerInput_Selections.size()==sizeAtEntrance ){
    std::ostringstream mess;
    mess << " addToTriggerInput called with selectionNameWithWildChar=" << selectionNameWithWildChar
         << " added no selection to the Trigger Input, which has size=" << m_triggerInput_Selections.size();
    Warning( mess.str(),StatusCode::SUCCESS, 50 );
  }
}
 
std::vector< std::string > TriggerTisTosHS::triggerSelectionNames(unsigned int decisionRequirement,
                                                                unsigned int tisRequirement,
                                                                unsigned int tosRequirement )
{ 
  if( (decisionRequirement>=kAnything) && ( tisRequirement>=kAnything) && ( tosRequirement>=kAnything) )
  {
    return m_triggerInput_Selections;
  }
  std::vector< std::string > selections;  
  for( std::vector<std::string>::const_iterator iSel = m_triggerInput_Selections.begin();
       iSel != m_triggerInput_Selections.end(); ++iSel ){
    bool decision,tis,tos;
    selectionTisTos( *iSel,decision,tis,tos);
    if( ((decisionRequirement>=kAnything)||(decision==decisionRequirement))
        && ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
        && ((tosRequirement>=kAnything)||(tos==tosRequirement)) ){
      selections.push_back( *iSel );
    }
  }
  return selections;  
}

void TriggerTisTosHS::triggerTisTos( bool & decision, bool & tis, bool & tos)
{
  decision = false; tis=false; tos=false;
  if( m_triggerInput_Selections.size()==0 ){
    Warning(" triggerTisTos called with empty Trigger Input");
    return;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    bool decisionThis, tisThis, tosThis;
    selectionTisTos( *iTriggerSelection, decisionThis, tisThis, tosThis );
    decision = decision || decisionThis;
    tis = tis || tisThis;
    tos = tos || tosThis;
    if( tis && tos ){ break; }
  }
}

std::vector<const LHCb::HltObjectSummary*> TriggerTisTosHS::hltObjectSummaries( unsigned int tisRequirement,
                                                                                unsigned int tosRequirement)
{
  std::vector<const LHCb::HltObjectSummary*> hosVec;  
  if( m_triggerInput_Selections.size()==0 ){
    Warning(" hltObjectSummaries called with empty Trigger Input");
    return hosVec;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    std::vector<const LHCb::HltObjectSummary*> selHosVec = 
      hltSelectionObjectSummaries(*iTriggerSelection,tisRequirement,tosRequirement);
    hosVec.insert(hosVec.end(),selHosVec.begin(),selHosVec.end());
  }
  return hosVec;
}
