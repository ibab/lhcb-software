// $Id: L0MuonProcessorDatasFromRaw.cpp,v 1.2 2006-10-20 14:48:55 cattanem Exp $
// Include files 
#include <boost/dynamic_bitset.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "L0MuonProcessorDatasFromRaw.h"

// from L0Event
#include "Event/L0MuonBase.h"
#include "Event/L0ProcessorData.h"

#include "L0MuonKernel/DecodeRawBufferCtrl.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonProcessorDatasFromRaw
//
// 2003-12-15 : Julien Cogan
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( L0MuonProcessorDatasFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonProcessorDatasFromRaw::L0MuonProcessorDatasFromRaw( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonProcessorDatasFromRaw::~L0MuonProcessorDatasFromRaw() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0MuonProcessorDatasFromRaw::initialize() {

  debug() << "==> Initialise" << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonProcessorDatasFromRaw::execute() {

  debug() << "==> Execute" << endreq;

  SmartDataPtr<LHCb::RawEvent> buf ( eventSvc(), LHCb::RawEventLocation::Default );
  debug() << "==> Execute buf point to the rawevent" << endreq;
  LHCb::RawEvent rawEvt( buf );

  debug() << "==> Execute rawEvt instanciated" << endreq;

  const std::vector<LHCb::RawBank*>& data = rawEvt.banks( LHCb::RawBank::L0Muon );
  if (data.size()==0) {
    error() << "No L0Muon bank found"<<endmsg;
    return StatusCode::FAILURE;
  }
  
  debug() << "==> Execute before reading first bank data.size()= "<<data.size() << endreq;
  // Loop over banks; search the one with sourceID=0 (TELL1 linked to the ctrl board)
  std::vector<LHCb::RawBank*>::const_iterator itBnk;
  bool bankfound=false;
  for (itBnk= data.begin(); itBnk<data.end(); itBnk++) {
    // Bank source id
    int sourceID = (*itBnk)->sourceID();
    debug() << "==> Execute   bank's source id is sourceID= "<<sourceID << endmsg;
    bankfound=true;
    if (sourceID==0) break;
    bankfound=false;
  } // End of Loop over banks
  if (!bankfound) {
    error() << "Bank with sourceID = 0 not found"<<endmsg;
    return StatusCode::FAILURE;
  }

  const unsigned int* ptData = (*itBnk)->data();
  int bankSize = (*itBnk)->size();
  // Fill a bit set with the bank content
  boost::dynamic_bitset<> rawbufbitset(bankSize*32);
  int bankCount=0;
  debug() << "==> Execute before loop over bank's content; bankSize= "<< bankSize<< endreq;
  while ( bankCount<bankSize ){
    unsigned long word = (*ptData++);
    boost::dynamic_bitset<> wordBitset(bankSize*32,word);
    wordBitset=wordBitset<<(bankCount*32);
    rawbufbitset|=wordBitset;
    bankCount++;
  }
  debug() << "==> Execute after loop over bank's content \n" << rawbufbitset << endreq;

  // Decode the bit set and fill the L0MuonCandidates container
  L0Muon::DecodeRawBufferCtrl decodedRawBufCtrl ;
  decodedRawBufCtrl.setBitset(rawbufbitset);
  std::vector<L0Muon::MuonCandidate> candidatesCtrl = decodedRawBufCtrl.candidatesCTRL();
  debug() << "==> Execute candidatesCtrl.size()= "<<candidatesCtrl.size()  << endreq;

  // Initialize the words to be sent
  unsigned long cu[4],su[4];
  unsigned long  index[4];
  for (int iq=0;iq<4;iq++) {
    cu[iq] = L0MuonBase::emptyMuonData ;
    su[iq] = L0MuonBase::emptyMuonData ;
    index[iq] = 0;
  }

  // Fill the words to be sent to L0DU
  std::vector<L0Muon::MuonCandidate>::iterator icand;
  for (icand=candidatesCtrl.begin(); icand<candidatesCtrl.end(); icand++) {
    // Prepare words to be sent to the L0DU
    int quarter = (*icand).quarter();
    debug() << "==> fillL0ProcessorData candidate quarter= "<< quarter <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate pT= "<< (*icand).pT() <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate PU= "<< (*icand).pu() <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate board= "<< (*icand).board() <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate addM3= "<< (*icand).addM3() <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate charge= "<< (*icand).charge() <<"\n" << endreq;
    if (index[quarter]==0) {
      cu[quarter] += ( (*icand).pT()     << L0DUBase::Muon::Pt1::Shift) & L0DUBase::Muon::Pt1::Mask;
      cu[quarter] += ( (*icand).addM3()  << L0DUBase::Muon::Address1::Shift) & L0DUBase::Muon::Address1::Mask;
      su[quarter] += ( (*icand).pu()     << L0DUBase::Muon::Pu1::Shift) & L0DUBase::Muon::Pu1::Mask;
      su[quarter] += ( (*icand).board()  << L0DUBase::Muon::Pb1::Shift) & L0DUBase::Muon::Pb1::Mask;
      su[quarter] += ( (*icand).charge() << L0DUBase::Muon::Sign1::Shift) & L0DUBase::Muon::Sign1::Mask;  
    } else {
      cu[quarter] += ( (*icand).pT()     << L0DUBase::Muon::Pt2::Shift) & L0DUBase::Muon::Pt2::Mask;
      cu[quarter] += ( (*icand).addM3()  << L0DUBase::Muon::Address2::Shift) & L0DUBase::Muon::Address2::Mask;
      su[quarter] += ( (*icand).pu()     << L0DUBase::Muon::Pu2::Shift) & L0DUBase::Muon::Pu2::Mask;
      su[quarter] += ( (*icand).board()  << L0DUBase::Muon::Pb2::Shift) & L0DUBase::Muon::Pb2::Mask;
      su[quarter] += ( (*icand).charge() << L0DUBase::Muon::Sign2::Shift) & L0DUBase::Muon::Sign2::Mask;  
    }
    index[quarter]++;
  }
  for (int iq=0;iq<4;iq++) {
    debug() << "==> Execute cu["<<iq<<"] = " << cu[iq] <<"\n" << endreq;;
    debug() << "==> Execute su["<<iq<<"] = " << su[iq] <<"\n" << endreq;;
  }

  // Fill the keyedObject for the L0DU
  LHCb::L0ProcessorData* l0CU0 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonCU0 , cu[0]); 
  LHCb::L0ProcessorData* l0SU0 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonSU0 , su[0]); 
  LHCb::L0ProcessorData* l0CU1 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonCU1 , cu[1]); 
  LHCb::L0ProcessorData* l0SU1 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonSU1 , su[1]); 
  LHCb::L0ProcessorData* l0CU2 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonCU2 , cu[2]); 
  LHCb::L0ProcessorData* l0SU2 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonSU2 , su[2]); 
  LHCb::L0ProcessorData* l0CU3 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonCU3 , cu[3]); 
  LHCb::L0ProcessorData* l0SU3 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonSU3 , su[3]); 
  LHCb::L0ProcessorDatas* l0MuonDatas = new LHCb::L0ProcessorDatas();
  l0MuonDatas->insert( l0CU0 ) ;
  l0MuonDatas->insert( l0SU0 ) ; 
  l0MuonDatas->insert( l0CU1 ) ;
  l0MuonDatas->insert( l0SU1 ) ; 
  l0MuonDatas->insert( l0CU2 ) ;
  l0MuonDatas->insert( l0SU2 ) ; 
  l0MuonDatas->insert( l0CU3 ) ;
  l0MuonDatas->insert( l0SU3 ) ; 
  put(  l0MuonDatas ,  LHCb::L0ProcessorDataLocation::Muon );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonProcessorDatasFromRaw::finalize() {

  debug() << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================

