// $Id: ReadPackedDst.cpp,v 1.6 2009-02-24 15:15:22 ocallot Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/RawEvent.h"
#include "Event/PackedTrack.h"
#include "Event/PackedCaloHypo.h"
#include "Event/PackedProtoParticle.h"
#include "Event/PackedRecVertex.h"
#include "Event/PackedTwoProngVertex.h"
#include "Event/RecHeader.h"
#include "Event/ProcStatus.h"
#include "Event/ODIN.h"

// local
#include "ReadPackedDst.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ReadPackedDst
//
// 2009-01-19 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ReadPackedDst );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReadPackedDst::ReadPackedDst( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_odinDecoder("ODINDecodeTool", this)
{
  declareProperty( "InputLocation", m_inputLocation = "/Event/DAQ/DstEvent" );
  declareProperty( "Postfix",       m_postfix       = "Test" );
}
//=============================================================================
// Initialize
//=============================================================================
StatusCode ReadPackedDst::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  // Pass the Postfix property to the tool
  return Gaudi::Utils::setProperty(&(*m_odinDecoder), "ODINLocation", LHCb::ODINLocation::Default + m_postfix);
}

//=============================================================================
// Destructor
//=============================================================================
ReadPackedDst::~ReadPackedDst() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ReadPackedDst::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::RawEvent* dstEvent = get<LHCb::RawEvent>( m_inputLocation );

  // Decode the ODIN bank
  m_odinDecoder->execute();
  if ( !exist<LHCb::ODIN>(LHCb::ODINLocation::Default + m_postfix) ) {
    warning() << "ODIN not found." << endreq;
    return StatusCode::FAILURE;
  }
  const std::vector<LHCb::RawBank*>& adds = dstEvent->banks( LHCb::RawBank::DstAddress );
  for (std::vector<LHCb::RawBank*>::const_iterator itA = adds.begin(); adds.end() != itA; ++itA ) {
    unsigned int* iptr  = (*itA)->data();
    unsigned int clid = *iptr++;
    unsigned int par0 = *iptr++;
    unsigned int par1 = *iptr++;
    unsigned int styp = *iptr++;
    
    debug() << "Address bank: Class ID " << clid << " ipar[0] " << par0 << " ipar[1] " << par1
            << " svcType " << styp;
    char* cptr = (char*) iptr;
    char* dptr = cptr + strlen(cptr) + 1;
    debug() << " par[0]='" << cptr << "' par[1]='" << dptr << "'" << endreq;
  }

  const std::vector<LHCb::RawBank*>& banks = dstEvent->banks( LHCb::RawBank::DstBank );
  const std::vector<LHCb::RawBank*>& blobs = dstEvent->banks( LHCb::RawBank::DstData );
  for (std::vector<LHCb::RawBank*>::const_iterator itB = banks.begin(); banks.end() != itB; ++itB ) {
    LHCb::RawBank* bank = *itB;
    int version = bank->version();
    m_size      = bank->size()/4;   // in word
    m_data      = bank->data();
    unsigned int classID = nextInt();
    std::string name = stringFromData();

    debug() << "Bank nb " << bank->sourceID() << " version " << version << " size " << m_size
            << " classID " << classID << " name '" << name << "'  nLink " << *m_data
            << endreq;
    if (  LHCb::CLID_PackedTracks  == classID ) {  //== PackedTracks
      LHCb::PackedTracks* tracks = new LHCb::PackedTracks();
      put( tracks, name + m_postfix );
      processLinks( tracks, version );
      getFromBlob<LHCb::PackedTrack>  ( tracks->tracks() , blobs );
      getFromBlob<LHCb::PackedState>  ( tracks->states() , blobs );
      getFromBlob<int>                ( tracks->ids()    , blobs );
      getFromBlob<std::pair<int,int> >( tracks->extras() , blobs );

    } else if ( LHCb::CLID_PackedCaloHypos       == classID ) {
      LHCb::PackedCaloHypos* caloHypos = new LHCb::PackedCaloHypos();
      put( caloHypos, name + m_postfix );
      processLinks( caloHypos, version );
      getFromBlob<LHCb::PackedCaloHypo> ( caloHypos->hypos() , blobs );
      getFromBlob<int>                  ( caloHypos->refs()  , blobs );

    } else if ( LHCb::CLID_PackedProtoParticles  == classID ) {
      LHCb::PackedProtoParticles* protos = new LHCb::PackedProtoParticles();
      put( protos, name + m_postfix );
      processLinks( protos, version );
      getFromBlob<LHCb::PackedProtoParticle> ( protos->protos() , blobs );
      getFromBlob<int>                       ( protos->refs()    , blobs );
      getFromBlob<std::pair<int,int> >       ( protos->extras() , blobs );

    } else if ( LHCb::CLID_PackedRecVertices     == classID ) {
      LHCb::PackedRecVertices* recVertices = new LHCb::PackedRecVertices();
      put( recVertices, name + m_postfix );
      processLinks( recVertices, version );
      getFromBlob<LHCb::PackedRecVertex> ( recVertices->vertices() , blobs );
      getFromBlob<int>                   ( recVertices->refs()    , blobs );
      getFromBlob<std::pair<int,int> >   ( recVertices->extras() , blobs );

    } else if ( LHCb::CLID_PackedTwoProngVertices     == classID ) {
      LHCb::PackedTwoProngVertices* recVertices = new LHCb::PackedTwoProngVertices();
      put( recVertices, name + m_postfix );
      processLinks( recVertices, version );
      getFromBlob<LHCb::PackedTwoProngVertex> ( recVertices->vertices() , blobs );
      getFromBlob<int>                        ( recVertices->refs()    , blobs );
      getFromBlob<std::pair<int,int> >        ( recVertices->extras() , blobs );

    } else if ( LHCb::CLID_RecHeader == classID ) {
      LHCb::RecHeader* recHeader = new LHCb::RecHeader();
      put( recHeader, name + m_postfix );
      processLinks( recHeader, version );
      longlong eventNumber = nextInt();
      eventNumber = (eventNumber << 32) + nextInt();
      recHeader->setEvtNumber( eventNumber );
      unsigned nb = nextInt();
      std::vector<long int> seeds;
      for ( unsigned int kk=0 ; nb > kk; ++kk ) {
        seeds.push_back( nextInt() );
      }
      recHeader->setRandomSeeds( seeds );
      recHeader->setApplicationName( stringFromData() );
      recHeader->setApplicationVersion( stringFromData() );
      recHeader->setRunNumber( nextInt() );
      nb = nextInt();
      std::vector<std::pair<std::string,std::string> > allPairs;
      for ( unsigned int kk=0; nb > kk; ++kk ) {
        std::string temp = stringFromData();
        std::pair<std::string, std::string> aPair( temp, stringFromData() );
        allPairs.push_back( aPair );
      }
      recHeader->setCondDBTags( allPairs );

    } else if ( LHCb::CLID_ProcStatus == classID ) {
      LHCb::ProcStatus* procStatus = new LHCb::ProcStatus();
      put( procStatus, name + m_postfix );
      processLinks( procStatus, version );
      procStatus->setAborted( (nextInt()!=0) );
      unsigned int nbAlg = nextInt();
      std::string temp;
      for ( unsigned int kk = 0 ; nbAlg > kk ; ++kk ) {
        procStatus->addAlgorithmStatus( stringFromData(), nextInt() );
      }

    } else {
      warning() << "--- Unknown classID " << classID << " when decoding DstEvent" << endreq;
      return StatusCode::FAILURE;
    }
    //== End of bank. Check for leakage...
    if ( 0 != m_size ) {
      warning() << "Non empty bank " << name << " after processing : " << m_size << " words left." << endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
//=========================================================================
//  Decode a blob to recreate the data
//=========================================================================
template <class CLASS> void ReadPackedDst::getFromBlob( std::vector<CLASS>& vect,
                                                        const std::vector<LHCb::RawBank*>& blobs ) {
  unsigned int totSize = *m_data++;
  unsigned int nObj    = *m_data++;
  int blobNb           = *m_data++;
  m_size -= 3;
  if ( nObj * sizeof( CLASS) != totSize ) {
    warning() << "getFromBlob: Tot " << totSize << " nObj " << nObj << " size " << sizeof( CLASS ) << endreq;
  }
  debug() << "totSize " << totSize << " nObj " << nObj << " blobNb " << blobNb << endreq;

  //== First restore a big vector of int in case the data is split into several blobs
  std::vector<int> tempVect( totSize/4 );  // size in bytes
  int* temp = &*tempVect.begin();

  while ( totSize > 0 ) {
    LHCb::RawBank* myBank = blobs[blobNb];
    if ( myBank->sourceID() != blobNb ) {
      myBank = 0;
      for ( std::vector<LHCb::RawBank*>::const_iterator itB = blobs.begin(); blobs.end() != itB; ++itB ) {
        if ( (*itB)->sourceID() == blobNb ) {
          myBank = *itB;
          break;
        }
      }
      if ( 0 == myBank ) {
        warning() << "Blob number " << blobNb << " not found." << endreq;
        return;
      }
    }
    debug() << "Found " <<  myBank->size() << " bytes, need " << totSize << " " << endreq;
    memcpy( temp, myBank->data(), myBank->size() );
    temp    += (myBank->size()/4);
    totSize -= myBank->size();
    blobNb++;
  }

  //== Now copy to the target vector, changing lines to columns...

  unsigned int objSize = sizeof( CLASS )/4;
  vect.resize( nObj );
  int* vectPt = (int*) &*vect.begin();
  temp   = &*tempVect.begin();
  for ( unsigned int l = 0 ; nObj > l ; ++l ) {
    for ( unsigned int k = 0 ; objSize > k ; ++k ) {
      *vectPt++ =  *(temp + l + nObj * k );
    }
  }
}

//=============================================================================
