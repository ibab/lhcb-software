// $Id: HltVertexReportsWriter.cpp,v 1.2 2008-09-17 16:14:56 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RawEvent.h"
#include "Event/HltVertexReports.h"

// local
#include "HltVertexReportsWriter.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexReportsWriter
//
// 2008-08-05 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexReportsWriter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexReportsWriter::HltVertexReportsWriter( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{

  declareProperty("InputHltVertexReportsLocation",
    m_inputHltVertexReportsLocation= LHCb::HltVertexReportsLocation::Default);  
  declareProperty("OutputRawEventLocation",
    m_outputRawEventLocation= LHCb::RawEventLocation::Default);

  m_hltANNSvc = 0;

}
//=============================================================================
// Destructor
//=============================================================================
HltVertexReportsWriter::~HltVertexReportsWriter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltVertexReportsWriter::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltBaseAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexReportsWriter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get input hlt vertex reports
  if( !exist<HltVertexReports>(m_inputHltVertexReportsLocation) ){    
    return Warning( " No HltVertexReports at " + m_inputHltVertexReportsLocation.value(),
                    StatusCode::SUCCESS, 20 );
  }  
  HltVertexReports* inputSummary = get<HltVertexReports>(m_inputHltVertexReportsLocation);

 // get output
  if( !exist<RawEvent>(m_outputRawEventLocation) ){    
    return Error(" No RawEvent at " + m_outputRawEventLocation.value(),StatusCode::SUCCESS, 20 );
  }  
  RawEvent* rawEvent = get<RawEvent>(m_outputRawEventLocation);

  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  
  //    std::vector<IANNSvc::minor_value_type> hlt = m_hltANNSvc->items("SelectionID"); // old style
  //    selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt.begin(),hlt.end() );
  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); // new style
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );
  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );

  std::vector< unsigned int > hltVertexReportsRawBank;
  // first word will count number of vertex selections saved
  hltVertexReportsRawBank.push_back(0);
  
  // loop over vertex selections given in the input list
  for( HltVertexReports::Container::const_iterator is=inputSummary->begin();
       is!=inputSummary->end();++is){
     const std::string selName(is->first);     

     // save selection ---------------------------

     // int selection id
     int intSelID(0);   
     for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
          si!=selectionNameToIntMap.end();++si){
       if( si->first == selName ){
         intSelID=si->second;
         break;
       }
     }
     if( !intSelID ){
       Error(" selectionName=" +selName+ " from HltVertexReports not found in HltANNSvc. Skipped. ", StatusCode::SUCCESS, 20 ); 
       continue;
     }

     hltVertexReportsRawBank[0] = hltVertexReportsRawBank[0] + 1;

     unsigned int size = is->second.size();
     size = (size<65535)?size:65535;
     // first word for each selection contains number of vertices (low short) and selection ID (high short)
     hltVertexReportsRawBank.push_back(  (unsigned int)( size | (intSelID << 16) ) );
     
    
     for(unsigned int ivtx=0;ivtx!=size;++ivtx){
       const VertexBase & vtx = *(is->second[ivtx]);
       // now push vertex info
       
       hltVertexReportsRawBank.push_back( doubleToInt( vtx.position().x() ) );
       hltVertexReportsRawBank.push_back( doubleToInt( vtx.position().y() ) );
       hltVertexReportsRawBank.push_back( doubleToInt( vtx.position().z() ) );
       hltVertexReportsRawBank.push_back( doubleToInt( vtx.chi2() ) );
       hltVertexReportsRawBank.push_back( (unsigned int)(  (vtx.nDoF()>0)?vtx.nDoF():0 ) );       
    
     }
  }

  rawEvent->addBank(  kSourceID, RawBank::HltVertexReports, kVersionNumber, hltVertexReportsRawBank );
  
  if ( msgLevel(MSG::VERBOSE) ){

    verbose() << " ======= HltVertexReports RawBank size= " << hltVertexReportsRawBank.size() << endmsg;

    verbose() << " number of selections stored = " << hltVertexReportsRawBank[0] << endmsg;
    
    unsigned int iWord = 1;
    for(unsigned int i=0; i!=hltVertexReportsRawBank[0]; ++i){
      unsigned int n = hltVertexReportsRawBank[iWord] & 0xFFFFL;
      verbose() << " selection id " << (unsigned int)( hltVertexReportsRawBank[iWord] >> 16 ) 
                << " number of vertices " << n << endmsg;
      ++iWord;      
      for( unsigned int j=0; j!=n; ++j ){
        verbose() << " " << j 
                  << " x " << floatFromInt( hltVertexReportsRawBank[iWord] )
                  << " y " << floatFromInt( hltVertexReportsRawBank[iWord+1] )
                  << " z " << floatFromInt( hltVertexReportsRawBank[iWord+2] )
                  << " chi2 " << floatFromInt( hltVertexReportsRawBank[iWord+3] )
                  << " nDoF " << hltVertexReportsRawBank[iWord+4] 
                  << endmsg;
        iWord += 5;
  
      }
    }

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltVertexReportsWriter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return HltBaseAlg::finalize();  // must be called after all other actions
}

float HltVertexReportsWriter::floatFromInt(unsigned int i)
{
        union IntFloat { unsigned int mInt; float mFloat; };
        IntFloat a; a.mInt=i;
        return a.mFloat;
}

unsigned int HltVertexReportsWriter::doubleToInt(double d)
{
        union IntFloat { unsigned int mInt; float mFloat; };
        IntFloat a; a.mFloat = float(d);
        return a.mInt;
}

