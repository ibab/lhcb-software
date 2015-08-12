// $Id: TESTisTos.cpp,v 1.4 2010-07-21 21:22:16 tskwarni Exp $
// Include files 
#include <algorithm>
#include <vector>
#include <sstream>

// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"

// local
#include "TESTisTos.h"

#include "boost/regex.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TESTisTos
//
// 2007-08-20 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TESTisTos )

const std::vector< std::string > TESTisTos::m_empty_selections = std::vector< std::string >();

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TESTisTos::TESTisTos( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TESSelectionTisTos ( type, name , parent )
{
  declareInterface<ITriggerTisTos>(this);

  declareProperty("TriggerInputWarnings", m_trigInputWarn = false );
  declareProperty("AllowToPreloadTES", m_preLoad = true );

}
//=============================================================================
// Destructor
//=============================================================================
TESTisTos::~TESTisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TESTisTos::initialize() {
  StatusCode sc = TESSelectionTisTos::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  setOfflineInput();
  setTriggerInput();
  
  return StatusCode::SUCCESS;
}

//=============================================================================

void TESTisTos::getTriggerNames(const std::string & selectionNameWithWildChar)
{

  std::string preloadPath="None";  
  if( selectionNameWithWildChar.find('/') != 0 ){
    // if string does not start with / then relative to PrefixInTES
    preloadPath = m_PrefixInTES.value();
    if( preloadPath.find("/Event") != 0 ){
      preloadPath = "None";
    }    
  } else {
    // otherwise absolute path
    
    // find first regex special character if any
    unsigned int found = selectionNameWithWildChar.size();    
    for(unsigned int i=0; i< selectionNameWithWildChar.size(); ++i ){
      if( std::string(".*[^$|?+(){}\\").find(selectionNameWithWildChar.substr(i,i+1))
          != std::string::npos ){
        found=i;
        break;
      }
    }
    if( found == 0 ){
      preloadPath="/Event";
    } else if( found < selectionNameWithWildChar.size() ){      
      preloadPath = selectionNameWithWildChar.substr(0,found);      
    }    
    
  }
  if( preloadPath.find_last_of("/") == (preloadPath.size()-1) ){
    preloadPath = preloadPath.substr(0,preloadPath.size()-1);
  }

  m_triggerNames.clear(); 
  exploreTES("/Event",preloadPath);

  if( m_triggerNames.empty() ){
    Warning( "No TES locations found" , StatusCode::FAILURE, 1 ).setChecked();
  }
  
}

void TESTisTos::setTriggerInput()
{
  m_triggerInput_Selections.clear();
  m_newEvent = false;
}

void TESTisTos::addToTriggerInput( const std::string & selectionNameWithWildChar)
{
#if 0
  if( m_newEvent )setTriggerInput();
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
  getTriggerNames(selectionNameWithWildChar);
  boost::regex expr;
  if( selectionNameWithWildChar.find("/") == 0 ){    
    expr = boost::regex(selectionNameWithWildChar);
  } else {
    expr = boost::regex(m_PrefixInTES.value() + selectionNameWithWildChar);
  }
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
 
unsigned int TESTisTos::tisTosTrigger()
{
  unsigned int result=0;  
  if( m_newEvent )setTriggerInput();
  if( m_triggerInput_Selections.empty() ){
    if( m_trigInputWarn )Warning(" triggerTisTos called with empty Trigger Input").setChecked();
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

std::string TESTisTos::analysisReportTrigger()
{
  std::ostringstream report;
  report << offset() 
         << " Trigger #-of-sel " <<   m_triggerInput_Selections.size() << std::endl;
  unsigned int result=0;  
  if( m_newEvent )setTriggerInput();
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
bool TESTisTos::tosTrigger(){ 
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
bool TESTisTos::tisTrigger(){
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

// fast check for TIS
bool TESTisTos::tusTrigger(){
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




std::vector< std::string > TESTisTos::triggerSelectionNames(unsigned int decisionRequirement,
                                                            unsigned int tisRequirement,
                                                            unsigned int tosRequirement,
                                                            unsigned int tpsRequirement)
{
  if( m_newEvent )setTriggerInput();
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



std::vector<const LHCb::HltObjectSummary*> TESTisTos::hltObjectSummaries( unsigned int tisRequirement,
                                                                          unsigned int tosRequirement,
                                                                          unsigned int tpsRequirement)
{
  if( m_newEvent )setTriggerInput();
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

  
void TESTisTos::exploreTES(const std::string & path, const std::string & preloadPath )
{
  SmartIF<IDataManagerSvc> mgr( evtSvc() );
  if( exist<DataObject>(path) ){
    m_triggerNames.push_back(path);    
    DataObject* pObj = get<DataObject>(path);
    IRegistry* reg =  pObj->registry();
    typedef std::vector<IRegistry*> Leaves;
    Leaves leaves;
    StatusCode sc = mgr->objectLeaves(reg, leaves);
    if ( sc.isSuccess() )  {
      const std::string* par0 = 0;
      if ( reg->address() )  {
        par0 = reg->address()->par();
      }
      for ( Leaves::const_iterator leave=leaves.begin(); leave != leaves.end(); ++leave )   {
        const std::string& pathid = (*leave)->identifier();
        if ( (*leave)->address() )  {
          if ( par0 )  {
            const std::string* par1 = (*leave)->address()->par();
            if ( par1 )  {
              // do not access forign 
              if ( par0[0] != par1[0] )  {
                continue;
              }
            }
          }
        }
        bool preload(false);        
        if ( m_preLoad )  {
          if( pathid.find(preloadPath) == 0 ){            
            preload = true;
          } else if( (pathid != "/Event") && ( preloadPath.find(pathid) == 0 ) ){
            preload = true;
          }
        }
        DataObject* p = 0;
        if( preload ){
          sc = evtSvc()->retrieveObject(pathid, p);
        } else {
          sc = evtSvc()->findObject(pathid, p);
        }
        if( sc.isSuccess() ){
          exploreTES( pathid, preloadPath );
        }
      }
    }
  }
}
