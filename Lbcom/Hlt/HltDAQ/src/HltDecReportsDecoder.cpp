                                          // $Id: HltDecReportsDecoder.cpp,v 1.5 2010-08-08 18:16:28 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltDecReports.h"
#include "Event/RawEvent.h"



// local
#include "HltDecReportsDecoder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDecReportsDecoder
//
// 2008-08-02 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDecReportsDecoder )


using namespace LHCb;


namespace { 
         // version 1 layout:
         // decision:  0x        1                      x
         // error:     0x        e                   xxx0
         // #cand:     0x       f0              xxxx 0000
         // stage:     0x     ff00    xxxx xxxx 0000 0000
         // id:        0xffff 0000
         // version 0 layout:
         // decision:  0x        1                      x
         // error:     0x       70              0xxx 0000
         // #cand:     0x     ff80    xxxx xxxx x000 0000
         // stage:     0x        e                   xxx0
         // id:        0xffff 0000
    struct v0_v1 {
        HltDecReport convert( unsigned int x )  {
         // ID & decision stay the same
         unsigned int temp = ( x &   0xffff0001 );
         // stage needs to be moved
         temp |= (x&0xe)<<7; 
         // number of candidates -- move & truncate
         unsigned int nc = (x>>7)&0x1ff;
         temp |= ( nc>0xf ? 0xf : nc )<<4;
         // error just moves
         temp |= (x&0x70)>>3;
         return HltDecReport(temp);
        }
    };

    struct vx_vx {
        HltDecReport convert( unsigned int x )  { return HltDecReport(x); }
    };

}


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDecReportsDecoder::HltDecReportsDecoder( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_inputRawEventLocation(""),
    m_hltANNSvc(0)
{

  declareProperty("OutputHltDecReportsLocation",
                  m_outputHltDecReportsLocation= LHCb::HltDecReportsLocation::Default);  
  declareProperty("InputRawEventLocation",
                  m_inputRawEventLocation);  


}
//=============================================================================
// Destructor
//=============================================================================
HltDecReportsDecoder::~HltDecReportsDecoder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecReportsDecoder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  m_rawEventLocations.push_back(m_inputRawEventLocation);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Copied);
  m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);

  m_hltANNSvc = svc<IANNSvc>("ANNDispatchSvc");


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecReportsDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::RawEvent* rawEvent = 0;
  std::vector<std::string>::const_iterator iLoc = m_rawEventLocations.begin();
  for (; iLoc != m_rawEventLocations.end() && rawEvent==0 ; ++iLoc ) {
    if (exist<LHCb::RawEvent>(*iLoc)) {
      rawEvent = get<LHCb::RawEvent>(*iLoc);
    }
  }

  // create output container and put it on TES
  HltDecReports* outputSummary = new HltDecReports();
  put( outputSummary, m_outputHltDecReportsLocation );


   // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltdecreportsRawBanks = rawEvent->banks( RawBank::HltDecReports );
  if( !hltdecreportsRawBanks.size() ){
    return Warning( " No HltDecReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 20 );
  } else if( hltdecreportsRawBanks.size() != 1 ){
    Warning(" More then one HltDecReports RawBanks in RawEvent. Will only process the first one. " ,StatusCode::SUCCESS, 20 );
  }
  const RawBank* hltdecreportsRawBank = hltdecreportsRawBanks.front();
  if( hltdecreportsRawBank->magic() != RawBank::MagicPattern ){
    return Error(" HltDecReports RawBank has wrong magic number. Return without decoding.",StatusCode::FAILURE );
  }
  if( hltdecreportsRawBank->version() > kVersionNumber ){
    return Error(
" HltDecReports RawBank version # is larger then the known ones.... cannot decode, use newer version." ,StatusCode::FAILURE );
  }
  if( hltdecreportsRawBank->sourceID() != kSourceID ){
    Warning( " HltDecReports RawBank has unexpected source ID. Will try to decode it anyway.",StatusCode::SUCCESS, 20 );
  }

  // ----------------------------------------------------------
  const unsigned int *content = hltdecreportsRawBank->begin<unsigned int>();  

  // version 0 has only decreps, version 1 has TCK, taskID, then decreps...
  if (hltdecreportsRawBank->version() > 0 ) {
     outputSummary->setConfiguredTCK( *content++ );
     outputSummary->setTaskID( *content++ );
  } 

  // --------------------------------- get configuration --------------------
  // TODO: use configuredTCK to get the right mapping...
  //       if not available, go for the value in ODIN ( _not_ guaranteed to be correct! )
  //                      or go for the 'current' (most recent) mapping... (also not guaranteed)

  // ---------------- loop over decisions in the bank body; insert them into the output container

  switch ( hltdecreportsRawBank->version() ) {
    case 0 : this->decodeHDR<v0_v1>( content, hltdecreportsRawBank->end<unsigned int>(), 
                                     *outputSummary );
        break;
    case 1 : this->decodeHDR<vx_vx>( content, hltdecreportsRawBank->end<unsigned int>(), 
                                     *outputSummary );
        break;
    default : Error(
" HltDecReports RawBank version # is larger then the known ones.... cannot decode, use newer version. " ,StatusCode::FAILURE );
 }

#if 0
  // make this part inactive until have access to real config in off-line

  // ---------------- put decision=false decisions for "*Decision"s in configuration

  // loop over all selections in this configuration
  for( std::vector<IANNSvc::minor_value_type>::const_iterator si=selectionNameToIntMap.begin();
       si!=selectionNameToIntMap.end();++si){
    const std::string & selName = si->first;

    // don't bother if already in 
    if( outputSummary->hasSelectionName(selName) )continue;

    if( selName.find("Decision") != std::string::npos ){
      
      HltDecReport selSumOut( 0, 0, 0, si->second );
      if( selSumOut.invalidIntSelectionID() ){
        std::ostringstream mess;
        mess << " selectionName=" << selName << " has invalid intSelectionID=" << si->second;
        Warning( mess.str(), StatusCode::SUCCESS, 20 );
        continue;
      }

      // insert selection into the container
      if( outputSummary->insert( selName, selSumOut ) == StatusCode::FAILURE ){
        Error(" Failed to add HltDecReport selectionName=" + selName 
              + " to its container ");
      }    
    }
    

  }
#endif 
 
  if ( msgLevel(MSG::VERBOSE) ){
    // debugging info
    verbose() << " ====== HltDecReports container size=" << outputSummary->size() << endmsg;  
    verbose() << *outputSummary << endmsg;
  }
  
  
  return StatusCode::SUCCESS;
}

template <typename HDRConverter, typename I> 
void
HltDecReportsDecoder::decodeHDR(I i, I end,  
                               HltDecReports& output ) const 
{
   HDRConverter converter;
   while (i != end ) {

    HltDecReport dec(  converter.convert(*i++)  );

    int id=dec.intDecisionID();

    boost::optional<IANNSvc::minor_value_type> selName = m_hltANNSvc->value("Hlt1SelectionID",id);
    if (!selName) selName = m_hltANNSvc->value("Hlt2SelectionID",id);
    if( selName ){
      if( !output.insert( selName->first, dec ).isSuccess() ) {
        Warning(" Duplicate decision report in storage "+selName->first, StatusCode::SUCCESS, 20 ).ignore();
      }
    } else {
      std::ostringstream mess;
      mess << " No string key found for trigger decision in storage "
           << " id=" << id;
      Error(mess.str(), StatusCode::SUCCESS, 50 ).ignore();
    }
   }
    
}
//=============================================================================
