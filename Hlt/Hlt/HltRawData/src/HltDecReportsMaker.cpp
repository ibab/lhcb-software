// $Id: HltDecReportsMaker.cpp,v 1.3 2008-09-17 16:14:56 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltSummary.h"
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

  declareProperty("InputHltSummaryLocation",
    m_inputHltSummaryLocation= LHCb::HltSummaryLocation::Default);  
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

  // create output container and put it on TES
  HltDecReports* outputSummary = new HltDecReports();
  put( outputSummary, m_outputHltDecReportsLocation );


  // initialize global decisions

  // form global decisions if neccessary
  unsigned int hlt1global(0);
  unsigned int hlt1globalC(0);
  unsigned int errorsHlt1(0);  
  const HltDecReport* hlt1DecReport(0);
  unsigned int hlt2global(0);
  unsigned int hlt2globalC(0);
  unsigned int errorsHlt2(0);  
  const HltDecReport* hlt2DecReport(0);
  unsigned int l0global(0);
  unsigned int l0globalC(0);
  unsigned int errorsL0(0);  
  const HltDecReport* l0DecReport(0);
  

  // --------------------------------- get configuration --------------------

  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  

  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); // new style
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );
  if( hlt1.empty() ){ errorsHlt1 |= kNotConfigured;  }

  for( std::vector<IANNSvc::minor_value_type>::const_iterator si=hlt1.begin();
       si!=hlt1.end();++si){
    const std::string & selName = si->first;
     if( selName.find("Decision") != std::string::npos ){
       if( selName.find("Hlt1") != std::string::npos ){
         hlt1globalC += 1;
       } else if( selName.find("Hlt2") != std::string::npos ){ 
         //         hlt2globalC += 1;
       } else if( selName.find("L0") != std::string::npos ){ 
         l0globalC += 1;
       } else {
         Warning(" Unrecognized type of decision in configuration selection name="+selName, StatusCode::SUCCESS, 10 );
       }
     }
  }
  

  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );
  if( !hlt2.size() ){ errorsHlt2 |= kNotConfigured;  }

  for( std::vector<IANNSvc::minor_value_type>::const_iterator si=hlt2.begin();
       si!=hlt2.end();++si){
    const std::string & selName = si->first;
     if( selName.find("Decision") != std::string::npos ){
       if( selName.find("Hlt1") != std::string::npos ){
         //         hlt1globalC += 1;
       } else if( selName.find("Hlt2") != std::string::npos ){ 
         hlt2globalC += 1;
       } else if( selName.find("L0") != std::string::npos ){ 
         //        l0globalC += 1;
       } else {
         Warning(" Unrecognized type of decision in configuration selection name="+selName, StatusCode::SUCCESS, 10 );
       }
     }
  }
  
  if( !hlt1globalC )errorsHlt1 |= kDecisionNotConfigured;
  if( !hlt2globalC )errorsHlt2 |= kDecisionNotConfigured;
  if( !l0globalC )errorsL0 |= kDecisionNotConfigured;
  

  // ------------------------------- get input summary ------------------------

  // get input selection summary
  if( !exist<HltSummary>(m_inputHltSummaryLocation) ){    
    Warning(" No HltSummary at "+ m_inputHltSummaryLocation.value(),StatusCode::SUCCESS, 50 );

    //       hlt1 configured but no hlt summary
    int dec(0);
    int noc(0); 

    errorsHlt1 |= kNoHltSummary;    
    hlt1DecReport = new HltDecReport( dec, errorsHlt1, noc, kHlt1GlobalID );    
    outputSummary->insert( "Hlt1Global", *hlt1DecReport );


    errorsHlt2 |= kNoHltSummary;    
    hlt2DecReport = new HltDecReport( dec, errorsHlt2, noc, kHlt2GlobalID );    
    outputSummary->insert( "Hlt2Global", *hlt2DecReport );

    errorsL0 |= kNoHltSummary;    
    l0DecReport = new HltDecReport( dec, errorsL0, noc, kL0GlobalID );    
    outputSummary->insert( "L0Global", *l0DecReport );

    return StatusCode::SUCCESS;  
  }  
  HltSummary* inputSummary = get<HltSummary>(m_inputHltSummaryLocation);

  // ------------------------------- form or find global decisions ----------


  // selection names in HltSummary
  std::vector<std::string> selIDs = inputSummary->selectionSummaryIDs();


  // loop over selection summaries in HltSummary
  for( std::vector<std::string>::const_iterator is=selIDs.begin(); is!=selIDs.end(); ++is ){
     const std::string selName(*is);    

     const LHCb::HltSelectionSummary* selSumIn = &(inputSummary->selectionSummary(selName));     

     if( selName == "Hlt1Global" ){
       if( !hlt1DecReport ){
         // errorsHlt1 |= selSumIn->errors();         
         hlt1DecReport = new HltDecReport( selSumIn->decision() , errorsHlt1, selSumIn->data().size(), kHlt1GlobalID ); 
       }       
     } else if ( selName == "Hlt2Global" ){
       if( !hlt2DecReport ){
         // errorsHlt2 |= selSumIn->errors();         
         hlt2DecReport = new HltDecReport( selSumIn->decision() , errorsHlt2, selSumIn->data().size(), kHlt2GlobalID ); 
       }       
     } else if ( selName == "L02Global" ){
       if( !l0DecReport ){
         // errorsL0 |= selSumIn->errors();         
         l0DecReport = new HltDecReport( selSumIn->decision(), errorsL0, selSumIn->data().size(), kL0GlobalID ); 
       }       
     }

     if( !(selSumIn->decision()) )continue;
     

     unsigned int infoLevel = infoLevelFlag( selName );
     if( !infoLevel )continue;


     if( selName.find("Decision") != std::string::npos ){
       if( selName.find("Hlt1") != std::string::npos ){
         hlt1global += 1;
       } else if( selName.find("Hlt2") != std::string::npos ){ 
         hlt2global += 1;
       } else if( selName.find("L0") != std::string::npos ){ 
         l0global += 1;
       } else {
         Warning(" Unrecognized type of decision selection in HltSummary name="+selName,StatusCode::SUCCESS, 10 );
       }
     }

     int intSelID(0);     
     for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
          si!=selectionNameToIntMap.end();++si){
       if( si->first == selName ){
         intSelID=si->second;
         break;
       }
     }
     if( !intSelID ){
       Warning( " selectionName=" + selName + " from HltSummary not found in HltANNSvc. Skipped. ",StatusCode::SUCCESS, 20 );
       continue;
     }
     
  }
  
  if( !hlt1DecReport ){
    hlt1DecReport = new HltDecReport( hlt1global , errorsHlt1, hlt1global , kHlt1GlobalID ); 
  }
  outputSummary->insert( "Hlt1Global" , *hlt1DecReport );
  
  if( !hlt2DecReport ){
    hlt2DecReport = new HltDecReport( hlt2global , errorsHlt2, hlt2global , kHlt2GlobalID ); 
  }
  outputSummary->insert( "Hlt2Global" , *hlt2DecReport );

  if( !l0DecReport ){
    l0DecReport = new HltDecReport( l0global , errorsL0, l0global , kL0GlobalID ); 
  }
  outputSummary->insert( "L0Global" , *l0DecReport );
  

  // ---------------------------------------------------------------------------------

  // loop over all selections in this configuration
  for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
       si!=selectionNameToIntMap.end();++si){
    const std::string & selName = si->first;

    // skip over global decisions (done already)
    if( selName == "Hlt1Global" )continue;    
    if( selName == "Hlt2Global" )continue;    
    if( selName == "L0Global" )continue;    

     // don't bother if duplicate selection 
     if( outputSummary->hasSelectionName(selName) ){
         Warning(" duplicate selection ignored selectionName=" + selName, StatusCode::SUCCESS, 10 );
         continue;        
     }


     // filter selection for persistency
     unsigned int infoLevel = infoLevelFlag( selName );
     if( !infoLevel )continue;



     const LHCb::HltSelectionSummary* selSumIn(0);
     if(  inputSummary->hasSelectionSummary(selName) ) selSumIn = &(inputSummary->selectionSummary(selName));

     if( !(kAlwaysInfoLevel & infoLevel) ){       
       if( infoLevel & kPassInfoLevel ){      
         if( !selSumIn )continue;       
         if( !(selSumIn->decision()) )continue;
       }
       //if( infoLevel & kErrorsInfoLevel ){
       //  if( !selSumIn )continue;       
       //  if( !(selSumIn->errors()) )continue;
       //}
     }


     // save selection decision ---------------------------

     // int selection id
     int intSelID = si->second;
     
   
     // decision
     int dec(0);
     // errorbits
     unsigned int errorBits(0);
     // number of candidates
     int noc(0);
     
     if( selSumIn ){ 
       if( selSumIn->decision() )dec=1;
       // errorBits = selSumIn->errors(); 
        noc = selSumIn->data().size();
        if( !noc ){ noc=selSumIn->particles().size();   }      
     }
     
     HltDecReport* selSumOut = new HltDecReport( dec, errorBits, noc, intSelID );

     if( selSumOut->invalidIntSelectionID() ){
       delete selSumOut;
       std::ostringstream mess;
       mess << " selectionName=" << selName << " has invalid intSelectionID=" << intSelID << " Skipped. ";
       Warning( mess.str(), StatusCode::SUCCESS, 20 );
       continue;
     }
     
     // insert selection into the container
     if( outputSummary->insert( selName, *selSumOut ) == StatusCode::FAILURE ){
       Warning( " Failed to add HltDecReport selectionName=" + selName 
                + " to its container ", StatusCode::SUCCESS, 20 );
     }    

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
