// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/VeloLiteCluster.h"
#include "Event/FTRawCluster.h"
#include "Event/FTCluster.h"

#include "Event/RawEvent.h"

// local
#include "FTRawBankDecoder.h"
#include "FTRawBankParams.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FTRawBankDecoder
//
// 2012-05-11 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FTRawBankDecoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FTRawBankDecoder::FTRawBankDecoder( const std::string& name,
                                      ISvcLocator* pSvcLocator)
: Decoder::AlgBase ( name , pSvcLocator )
{
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Other, LHCb::RawEventLocation::Default};
  initRawEventSearch();
  declareProperty("OutputLocation",m_outputClusterLocation=LHCb::FTRawClusterLocation::Default,"Output location for clusters");
  
}
//=============================================================================
// Destructor
//=============================================================================
FTRawBankDecoder::~FTRawBankDecoder() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTRawBankDecoder::initialize() {
  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
 
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTRawBankDecoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Retrieve the RawEvent:
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  
  if( rawEvent == NULL ) {
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Raw Event not found in " << m_rawEventLocations << endmsg;
    return StatusCode::SUCCESS;
  }


  FastClusterContainer<LHCb::FTRawCluster,int>* clus = new FastClusterContainer<LHCb::FTRawCluster,int>();
  put( clus, m_outputClusterLocation);

  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks( LHCb::RawBank::FTCluster );
  if ( msgLevel(MSG::DEBUG) ) debug() << "Number of raw banks " << banks.size() << endmsg;
  for ( std::vector <LHCb::RawBank*>::const_iterator itB = banks.begin(); banks.end() != itB; ++itB ) {
    int source  = (*itB)->sourceID();
    int layer   = source/4;
    int quarter = source & 3;
    int size    = (*itB)->size(); // in bytes, multiple of 4
    short int* pt = (short int*)(*itB)->data();
    if ( msgLevel(MSG::DEBUG) ) debug() << "source " << source << " layer "
                                        << layer << " quarter "
                                        << quarter << " size " << size << endmsg;
    if ( 0 == (*itB)->version()  ) {
      size /= 2;   // in short int
      while( size > 0 ) {
        int sipmHeader = (*pt++);
        --size;
        if ( 0 == size && 0 == sipmHeader ) continue;  // padding at the end...
        int mySiPM = sipmHeader >> FTRawBank::sipmShift;
        int nClus  = sipmHeader &  FTRawBank::nbClusMaximum;
        if ( 0 < nClus && msgLevel(MSG::DEBUG) )
          debug() << "   SiPM " << mySiPM << " nClus " << nClus
                  << " size " << size << endmsg;
        while ( nClus > 0 ) {
          int fraction = ( (*pt) >> FTRawBank::fractionShift ) & FTRawBank::fractionMaximum;
          int cell     = ( (*pt) >> FTRawBank::cellShift     ) & FTRawBank::cellMaximum;
          int cSize    = ( (*pt) >> FTRawBank::sizeShift     ) & FTRawBank::sizeMaximum;
          int charge   = ( (*pt) >> FTRawBank::chargeShift   ) & FTRawBank::chargeMaximum;
          if ( msgLevel( MSG::VERBOSE ) ) {
            verbose() << format(  "  cell %4d frac %3d charge %5d size %3d code %4.4x",
                                  cell, fraction, charge, cSize, (*pt) ) << endmsg;
          }
          LHCb::FTChannelID id( layer, quarter, mySiPM, cell );
          LHCb::FTRawCluster tmp( id, fraction, cSize, charge );
          clus->push_back( tmp );
          pt++;
          --size;
          --nClus;
        }
      }
    } else {
      info() << "** Unsupported FT bank version " << (*itB)->version() 
             << " for source " << source << " size " << size << " bytes."
             << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTRawBankDecoder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return Decoder::AlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
