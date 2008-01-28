// $Id: L0MuonFromRawTrans.cpp,v 1.2 2008-01-28 08:54:09 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/RecHeader.h"
#include "Event/RawEvent.h"
#include "Event/L0MuonData.h"

// local
#include "L0MuonFromRawTrans.h"

#include "L0MuonKernel/ProcUtilities.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonFromRawTrans
//
// 2008-01-10 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonFromRawTrans );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonFromRawTrans::L0MuonFromRawTrans( const std::string& name,
                                        ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm ( name , pSvcLocator )
{
  m_configfile="L0MuonKernel.xml";
  declareProperty( "ConfigFile"     , m_configfile      );

  m_bankEventCounter  = 0;

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonFromRawTrans::~L0MuonFromRawTrans() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonFromRawTrans::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  L0Muon::RegisterFactory::selectInstance(1);
  // Instanciate the MuonTrigger registers
  std::string xmlFileName = m_configfile;
  info() <<  "XML file = " << xmlFileName << endmsg;
  L0Muon::L0MuonKernelFromXML(xmlFileName,false);

  for (int i= 0; i<2; ++i) {
    m_ctrlRaw[i] =  L0Muon::CtrlRawCnv(i);
  }
  // Converters for the banks of the 4 TELL1s connected to the processing boards
  for (int i= 0; i<4; ++i) {
    m_procRaw[i] = L0Muon::ProcRawCnv(i);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonFromRawTrans::execute() {

  debug() << "==> Execute" << endmsg;
  L0Muon::RegisterFactory::selectInstance(1);

  StatusCode sc;

  sc = decodeRawBanks();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  sc = writeOnTES(1);
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  //  m_procRaw[3].formattedDump(1,0);
  //m_procRaw[3].dump(1,0);
  //m_ctrlRaw[1].dump(1,0);

  sc = releaseRegisters();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonFromRawTrans::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


//=============================================================================
 
StatusCode L0MuonFromRawTrans::decodeRawBanks(){

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  debug() << "decodeRawBanks:  ==> got rawEvt " << endmsg;

  //   const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0Muon );
  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::TT );
  if (banks.size()!=0) {
    for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); banks.end() != itBnk; ++itBnk ) {
      int srcID = (*itBnk)->sourceID();
      int bankVersion  = (*itBnk)->version();
      std::vector<unsigned int> data;
      unsigned int* body = (*itBnk)->data();
      int size = (*itBnk)->size()/4;
      debug() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
              <<", sourceID is "<< srcID <<", size is "<< size <<endreq;
      for ( int k = 0; size > k; ++k ) {
        data.push_back( *body++ );
      }
      if (srcID==0)
        m_ctrlRaw[1].decodeBank(data,bankVersion);
      if (srcID==2)
        m_procRaw[3].decodeBank(data,bankVersion);      
    }
  }
  

  debug() << "decodeRawBanks:  ==> done" << endmsg;
  
  return StatusCode::SUCCESS;

}


StatusCode L0MuonFromRawTrans::writeOnTES(int procVersion, std::string extension){

  std::vector<L0Muon::PMuonCandidate> cands ;
  std::vector<L0Muon::PMuonCandidate>::iterator itcand;

  std::string location;
  location = rootInTES() + LHCb::L0MuonCandidateLocation::Default + extension;
  LHCb::L0MuonCandidates* pl0mcands = new LHCb::L0MuonCandidates();
  put(pl0mcands , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<2; ++i) {
    cands = m_ctrlRaw[i].muonCandidates();
    if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: => "<<cands.size()<<" candidates found"<< endreq;
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      pl0mcands->insert(l0mcand);
    }
  }

  location = rootInTES() + LHCb::L0MuonCandidateLocation::BCSU + extension;
  LHCb::L0MuonCandidates* pbcsucands = new LHCb::L0MuonCandidates();
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<2; ++i) {
    cands = m_ctrlRaw[i].muonCandidatesBCSU();
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES: side "<<i<< endreq;
      debug() << "writeOnTES: => "<<cands.size()<<" candidates found (BCSU)"<< endreq;
    }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      if( msgLevel(MSG::DEBUG) ) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      pbcsucands->insert(l0mcand);
    }
  }

  location = rootInTES() + LHCb::L0MuonCandidateLocation::PU  + extension;
  LHCb::L0MuonCandidates* ppucands = new LHCb::L0MuonCandidates();
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  put(ppucands , location );
  for (int i= 0; i<4; ++i) {      
    cands = m_procRaw[i].muonCandidatesPU();
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES: quarter "<<i<< endreq;
      debug() << "writeOnTES: => "<<cands.size()<<" candidates found (PU)"<< endreq;
    }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      ppucands->insert(l0mcand);
    }
  }


  location = rootInTES() + LHCb::L0MuonDataLocation::Default + extension;
  LHCb::L0MuonDatas* pdata = new LHCb::L0MuonDatas();
  put(pdata , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<4; ++i) {
    if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: quarter "<<i<< endreq;
    std::vector<LHCb::MuonTileID>  pus = m_procRaw[i].pus();
    for (std::vector<LHCb::MuonTileID>::iterator itpu=pus.begin(); itpu!=pus.end(); ++itpu){
      //debug() << "writeOnTES: pu = "<<itpu->toString()<< endreq;
      std::vector<LHCb::MuonTileID> ols = m_procRaw[i].ols(*itpu);
      //debug() << "writeOnTES: ols length = "<<ols.size()<< endreq;
      std::vector<LHCb::MuonTileID> neighs = m_procRaw[i].neighs(*itpu);
      //debug() << "writeOnTES: neighs length = "<<neighs.size()<< endreq;
      LHCb::L0MuonData *l0muondata = new LHCb::L0MuonData(*itpu, ols, neighs);
      pdata->insert(l0muondata);
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode L0MuonFromRawTrans::releaseRegisters(){

  for (int i= 0; i<2; ++i) m_ctrlRaw[i].release();
  for (int i= 0; i<4; ++i) m_procRaw[i].release();

  return StatusCode::SUCCESS;
}

LHCb::L0MuonCandidate* L0MuonFromRawTrans::l0muoncandidate(L0Muon::PMuonCandidate cand,int procVersion)
{
  bool debug = false;
  if (msgLevel( MSG::VERBOSE )) debug=true;
  
  std::vector<LHCb::MuonTileID> pads = L0Muon::add2pads(cand->quarter(),cand->board(),cand->pu(),
                                                        cand->colM3(),cand->rowM3(), cand->offM2(),cand->offM1(),
                                                        procVersion,debug);
  std::vector<double> kine = L0Muon::kine(pads[0],pads[1],procVersion,debug);
  LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], pads,cand->pT());
  return pl0muoncandidate;
}

