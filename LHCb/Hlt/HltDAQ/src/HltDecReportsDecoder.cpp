// $Id$
// Include files 
// from Gaudi
#include "Event/HltDecReport.h"
#include "Event/HltDecReports.h"

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
  : HltRawBankDecoderBase(  name, pSvcLocator )
{
  declareProperty("OutputHltDecReportsLocation",
                  m_outputHltDecReportsLocation= LHCb::HltDecReportsLocation::Default);
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecReportsDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // create output container and put it on TES
  auto  outputSummary = new HltDecReports();
  put( outputSummary, m_outputHltDecReportsLocation );
  
  std::vector<const RawBank*> hltdecreportsRawBanks = selectRawBanks( RawBank::HltDecReports );
  if ( hltdecreportsRawBanks.empty() ) {
    return Warning(" Could not find HltDecReports raw bank. Returning empty HltDecReports.", StatusCode::SUCCESS,20);
  }
  if( hltdecreportsRawBanks.size() != 1 ){
    Warning(" More then one HltDecReports RawBanks for requested SourceID in RawEvent. Will only process the first one. " ,StatusCode::SUCCESS, 20 ).ignore();
  }
  const RawBank *hltdecreportsRawBank = hltdecreportsRawBanks.front();
  if( hltdecreportsRawBank->magic() != RawBank::MagicPattern ){
    return Error(" HltDecReports RawBank has wrong magic number. Return without decoding.",StatusCode::FAILURE );
  }
  if( hltdecreportsRawBank->version() > kVersionNumber ){
    return Error(
" HltDecReports RawBank version # is larger then the known ones.... cannot decode, use newer version." ,StatusCode::FAILURE );
  }

  // ----------------------------------------------------------
  const unsigned int *content = hltdecreportsRawBank->begin<unsigned int>();  

  // version 0 has only decreps, version 1 has TCK, taskID, then decreps...
  if (hltdecreportsRawBank->version() > 0 ) {
     outputSummary->setConfiguredTCK( *content++ );
     outputSummary->setTaskID( *content++ );
  } 
  // --------------------------------- get configuration --------------------
  unsigned int tck = outputSummary->configuredTCK();
  const auto& tbl = id2string( tck ); 

  // ---------------- loop over decisions in the bank body; insert them into the output container
  int err=0;
  switch ( hltdecreportsRawBank->version() ) {
    case 0 : err+=this->decodeHDR<v0_v1>( content, hltdecreportsRawBank->end<unsigned int>(), 
                                          *outputSummary, tbl );
        break;
    case 1 :
    case 2 : err+=this->decodeHDR<vx_vx>( content, hltdecreportsRawBank->end<unsigned int>(), 
                                          *outputSummary, tbl );
        break;
    default : Error(
" HltDecReports RawBank version # is larger then the known ones.... cannot decode, use newer version. " ,StatusCode::FAILURE ).ignore();
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
   while ( i != end ) {
    HltDecReport dec(  converter.convert(*i++)  );
    auto isel = table.find( dec.intDecisionID() );
    if ( isel == std::end(table) ) { // oops missing.
      Error( std::string{ " No string key found for trigger decision in storage id = "} + std::to_string(dec.intDecisionID()),
             StatusCode::FAILURE, 50 ).ignore();
      ++ret;
    } else if (!!isel->second){  // has a non-zero string -- insert!!
        // debug() << " adding " << id << " as " << isel->second << endmsg;
        if ( !output.insert( isel->second, dec ).isSuccess() ) {
          Error(" Duplicate decision report in storage "+std::string(isel->second), StatusCode::FAILURE, 20 ).ignore();
          ++ret;
        }
    }  // otherwise, present, but should be skipped
   }
   return ret;
}
//=============================================================================
