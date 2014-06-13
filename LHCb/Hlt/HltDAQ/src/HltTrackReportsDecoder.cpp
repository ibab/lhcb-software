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
  : Decoder::AlgBase ( name , pSvcLocator ) ,m_callcount(0)
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

  //if(outputTracks->size()>0) {
  // if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
  //    debug() << "outputLocation already contains tracks. Skipping decoding." << endmsg;
  //  return StatusCode::SUCCESS;
  //} 


  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------
  std::vector<RawBank*> hltTrackReportsRawBanks = rawEvent->banks( RawBank::HltTrackReports );
  hltTrackReportsRawBanks.erase( std::remove_if( std::begin( hltTrackReportsRawBanks ), std::end( hltTrackReportsRawBanks ),
                                                 [&](const RawBank* bank) {
                int sourceID = bank->sourceID() >> HltTrackReportsWriter::kSourceID_BitShift;
                if( m_sourceID != sourceID ) return false; // Not our major SourceID a.k.a different tracks
                if( bank->magic() != RawBank::MagicPattern ){
                  Error(" HltTrackReports RawBank has wrong magic number. Skipped ",StatusCode::SUCCESS, 20 );
                  return false;
                }
                return true;
                                                 }),
                                 std::end(hltTrackReportsRawBanks) );
  // collect possibly split up banks -- first put them into the right order
  std::sort( std::begin(hltTrackReportsRawBanks), std::end(hltTrackReportsRawBanks), 
             []( const RawBank* lhs, const RawBank* rhs) {
    auto c1 = lhs->sourceID() & HltTrackReportsWriter::kSourceID_MinorMask;
    auto c2 = rhs->sourceID() & HltTrackReportsWriter::kSourceID_MinorMask;
    return c1 < c2 ;
  });
  // verify all present in expected order...
  bool ok = true;
  std::accumulate( std::begin(hltTrackReportsRawBanks), std::end(hltTrackReportsRawBanks), 0u, 
                              [&ok] (unsigned int i, const RawBank* b ) {  
     ok = ok && ( i == ( b->sourceID() & HltTrackReportsWriter::kSourceID_MinorMask ) );
     return ++i;
                              } );
  if (!ok) return Error("Missing HltTrackReports RawBank part - quiting.", StatusCode::SUCCESS, 100 );
  
  // figure out the total size
  unsigned int bankSize = std::accumulate( std::begin(hltTrackReportsRawBanks), std::end(hltTrackReportsRawBanks), 0u,
                                           [](unsigned int s, const RawBank* bank) {
                                               return s+bank->size();
                                           } );
                
  if( bankSize==0 ){
    return Warning( " No HltTrackReports RawBank for requested SourceID in RawEvent. Quiting. ",StatusCode::SUCCESS, 0 );
  }    
  bankSize = (bankSize+3)/4; // from bytes to words
  // concatenate banks into a local array -- TODO: avoid copy if there is only a single bank (which should be the most likely case)
  std::unique_ptr<unsigned int> completeBank{ new unsigned int[bankSize] };
  std::accumulate( std::begin(hltTrackReportsRawBanks), std::end(hltTrackReportsRawBanks), completeBank.get(),
                   [](unsigned int *p, const RawBank* bank) {
                        return std::copy( bank->begin<unsigned int>(),  bank->end<unsigned int>(), p );
                   }
  );

   // -------------------------------------------------------
   // do the actual decoding: see HltTrackingCoder.cpp
   // -------------------------------------------------------

  decodeTracks(completeBank.get(),bankSize,outputTracks);
  
   // for debug purposes print the contents of the outputLocation
   if( msgLevel(MSG::VERBOSE) )
     {
       verbose() << "----------------------------------------\n";
       verbose() << "Decoded event " << m_callcount << endmsg;
       verbose() << outputTracks->size() <<" Resurrected tracks: \n"; 
       counter("#Tracks") += outputTracks->size();
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

