// $Id: WritePackedDst.cpp,v 1.7 2010-05-18 09:03:21 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"

// Kernel
#include "Kernel/StandardPacker.h"

// Event model
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
#include "Event/RecHeader.h"
#include "Event/ProcStatus.h"
#include "Event/ODIN.h"
#include "Event/RecSummary.h"

#include "MDF/MDFHeader.h"
#include "MDF/RawEventHelpers.h"

// local
#include "WritePackedDst.h"
#include "PackedBank.h"

//-----------------------------------------------------------------------------
// Implementation file for class : WritePackedDst
//
// 2008-12-01 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( WritePackedDst )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
WritePackedDst::WritePackedDst( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_dst          ( NULL ),
    m_blobNumber   ( 0    ),
    m_bankNb       ( 0    )
{
  declareProperty( "Containers", m_containers );
}

//=============================================================================
// Destructor
//=============================================================================
WritePackedDst::~WritePackedDst() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode WritePackedDst::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "Write containers ";
  for ( std::vector<std::string>::const_iterator itS = m_containers.begin();
        m_containers.end() != itS; ++itS ) {
    info() << " '" << *itS << "',";
  }
  info() << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode WritePackedDst::execute()
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_dst = new LHCb::RawEvent();
  put( m_dst, "/Event/DAQ/DstEvent" );
  m_blobNumber = 0;
  m_bankNb     = 0;

  //== Write the opaque address of the Raw Event according to Markus specifications
  LHCb::RawEvent* evt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  IRegistry* reg = evt->registry();
  if ( !reg )  {
    warning() << "No opaque address for " <<  LHCb::RawEventLocation::Default << " !!!" << endmsg;
    return StatusCode::FAILURE;
  }
  IOpaqueAddress* padd = reg->address();

  std::vector<unsigned int> data(200);
  unsigned int* dataPt = &*data.begin();
  *dataPt++ = evt->classID();
  *dataPt++ = padd->ipar()[0];
  *dataPt++ = padd->ipar()[1];
  *dataPt++ = padd->svcType();
  char* charPt = (char*)dataPt;
  strcpy( charPt, padd->par()[0].c_str() );
  charPt = charPt + strlen( padd->par()[0].c_str() ) + 1;
  strcpy( charPt, padd->par()[1].c_str() );
  charPt = charPt + strlen( padd->par()[1].c_str() ) + 1;
  strcpy( charPt, reg->identifier().c_str() );
  charPt = charPt + strlen( reg->identifier().c_str()+7 ) + 1; // Omit '/Event' !
  int bLen = charPt - (char*)&*data.begin();
  data.resize( (bLen+3)/4);
  m_dst->addBank( 0, LHCb::RawBank::DstAddress, 0, data );

  for ( std::vector<std::string>::const_iterator itC = m_containers.begin();
        m_containers.end() != itC; ++itC ) {
    //== Try to get the container, a data object
    if ( !exist<DataObject>( *itC ) ) {
      warning() << "Container " << *itC << " does not exist." << endmsg;
      return StatusCode::FAILURE;
    }
    DataObject* myObj = get<DataObject>( *itC );
    unsigned int myClID = myObj->clID();

    if ( LHCb::CLID_PackedTracks  == myClID ) {  //== PackedTracks

      LHCb::PackedTracks* in = get<LHCb::PackedTracks>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->begin())      , (in->end() - in->begin()) , sizeof( LHCb::PackedTrack) );
      storeInBlob( bank, &(*in->beginState()) , in->sizeState()           , sizeof( LHCb::PackedState) );
      storeInBlob( bank, &(*in->beginIds())   , in->sizeId()              , sizeof( int ) );
      storeInBlob( bank, &(*in->beginExtra()) , in->sizeExtra()           , sizeof( std::pair<int,int> ) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedRichPIDs        == myClID ) {

      LHCb::PackedRichPIDs* in = get<LHCb::PackedRichPIDs>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->data().begin()), in->data().size(), sizeof(LHCb::PackedRichPID) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedMuonPIDs        == myClID ) {

      LHCb::PackedMuonPIDs* in = get<LHCb::PackedMuonPIDs>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->data().begin()), in->data().size(), sizeof(LHCb::PackedMuonPID) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedCaloHypos       == myClID ) {

      LHCb::PackedCaloHypos* in = get<LHCb::PackedCaloHypos>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->begin())     , (in->end() - in->begin()) , sizeof( LHCb::PackedCaloHypo) );
      storeInBlob( bank, &(*in->beginRefs()) , in->sizeRef()             , sizeof( int ) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedProtoParticles  == myClID ) {

      LHCb::PackedProtoParticles* in = get<LHCb::PackedProtoParticles>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->begin())      , (in->end() - in->begin()) , sizeof( LHCb::PackedProtoParticle) );
      storeInBlob( bank, &(*in->beginRefs())  , in->sizeRef()             , sizeof( int ) );
      storeInBlob( bank, &(*in->beginExtra()) , in->sizeExtra()           , sizeof( std::pair<int,int> ) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedRecVertices     == myClID ) {

      LHCb::PackedRecVertices* in = get<LHCb::PackedRecVertices>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->begin())      , (in->end() - in->begin()) , sizeof( LHCb::PackedRecVertex) );
      storeInBlob( bank, &(*in->beginRefs())  , in->sizeRefs()            , sizeof( int ) );
      storeInBlob( bank, &(*in->beginExtra()) , in->sizeExtra()           , sizeof( std::pair<int,int> ) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedTwoProngVertices     == myClID ) {

      LHCb::PackedTwoProngVertices* in = get<LHCb::PackedTwoProngVertices>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->begin())      , (in->end() - in->begin()) , sizeof( LHCb::PackedTwoProngVertex) );
      storeInBlob( bank, &(*in->beginRefs())  , in->sizeRefs()            , sizeof( int ) );
      storeInBlob( bank, &(*in->beginExtra()) , in->sizeExtra()           , sizeof( std::pair<int,int> ) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_RecSummary == myClID ) {

      LHCb::RecSummary * in = get<LHCb::RecSummary>( *itC );
      PackedBank bank( in );
      bank.storeInt( in->summaryData().size() );
      for ( LHCb::RecSummary::SummaryData::const_iterator iS = in->summaryData().begin();
            iS != in->summaryData().end(); ++iS )
      {
        bank.storeInt( iS->first  );
        bank.storeInt( iS->second ); 
      }
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_RecHeader == myClID ) {

      LHCb::RecHeader* in = get<LHCb::RecHeader>( *itC );
      PackedBank bank( in );
      unsigned int evHigh = ((in->evtNumber() ) >> 32) && 0xFFFFFFFF;
      unsigned int evLow  = ( in->evtNumber() && 0xFFFFFFFF );
      bank.addEntry( evHigh, evLow, in->randomSeeds().size() );
      for ( unsigned int kk=0 ; in->randomSeeds().size() > kk; ++kk ) {
        bank.storeInt( in->randomSeeds()[kk] );
      }
      bank.storeString( in->applicationName() );
      bank.storeString( in->applicationVersion() );
      bank.storeInt( in->runNumber() );
      bank.storeInt( in->condDBTags().size() );
      for ( unsigned int kk=0; in->condDBTags().size() != kk; ++kk ) {
        bank.storeString( in->condDBTags()[kk].first );
        bank.storeString( in->condDBTags()[kk].second );
      }
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedParticles        == myClID ) {

      LHCb::PackedParticles* in = get<LHCb::PackedParticles>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->data().begin()), in->data().size(), sizeof(LHCb::PackedParticle) );
      storeInBlob( bank, &(*in->extra().begin()), in->extra().size(), sizeof(std::pair<int,int>) );
      storeInBlob( bank, &(*in->daughters().begin()), in->daughters().size(), sizeof(int) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedVertices        == myClID ) {

      LHCb::PackedVertices* in = get<LHCb::PackedVertices>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->data().begin()), in->data().size(), sizeof(LHCb::PackedVertex) );
      storeInBlob( bank, &(*in->outgoingParticles().begin()), in->outgoingParticles().size(), sizeof(int) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_PackedWeightsVector   == myClID ) {

      LHCb::PackedWeightsVector* in = get<LHCb::PackedWeightsVector>( *itC );
      PackedBank bank( in );
      storeInBlob( bank, &(*in->data().begin()), in->data().size(), sizeof(LHCb::PackedWeights) );
      storeInBlob( bank, &(*in->weights().begin()), in->weights().size(), sizeof(LHCb::PackedWeight) );
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_ProcStatus == myClID ) {

      LHCb::ProcStatus* in = get<LHCb::ProcStatus>( *itC );
      PackedBank bank( in );

      bank.storeInt( in->aborted() );
      bank.storeInt( in->algs().size() );
      for ( unsigned int kk=0; in->algs().size() != kk; ++kk ) {
        bank.storeString( in->algs()[kk].first );
        bank.storeInt( in->algs()[kk].second );
      }
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else if ( LHCb::CLID_ODIN == myClID ) {

      //== Get the ODIN from raw data, and copy it!
      const std::vector<LHCb::RawBank*>& odinBanks = evt->banks(LHCb::RawBank::ODIN);
      if ( odinBanks.size() ) {
        LHCb::RawBank* odin = *odinBanks.begin();
        m_dst->addBank( odin );
      }

    } else if ( LHCb::CLID_RawEvent == myClID ) {

      //== For RawEvent data, copy all banks to the output RawEvent, keep track of the banks
      LHCb::RawEvent* in = get<LHCb::RawEvent>( *itC );
      PackedBank bank( in );
      for ( unsigned int bnkTyp = LHCb::RawBank::L0Calo;
            LHCb::RawBank::LastType != bnkTyp; ++bnkTyp ) {
        const std::vector<LHCb::RawBank*>& banks = in->banks( (LHCb::RawBank::BankType)bnkTyp );
        for ( std::vector<LHCb::RawBank*>::const_iterator itB = banks.begin(); banks.end() != itB; ++itB ) {
          bank.storeInt( bnkTyp );
          bank.storeInt( (*itB)->sourceID() );
          if ( msgLevel(MSG::DEBUG) )
            debug() << "Added bank type " << bnkTyp << " source " << (*itB)->sourceID() << endmsg;
          m_dst->addBank( *itB );
        }
      }
      m_dst->addBank( m_bankNb++, LHCb::RawBank::DstBank, in->version(), bank.data() );

    } else {
      warning() << "--- Unknown class ID " <<  myClID
                << " for container " << *itC << " --" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  //== Write the DAQ status bank

  int len = 0;
  size_t i;
  for(len = 0, i = LHCb::RawBank::L0Calo; i < LHCb::RawBank::LastType; ++i)  {
    const std::vector<LHCb::RawBank*>& banks = m_dst->banks( LHCb::RawBank::BankType(i) ) ;
    for(std::vector<LHCb::RawBank*>::const_iterator j = banks.begin(); j != banks.end(); ++j)  {
      len += (*j)->totalSize();
    }
  }
  LHCb::RawBank* hdrBank = m_dst->createBank( 0, LHCb::RawBank::DAQ, DAQ_STATUS_BANK,
                                              sizeof(LHCb::MDFHeader)+sizeof(LHCb::MDFHeader::Header1), 0);
  LHCb::MDFHeader* hdr = (LHCb::MDFHeader*)hdrBank->data();
  hdr->setChecksum(0);
  hdr->setCompression(0);
  hdr->setHeaderVersion(3);
  hdr->setSpare(0);
  hdr->setDataType(LHCb::MDFHeader::BODY_TYPE_BANKS);
  hdr->setSubheaderLength(sizeof(LHCb::MDFHeader::Header1));
  hdr->setSize( len );
  LHCb::MDFHeader::SubHeader h = hdr->subHeader();

  unsigned int trMask[] = {~0,~0,~0,~0};
  LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default );
  h.H1->setTriggerMask(trMask);
  h.H1->setRunNumber(   odin->runNumber() );
  h.H1->setOrbitNumber( odin->orbitNumber() );
  h.H1->setBunchID(     odin->bunchId() );
  m_dst->adoptBank( hdrBank, true );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Store the data part as a (series of) blob of bytes....
//=========================================================================
void WritePackedDst::storeInBlob( PackedBank& pBnk, const void* data, unsigned int nb, unsigned int bSize ) {

#define MAXBANK 65000

  //== First, put the data in column order: x for all tracks, then y, then ...
  unsigned int iSize = bSize/4;
  int totSize = nb * bSize;
  int* start = (int*) data;
  std::vector<int> temp;
  temp.reserve( totSize/4 );
  for ( unsigned int k = 0 ; iSize > k ; ++k ) {
    for ( unsigned int l = 0 ; nb > l ; ++l ) {
      temp.push_back( *(start + iSize*l + k ) );
    }
  }

  start = &(*temp.begin());
  unsigned int len   = totSize;

  pBnk.addEntry( len, nb, m_blobNumber );
  while ( MAXBANK < len ) {
    LHCb::RawBank* bank = m_dst->createBank( m_blobNumber++, LHCb::RawBank::DstData,
                                             0, MAXBANK, start );
    m_dst->addBank( bank );
    start = start + MAXBANK/4;
    len  -= MAXBANK;
  }
  if ( 0 < len ) {
    LHCb::RawBank* bank = m_dst->createBank( m_blobNumber++, LHCb::RawBank::DstData,
                                             0, len, start );
    m_dst->addBank( bank );
  }
}

//=============================================================================
