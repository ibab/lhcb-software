// $Id: HltVertexReportsDecoder.cpp,v 1.5 2010-06-14 13:38:00 tskwarni Exp $
// Include files

#include "Event/HltVertexReports.h"
#include "Event/RawEvent.h"

// local
#include "HltVertexReportsDecoder.h"
#include "HltVertexReportsWriter.h"

namespace {
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
: HltRawBankDecoderBase( name , pSvcLocator )
{
  declareProperty("OutputHltVertexReportsLocation",
    m_outputHltVertexReportsLocation= LHCb::HltVertexReportsLocation::Default);
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexReportsDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // create output container for vertex selections keyed with string and put it on TES
  HltVertexReports* outputSummary = new HltVertexReports();
  put( outputSummary, m_outputHltVertexReportsLocation );

  // ----------------------------------------------------------
  // get the bank from RawEvent
  // ----------------------------------------------------------
  std::vector<const RawBank*> hltvertexreportsRawBanks = selectRawBanks( RawBank::HltVertexReports );

  if( hltvertexreportsRawBanks.empty() ){
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
    Warning( std::string{ " HltVertexReports Raw Bank version number " }
           + std::to_string( bankVersionNumber)
           + " higher than the one of the decoder " + std::to_string(kVersionNumber),
             StatusCode::SUCCESS, 20 ).ignore();
  }

  const auto& tbl = id2string( tck() );

  const unsigned int *i   = hltvertexreportsRawBank->begin<unsigned int>();
  const unsigned int *end = hltvertexreportsRawBank->end<unsigned int>();
  int nSel = *i++;
  while ( i < end ) { // avoid infinite loop with corrupt/incompatible data... (i.e. do NOT use i!=end)
    --nSel;
    unsigned nVert    = ( ( *i ) & 0xFFFFL ); // can we decode the per vertex size here???
    unsigned intSelID = ( *i++ >> 16 );

    auto  value = tbl.find( intSelID );
    if (value == std::end(tbl)) {
      Error( std::string{ " did not find name for id = "} + std::to_string(intSelID) + "; skipping this selection",
            StatusCode::SUCCESS, 50 );
      i += nVert * ( bankVersionNumber == 0 ? 5 : 11 ); // would have been nice to have a size / vtx in the bank...
      continue;
    }
    // skip reports if of wrong type
    if( !value->second ) {
	    i += nVert * ( bankVersionNumber == 0 ? 5 : 11 ); // would have been nice to have a size / vtx in the bank...
	    continue;
    }

    // create output container for vertices and put it on TES
    VertexBase::Container* verticesOutput = new VertexBase::Container();
    put( verticesOutput, m_outputHltVertexReportsLocation.value() + "/" + value->second.str()  );

    SmartRefVector<VertexBase> pVtxs;

    for( unsigned int j=0; j!=nVert; ++j ){
      VertexBase* pVtx = new VertexBase();
      double x = doubleFromInt( *i++ );
      double y = doubleFromInt( *i++ );
      double z = doubleFromInt( *i++ );
      pVtx->setPosition( { x,y,z } );
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
      pVtxs.emplace_back( pVtx );
    }

    // insert selection into the container
    if( outputSummary->insert(value->second,pVtxs) == StatusCode::FAILURE ){
      Error(" Failed to add Hlt vertex selection name "
            + std::string(value->second)
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
