// $Id: HltVertexReportsDecoder.cpp,v 1.5 2010-06-14 13:38:00 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltVertexReports.h"
#include "Event/RawEvent.h"



// local
#include "HltVertexReportsDecoder.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexReportsDecoder
//
// 2008-08-05 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexReportsDecoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexReportsDecoder::HltVertexReportsDecoder( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  declareProperty("OutputHltVertexReportsLocation",
    m_outputHltVertexReportsLocation= LHCb::HltVertexReportsLocation::Default);  
  declareProperty("InputRawEventLocation",
    m_inputRawEventLocation= LHCb::RawEventLocation::Default);

  m_hltANNSvc = 0;

}
//=============================================================================
// Destructor
//=============================================================================
HltVertexReportsDecoder::~HltVertexReportsDecoder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltVertexReportsDecoder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("ANNDispatchSvc");

  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexReportsDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get inputs
  if( !exist<RawEvent>(m_inputRawEventLocation) ){    
    return Error(" No RawEvent at "+m_inputRawEventLocation.value());
  }  
  RawEvent* rawEvent = get<RawEvent>(m_inputRawEventLocation);

  // create output container for vertex selections keyed with string and put it on TES
  HltVertexReports* outputSummary = new HltVertexReports();
  put( outputSummary, m_outputHltVertexReportsLocation );

  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltVertexReportsRawBanks = rawEvent->banks( RawBank::HltVertexReports );
  if( !hltVertexReportsRawBanks.size() ){
    return Warning(" No HltVertexReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 20 );
  } else if( hltVertexReportsRawBanks.size() != 1 ){
    Warning(" More then one HltVertexReports RawBanks in RawEvent. Will process only the first one. ",StatusCode::SUCCESS, 20 );
  }
  const RawBank* hltvertexReportsRawBank = *(hltVertexReportsRawBanks.begin());
  if( hltvertexReportsRawBank->magic() != RawBank::MagicPattern ){
    return Error(" HltVertexReports RawBank has wrong magic number. Return without decoding.",StatusCode::FAILURE );
  }
  const unsigned int bankVersionNumber = hltvertexReportsRawBank->version();  
  if( bankVersionNumber > kVersionNumber ){
    std::ostringstream mess;
    mess <<  " HltVertexReports Raw Bank version number " << bankVersionNumber
         << " higher than the one of the decoder " << int(kVersionNumber);
    Warning( mess.str(),  StatusCode::SUCCESS, 20 );
  }
  if( hltvertexReportsRawBank->sourceID() != kSourceID ){
    Warning(" HltVertexReports RawBank has unexpected source ID. Will try to decode it anyway." ,  StatusCode::SUCCESS, 20 );
  }


  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  
  //    std::vector<IANNSvc::minor_value_type> hlt = m_hltANNSvc->items("SelectionID"); // old style
  //    selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt.begin(),hlt.end() );
  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); // new style
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );
  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );
 
  unsigned int bankSize = (hltvertexReportsRawBank->size()+3)/4; // from bytes to words

  std::vector< unsigned int > hltVertexReportsRawBank(&(hltvertexReportsRawBank->data()[0]),
						      &(hltvertexReportsRawBank->data()[bankSize]) );


  unsigned int iWord = 1;
  for(unsigned int i=0; i!=hltVertexReportsRawBank[0]; ++i){
    unsigned int n = hltVertexReportsRawBank[iWord] & 0xFFFFL;
    int intSelID = hltVertexReportsRawBank[iWord] >> 16;
    ++iWord;
    

    std::string selName="Dummy";
    for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
         si!=selectionNameToIntMap.end();++si){
      if( si->second == intSelID ){
        selName = si->first;
        break;
      }
    }
    if( selName != "Dummy" ){

      // create output container for vertices and put it on TES
      VertexBase::Container* verticesOutput = new VertexBase::Container();
      put( verticesOutput, m_outputHltVertexReportsLocation.value() + "/" + selName  );

      SmartRefVector<VertexBase> pVtxs;

      for( unsigned int j=0; j!=n; ++j ){
        VertexBase* pVtx = new VertexBase();
        Gaudi::XYZPoint position( doubleFromInt( hltVertexReportsRawBank[iWord] ),
                                  doubleFromInt( hltVertexReportsRawBank[iWord+1] ),
                                  doubleFromInt( hltVertexReportsRawBank[iWord+2] ) );
        pVtx->setPosition( position );
        pVtx->setChi2( doubleFromInt( hltVertexReportsRawBank[iWord+3] ) );
        pVtx->setNDoF( hltVertexReportsRawBank[iWord+4] );
        if( bankVersionNumber==0 ){          
          iWord += 5;
        } else {
          Gaudi::SymMatrix3x3 cov;
          cov[0][0] = doubleFromInt( hltVertexReportsRawBank[iWord+5] );
          cov[1][1] = doubleFromInt( hltVertexReportsRawBank[iWord+6] );
          cov[2][2] = doubleFromInt( hltVertexReportsRawBank[iWord+7] );
          cov[0][1] = doubleFromInt( hltVertexReportsRawBank[iWord+8] );
          cov[0][2] = doubleFromInt( hltVertexReportsRawBank[iWord+9] );
          cov[1][2] = doubleFromInt( hltVertexReportsRawBank[iWord+10] );          
          pVtx->setCovMatrix(cov);
          iWord += 11;
        }

        verticesOutput->insert(pVtx);
        pVtxs.push_back( SmartRef<VertexBase>( pVtx ) );

      }

      // insert selection into the container
      if( outputSummary->insert(selName,pVtxs) == StatusCode::FAILURE ){
        Error(" Failed to add Hlt vertex selection name " + selName
              + " to its container ",StatusCode::SUCCESS, 20 );
      }    
 
    } else {
      std::ostringstream mess;
      mess <<  " did not find selection name for int selection id in HltVertexReports id=" << intSelID;
      Error(mess.str(), StatusCode::SUCCESS, 50 );
      for( unsigned int j=0; j!=n; ++j ) iWord +=5;

    }
  
  }

  if ( msgLevel(MSG::VERBOSE) ){

    verbose() << " ======= HltVertexReports size= " << outputSummary->size() << endmsg;

    verbose() << *outputSummary << endmsg;

  }


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltVertexReportsDecoder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

double HltVertexReportsDecoder::doubleFromInt(unsigned int i)
{
        union IntFloat { unsigned int mInt; float mFloat; };
        IntFloat a; a.mInt=i;
        return double(a.mFloat);
}
