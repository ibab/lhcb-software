// Include files

// from Gaudi
#include "Event/FTLiteCluster.h"
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
  declareProperty("OutputLocation",m_outputClusterLocation=LHCb::FTLiteClusterLocation::Default,"Output location for clusters");
  
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
  
  if( rawEvent == nullptr ) {
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Raw Event not found in " << m_rawEventLocations << endmsg;
    return StatusCode::SUCCESS;
  }


  FastClusterContainer<LHCb::FTLiteCluster,int>* clus = new FastClusterContainer<LHCb::FTLiteCluster,int>();
  put( clus, m_outputClusterLocation);

  const std::vector<LHCb::RawBank*>& banks = rawEvent->banks( LHCb::RawBank::FTCluster );
  if ( msgLevel(MSG::DEBUG) ) debug() << "Number of raw banks " << banks.size() << endmsg;
  
  for ( const LHCb::RawBank* bank : banks){
    int source  = bank->sourceID();
    int layer   = source/4;
    int quarter = source & 3;
    int size    = bank->size(); // in bytes, multiple of 4
    short int* pt = (short int*)bank->data();
    
    if ( msgLevel(MSG::DEBUG) ) debug() << "source " << source << " layer "
                                        << layer << " quarter "
                                        << quarter << " size " << size << endmsg;
    if ( 1 == bank->version()  ) {
      size /= 2;   // in short int
      while( size > 0 ) {
        int sipmHeader = (*pt++);
        --size;
        if ( 0 == size && 0 == sipmHeader ) continue;  // padding at the end...
        int QuarterSiPMNber = sipmHeader >> FTRawBank::sipmShift;
        int module = 99;
        int mat = 9;

        StatusCode sc = RetrieveModuleMat(QuarterSiPMNber,quarter,module,mat);
        if(sc.isFailure()) return sc;

        int mySiPM = QuarterSiPMNber & 15;
        int nClus  = sipmHeader &  FTRawBank::nbClusMaximum;
        if ( 0 < nClus && msgLevel(MSG::DEBUG) )
          debug() << "   SiPM " << mySiPM << " nClus " << nClus
                  << " size " << size << endmsg;
        while ( nClus > 0 ) {
          int fraction = ( (*pt) >> FTRawBank::fractionShift ) & FTRawBank::fractionMaximum;
          int cell     = ( (*pt) >> FTRawBank::cellShift     ) & FTRawBank::cellMaximum;
          int sipmId   = ( (*pt) >> FTRawBank::sipmIdShift   ) & FTRawBank::sipmIdMaximum;
          int cSize    = 1+(( (*pt) >> FTRawBank::sizeShift     ) & FTRawBank::sizeMaximum); // add 1 to make sure to keep clusters with size 1,2,3 and 4 using 2 bits to encode the cluster size in the data format
          int charge   = ( (*pt) >> FTRawBank::chargeShift   ) & FTRawBank::chargeMaximum;
          if ( msgLevel( MSG::VERBOSE ) ) {
            verbose() << format(  "  cell %4d sipmId %3d frac %3d charge %5d size %3d code %4.4x",
                                  cell, sipmId,fraction, charge, cSize, (*pt) ) << endmsg;
          }
          //LHCb::FTChannelID id( layer, quarter, mySiPM, cell );
          LHCb::FTChannelID id( layer, module, mat, mySiPM, cell );
          LHCb::FTLiteCluster tmp( id, fraction, cSize, charge );
          clus->push_back( tmp );
          pt++;
          --size;
          --nClus;
        }
      }
    } else {
      error() << "** Unsupported FT bank version " << bank->version() 
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
StatusCode FTRawBankDecoder::RetrieveModuleMat(const int quartSipmNb, const int quarter , int &locmod, int &locmat)
{
  if((quarter == 0)||(quarter == 1) ) locmat = 1;
  if((quarter == 2)||(quarter == 3) ) locmat = 0;
  int intermod = quartSipmNb / 16;
  if((quarter % 2) != 0) // x positive part
  {
    if (intermod < 5) locmod = intermod;
    else locmod = 10;
  }
  else  // x negative part
  {
    if (intermod > 0) locmod = intermod + 4;
    else locmod = 11;
  }
  if(locmat > 1)  return StatusCode::FAILURE;
  if(locmod > 11) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}
