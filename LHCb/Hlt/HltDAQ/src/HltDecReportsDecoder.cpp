                                          // $Id: HltDecReportsDecoder.cpp,v 1.5 2010-08-08 18:16:28 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/HltDecReports.h"
#include "Event/RawEvent.h"



// local
#include "HltDecReportsDecoder.h"
#include "HltDecReportsWriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDecReportsDecoder
//
// 2008-08-02 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDecReportsDecoder )


using namespace LHCb;


namespace { 
const Gaudi::StringKey  Hlt1SelectionID{"Hlt1SelectionID"};
const Gaudi::StringKey  Hlt2SelectionID{"Hlt2SelectionID"};

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
  : Decoder::AlgBase ( name , pSvcLocator ),
    m_hltANNSvc{nullptr}
{
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Trigger, LHCb::RawEventLocation::Copied, LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
  declareProperty("OutputHltDecReportsLocation",
                  m_outputHltDecReportsLocation= LHCb::HltDecReportsLocation::Default);
  
  declareProperty("SourceID",
		  m_sourceID= HltDecReportsWriter::kSourceID_Dummy );  


}
//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecReportsDecoder::initialize() {
  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  
  m_hltANNSvc = svc<IANNSvc>("ANNDispatchSvc");
  m_TCKANNSvc = svc<IIndexedANNSvc>("TCKANNSvc");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecReportsDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  
  // create output container and put it on TES
  HltDecReports* outputSummary = new HltDecReports();
  put( outputSummary, m_outputHltDecReportsLocation );
  
  if( ! rawEvent ){
    return Error(" No RawEvent found at any location. Empty HltDecReports created. ");
  }  

   // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------

  const std::vector<RawBank*> hltdecreportsRawBanksAll = rawEvent->banks( RawBank::HltDecReports );
  if( !hltdecreportsRawBanksAll.size() ){
    return Warning( " No HltDecReports RawBank in RawEvent. Quiting. ",StatusCode::SUCCESS, 20 );
  } 

  std::vector<const RawBank*> hltdecreportsRawBanks;
  for(const RawBank* hltdecreportsRawBank : hltdecreportsRawBanksAll ) {

    unsigned int sourceID=HltDecReportsWriter::kSourceID_Hlt;
    if( hltdecreportsRawBank->version() > 1 ){
      sourceID = hltdecreportsRawBank->sourceID() >> HltDecReportsWriter::kSourceID_BitShift;
    }
    if( m_sourceID != sourceID )continue;

    hltdecreportsRawBanks.push_back( hltdecreportsRawBank );
  }
  if( !hltdecreportsRawBanks.size() ){
    if( ( m_sourceID == HltDecReportsWriter::kSourceID_Hlt1 ) ||
        ( m_sourceID == HltDecReportsWriter::kSourceID_Hlt2 ) ){

      for(const RawBank* hltdecreportsRawBank : hltdecreportsRawBanksAll ) {

	unsigned int sourceID=HltDecReportsWriter::kSourceID_Hlt;
	if( hltdecreportsRawBank->version() > 1 ){
	  sourceID = hltdecreportsRawBank->sourceID() >> HltDecReportsWriter::kSourceID_BitShift;
	}
	if( HltDecReportsWriter::kSourceID_Hlt != sourceID )continue;

	hltdecreportsRawBanks.push_back( hltdecreportsRawBank );
      }

    }
  }
  if( !hltdecreportsRawBanks.size() ){
    return Warning( " No HltDecReports RawBank for requested SourceID in RawEvent. Quiting. ",StatusCode::SUCCESS, 20 );
  } else if( hltdecreportsRawBanks.size() != 1 ){
    Warning(" More then one HltDecReports RawBanks for requested SourceID in RawEvent. Will only process the first one. " ,StatusCode::SUCCESS, 20 );
  }
  const RawBank* hltdecreportsRawBank = hltdecreportsRawBanks.front();
  if( hltdecreportsRawBank->magic() != RawBank::MagicPattern ){
    return Error(" HltDecReports RawBank has wrong magic number. Return without decoding.",StatusCode::FAILURE );
  }
  if( hltdecreportsRawBank->version() > kVersionNumber ){
    return Error(
" HltDecReports RawBank version # is larger then the known ones.... cannot decode, use newer version." ,StatusCode::FAILURE );
  }
  //if( hltdecreportsRawBank->sourceID() != kSourceID ){
  //  Warning( " HltDecReports RawBank has unexpected source ID. Will try to decode it anyway.",StatusCode::SUCCESS, 20 );
  // }

  // ----------------------------------------------------------
  const unsigned int *content = hltdecreportsRawBank->begin<unsigned int>();  

  // version 0 has only decreps, version 1 has TCK, taskID, then decreps...
  if (hltdecreportsRawBank->version() > 0 ) {
     outputSummary->setConfiguredTCK( *content++ );
     outputSummary->setTaskID( *content++ );
  } 
  // --------------------------------- get configuration --------------------
  unsigned int tck = outputSummary->configuredTCK();
  auto itbl =  m_translationtables.find(tck) ;
  if ( itbl  == std::end(m_translationtables) ) {
      auto& tbl = m_translationtables[tck]; // create new entry

      auto append0 =  [&]( const Gaudi::StringKey& id, bool empty ) { 
             for ( const auto& item : m_hltANNSvc->item_map( id ) ) {
                 tbl.insert( { item.second, empty ? item.first : Gaudi::StringKey{}  } );
             }
      };
      auto append1 =  [&]( const Gaudi::StringKey& id, bool empty ) { 
             for ( const auto& item : m_TCKANNSvc->i2s( tck,  id ) ) {
                 tbl.insert( { item.first, empty ? item.second : Gaudi::StringKey{} } ); // TODO: check for clashes...
             }
      };

      bool doHlt1 = ( m_sourceID == HltDecReportsWriter::kSourceID_Hlt1 || m_sourceID == 0);
      bool doHlt2 = ( m_sourceID == HltDecReportsWriter::kSourceID_Hlt2 || m_sourceID == 0);
      if (tck==0) {
         warning() << "configured TCK in rawbank = 0 -- assuming that the current HltANNSvc has the proper mapping configured. Proceed at your own risk..." << endmsg;
         append0(Hlt1SelectionID,doHlt1 );
         append0(Hlt2SelectionID,doHlt2 );
      } else {
         append1(Hlt1SelectionID,doHlt1 );
         append1(Hlt2SelectionID,doHlt2 );
      } 
      itbl =  m_translationtables.find(tck) ;
      assert(itbl!=std::end(m_translationtables));
  }
  // ---------------- loop over decisions in the bank body; insert them into the output container

  int err=0;
  switch ( hltdecreportsRawBank->version() ) {
    case 0 : err+=this->decodeHDR<v0_v1>( content, hltdecreportsRawBank->end<unsigned int>(), 
                                     *outputSummary, itbl->second );
        break;
    case 1 :
    case 2 : err+=this->decodeHDR<vx_vx>( content, hltdecreportsRawBank->end<unsigned int>(), 
                                     *outputSummary, itbl->second );
        break;
    default : Error(
" HltDecReports RawBank version # is larger then the known ones.... cannot decode, use newer version. " ,StatusCode::FAILURE );
    err+=1;
 }

 
  if ( msgLevel(MSG::VERBOSE) ){
    // debugging info
    verbose() << " ====== HltDecReports container size=" << outputSummary->size() << endmsg;  
    verbose() << *outputSummary << endmsg;
  }
  
  
  return err==0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

template <typename HDRConverter, typename I, typename Table> 
int HltDecReportsDecoder::decodeHDR(I i, I end,  HltDecReports& output, const Table& table ) const 
{
   int ret = 0;
   HDRConverter converter;
   while (i != end ) {
    HltDecReport dec(  converter.convert(*i++)  );
    int id=dec.intDecisionID();
    auto isel  = table.find( id );
    if ( isel == std::end(table) ) { // oops missing.
      std::ostringstream mess;
      mess << " No string key found for trigger decision in storage "
           << " id=" << id;
      Error(mess.str(), StatusCode::FAILURE, 50 ).ignore();
      ++ret;
    } else if (!isel->second.empty() ){  // has a non-zero string -- insert!!
        // debug() << " adding " << id << " as " << isel->second << endmsg;
        if( !output.insert( isel->second, dec ).isSuccess() ) {
          Error(" Duplicate decision report in storage "+std::string(isel->second), StatusCode::FAILURE, 20 ).ignore();
          ++ret;
        }
    }  // otherwise, present, but empty string -- skip!!
   }
   return ret;
}
//=============================================================================
