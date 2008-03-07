// $Id: L0MuonFromRawTrans.cpp,v 1.7 2008-03-07 15:34:42 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/RecHeader.h"
#include "Event/RawEvent.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

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
  m_ievt=0;
  
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

  // Converters for the banks of the 2 TELL1s connected to the controller boards
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

  if (exist<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default)) {
    LHCb::RecHeader* evt = get<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default);
    m_ievt = evt->evtNumber();
  } else{
    ++m_ievt;
  }
  

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
  if (msgLevel( MSG::DEBUG )) debug() << "decodeRawBanks:  ==> got rawEvt " << endmsg;

  //   const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0Muon );
  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::TT );
  if (banks.size()!=0) {
    for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); banks.end() != itBnk; ++itBnk ) {
      int srcID = (*itBnk)->sourceID();
      int bankVersion  = (*itBnk)->version();
      std::vector<unsigned int> data;
      unsigned int* body = (*itBnk)->data();
      int size = (*itBnk)->size()/4;
      if (msgLevel( MSG::DEBUG ))
        debug() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
                <<", sourceID is "<< srcID <<", size is "<< size <<endreq;
      for ( int k = 0; size > k; ++k ) {
        data.push_back( *body++ );
      }
      if (srcID==0) {
        m_ctrlRaw[1].decodeBank(data,bankVersion);
      }
      if (srcID==5) {
        m_ctrlRaw[0].decodeBank(data,bankVersion);
      }
       if (srcID==3){
        m_procRaw[0].decodeBank(data,bankVersion);
      }
       if (srcID==4){
        m_procRaw[1].decodeBank(data,bankVersion);
      }
      if (srcID==2) {
        m_procRaw[2].decodeBank(data,bankVersion);      
      }
       if (srcID==1){
        m_procRaw[3].decodeBank(data,bankVersion);
      }
     }
  }
  

  debug() << "decodeRawBanks:  ==> done" << endmsg;
  
  return StatusCode::SUCCESS;

}

int L0MuonFromRawTrans::l0EventNumber()
{
  int L0EventNumber=-1;
  for (int iside=0; iside<2; ++iside) {
    if (m_ctrlRaw[iside].isActiv()){
      L0EventNumber = m_ctrlRaw[iside].L0EventNumber(0,0);
      break;
    }
  }
  if ( L0EventNumber==-1) {
    for (int iq=0; iq<4; ++iq) {
      if (m_procRaw[iq].isActiv()){ 
        L0EventNumber = m_procRaw[iq].L0EventNumber(0,0);
        break;
      }
    }
  }
  return L0EventNumber;
}

int L0MuonFromRawTrans::l0_B_Id()
{
  int L0_B_Id=-1;
  for (int iside=0; iside<2; ++iside) {
    if (m_ctrlRaw[iside].isActiv()){
      L0_B_Id = m_ctrlRaw[iside].L0_B_Id(0,0);
      break;
    }
  }
  if ( L0_B_Id==-1) {
    for (int iq=0; iq<4; ++iq) {
      if (m_procRaw[iq].isActiv()){ 
        L0_B_Id = m_procRaw[iq].L0_B_Id(0,0);
        break;
      }
    }
  }
  return L0_B_Id;
}


StatusCode L0MuonFromRawTrans::writeOnTES(int procVersion, std::string extension){

  std::vector<L0Muon::PMuonCandidate> cands ;
  std::vector<L0Muon::PMuonCandidate>::iterator itcand;

  std::string location;
  location = LHCb::L0MuonCandidateLocation::Default + extension;
  LHCb::L0MuonCandidates* pl0mcands = new LHCb::L0MuonCandidates();
  put(pl0mcands , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<2; ++i) {
    if (!m_ctrlRaw[i].isActiv()) continue;
    cands = m_ctrlRaw[i].muonCandidates();
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES: side "<<i<< endreq;
      debug() << "writeOnTES: => "<<cands.size()<<" candidates found"<< endreq;
    }
    if (m_ctrlRaw[i].inError()) {
      if (msgLevel( MSG::DEBUG )){
        debug() << "writeOnTES: => in Error !!!"<< endreq;
      }
      //continue;
    }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      pl0mcands->insert(l0mcand);
    }
  }

  //   location = LHCb::L0MuonCandidateLocation::BCSU + "Ctrl"+ extension;
  //   LHCb::L0MuonCandidates* pbcsucands = new LHCb::L0MuonCandidates();
  //   put(pbcsucands , location );
  //   if (msgLevel( MSG::DEBUG )) {
  //     debug() << "writeOnTES -------------------------"<< endreq;
  //     debug() << "writeOnTES at "<< location << endreq;
  //   }
  //   for (int i= 0; i<2; ++i) {
  //     if (!m_ctrlRaw[i].isActiv()) continue;
  //     cands = m_ctrlRaw[i].muonCandidatesBCSU();
  //     if (msgLevel( MSG::DEBUG )) {
  //       debug() << "writeOnTES: side "<<i<< endreq;
  //       debug() << "writeOnTES: => "<<cands.size()<<" candidates found (BCSU)"<< endreq;
  //     }
  //     if (m_ctrlRaw[i].inError()) {
  //       if (msgLevel( MSG::DEBUG )){
  //         debug() << "writeOnTES: => in Error !!!"<< endreq;
  //       }
  //       //continue;
  //     }
  //     for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
  //       if( msgLevel(MSG::DEBUG) ) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
  //       LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
  //       pbcsucands->insert(l0mcand);
  //     }
  //   }

  location = LHCb::L0MuonCandidateLocation::BCSU + extension;
  LHCb::L0MuonCandidates* pbcsucands = new LHCb::L0MuonCandidates();
  put(pbcsucands , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<4; ++i) {
    if (!m_procRaw[i].isActiv()) continue;
    cands = m_procRaw[i].muonCandidatesBCSU();
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES: side "<<i<< endreq;
      debug() << "writeOnTES: => "<<cands.size()<<" candidates found (BCSU)"<< endreq;
    }
    if (m_procRaw[i].inError()) {
      if (msgLevel( MSG::DEBUG )){
        debug() << "writeOnTES: => in Error !!!"<< endreq;
      }
      //continue;
    }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      if( msgLevel(MSG::DEBUG) ) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      pbcsucands->insert(l0mcand);
    }
  }

  location = LHCb::L0MuonCandidateLocation::PU  + extension;
  LHCb::L0MuonCandidates* ppucands = new LHCb::L0MuonCandidates();
  put(ppucands , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<4; ++i) {      
    if (!m_procRaw[i].isActiv()) continue;
    cands = m_procRaw[i].muonCandidatesPU();
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES: quarter "<<i<< endreq;
      debug() << "writeOnTES: => "<<cands.size()<<" candidates found (PU)"<< endreq;
    }
    if (m_procRaw[i].inError()) {
      if (msgLevel( MSG::DEBUG )){
        debug() << "writeOnTES: => in Error !!!"<< endreq;
      }
      //continue;
    }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      ppucands->insert(l0mcand);
    }
  }


  location = LHCb::L0MuonDataLocation::Default + extension;
  LHCb::L0MuonDatas* pdata = new LHCb::L0MuonDatas();
  put(pdata , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<4; ++i) {
    if (!m_procRaw[i].isActiv()) continue;
    if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: quarter "<<i<< endreq;
    if (m_procRaw[i].inError()) {
      if (msgLevel( MSG::DEBUG )){
        debug() << "writeOnTES: => in Error !!!"<< endreq;
      }
      //continue;
    }
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

  int L0EventNumber= l0EventNumber();
  int L0_B_Id      = l0_B_Id();
  location = LHCb::L0MuonInfoLocation::Default + extension;
  LHCb::L0MuonInfo* pl0minfo = new LHCb::L0MuonInfo();
  pl0minfo->setL0EventNumber(L0EventNumber);
  pl0minfo->setL0_B_Id(L0_B_Id);
  put(pl0minfo, location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }

  location = LHCb::L0MuonCtrlErrorLocation::Default + extension;
  LHCb::L0MuonCtrlErrors* pl0mctrlerrors = new LHCb::L0MuonCtrlErrors();
  put(pl0mctrlerrors, location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<2; ++i) {
    if (!m_ctrlRaw[i].isActiv()) continue;
    for (int ii=0; ii<2; ++ii){
      int quarter = i*2+ii;
      LHCb::L0MuonCtrlError * l0mctrlerror = new LHCb::L0MuonCtrlError(quarter);
      // Fill errors
      if (m_ctrlRaw[i].decodingError(ii)>0 ) l0mctrlerror->setDecoding(true);
      int header=0;
      for (int ich=0; ich<2; ++ich){
        if (L0EventNumber!=m_ctrlRaw[i].L0EventNumber(ii,ich)) header+=1<<(ich*3);
        if (L0_B_Id      !=m_ctrlRaw[i].L0_B_Id(ii,ich)) header+=1<<(1+ich*3);
        if (m_ctrlRaw[i].boardIndexError(ii,ich)) header+=1<<(2+ich*3);
      }
      l0mctrlerror->setHeader(header);
      if ((L0_B_Id&0xF)!=m_ctrlRaw[i].BCID_CU(ii)) l0mctrlerror->setCu_bcid_error(m_ctrlRaw[i].BCID_CU(ii));
      if ((L0_B_Id&0xF)!=m_ctrlRaw[i].BCID_SU(ii)) l0mctrlerror->setSu_bcid_error(m_ctrlRaw[i].BCID_SU(ii));
      for (int ib=0; ib<12; ++ib) {
        int bcid1=m_ctrlRaw[i].BCID_BCSU(ii,0,ib)==(L0_B_Id&0xF) ? -1 : m_ctrlRaw[i].BCID_BCSU(ii,0,ib);
        int bcid2=m_ctrlRaw[i].BCID_BCSU(ii,1,ib)==(L0_B_Id&0xF) ? -1 : m_ctrlRaw[i].BCID_BCSU(ii,1,ib);
        if ( (bcid1!=-1) || (bcid2!=-1) ){
          l0mctrlerror->setBcsu_bcid_error(ib,bcid1,bcid2);
        }
        l0mctrlerror->setCu_link_error(ib,m_ctrlRaw[i].pb_link_error(ii,0,ib)) ;
        l0mctrlerror->setSu_link_error(ib,m_ctrlRaw[i].pb_link_error(ii,1,ib)) ;
        if(m_ctrlRaw[i].status_BCSU(ii,ib)>2) l0mctrlerror->setBcsu_status(ib,m_ctrlRaw[i].status(ib));
      }
      if(m_ctrlRaw[i].status(ii)>2) l0mctrlerror->setStatus(m_ctrlRaw[i].status(ii));
      if (!l0mctrlerror->isEmpty()){
        debug() << "before inserting Q"<<quarter+1<< endreq;
        pl0mctrlerrors->insert(l0mctrlerror);
        debug() << "... inserting Q"<<quarter+1<< " done"<<endreq;
      }
    }
  }
  location = LHCb::L0MuonProcErrorLocation::Default + extension;
  LHCb::L0MuonProcErrors* pl0mprocerrors = new LHCb::L0MuonProcErrors();
  put(pl0mprocerrors, location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int iq= 0; iq<4; ++iq) {
    if (!m_procRaw[iq].isActiv()) continue;
    for (int ib=0; ib<12; ++ib){
      LHCb::L0MuonProcError * l0mprocerror = new LHCb::L0MuonProcError(m_procRaw[iq].mid_BCSU(ib),ib);
      // Fill errors
      if (m_procRaw[iq].decodingError(ib)>0 ) l0mprocerror->setDecoding(true);
      int header=0;
      for (int ich=0; ich<2; ++ich){
        if (L0EventNumber!=m_procRaw[iq].L0EventNumber(ib,ich)) header+=1<<(ich*3);
        if (L0_B_Id      !=m_procRaw[iq].L0_B_Id(ib,ich)) header+=1<<(1+ich*3);
        if (m_procRaw[iq].boardIndexError(ib,ich)) header+=1<<(2+ich*3);
      }
      l0mprocerror->setHeader(header);
      if ((L0_B_Id&0xF)!=m_procRaw[iq].BCID_BCSU(ib)) l0mprocerror->setBcsu_bcid_error(m_procRaw[iq].BCID_BCSU(ib));
      for (int ipu=0; ipu<4; ++ipu){
        if ((L0_B_Id&0xF)!=m_procRaw[iq].BCID_PU(ib,ipu)) l0mprocerror->setPu_bcid_error(ipu,m_procRaw[iq].BCID_PU(ib,ipu));
        l0mprocerror->setPu_opt_link_error(ipu,m_procRaw[iq].opt_link_error(ib,ipu));
        l0mprocerror->setPu_ser_link_error(ipu,m_procRaw[iq].ser_link_error(ib,ipu));
        l0mprocerror->setPu_par_link_error(ipu,m_procRaw[iq].par_link_error(ib,ipu));
        if (m_procRaw[iq].status_PU(ib,ipu)>2) l0mprocerror->setPu_status(ipu,m_procRaw[iq].status_PU(ib,ipu));
      }
      if (m_procRaw[iq].status_BCSCU(ib)>2) l0mprocerror->setBcsu_status(m_procRaw[iq].status_BCSCU(ib));
      
      if (!l0mprocerror->isEmpty()){
        debug() << "before inserting ib"<<ib<<" mid="<<m_procRaw[iq].mid_BCSU(ib)<< endreq;
        pl0mprocerrors->insert(l0mprocerror);
        debug() << "... inserting ib"<<ib<<" done"<< endreq;
      }
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
//   debug()<<"l0muoncandidate Seed"<<pads[2].toString()<<" M2: "<<pads[1].toString()<<" M1: "<<pads[0].toString()<< endreq;
  LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], pads,cand->pT());
  return pl0muoncandidate;
}

