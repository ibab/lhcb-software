// Include files 
#include "boost/format.hpp"
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RawEvent.h"

// local
#include "HltTrackReportsDecoder.h"
#include "HltTrackReportsWriter.h"
#include "HltTrackingCoder.h"

#include "Event/Track.h"


using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackReportsDecoder
//
// The actual decoding of tracks is delegated 
// to the functions in HltTrackingCoder.
//
// 2014-01-16 : Sebastian Neubert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackReportsDecoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackReportsDecoder::HltTrackReportsDecoder( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : Decoder::AlgBase ( name , pSvcLocator ) , m_trackcount(0),m_callcount(0)
{
   //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Copied, LHCb::RawEventLocation::Default};
  initRawEventSearch();

  declareProperty("OutputLocation",
    m_outputLocation= "/Hlt/Track/Velo" );  


  declareProperty("SourceID",
		  m_sourceID= HltTrackReportsWriter::kSourceID_Dummy );  

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackReportsDecoder::~HltTrackReportsDecoder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackReportsDecoder::initialize() {
  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  //m_hltANNSvc = svc<IANNSvc>("ANNDispatchSvc");

  if( m_sourceID > HltTrackReportsWriter::kSourceID_Max ){
    m_sourceID = m_sourceID & HltTrackReportsWriter::kSourceID_Max;
    return Error("Illegal SourceID specified (too large);" , StatusCode::FAILURE, 50 );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackReportsDecoder::execute() {
  ++m_callcount;
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // get inputs
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  
  if( ! rawEvent ){
    return Error(" No RawEvent found at any location. No HltTracks created.");
  }  

  // check if the container is already present
  
 
  // create output container and put it on TES
  LHCb::Tracks* outputTracks = NULL;
  try {
    outputTracks=getOrCreate<LHCb::Tracks,LHCb::Tracks>(m_outputLocation);
    //put( outputTracks, m_outputLocation );
  }
  catch(GaudiException ex) {
    warning() << "Failed to create output location " <<  m_outputLocation << endmsg;
    warning() << ex.message() << endmsg;
    return StatusCode::FAILURE;
  }

  if(outputTracks->size()>0) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "outputLocation already contains tracks. Skipping decoding." << endmsg;
    return StatusCode::SUCCESS;
  } 


  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltTrackReportsRawBanks = rawEvent->banks( RawBank::HltTrackReports);
  if( !hltTrackReportsRawBanks.size() ){
    return Warning( " No HltTrackReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 10 );
  }

  // collect possibly split up banks
  unsigned int bankCounterMax = 0;
  unsigned int bankSize =0; // this will be the total size of the bank
  // store the pieces of the bank with the correct major sourceID in an ordered list:
  std::vector<const RawBank*> orderedBanks(hltTrackReportsRawBanks.size(),(const RawBank*)0);
  for( RawBank* trbank : hltTrackReportsRawBanks ){    
    RawBank* bank = trbank;

    int sourceID = bank->sourceID() >> HltTrackReportsWriter::kSourceID_BitShift;
    if( m_sourceID != sourceID )continue; // Not our major SourceID a.k.a different tracks

    if( bank->magic() != RawBank::MagicPattern ){
      Error(" HltTrackReports RawBank has wrong magic number. Skipped ",StatusCode::SUCCESS, 20 );
      continue;
    }

    // which piece of the bank is that?
    unsigned int bankCounter = bank->sourceID() & HltTrackReportsWriter::kSourceID_MinorMask;
    if( bankCounter < hltTrackReportsRawBanks.size() ){
      orderedBanks[bankCounter]= bank;
      if( bankCounter > bankCounterMax ) bankCounterMax = bankCounter;
    } else {
      Error( " Illegal Source ID HltTrackReports bank skipped ", StatusCode::SUCCESS, 20 );
    }
    bankSize += bank->size();
  }
  if( !bankSize ){
    return Warning( " No HltTrackReports RawBank for requested SourceID in RawEvent. Quiting. ",StatusCode::SUCCESS, 10 );
  }    
  bankSize = (bankSize+3)/4; // from bytes to words
  // need to copy it to local array 
  unsigned int* pBank = new unsigned int[bankSize];
  unsigned int* completeBank=pBank;
  ++bankCounterMax; // increase by one to get entries from max index
  for( unsigned int iBank=0; iBank<bankCounterMax; ++iBank){
    const RawBank* bank = orderedBanks[iBank];
    if( bank == NULL ){
      delete pBank;
      return Error("Missing HltTrackReports RawBank part - quiting.", StatusCode::SUCCESS, 100 );
    }
    unsigned int bankSize1 =  (bank->size()+3)/4; // from bytes to words
    for(unsigned int i=0; i!=bankSize1; ++i){
      (*pBank) = bank->data()[i]; ++pBank;
    }
  }

   // -------------------------------------------------------
   // do the actual decoding: see HltTrackingCoder.cpp
   // -------------------------------------------------------

  decodeTracks(completeBank,bankSize,outputTracks);
  
  delete completeBank;
  
  
   // for debug purposes print the contents of the outputLocation
   if( msgLevel(MSG::VERBOSE) )
     {
       verbose() << "----------------------------------------\n";
       verbose() << "Decoded event " << m_callcount << endmsg;
       verbose() << outputTracks->size() <<" Resurrected tracks: \n"; 
       m_trackcount += outputTracks->size();
       LHCb::Tracks::const_iterator pItr;
       for(pItr = outputTracks->begin(); outputTracks->end() != pItr; ++pItr){
	 LHCb::Track* Tr = (*pItr);
	 verbose()  << *Tr << endmsg ;
	 // also dump IDs
	 verbose() << "LHCbIDs: [\n"; 
	 unsigned int nhits= Tr->nLHCbIDs();
	 for(unsigned int i=0;i<nhits;++i){
	   verbose() << Tr->lhcbIDs()[i] << ",\n";
	 }
	 verbose() << "]" << endmsg;
       }
       
     }
   
   return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackReportsDecoder::finalize() {

  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "==> Finalize" << endmsg;
    verbose() << "Decoded " << m_trackcount << " tracks to " << m_outputLocation << endmsg;
  }

  return Decoder::AlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
    
  
// float HltTrackReportsDecoder::floatFromInt(unsigned int i)
// {
//         union IntFloat { unsigned int mInt; float mFloat; };
//         IntFloat a; a.mInt=i;
//         return a.mFloat;
// }

