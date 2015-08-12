// $Id: TriggerTisTosSummary.cpp,v 1.15 2010-07-21 21:22:17 tskwarni Exp $
// Include files 
#include <algorithm>
#include <sstream>

// local
#include "TriggerTisTosSummary.h"

#include "boost/regex.hpp"

#include "Event/HltDecReports.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerTisTosSummary
//
// 2010-10-18 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TriggerTisTosSummary )

const std::vector< std::string > TriggerTisTosSummary::m_empty_selections = std::vector< std::string >();

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TriggerTisTosSummary::TriggerTisTosSummary( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TriggerSelectionTisTosSummary ( type, name , parent )
{
  declareInterface<ITriggerTisTos>(this);

  declareProperty("TriggerInputWarnings", m_trigInputWarn = false );

}
//=============================================================================
// Destructor
//=============================================================================
TriggerTisTosSummary::~TriggerTisTosSummary() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerTisTosSummary::initialize() {
  StatusCode sc = TriggerSelectionTisTosSummary::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  setOfflineInput();
  setTriggerInput();
  
  return StatusCode::SUCCESS;
}

//=============================================================================


void TriggerTisTosSummary::getTriggerNames()
{
  if( m_newEvent ){
    m_triggerNames.clear(); 
    m_newEvent = false;
  }

  // done before ?
  if( !(m_triggerNames.empty()) ){ return; }


  // get trigger names from HltDecReports
  if( !m_hltDecReports ) getHltSummary();
  //    for the same TCK this should be fixed list for events which passed Hlt
  if( m_hltDecReports ){
    m_triggerNames = m_hltDecReports->decisionNames();
  }
  
  if( m_hltDecReportsL0 ){
    std::vector< std::string > triggerNamesL0 = m_hltDecReportsL0->decisionNames();
    m_triggerNames.insert(m_triggerNames.end(),triggerNamesL0.begin(),triggerNamesL0.end());
  }
  
  if( m_triggerNames.empty() ){
    Warning( "No known trigger names found" , StatusCode::FAILURE, 1 ).setChecked();
  }
  
}

void TriggerTisTosSummary::setTriggerInput()
{
  m_triggerInput_Selections.clear();
}

void TriggerTisTosSummary::addToTriggerInput( const std::string & selectionNameWithWildChar)
{
#if 0  
  // if selectionNameWithWildChar contains a * without a . in front of it: print warning...
  static boost::regex warn("[^\\.]\\*");
  if ( boost::regex_search( selectionNameWithWildChar, warn )  ) {
        Warning( " addToTriggerInput now does Posix (Perl) regular expression matching instead of globbing;" 
                 " this implies that eg. a '*' should be replaced by '.*'. You've specified "
                 " a selectionName using a '*', without leading '.': '" + selectionNameWithWildChar 
               + "'. Please verify whether this is what you still want\n "
               " For more information on the supported syntax, please check "
               " http://www.boost.org/doc/libs/1_39_0/libs/regex/doc/html/boost_regex/syntax/perl_syntax.html"
                 ,StatusCode::SUCCESS 
                 ).ignore();
  }
#endif
  unsigned int sizeAtEntrance( m_triggerInput_Selections.size() );
  getTriggerNames();
  boost::regex expr(selectionNameWithWildChar);
  for( std::vector< std::string >::const_iterator inpt=m_triggerNames.begin();inpt!=m_triggerNames.end();++inpt){
    if( boost::regex_match( *inpt, expr ) ) {
      if( find( m_triggerInput_Selections.begin(), m_triggerInput_Selections.end(), *inpt ) 
          == m_triggerInput_Selections.end() ){
        m_triggerInput_Selections.push_back(*inpt);
      }
    }
  }
  if( m_trigInputWarn && (m_triggerInput_Selections.size()==sizeAtEntrance) ){
    std::ostringstream mess;
    mess << " addToTriggerInput called with selectionNameWithWildChar=" << selectionNameWithWildChar
         << " added no selection to the Trigger Input, which has size=" << m_triggerInput_Selections.size();
    Warning( mess.str(),StatusCode::SUCCESS, 50 ).setChecked();
  }
}
 

unsigned int TriggerTisTosSummary::tisTosTrigger()
{
  unsigned int result=0;  
  if( m_triggerInput_Selections.empty() ){
    if( m_trigInputWarn )Warning(" tisTosTrigger called with empty Trigger Input").setChecked();
    return result;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    unsigned int res = tisTosSelection( *iTriggerSelection );
    if( res & kDecision ){      
      result |= res;
    }  
    if( (result & kTOS) && (result & kTIS) && (result & kTPS ) )break;  
  }
  return result;  
}

// analysisReport for Trigger (define Trigger & Offline Input before calling)
std::string TriggerTisTosSummary::analysisReportTrigger(){
  std::ostringstream report;
  report << offset() 
         << " Trigger #-of-sel " <<   m_triggerInput_Selections.size() << std::endl;
  unsigned int result=0;  
  if( m_triggerInput_Selections.empty() )
    { report << "Trigger Input empty" << std::endl; return report.str();}
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    unsigned int res = tisTosSelection( *iTriggerSelection );
    ++m_reportDepth;
    if( res & kDecision ){      
      report << analysisReportSelection( *iTriggerSelection );
      result |= res;
    } else {
      report << offset() << " Selection " + *iTriggerSelection + " decision=false " << std::endl;      
    }
    --m_reportDepth;
    
    //    if( (result & kTOS) && (result & kTIS) && (result & kTPS ) )break;  
  }
  TisTosTob res( result );
  report << offset() 
         << " Trigger #-of-sel " <<   m_triggerInput_Selections.size()
         << " TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() 
         << " decision= " << res.decision() << std::endl;
  return report.str();
}


// fast check for TOS
bool TriggerTisTosSummary::tosTrigger(){ 
  if( m_triggerInput_Selections.empty() ){
    if( m_trigInputWarn )Warning(" tosTrigger called with empty Trigger Input").setChecked();
    return false;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    if( tosSelection( *iTriggerSelection ) )return true;
  }
  return false;  
}

// fast check for TIS
bool TriggerTisTosSummary::tisTrigger(){
  if( m_triggerInput_Selections.empty() ){
    if( m_trigInputWarn )Warning(" tisTrigger called with empty Trigger Input").setChecked();
    return false;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    if( tisSelection( *iTriggerSelection ) )return true;
  }
  return false;  
}

// fast check for TUS
bool TriggerTisTosSummary::tusTrigger(){
  if( m_triggerInput_Selections.empty() ){
    if( m_trigInputWarn )Warning(" tpsTrigger called with empty Trigger Input").setChecked();
    return false;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    if( tusSelection( *iTriggerSelection ) )return true;
  }
  return false;  
}

std::vector< std::string > TriggerTisTosSummary::triggerSelectionNames(unsigned int decisionRequirement,
                                                                unsigned int tisRequirement,
                                                                unsigned int tosRequirement,
                                                                unsigned int tpsRequirement)
{ 
  if( (decisionRequirement>=kAnything) && ( tisRequirement>=kAnything) && ( tosRequirement>=kAnything) 
      && ( tpsRequirement>=kAnything) )
  {
    return m_triggerInput_Selections;
  }
  std::vector< std::string > selections;  
  for( std::vector<std::string>::const_iterator iSel = m_triggerInput_Selections.begin();
       iSel != m_triggerInput_Selections.end(); ++iSel ){
    bool decision,tis,tos,tps;
    unsigned int result = tisTosSelection( *iSel);
    decision = result & kDecision;
    tos = result & kTOS;
    tis = result & kTIS;
    tps = result & kTPS;    
    if( ((decisionRequirement>=kAnything)||(decision==decisionRequirement))
        && ((tisRequirement>=kAnything)||(tis==tisRequirement)) 
        && ((tosRequirement>=kAnything)||(tos==tosRequirement)) 
        && ((tpsRequirement>=kAnything)||(tps==tpsRequirement)) ){
      selections.push_back( *iSel );
    }
  }
  return selections;  
}

std::vector<const LHCb::HltObjectSummary*> TriggerTisTosSummary::hltObjectSummaries( unsigned int tisRequirement,
                                                                              unsigned int tosRequirement,
                                                                              unsigned int tpsRequirement)
{
  std::vector<const LHCb::HltObjectSummary*> hosVec;  
  if( m_triggerInput_Selections.empty() ){
    if( m_trigInputWarn )Warning(" hltObjectSummaries called with empty Trigger Input").setChecked();
    return hosVec;
  }
  for( std::vector< std::string >::const_iterator iTriggerSelection=m_triggerInput_Selections.begin();
       iTriggerSelection!=m_triggerInput_Selections.end(); ++iTriggerSelection){
    std::vector<const LHCb::HltObjectSummary*> selHosVec = 
      hltSelectionObjectSummaries(*iTriggerSelection,tisRequirement,tosRequirement,tpsRequirement);
    hosVec.insert(hosVec.end(),selHosVec.begin(),selHosVec.end());
  }
  return hosVec;
}

  

  
