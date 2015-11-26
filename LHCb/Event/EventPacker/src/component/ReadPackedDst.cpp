// Include files

// from Gaudi
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IOpaqueAddress.h"

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/RawEvent.h"
#include "Event/PackedTrack.h"
#include "Event/PackedCaloHypo.h"
#include "Event/PackedProtoParticle.h"
#include "Event/PackedRecVertex.h"
#include "Event/PackedTwoProngVertex.h"
#include "Event/PackedRichPID.h"
#include "Event/PackedMuonPID.h"
#include "Event/PackedParticle.h"
#include "Event/PackedVertex.h"
#include "Event/PackedWeightsVector.h"
#include "Event/PackedCaloCluster.h"
#include "Event/RecHeader.h"
#include "Event/ProcStatus.h"
#include "Event/ODIN.h"
#include "Event/RecSummary.h"

// local
#include "ReadPackedDst.h"

#ifdef _WIN32
// Disable warning C4355: 'this' : used in base member initializer list
#pragma warning ( disable : 4355 )
#endif

//-----------------------------------------------------------------------------
// Implementation file for class : ReadPackedDst
//
// 2009-01-19 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ReadPackedDst )

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
StatusCode ReadPackedDst::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  // Pass the input RawEvent to the tool
  sc = Gaudi::Utils::setProperty(&(*m_odinDecoder), "RawEventLocation", m_inputLocation );
  if (sc.isFailure()) return sc;
  // Attach data loader facility
  m_evtMgr = SmartIF<IDataManagerSvc>(eventSvc());
  if ( !m_evtMgr ) return StatusCode::FAILURE;
  m_addrCreator = serviceLocator()->service("EventPersistencySvc");
  if ( !m_addrCreator ) return StatusCode::FAILURE;

  // Pass the Postfix property to the tool
  return Gaudi::Utils::setProperty(&(*m_odinDecoder), "ODINLocation", LHCb::ODINLocation::Default + m_postfix);
}
//=============================================================================
// Initialize
//=============================================================================
StatusCode ReadPackedDst::finalize() {
  m_evtMgr = 0;
  m_addrCreator = 0;
  return GaudiAlgorithm::finalize();
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
    return Warning( "ODIN not found" );
  }

  const std::vector<LHCb::RawBank*>& adds = dstEvent->banks( LHCb::RawBank::DstAddress );
  for (std::vector<LHCb::RawBank*>::const_iterator itA = adds.begin(); adds.end() != itA; ++itA ) {
    std::string spars[3];
    unsigned long ipars[2];
    unsigned long clid;
    long styp;
    char* cptr;
    unsigned int* iptr  = (*itA)->data();
    clid = *iptr++;
    ipars[0] = *iptr++;
    ipars[1] = *iptr++;
    styp = *iptr++;

    if ( msgLevel(MSG::DEBUG) )
      debug() << "Address bank: Class ID " << clid << " par[0] " << ipars[0] << " par[1] " << ipars[1]
              << " svcType " << std::hex << styp;

    cptr = (char*)iptr;
    spars[0] = (char*)iptr;
    cptr += strlen(cptr) + 1;
    spars[1] = cptr;
    cptr += strlen(cptr) + 1;
    spars[2] = cptr;
    if ( msgLevel(MSG::DEBUG) )
      debug() << " par[0]='" << spars[0] << "' par[1]='" << spars[1] << "' par[2]='" << spars[2] << "'" << endmsg;

    /*
      IOpaqueAddress* pAddr = 0;
      if ( !m_addrCreator->createAddress(styp,clid, &spars[0],&ipars[0],pAddr).isSuccess() ) {
      error() << "Failed to create raw data address from the DstAddress bank!" << endmsg;
      }
      else if ( !m_evtMgr->registerAddress(spars[2],pAddr).isSuccess() ) {
      error() << "Failed to register raw data address from the DstAddress bank in TES!" << endmsg;
      pAddr->release();
      pAddr = 0;
      }
    */
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

    if ( msgLevel(MSG::DEBUG) )
      debug() << "Bank nb " << bank->sourceID() << " version " << version << " size " << m_size
              << " classID " << classID << " name '" << name << "'  nLink " << *m_data
              << endmsg;

    if (  LHCb::CLID_PackedTracks  == classID ) {

      LHCb::PackedTracks* tracks = new LHCb::PackedTracks();
      put( tracks, name + m_postfix );
      processLinks( tracks, version );
      getFromBlob<LHCb::PackedTrack>  ( tracks->tracks() , blobs );
      getFromBlob<LHCb::PackedState>  ( tracks->states() , blobs );
      getFromBlob<int>                ( tracks->ids()    , blobs );
      getFromBlob<std::pair<int,int> >( tracks->extras() , blobs );

    } else if ( LHCb::CLID_PackedRichPIDs        == classID ) {

      LHCb::PackedRichPIDs* pids = new LHCb::PackedRichPIDs();
      put( pids, name + m_postfix );
      processLinks( pids, version );
      getFromBlob<LHCb::PackedRichPID> ( pids->data(), blobs );

    } else if ( LHCb::CLID_PackedMuonPIDs        == classID ) {

      LHCb::PackedMuonPIDs* pids = new LHCb::PackedMuonPIDs();
      put( pids, name + m_postfix );
      processLinks( pids, version );
      getFromBlob<LHCb::PackedMuonPID> ( pids->data(), blobs );

    } else if ( LHCb::CLID_PackedCaloHypos       == classID ) {

      LHCb::PackedCaloHypos* caloHypos = new LHCb::PackedCaloHypos();
      put( caloHypos, name + m_postfix );
      processLinks( caloHypos, version );
      getFromBlob<LHCb::PackedCaloHypo> ( caloHypos->hypos() , blobs );
      getFromBlob<long long>            ( caloHypos->refs()  , blobs );

    } else if ( LHCb::CLID_PackedProtoParticles  == classID ) {

      LHCb::PackedProtoParticles* protos = new LHCb::PackedProtoParticles();
      put( protos, name + m_postfix );
      processLinks( protos, version );
      getFromBlob<LHCb::PackedProtoParticle> ( protos->protos() , blobs );
      getFromBlob<long long>                 ( protos->refs()   , blobs );
      getFromBlob<std::pair<int,int> >       ( protos->extras() , blobs );

    } else if ( LHCb::CLID_PackedRecVertices     == classID ) {

      LHCb::PackedRecVertices* recVertices = new LHCb::PackedRecVertices();
      put( recVertices, name + m_postfix );
      processLinks( recVertices, version );
      getFromBlob<LHCb::PackedRecVertex> ( recVertices->vertices() , blobs );
      getFromBlob<long long>             ( recVertices->refs()     , blobs );
      getFromBlob<std::pair<int,int> >   ( recVertices->extras()   , blobs );

    } else if ( LHCb::CLID_PackedTwoProngVertices     == classID ) {

      LHCb::PackedTwoProngVertices* recVertices = new LHCb::PackedTwoProngVertices();
      put( recVertices, name + m_postfix );
      processLinks( recVertices, version );
      getFromBlob<LHCb::PackedTwoProngVertex> ( recVertices->vertices() , blobs );
      getFromBlob<long long>                  ( recVertices->refs()     , blobs );
      getFromBlob<std::pair<int,int> >        ( recVertices->extras()   , blobs );

    } else if ( LHCb::CLID_RecSummary == classID ) {

      LHCb::RecSummary* summary = new LHCb::RecSummary();
      put( summary, name + m_postfix );
      processLinks( summary, version );
      const int nSums = nextInt();
      for ( int iSum = 0; iSum < nSums; ++iSum )
      {
        const int key   = nextInt();
        const int value = nextInt();
        summary->addInfo( (LHCb::RecSummary::DataTypes)key, value );
      }

    } else if ( LHCb::CLID_RecHeader == classID ) {

      LHCb::RecHeader* recHeader = new LHCb::RecHeader();
      put( recHeader, name + m_postfix );
      processLinks( recHeader, version );
      long long eventNumber = nextInt();
      eventNumber = (eventNumber << 32) + nextInt();
      recHeader->setEvtNumber( eventNumber );
      // Dummy reads from old data when RecHeader contained random seeds.
      // Data encoded after random seeds were removed has nb=0
      unsigned nb = nextInt();
      for ( unsigned int kk=0 ; nb > kk; ++kk ) {
        nextInt();
      }
      recHeader->setApplicationName( stringFromData() );
      recHeader->setApplicationVersion( stringFromData() );
      recHeader->setRunNumber( nextInt() );
      nb = nextInt();
      std::pair<std::string, std::string> aPair;
      std::vector<std::pair<std::string,std::string> > allPairs;
      for ( unsigned int kk=0; nb > kk; ++kk ) {
        aPair.first = stringFromData();
        aPair.second = stringFromData();
        allPairs.push_back( aPair );
      }
      recHeader->setCondDBTags( allPairs );

    } else if ( LHCb::CLID_PackedParticles        == classID ) {

      LHCb::PackedParticles* parts = new LHCb::PackedParticles();
      put( parts, name + m_postfix );
      processLinks( parts, version );
      getFromBlob<LHCb::PackedParticle> ( parts->data(), blobs );
      getFromBlob<std::pair<int,int> >  ( parts->extra(), blobs );
      getFromBlob<long long>            ( parts->daughters(), blobs );

    } else if ( LHCb::CLID_PackedVertices        == classID ) {

      LHCb::PackedVertices* verts = new LHCb::PackedVertices();
      put( verts, name + m_postfix );
      processLinks( verts, version );
      getFromBlob<LHCb::PackedVertex> ( verts->data(), blobs );
      getFromBlob<long long>          ( verts->outgoingParticles(), blobs );

    } else if ( LHCb::CLID_PackedWeightsVector   == classID ) {
      
      LHCb::PackedWeightsVector* weights = new LHCb::PackedWeightsVector();
      put( weights, name + m_postfix );
      processLinks( weights, version );
      getFromBlob<LHCb::PackedWeights> ( weights->data(),    blobs );
      getFromBlob<LHCb::PackedWeight>  ( weights->weights(), blobs );

    } else if ( LHCb::CLID_PackedCaloClusters   == classID ) {
      
      LHCb::PackedCaloClusters* clusters = new LHCb::PackedCaloClusters();
      put( clusters, name + m_postfix );
      processLinks( clusters, version );
      getFromBlob<LHCb::PackedCaloCluster>      ( clusters->data(),    blobs );
      getFromBlob<LHCb::PackedCaloClusterEntry> ( clusters->entries(), blobs );

    } else if ( LHCb::CLID_ProcStatus == classID ) {

      LHCb::ProcStatus* procStatus = new LHCb::ProcStatus();
      put( procStatus, name + m_postfix );
      processLinks( procStatus, version );
      procStatus->setAborted( (nextInt()!=0) );
      const unsigned int nbAlg = nextInt();
      LHCb::ProcStatus::AlgStatusVector algs;
      algs.reserve(nbAlg);
      for ( unsigned int kk = 0 ; nbAlg > kk ; ++kk ) 
      {
        const std::string temp = stringFromData();
        algs.push_back( LHCb::ProcStatus::AlgStatus(temp,nextInt()) );
      }
      procStatus->setAlgs(algs);

    } else if ( LHCb::CLID_RawEvent == classID ) {

      LHCb::RawEvent* rawEvent = new LHCb::RawEvent();
      put( rawEvent, name + m_postfix );
      processLinks( rawEvent, version );
      while ( 0 < m_size ) {
        int type     = nextInt();
        int sourceID = nextInt();
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Read bank " << type << " source " << sourceID << " and add to " << name + m_postfix << endmsg;
        const std::vector<LHCb::RawBank*>& myBanks = dstEvent->banks( (LHCb::RawBank::BankType)type );
        for (std::vector<LHCb::RawBank*>::const_iterator itBnk = myBanks.begin(); myBanks.end() != itBnk; ++itBnk ) {
          if ( (*itBnk)->sourceID() == sourceID ) {
            rawEvent->addBank( *itBnk );
          }
        }
      }

    } else {
      warning() << "--- Unknown classID " << classID << " when decoding DstEvent" << endmsg;
      return StatusCode::FAILURE;
    }
    //== End of bank. Check for leakage...
    if ( 0 != m_size ) {
      warning() << "Non empty bank " << name << " after processing : " << m_size << " words left." << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
//=========================================================================
//  Decode a blob to recreate the data
//=========================================================================
template <class CLASS> 
void ReadPackedDst::getFromBlob( std::vector<CLASS>& vect,
                                 const std::vector<LHCb::RawBank*>& blobs ) {
  unsigned int totSize = *m_data++;
  unsigned int nObj    = *m_data++;
  int blobNb           = *m_data++;
  m_size -= 3;
  unsigned int extraSize = 0;
  if ( nObj * sizeof( CLASS) != totSize ) {
    if ( totSize > nObj * sizeof( CLASS ) ) {
      if ( ( totSize / nObj ) * nObj == totSize ) {       // extra words per object...
        extraSize = ( totSize / nObj ) - sizeof( CLASS );
      }
    }
    if ( 0 == extraSize ) {
      warning() << "getFromBlob: Total " << totSize << " nObj " << nObj
                << " of size " << sizeof( CLASS ) << " = " << nObj * sizeof( CLASS)
                << " *** DOES NOT MATCH ***" << endmsg;
    }
  }
  if ( msgLevel(MSG::DEBUG) )
    debug() << "totSize " << totSize << " nObj " << nObj << " extraSize " << extraSize << " blobNb " << blobNb << endmsg;

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
        warning() << "Blob number " << blobNb << " not found." << endmsg;
        return;
      }
    }
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Found " <<  myBank->size() << " bytes, need " << totSize << " " << endmsg;
    memcpy( temp, myBank->data(), myBank->size() );
    temp    += (myBank->size()/4);
    totSize -= myBank->size();
    blobNb++;
  }

  //== Now copy to the target vector, changing lines to columns...
  //== and padding with 0 the extra size.

  unsigned int objSize = sizeof( CLASS )/4;
  vect.resize( nObj );
  int* vectPt = (int*) &*vect.begin();
  temp   = &*tempVect.begin();
  for ( unsigned int l = 0 ; nObj > l ; ++l ) {
    for ( unsigned int k = 0 ; objSize > k ; ++k ) {
      *vectPt++ =  *(temp + l + nObj * k );
    }
    if ( 0 < extraSize ) {
      for ( unsigned int k = 0 ; extraSize > k ; ++k ) {
        *vectPt++ =  0;
      }
    }
  }
}

//=============================================================================
