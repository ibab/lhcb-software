// $Id: HltDecReportsMaker.cpp,v 1.8 2009-02-24 13:50:27 graven Exp $
#define DEBUGCODE
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltDecReports.h"

// local
#include "HltDecReportsMaker.h"

using namespace LHCb;


///-----------------------------------------------------------------------------
// Implementation file for class : HltDecReportsMaker
//
// 2008-07-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDecReportsMaker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDecReportsMaker::HltDecReportsMaker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{

  declareProperty("OutputHltDecReportsLocation",
    m_outputHltDecReportsLocation= LHCb::HltDecReportsLocation::Default);  

  declareProperty("DebugEventPeriod",m_debugPeriod = 0 );

  declareProperty("InfoLevelDecision", m_infoLevelDecision = ((unsigned int)kAlwaysInfoLevel) );
  declareProperty("InfoLevelDecisionDebug", m_infoLevelDecisionDebug = ((unsigned int)kAlwaysInfoLevel) );
  
  declareProperty("InfoLevelNonDecision", m_infoLevelNonDecision = ((unsigned int)kNoInfoLevel) );
  declareProperty("InfoLevelNonDecisionDebug", m_infoLevelNonDecisionDebug = ((unsigned int)kPassOrErrorsInfoLevel) );
  
  declareProperty("SelectionInfoLevel", m_infoLevel );
  declareProperty("SelectionInfoLevelDebug", m_infoLevelDebug );

  m_hltANNSvc = 0;

}
//=============================================================================
// Destructor
//=============================================================================
HltDecReportsMaker::~HltDecReportsMaker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecReportsMaker::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltBaseAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  m_debugMode=0;
  m_event=0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecReportsMaker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

 // determine output mode 
  ++m_event;
  if( m_event == m_debugPeriod ){
    // debug
    m_debugMode = 1;
    m_event =0;    
  } else {
    // normal
    m_debugMode = 0;
  }

  HltDecReports* outputSummary(0);  
  if( exist<HltDecReports>(m_outputHltDecReportsLocation) ){  
    // get existing decision report if exists (to be amended)
    outputSummary = get<HltDecReports>(m_outputHltDecReportsLocation);
  } else {
    // create output container and put it on TES
    outputSummary = new HltDecReports();
    put( outputSummary, m_outputHltDecReportsLocation );
  }
  
  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  

  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); 
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );

  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );


  // ----------------------------------------------------------------
  // form Global decisions if missing 
  // ----------------------------------------------------------------


  // Hlt1Global

  if( !outputSummary->hasSelectionName("Hlt1Global") ){
    
    int dec(0);
    unsigned int errors(0);  
    int noc(0);

    for(HltDecReports::Container::const_iterator it=outputSummary->begin();
        it!=outputSummary->end();++it){      
      const std::string & selName = it->first;
      if( ( selName.find("Hlt1") == 0 ) && 
          ( selName.find("Decision") != std::string::npos ) ) 
        if( it->second.decision() ){
          ++noc;
        }
    }
    if( noc )dec=1;
    
    bool config( noc!=0 );
    if( !config ){
      
      // see if configured (use dataSvc)
      std::vector<stringKey> selectionIDs = dataSvc().selectionKeys(); 
      for( std::vector<stringKey>::const_iterator si=selectionIDs.begin();
           si != selectionIDs.end(); ++si){
        const std::string & selName = si->str();

        if( selName.find("Hlt1") == 0 ){
          if( selName.find("Decision") != std::string::npos ){
            config=true;
            break;            
          }
        }
      }
      if( !config ){
        errors |= kDecisionNotConfigured;
      }
    }

    // create Hlt1Global dec report only when configured:
    if( config )outputSummary->insert( "Hlt1Global", HltDecReport( dec, errors, noc, kHlt1GlobalID ) );
#ifdef DEBUGCODE    
    if ( msgLevel(MSG::VERBOSE) ){
      if( config ){
        verbose() << " Hlt1Global HltDecReport created " << endmsg;
      }
    }
#endif    
  }

  // Hlt2Global

  if( !outputSummary->hasSelectionName("Hlt2Global") ){
    
    int dec(0);
    unsigned int errors(0);  
    int noc(0);

    for(HltDecReports::Container::const_iterator it=outputSummary->begin();
        it!=outputSummary->end();++it){      
      const std::string & selName = it->first;
      if( ( selName.find("Hlt2") == 0 ) && 
          ( selName.find("Decision") != std::string::npos ) ) 
        if( it->second.decision() ){
          ++noc;
        }
    }
    if( noc )dec=1;
    
    bool config( noc!=0 );
    if( !config ){
      
      // see if configured (use dataSvc)
      std::vector<stringKey> selectionIDs = dataSvc().selectionKeys(); 
      for( std::vector<stringKey>::const_iterator si=selectionIDs.begin();
           si != selectionIDs.end(); ++si){
        const std::string & selName = si->str();

        if( selName.find("Hlt2") == 0 ){
          if( selName.find("Decision") != std::string::npos ){
            config=true; 
            break;            
          }
        }
      }
      if( !config ){
        // for Hlt2 also try HltANNSvc
        for( std::vector<IANNSvc::minor_value_type>::const_iterator si=hlt2.begin();
             si!=hlt2.end();++si){
          const std::string & selName = si->first;
          if( selName.find("Decision") != std::string::npos ){
            if( selName.find("Hlt2") == 0 ){
              config=true;
              break;
            }
          }
        }
      }
      if( !config ){
        errors |= kDecisionNotConfigured;
      }
    }

    // create Hlt2Global dec report only when configured:
    if( config )outputSummary->insert( "Hlt2Global", HltDecReport( dec, errors, noc, kHlt2GlobalID ) );
#ifdef DEBUGCODE    
    if ( msgLevel(MSG::VERBOSE) ){
      if( config ){
        verbose() << " Hlt2Global HltDecReport created " << endmsg;
      }
    }
#endif    
    
  }

  if ( msgLevel(MSG::VERBOSE) ){
    // debugging info
    verbose() << " ====== HltDecReports container size=" << outputSummary->size() << endmsg;  
    verbose() << *outputSummary << endmsg;
  }
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltDecReportsMaker::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return HltBaseAlg::finalize();  // must be called after all other actions
}

//=============================================================================

//=============================================================================
// qualify trigger selections for persistency
//=============================================================================
unsigned int HltDecReportsMaker::infoLevelFlag( const std::string & selName )
{
  if( m_debugMode ){
    // DEBUG MODE

    // set info level
    const SelectionSetting & infoLevelDebug =  m_infoLevelDebug.value();    
    for( SelectionSetting::const_iterator i=infoLevelDebug.begin();
         i!=infoLevelDebug.end(); ++i){
      if( selName == i->first ){
        if( selName.find("Decision")!=std::string::npos )return (i->second | kPassInfoLevel);
        return i->second;
      }
    } 
    
    // not set specifically for this selection ; use default
    if( selName.find("Decision")!=std::string::npos ){
      return (m_infoLevelDecisionDebug.value() | kPassInfoLevel);      
    }
    return m_infoLevelNonDecisionDebug.value();

  } else {
    
    // NORMAL MODE

    // set info level
    const SelectionSetting & infoLevel =  m_infoLevel.value();    
    for( SelectionSetting::const_iterator i=infoLevel.begin();
         i!=infoLevel.end(); ++i){
      if( selName == i->first ){
        if( selName.find("Decision")!=std::string::npos )return (i->second | kPassInfoLevel);
        return i->second;
      }
    }
    
    // not set specifically for this selection ; use default
    if( selName.find("Decision")!=std::string::npos ){
      return (m_infoLevelDecision.value() | kPassInfoLevel);      
    }
    return m_infoLevelNonDecision.value();
  }

}
