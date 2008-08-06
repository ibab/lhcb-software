// $Id: HltVertexReportsReader.cpp,v 1.1 2008-08-06 14:51:54 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "HltVertexReports.h"
#include "Event/RawEvent.h"



// local
#include "HltVertexReportsReader.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexReportsReader
//
// 2008-08-05 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexReportsReader );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexReportsReader::HltVertexReportsReader( const std::string& name,
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
HltVertexReportsReader::~HltVertexReportsReader() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltVertexReportsReader::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_hltANNSvc = svc<IANNSvc>("HltANNSvc");

  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexReportsReader::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get inputs
  if( !exist<RawEvent>(m_inputRawEventLocation) ){    
    error() << " No RawEvent at " << m_inputRawEventLocation << endmsg;
    return StatusCode::SUCCESS;  
  }  
  RawEvent* rawEvent = get<RawEvent>(m_inputRawEventLocation);

  // create output container for vertex selections keyed with string and put it on TES
  HltVertexReports* outputSummary = new HltVertexReports();
  put( outputSummary, m_outputHltVertexReportsLocation );

  // get string-to-int selection ID map
  std::vector<IANNSvc::minor_value_type> selectionNameToIntMap;  
  //    std::vector<IANNSvc::minor_value_type> hlt = m_hltANNSvc->items("SelectionID"); // old style
  //    selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt.begin(),hlt.end() );
  std::vector<IANNSvc::minor_value_type> hlt1 = m_hltANNSvc->items("Hlt1SelectionID"); // new style
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt1.begin(),hlt1.end() );
  std::vector<IANNSvc::minor_value_type> hlt2 = m_hltANNSvc->items("Hlt2SelectionID");
  selectionNameToIntMap.insert( selectionNameToIntMap.end(),hlt2.begin(),hlt2.end() );

  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltVertexReportsRawBanks = rawEvent->banks( RawBank::HltVertexReports );
  if( !hltVertexReportsRawBanks.size() ){
    warning() << " No HltVertexReports RawBank in RawEvent. Quiting. " << endmsg;
    return StatusCode::SUCCESS;  
  } else if( hltVertexReportsRawBanks.size() != 1 ){
    warning() << " More then one HltSelReports RawBanks in RawEvent. Will process only the first one. " << endmsg;
  }
  const RawBank* hltvertexReportsRawBank = *(hltVertexReportsRawBanks.begin());
  if( hltvertexReportsRawBank->version() > kVersionNumber ){
    warning() << " HltVertexReports Raw Bank version number " << hltvertexReportsRawBank->version()
	      << " higher than the one of the reader " << int(kVersionNumber) << endmsg;
  }
  if( hltvertexReportsRawBank->sourceID() != kSourceID ){
    warning() << " HltVertexReports RawBank has unexpected source ID. Will try to decode it anyway." << endmsg;
  }

 
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
	iWord += 5;

	verticesOutput->insert(pVtx);
	pVtxs.push_back( SmartRef<VertexBase>( pVtx ) );

      }

      // insert selection into the container
      if( outputSummary->insert(selName,pVtxs) == StatusCode::FAILURE ){
	warning() << " Failed to add Hlt vertex selection name " << selName
		  << " to its container "
		  << endmsg;
      }    
 
    } else {
      error() << " did nit find selection name for int selection id in HltVertexReports " << endmsg; 
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
StatusCode HltVertexReportsReader::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

float HltVertexReportsReader::doubleFromInt(unsigned int i)
{
        union IntFloat { unsigned int mInt; float mFloat; };
        IntFloat a; a.mInt=i;
        return double(a.mFloat);
}

