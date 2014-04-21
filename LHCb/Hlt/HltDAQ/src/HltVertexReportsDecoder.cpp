// $Id: HltVertexReportsDecoder.cpp,v 1.5 2010-06-14 13:38:00 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltVertexReports.h"
#include "Event/RawEvent.h"

// local
#include "HltVertexReportsDecoder.h"
#include "HltVertexReportsWriter.h"

namespace { 
    const Gaudi::StringKey Hlt1SelectionID {"Hlt1SelectionID"};
    const Gaudi::StringKey Hlt2SelectionID {"Hlt2SelectionID"};
    double doubleFromInt(unsigned int i) {
            union IntFloat { unsigned int mInt; float mFloat; };
            IntFloat a; a.mInt=i;
            return double(a.mFloat);
    }
}

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
: Decoder::AlgBase ( name , pSvcLocator ),
  m_hltANNSvc(0)
{
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Copied, LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
  declareProperty("OutputHltVertexReportsLocation",
    m_outputHltVertexReportsLocation= LHCb::HltVertexReportsLocation::Default);  
  declareProperty("SourceID",
		  m_sourceID= HltVertexReportsWriter::kSourceID_Dummy );  


}
//=============================================================================
// Destructor
//=============================================================================
HltVertexReportsDecoder::~HltVertexReportsDecoder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltVertexReportsDecoder::initialize() {
  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_hltANNSvc = svc<IANNSvc>("ANNDispatchSvc");
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexReportsDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get inputs
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  if( ! rawEvent ){
    return Error(" No RawEvent found at any location. No HltVertexReports created.");
  }  


  // create output container for vertex selections keyed with string and put it on TES
  HltVertexReports* outputSummary = new HltVertexReports();
  put( outputSummary, m_outputHltVertexReportsLocation );

  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltvertexreportsRawBanksAll = rawEvent->banks( RawBank::HltVertexReports );
  if( !hltvertexreportsRawBanksAll.size() ){
    return Warning( " No HltVertexReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 20 );
  } 

  std::vector<const RawBank*> hltvertexreportsRawBanks;
  for( std::vector<RawBank*>::const_iterator hltvertexreportsRawBankP=hltvertexreportsRawBanksAll.begin();
	 hltvertexreportsRawBankP!=hltvertexreportsRawBanksAll.end(); ++hltvertexreportsRawBankP ){    
    const RawBank* hltvertexreportsRawBank = *hltvertexreportsRawBankP;

    unsigned int sourceID=HltVertexReportsWriter::kSourceID_Hlt;
    if( hltvertexreportsRawBank->version() > 1 ){
      sourceID = hltvertexreportsRawBank->sourceID() >> HltVertexReportsWriter::kSourceID_BitShift;
    }
    if( m_sourceID != sourceID )continue;

    hltvertexreportsRawBanks.push_back( hltvertexreportsRawBank );
  }
  if( !hltvertexreportsRawBanks.size() ){
    if( ( m_sourceID == HltVertexReportsWriter::kSourceID_Hlt1 ) ||
        ( m_sourceID == HltVertexReportsWriter::kSourceID_Hlt2 ) ){

      for( std::vector<RawBank*>::const_iterator hltvertexreportsRawBankP=hltvertexreportsRawBanksAll.begin();
	   hltvertexreportsRawBankP!=hltvertexreportsRawBanksAll.end(); ++hltvertexreportsRawBankP ){    
	const RawBank* hltvertexreportsRawBank = *hltvertexreportsRawBankP;

	unsigned int sourceID=HltVertexReportsWriter::kSourceID_Hlt;
	if( hltvertexreportsRawBank->version() > 1 ){
	  sourceID = hltvertexreportsRawBank->sourceID() >> HltVertexReportsWriter::kSourceID_BitShift;
	}
	if( HltVertexReportsWriter::kSourceID_Hlt != sourceID )continue;

	hltvertexreportsRawBanks.push_back( hltvertexreportsRawBank );
      }

    }
  }
  if( !hltvertexreportsRawBanks.size() ){
    return Warning( " No HltVertexReports RawBank for requested SourceID in RawEvent. Quiting. ",StatusCode::SUCCESS, 20 );
  } else if( hltvertexreportsRawBanks.size() != 1 ){
    Warning(" More then one HltVertexReports RawBanks for requested SourceID in RawEvent. Will only process the first one. " ,StatusCode::SUCCESS, 20 );
  }
  const RawBank* hltvertexreportsRawBank = hltvertexreportsRawBanks.front();

  if( hltvertexreportsRawBank->magic() != RawBank::MagicPattern ){
    return Error(" HltVertexReports RawBank has wrong magic number. Return without decoding.",StatusCode::FAILURE );
  }
  const unsigned int bankVersionNumber = hltvertexreportsRawBank->version();  
  if( bankVersionNumber > kVersionNumber ){
    std::ostringstream mess;
    mess <<  " HltVertexReports Raw Bank version number " << bankVersionNumber
         << " higher than the one of the decoder " << int(kVersionNumber);
    Warning( mess.str(),  StatusCode::SUCCESS, 20 ).ignore();
  }
  //  if( hltvertexreportsRawBank->sourceID() != kSourceID ){
  //  Warning(" HltVertexReports RawBank has unexpected source ID. Will try to decode it anyway." ,  StatusCode::SUCCESS, 20 ).ignore();
  // }

  const unsigned int *i   = hltvertexreportsRawBank->begin<unsigned int>();
  const unsigned int *end = hltvertexreportsRawBank->end<unsigned int>();
  int nSel = *i++;
  while ( i < end ) { // avoid infinite loop with corrupt/incompatible data... (i.e. do NOT use i!=end)
    --nSel;
    unsigned nVert    = ( ( *i ) & 0xFFFFL ); // can we decode the per vertex size here???
    unsigned intSelID = ( *i++ >> 16 );

    unsigned int hltType(HltVertexReportsWriter::kSourceID_Hlt2); 
    boost::optional<IANNSvc::minor_value_type> value = m_hltANNSvc->value(Hlt1SelectionID,intSelID);
    if (!value) {
      value = m_hltANNSvc->value(Hlt2SelectionID,intSelID);
    } else {
      hltType=HltVertexReportsWriter::kSourceID_Hlt1;
    }
    if (!value) {
      std::ostringstream mess; mess <<  " did not find name for id = " << intSelID << "; skipping this selection";
      Error(mess.str(), StatusCode::SUCCESS, 50 );
      i += nVert * ( bankVersionNumber == 0 ? 5 : 11 ); // would have been nice to have a size / vtx in the bank...
      continue;
    }
    // skip reports if of wrong type
    if( ( m_sourceID == HltVertexReportsWriter::kSourceID_Hlt1 ) ||
	( m_sourceID == HltVertexReportsWriter::kSourceID_Hlt2 ) ){
      if( hltType != m_sourceID ) {
	i += nVert * ( bankVersionNumber == 0 ? 5 : 11 ); // would have been nice to have a size / vtx in the bank...
	continue;
      }
    } 

    // create output container for vertices and put it on TES
    VertexBase::Container* verticesOutput = new VertexBase::Container();
    put( verticesOutput, m_outputHltVertexReportsLocation.value() + "/" + std::string(value->first)  );

    SmartRefVector<VertexBase> pVtxs;

    for( unsigned int j=0; j!=nVert; ++j ){
      VertexBase* pVtx = new VertexBase();
      double x = doubleFromInt( *i++ );
      double y = doubleFromInt( *i++ );
      double z = doubleFromInt( *i++ );
      pVtx->setPosition( Gaudi::XYZPoint( x,y,z ) );
      pVtx->setChi2( doubleFromInt( *i++ ) );
      pVtx->setNDoF( *i++ ) ; 
      if( bankVersionNumber>0 ){          
        Gaudi::SymMatrix3x3 cov;
        cov[0][0] = doubleFromInt( *i++ ) ;
        cov[1][1] = doubleFromInt( *i++ ) ;
        cov[2][2] = doubleFromInt( *i++ ) ;
        cov[0][1] = doubleFromInt( *i++ ) ;
        cov[0][2] = doubleFromInt( *i++ ) ;
        cov[1][2] = doubleFromInt( *i++ ) ;
        pVtx->setCovMatrix(cov);
      }

      verticesOutput->insert(pVtx);
      pVtxs.push_back( SmartRef<VertexBase>( pVtx ) );
    }

    // insert selection into the container
    if( outputSummary->insert(value->first,pVtxs) == StatusCode::FAILURE ){
      Error(" Failed to add Hlt vertex selection name " 
            + std::string(value->first)
            + " to its container ",StatusCode::SUCCESS, 20 );
    }    
  }
  if (nSel!=0) { 
     error()  << "Unexpected banksize while decoding (case 1).... " << endmsg;
  }
  if (i!=end) { 
     error()  << "Unexpected banksize while decoding (case 2).... " << endmsg;
  }

  if ( msgLevel(MSG::VERBOSE) ){

    verbose() << " ======= HltVertexReports size= " << outputSummary->size() << endmsg;
    verbose() << *outputSummary << endmsg;

  }
  return StatusCode::SUCCESS;
}
