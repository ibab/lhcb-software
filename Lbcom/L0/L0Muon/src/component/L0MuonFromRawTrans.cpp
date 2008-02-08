// $Id: L0MuonFromRawTrans.cpp,v 1.4 2008-02-08 11:17:00 jucogan Exp $
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
  : GaudiHistoAlg ( name , pSvcLocator )
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
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

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
  
  // Histos
  setHistoDir("L0Muon/Decoding");
  std::string name;
  // Histogram the ref event number and BId
  name = "L0EventNumber";
  book1D(name,name,-0.5,-0.5+4096,4096);
  name = "L0_B_Id";
  book1D(name,name,-0.5,-0.5+3564,3564);
  name = "Decoding_errors";
  book1D(name,name,-0.5,-0.5+13*4,13*4);
  
  // Record errors : 
  // * 1 bin per TELL1
  name = "L0Muon_Error";
  book1D(name,name,-0.5,-0.5+6.,6);

  // Ctrl boards
  // * 2 bins per quarter (1 for CU and 1 for SU) 
  name = "CtrlError_L0EventNumber";
  book1D(name,name,-0.5,-0.5+8.,8);
  name = "CtrlError_L0_B_Id";
  book1D(name,name,-0.5,-0.5+8.,8);
  name = "CtrlError_BoardIndex";
  book1D(name,name,-0.5,-0.5+8.,8);
  name = "CtrlError_BCID_CU_SU";
  book1D(name,name,-0.5,-0.5+8.,8);  
  // * 2 bins per BCSU (1 for CU and 1 for SU) 
  name = "CtrlError_BCID_BCSU";
  book1D(name,name,-0.5,-0.5+96.,96);
  name = "CtrlError_pb_link";
  book1D(name,name,-0.5,-0.5+96.,96);
  
  // Proc boards
  // * 2 bins per board (1 per frame)
  name = "ProcError_L0EventNumber";
  book1D(name,name,-0.5,-0.5+96.,96);
  name = "ProcError_L0_B_Id";
  book1D(name,name,-0.5,-0.5+96.,96);
  name = "ProcError_BoardIndex";
  book1D(name,name,-0.5,-0.5+96.,96);
  name = "ProcError_BCID_BCSU";
  book1D(name,name,-0.5,-0.5+48.,48);
  name = "ProcError_BCID_PU";
  book1D(name,name,-0.5,-0.5+192.,192);
  name = "ProcError_opt_link";
  book1D(name,name,-0.5,-0.5+192.,192);
  name = "ProcError_ser_link";
  book1D(name,name,-0.5,-0.5+192.,192);
  name = "ProcError_par_link";
  book1D(name,name,-0.5,-0.5+192.,192);
  
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
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  sc = writeOnTES(1);
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  //  m_procRaw[3].formattedDump(1,0);
  //m_procRaw[3].dump(1,0);
  //m_ctrlRaw[1].dump(1,0);

  fillHisto();
  
  sc = releaseRegisters();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonFromRawTrans::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
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
      if (msgLevel( MSG::DEBUG ))
        debug() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
                <<", sourceID is "<< srcID <<", size is "<< size <<endreq;
      info() << "decodeRawBanks: evt# "<<m_ievt<<" L0Muon bank (version "<< bankVersion <<" ) found"
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

void L0MuonFromRawTrans::fillHisto()
{
  std::string name;
  int ibin;

  // Try to find a L0EvnetNumber and a L0_B_Id ans set it as a reference
  int L0EventNumber=-1;
  int L0_B_Id = -1;
  for (int iside=0; iside<2; ++iside) {
    if (m_ctrlRaw[iside].isActiv()){
      L0EventNumber = m_ctrlRaw[iside].L0EventNumber(0,0);
      L0_B_Id       = m_ctrlRaw[iside].L0_B_Id(0,0);
      break;
    }
  }
  if ( (L0EventNumber==-1) || (L0_B_Id==-1) ) {
    for (int iq=0; iq<4; ++iq) {
      if (m_procRaw[iq].isActiv()){
        L0EventNumber = m_procRaw[iq].L0EventNumber(0,0);
        L0_B_Id       = m_procRaw[iq].L0_B_Id(0,0);
        break;
      }
    }
  }

  name = "L0EventNumber";
  fill(histo1D(name),L0EventNumber,1,name);
  name = "L0_B_Id";
  fill(histo1D(name),L0_B_Id,1,name);
  
  // If no reference has been found, return
  if ( (L0EventNumber==-1) || (L0_B_Id==-1) ) return;

  //  info()<<std::hex<<"L0_B_Id 0x"<<L0_B_Id<<std::dec<<" = "<<L0_B_Id<<endmsg;
  name = "Decoding_errors";
  for (int iside=0; iside<2; ++iside){ // Loop over side A and C
    for (int ii=0; ii<2; ++ii){ // Loop over the 2 quarters of a side
      int iq = iside*2+ii;
      if (m_ctrlRaw[iside].isActiv() && m_ctrlRaw[iside].decodingError(ii)>0 )
        fill(histo1D(name),iq*13+12,1,name);
      for (int ib=0; ib<12; ++ib) { // Loop over the 12 processing boards
        if (m_procRaw[iq].isActiv() && m_procRaw[iq].decodingError(ib)>0 )
          fill(histo1D(name),iq*13+ib,1,name);
      } // End of loop over the 12 processing boards
    } // End of loop over the 2 quarters of a side
  } // End of loop over side A and C
  
  // Ctrl
  for (int iside=0; iside<2; ++iside){ // Loop over side A and C

    if (!m_ctrlRaw[iside].isActiv()) continue;
    
    for (int ii=0; ii<2; ++ii){ // Loop over the 2 quarters of a side

      if (m_ctrlRaw[iside].decodingError(ii)>0) continue;
      
      int iq = iside*2+ii;
      for (int ich=0; ich<2; ++ich){ // Loop over the 2 frame sent by a controller board to the TELL1
        ibin =ich*4+iq;
        name = "CtrlError_L0EventNumber";
        if (L0EventNumber!=m_ctrlRaw[iside].L0EventNumber(ii,ich)){
          fill(histo1D(name),ibin,1,name);
          info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" ch: "<<ich<< endmsg;
        } 
        name = "CtrlError_L0_B_Id";      
        if (L0_B_Id!=m_ctrlRaw[iside].L0_B_Id(ii,ich)){
          fill(histo1D(name),ibin,1,name);
          info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" ch: "<<ich<< endmsg;
        } 
        name = "CtrlError_BoardIndex";   
        if (m_ctrlRaw[iside].boardIndexError(ii,ich)){
          fill(histo1D(name),ibin,1,name);
          info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" ch: "<<ich<< endmsg;
        } 
        for (int ib=0; ib<12; ++ib) { // Loop over the 12 BCSU of this controller board
          ibin = ich*48 + iq*12 + ib;
          name = "CtrlError_BCID_BCSU";
          if ((L0_B_Id&0xF)!=m_ctrlRaw[iside].BCID_BCSU(ii,ich,ib)){
            fill(histo1D(name),ibin,1,name);
            info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" ch: "<<ich<<" board "<<ib<< endmsg;
          }
          name = "CtrlError_pb_link";
          if (m_ctrlRaw[iside].pb_link_error(ii,ich,ib)!=0){
            fill(histo1D(name),ibin,1,name);
            info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" ch: "<<ich<<" board "<<ib<< endmsg;
          }
        } // End of loop over the 12 BCSU of this controller board
      } // End of loop over the 2 frame sent by a controller board to the TELL1
      name = "CtrlError_BCID_CU_SU";   
      if ((L0_B_Id&0xF)!=m_ctrlRaw[iside].BCID_SU(ii)){
        fill(histo1D(name),iq,1,name); 
        info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<< endmsg;
      }
      if ((L0_B_Id&0xF)!=m_ctrlRaw[iside].BCID_SU(ii)){
        fill(histo1D(name),iq+4,1,name); 
        info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<< endmsg;
      }
    } // End of loop over the 2 quarters of a side
  } // End of loop over side A and C

  // Proc
  for (int iq=0; iq<4; ++iq){ // Loop over the 4 quarters

    if (!m_procRaw[iq].isActiv()) continue;
    
    for (int ib=0; ib<12; ++ib) { // Loop over the 12 proc. boards of a quarter

      if ( m_procRaw[iq].decodingError(ib)>0) continue;
      
      for (int ich=0; ich<2; ++ich){ // Loop over the 2 frame sent by a proc board to the TELL1
        ibin =ich*48 + iq*12 + ib;
        name = "ProcError_L0EventNumber";
        if (L0EventNumber!=m_procRaw[iq].L0EventNumber(ib,ich)){
          fill(histo1D(name),ibin,1,name);
          info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" board:"<<ib <<" ch: "<<ich<< endmsg;
        }
        name = "ProcError_L0_B_Id";
        if (L0_B_Id!=m_procRaw[iq].L0_B_Id(ib,ich)){
          fill(histo1D(name),ibin,1,name);
          info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" board:"<<ib <<" ch: "<<ich<< endmsg;
        }
        name = "ProcError_BoardIndex";
        if (m_procRaw[iq].boardIndexError(ib,ich)){
          fill(histo1D(name),ibin,1,name);
          info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" board:"<<ib <<" ch: "<<ich<< endmsg;
        }
      } // End of loop over the 2 frame sent by a proc board to the TELL1
      ibin =iq*12 + ib;
      name = "ProcError_BCID_BCSU";
      if ((L0_B_Id&0xF)!=m_procRaw[iq].BCID_BCSU(ib)){
        fill(histo1D(name),ibin,1,name);
        info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" board "<<ib<< endmsg;
      }
      for (int ipu=0; ipu<4; ++ipu){ // Loop over the 4 PUs of a proc board
        ibin = iq*48 + ib*4 + ipu;
        name = "ProcError_BCID_PU";
        if ((L0_B_Id&0xF)!=m_procRaw[iq].BCID_PU(ib,ipu)){
          fill(histo1D(name),ibin,1,name);
          info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" board "<<ib<<" PU"<<ipu<< endmsg;
        }
        name = "ProcError_opt_link";
        if (m_procRaw[iq].opt_link_error(ib,ipu)!=0){
          fill(histo1D(name),ibin,1,name);
          info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" board "<<ib<<" PU"<<ipu<< endmsg;
        }
        name = "ProcError_ser_link";
        if (m_procRaw[iq].ser_link_error(ib,ipu)!=0){
          fill(histo1D(name),ibin,1,name);
//           info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" board "<<ib<<" PU"<<ipu<< endmsg;
        }
        name = "ProcError_par_link";
        if (m_procRaw[iq].par_link_error(ib,ipu)!=0){
          fill(histo1D(name),ibin,1,name);
//           info()<<"evt# "<<m_ievt<<" "<<name<<" "<<ibin<<" Q"<<iq+1<<" board "<<ib<<" PU"<<ipu<< endmsg;
        }
      } // End of loop over the 4 PUs of a proc board
    } // End of loop over the 12 proc. boards of a quarter
  } // end of loop over the 4 quarters
  
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

  location = rootInTES() + LHCb::L0MuonCandidateLocation::BCSU + extension;
  LHCb::L0MuonCandidates* pbcsucands = new LHCb::L0MuonCandidates();
  put(pbcsucands , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endreq;
    debug() << "writeOnTES at "<< location << endreq;
  }
  for (int i= 0; i<2; ++i) {
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

  location = rootInTES() + LHCb::L0MuonCandidateLocation::PU  + extension;
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


  location = rootInTES() + LHCb::L0MuonDataLocation::Default + extension;
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
//   info()<<"l0muoncandidate Seed"<<pads[2].toString()<<" M2: "<<pads[1].toString()<<" M1: "<<pads[0].toString()<< endreq;
  LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], pads,cand->pT());
  return pl0muoncandidate;
}

