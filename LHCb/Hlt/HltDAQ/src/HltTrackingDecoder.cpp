// Include files 
#include "boost/format.hpp"
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RawEvent.h"

// local
#include "HltTrackingDecoder.h"
#include "HltTrackingWriter.h"
#include "HltTrackingCoder.h"

#include "Event/Track.h"


using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackingDecoder
//
// The actual decoding of tracks is delegated 
// to the functions in HltTrackingCoder.
//
// 2014-01-16 : Sebastian Neubert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackingDecoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackingDecoder::HltTrackingDecoder( const std::string& name,
                                          ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
    m_inputRawEventLocation(""),
    m_hltANNSvc(0)
{

  declareProperty("OutputHltTrackLocation",
    m_outputHltTrackLocation= "/Hlt/Track/Velo" );  

  declareProperty("InputRawEventLocation",
                  m_inputRawEventLocation);  

  declareProperty("SourceID",
		  m_sourceID= HltTrackingWriter::kSourceID_Dummy );  

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackingDecoder::~HltTrackingDecoder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackingDecoder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_rawEventLocations.clear();
  if( m_inputRawEventLocation != "" )m_rawEventLocations.push_back(m_inputRawEventLocation);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Trigger);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Copied);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);

  m_hltANNSvc = svc<IANNSvc>("ANNDispatchSvc");

  if( m_sourceID > HltTrackingWriter::kSourceID_Max ){
    m_sourceID = m_sourceID & HltTrackingWriter::kSourceID_Max;
    return Error("Illegal SourceID specified; maximal allowed value is 7" , StatusCode::FAILURE, 50 );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackingDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // get inputs
  LHCb::RawEvent* rawEvent = 0;
  std::vector<std::string>::const_iterator iLoc = m_rawEventLocations.begin();
  for (; (iLoc != m_rawEventLocations.end()&& rawEvent==0) ; ++iLoc ) {
    //    try RootInTES independent path first
      rawEvent = getIfExists<LHCb::RawEvent>(*iLoc, false);
    //   now try RootInTES dependent path
    if (rawEvent==0) {
      rawEvent = getIfExists<LHCb::RawEvent>(*iLoc);
    }
  }

  if( ! rawEvent ){
    return Error(" No RawEvent found at any location. No HltTracks created.");
  }  

  // create output container and put it on TES
  LHCb::Tracks* outputTracks = new LHCb::Tracks();
  put( outputTracks, m_outputHltTrackLocation );

  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltTrackingRawBanks = rawEvent->banks( RawBank::HltTrackingReports );
  if( !hltTrackingRawBanks.size() ){
    return Warning( " No HltTrackingReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 10 );
  }

  // Check version number to make sure we are on the same page here (this should go to the decoder function?)
   const RawBank* hltTrackingRawBank0 = *(hltTrackingRawBanks.begin());
   if( hltTrackingRawBank0->version() > kVersionNumber ){
     Warning( " HltTrackingReports RawBank version is higher than expected. Will try to decode it anyway." ,StatusCode::SUCCESS, 20 );
   }

   // -------------------------------------------------------
   // do the actual decoding: see HltTrackingCoder.cpp
   // -------------------------------------------------------

   for(RawBank* bank : hltTrackingRawBanks){
     decodeTracks(bank->data(),bank->size(),outputTracks);
   }


 return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackingDecoder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
    
  
// float HltTrackingDecoder::floatFromInt(unsigned int i)
// {
//         union IntFloat { unsigned int mInt; float mFloat; };
//         IntFloat a; a.mInt=i;
//         return a.mFloat;
// }

