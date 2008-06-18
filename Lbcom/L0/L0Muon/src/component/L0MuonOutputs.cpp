// $Id: L0MuonOutputs.cpp,v 1.12 2008-06-18 12:46:18 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from Event
#include "Event/RecHeader.h"
#include "Event/RawEvent.h"
#include "Event/L0ProcessorData.h"
#include "Event/L0MuonData.h"

// from Kernel
#include "Kernel/MuonTileID.h"

#include "L0MuonKernel/ProcUtilities.h"

// local
#include "L0MuonOutputs.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonOutputs
//
// 2007-07-09 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonOutputs );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonOutputs::L0MuonOutputs( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<L0MuonOutputs>(this);
  
  // Converters for the banks of the 2 TELL1s connected to the controller boards
  for (int i= 0; i<2; ++i) {
    m_ctrlCand[i]  =  L0Muon::CtrlCandCnv(i);
  }
  // Converters for the banks of the 4 TELL1s connected to the processing boards
  for (int i= 0; i<4; ++i) {
    m_procCand[i] = L0Muon::ProcCandCnv(i);
    m_procData[i] = L0Muon::ProcDataCnv(i);
  }

  m_rawBankNorm = 0;
  m_nCandFinalNorm = 0;
  
  m_rawBankSizeTot= 0;
  m_nCandFinalTot = 0;
  m_nTriggeredEvt = 0;
  
  m_bankEventCounter  = 0;

  m_averagePt =0.;
  m_averageCh=0.;

  m_ctrlCandFlag = true ;
  m_ctrlCandFlagBCSU   = true ;
  m_procCandFlag  = true ;
  m_procDataFlag  = true ;

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonOutputs::~L0MuonOutputs() {} 

//=============================================================================
 
StatusCode L0MuonOutputs::decodeRawBanks(int mode){

  m_ctrlCandFlag = false ;
  m_ctrlCandFlagBCSU = false ;
  m_procCandFlag = false ;
  m_procDataFlag = false ;
  
  m_l0EventNumber=-1;
  m_l0_B_Id=-1;

  int rawBankSize = 0;

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  if( msgLevel(MSG::DEBUG) ) debug() << "decodeRawBanks:  ==> got rawEvt " << endmsg;

  // ======================
  //
  // L0Muon(CtrlCand) Banks
  //
  // ======================
  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0Muon );
  if (banks.size()!=0) {
    for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); banks.end() != itBnk; ++itBnk ) {
      int ctrlCandF=0;
      int srcID = (*itBnk)->sourceID();
      int bankVersion  = (*itBnk)->version();
      std::vector<unsigned int> data;
      unsigned int* body = (*itBnk)->data();
      int size = (*itBnk)->size()/4;
      if( msgLevel(MSG::DEBUG) ) debug() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
                                         <<", sourceID is "<< srcID <<", size is "<< size <<endreq;
      rawBankSize+=size;
      for ( int k = 0; size > k; ++k ) {
        data.push_back( *body++ );
      }
      if (bankVersion==0) { // DC06 
        if (srcID>0) continue;
        if (srcID==0) {
          ctrlCandF=m_ctrlCand[0].decodeBankDC06(data,bankVersion);
          ctrlCandF=m_ctrlCand[1].decodeBankDC06(data,bankVersion);
          m_ctrlCandFlag=true;
          break; // only 1 bank
        }
      }// End  DC06
      if (bankVersion==1) 
        ctrlCandF=m_ctrlCand[ctrlSourceID(srcID,bankVersion)].decodeBankFinalCandidates(data,bankVersion);
      else if (bankVersion>1) {
        L0Muon::CtrlCandCnv * ctrlCand=&m_ctrlCand[ctrlSourceID(srcID,bankVersion)];
        ctrlCand->submitL0EventNumber(m_l0EventNumber);
        ctrlCand->submitL0_B_Id(m_l0_B_Id);        
        ctrlCandF=ctrlCand->decodeBank(data,bankVersion,mode);
        m_l0EventNumber=ctrlCand->ref_l0EventNumber();
        m_l0_B_Id=ctrlCand->ref_l0_B_Id();
      }
      if (ctrlCandF>0) m_ctrlCandFlag=true;
      if (ctrlCandF>1) m_ctrlCandFlagBCSU=true;
    }
  }

  // ======================
  //
  // L0MuonProcCand Banks
  //
  // ======================
  if (mode>0) {
    const std::vector<LHCb::RawBank*>& proccandbanks = rawEvt->banks( LHCb::RawBank::L0MuonProcCand );
    if (proccandbanks.size()!=0) {
      m_procCandFlag =true;
      for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = proccandbanks.begin(); proccandbanks.end() != itBnk; ++itBnk ) {
        int procCandF=0;
        int srcID = (*itBnk)->sourceID();
        int bankVersion  = (*itBnk)->version();
        std::vector<unsigned int> data;
        unsigned int* body = (*itBnk)->data();
        int size = (*itBnk)->size()/4;
        if( msgLevel(MSG::DEBUG) ) debug() << "decodeRawBanks: L0MuonProcCand bank (version "<< bankVersion <<" ) found"
                                           <<", sourceID is "<< srcID <<", size is "<< size <<endreq;
        rawBankSize+=size;
        for ( int k = 0; size > k; ++k ) {
          data.push_back( *body++ );
        }
        L0Muon::ProcCandCnv * procCand=&m_procCand[procSourceID(srcID,bankVersion)];
        procCand->submitL0EventNumber(m_l0EventNumber);
        procCand->submitL0_B_Id(m_l0_B_Id);        
        procCandF=m_procCand[procSourceID(srcID,bankVersion)].decodeBank(data,bankVersion);
        m_l0EventNumber=procCand->ref_l0EventNumber();
        m_l0_B_Id=procCand->ref_l0_B_Id();
        if( msgLevel(MSG::DEBUG) ) debug() << "decodeRawBanks:  Q"<<(procSourceID(srcID,bankVersion)+1)
                                           << " # of PU candidates= "
                                           <<m_procCand[procSourceID(srcID,bankVersion)].muonCandidatesPU()<<endreq;
      }    
    }
  }
  
  // ======================
  //
  // L0MuonProcData Banks
  //
  // ======================
  if (mode>0) {
    const std::vector<LHCb::RawBank*>& procdatabanks = rawEvt->banks( LHCb::RawBank::L0MuonProcData );
    if (procdatabanks.size()!=0) {
      m_procDataFlag =true;
      for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = procdatabanks.begin(); procdatabanks.end() != itBnk; ++itBnk ) {
        int procData=0;
        int srcID = (*itBnk)->sourceID();
        int bankVersion  = (*itBnk)->version();
        std::vector<unsigned int> data;
        unsigned int* body = (*itBnk)->data();
        int size = (*itBnk)->size()/4;
        if( msgLevel(MSG::DEBUG) ) debug() << "decodeRawBanks: L0MuonProcData bank (version "<< bankVersion <<" ) found"
                                           <<", sourceID is "<< srcID <<", size is "<< size <<endreq;
        rawBankSize+=size;
        for ( int k = 0; size > k; ++k ) {
          data.push_back( *body++ );
        }
        if( msgLevel(MSG::DEBUG) ) debug() << "decodeRawBanks: L0MuonProcData bank calling decoding of "
                                           << data.size()<<" words"<<endreq;
        procData=m_procData[procSourceID(srcID,bankVersion)].decodeBank(data,bankVersion);
      }    
    }
  }

  ++m_rawBankNorm;
  m_rawBankSizeTot += rawBankSize;

  if( msgLevel(MSG::DEBUG) ) debug() << "decodeRawBanks:  ==> done" << endmsg;
  
  return StatusCode::SUCCESS;

}

StatusCode L0MuonOutputs::writeRawBanks(int mode, int bankVersion){

  int rawBankSize = 0;
  int ievt;
  
  if (exist<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default)) {
    LHCb::RecHeader* evt = get<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default);
    ievt = evt->evtNumber();
  } else {
    ievt = m_bankEventCounter;
    ++m_bankEventCounter;
  }  
  
  LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  std::vector<unsigned int> data;

  // L0Muon DC06 (only 5 TELL1s where foreseen)
  if (bankVersion==0) { // Bank version =0

    data=DC06RawBanks();
    raw->addBank(0, LHCb::RawBank::L0Muon,0,data);
    rawBankSize += data.size();
    if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0Muon bank written (DC06) size is "<< data.size() <<endreq;
    ++m_rawBankNorm;
    m_rawBankSizeTot += rawBankSize;
    return StatusCode::SUCCESS;

  } else if (bankVersion==1) {// Bank version =1 (2 banks where foreseen for the controller boards)

    // L0Muon(CtrlCand) (always there - light, standard and debug modes)
    for (int i= 0; i<2; ++i) {
      m_ctrlCand[i].rawBankFinalCandidates(data,ievt);
      raw->addBank(i, LHCb::RawBank::L0Muon,bankVersion,data);
      if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0Muon bank written (version "<< bankVersion<<" ) size is "
                                         << data.size() <<endreq;
      rawBankSize += data.size();
    }

    // L0MuonProcCand (always there - light, standard and debug modes)
    for (int i= 0; i<4; ++i) {
      m_procCand[i].rawBank(data,bankVersion,ievt);
      raw->addBank(i, LHCb::RawBank::L0MuonProcCand,bankVersion,data);
      if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0MuonProcCand bank written (version "<< bankVersion<<" ) size is "
                                         << data.size() <<endreq;
      rawBankSize += data.size();
    }

    // L0MuonProcData (only for standard and debug modes)
    if (mode>0) {
      for (int i= 0; i<4; ++i) {
        m_procData[i].rawBank(data,bankVersion,mode);
        raw->addBank(i, LHCb::RawBank::L0MuonProcData,bankVersion,data);
        if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0MuonProcData bank written (version "<< bankVersion<<" ) size is "
                                           << data.size() <<endreq;
        rawBankSize += data.size();
      }
    } 

    
  } else { // Bank version >1

    // L0Muon (always there - light, standard and debug modes)
    for (int i= 0; i<2; ++i) {
      m_ctrlCand[i].rawBank(data,bankVersion,mode,ievt);
      raw->addBank(i, LHCb::RawBank::L0Muon,bankVersion,data);
      if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0Muon bank written (version "<< bankVersion<<" ) size is "
                                         << data.size() <<endreq;
      rawBankSize += data.size();
    }

    // L0MuonProcCand (always there - light, standard and debug modes)
    for (int i= 0; i<4; ++i) {
      m_procCand[i].rawBank(data,bankVersion,ievt);
      raw->addBank(i, LHCb::RawBank::L0MuonProcCand,bankVersion,data);
      if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0MuonProcCand bank written (version "<< bankVersion<<" ) size is "
                                         << data.size() <<endreq;
      rawBankSize += data.size();
    }

    // L0MuonProcData (only for standard and debug modes)
    if (mode>0) {
      for (int i= 0; i<4; ++i) {
        m_procData[i].rawBank(data,bankVersion,mode);
        raw->addBank(i, LHCb::RawBank::L0MuonProcData,bankVersion,data);
        if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0MuonProcData bank written (version "<< bankVersion<<" ) size is "
                                           << data.size() <<endreq;
        rawBankSize += data.size();
      }
    } 

  }
  
  
  ++m_rawBankNorm;
  m_rawBankSizeTot += rawBankSize;

  return StatusCode::SUCCESS;
  
}

StatusCode L0MuonOutputs::writeOnTES(int procVersion, std::string extension){

  int nCandFinal  = 0;
  double sumPt = 0;
  double sumCh = 0;
 
  std::vector<L0Muon::PMuonCandidate> cands ;
  std::vector<L0Muon::PMuonCandidate>::iterator itcand;
  std::string location;

  if (m_ctrlCandFlag) {
    // Candidates selected by the controller boards
    // L0Muon (always there - light, standard and debug modes)
    location = LHCb::L0MuonCandidateLocation::Default + extension;
    LHCb::L0MuonCandidates* pl0mcands = new LHCb::L0MuonCandidates();
    put(pl0mcands , location );
    if( msgLevel(MSG::DEBUG) ) {
      debug() << "writeOnTES -------------------------"<< endreq;
      debug() << "writeOnTES at "<< location << endreq;
    }
    for (int i= 0; i<2; ++i) {
      cands = m_ctrlCand[i].muonCandidates();
      nCandFinal+=cands.size();
      if( msgLevel(MSG::DEBUG) ) {
        debug() << "writeOnTES: side "<<i<< endreq;
        m_ctrlCand[i].dump("\t=> ");
        debug() << "writeOnTES: => "<<cands.size()<<" candidates found"<< endreq;
      }
      for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
        if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
        LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
        if (l0mcand==NULL) {
          if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: ctrl side "<<i
                                            <<"\n"<<(*itcand)->dump("\t=> ")<< endreq;
          continue;
        }
        double pt = l0mcand->pt();
        sumPt += fabs(pt);
        sumCh += (pt>0. ? +1 : -1);      
        pl0mcands->insert(l0mcand);
      }
    }
    if (m_nCandFinalTot+nCandFinal>0) {
      m_averagePt  = ((m_averagePt*m_nCandFinalTot)+sumPt)/(m_nCandFinalTot+nCandFinal); 
      m_averageCh  = ((m_averageCh*m_nCandFinalTot)+sumCh)/(m_nCandFinalTot+nCandFinal); 
    }
    m_nCandFinalTot+=nCandFinal;
    if (nCandFinal>0) ++m_nTriggeredEvt;
    ++m_nCandFinalNorm;
  }

  // Candidates selected by the BCSUs (seen by the controller boards)
  if (m_ctrlCandFlagBCSU) {
    location = LHCb::L0MuonCandidateLocation::BCSU +"fromCB"+ extension;
    LHCb::L0MuonCandidates* pbcsucands = new LHCb::L0MuonCandidates();
    put(pbcsucands , location );
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES -------------------------"<< endreq;
      debug() << "writeOnTES at "<< location << endreq;
    }
    for (int i= 0; i<2; ++i) {
      cands = m_ctrlCand[i].muonCandidatesBCSU();
      if (msgLevel( MSG::DEBUG )) {
        debug() << "writeOnTES: side "<<i<< endreq;
        debug() << "writeOnTES: => "<<cands.size()<<" candidates found (BCSU)"<< endreq;
      }
      for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
        if( msgLevel(MSG::DEBUG) ) debug() << "writeOnTES: BCSU candidate\n"<<(*itcand)->dump("\t=> ")<< endreq;
        LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
        if (l0mcand==NULL) {
          if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: ctrl side "<<i<<" BCSU from CB"
                                            <<"\n"<<(*itcand)->dump("\t=> ")<< endreq;
          continue;
        }
        pbcsucands->insert(l0mcand);
      }
    }
  }

  // Candidates selected by the BCSUs (sent by the processing boards)
  if (m_procCandFlag) {
    location = LHCb::L0MuonCandidateLocation::BCSU +"fromPB"+ extension;
    LHCb::L0MuonCandidates* ppucands = new LHCb::L0MuonCandidates();
    put(ppucands , location );
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES -------------------------"<< endreq;
      debug() << "writeOnTES at "<< location << endreq;
    }
    for (int i= 0; i<4; ++i) {      
      cands = m_procCand[i].muonCandidatesBCSU();
      if (msgLevel( MSG::DEBUG )) {
        debug() << "writeOnTES: quarter "<<i<< endreq;
        debug() << "writeOnTES: => "<<cands.size()<<" candidates found (PU)"<< endreq;
      }
      for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
        if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: PU candidate\n"<<(*itcand)->dump("\t=> ")<< endreq;
        LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
        if (l0mcand==NULL) {
          if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: proc Q"<<(i+1)<<"BCSU from PB"
                                            <<"\n"<<(*itcand)->dump("\t=> ")<< endreq;
          continue;
        }
        ppucands->insert(l0mcand);
      }
    }
  }
  
  // Candidates found by the PUs
  if (m_procCandFlag) {
    location = LHCb::L0MuonCandidateLocation::PU  + extension;
    LHCb::L0MuonCandidates* ppucands = new LHCb::L0MuonCandidates();
    put(ppucands , location );
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES -------------------------"<< endreq;
      debug() << "writeOnTES at "<< location << endreq;
    }
    for (int i= 0; i<4; ++i) {      
      cands = m_procCand[i].muonCandidatesPU();
      if (msgLevel( MSG::DEBUG )) {
        debug() << "writeOnTES: quarter "<<i<< endreq;
        debug() << "writeOnTES: => "<<cands.size()<<" candidates found (PU)"<< endreq;
      }
      for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
        if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: PU candidate\n"<<(*itcand)->dump("\t=> ")<< endreq;
        LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
        if (l0mcand==NULL) {
          if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: proc Q"<<(i+1)<<"PU"
                                            <<"\n"<<(*itcand)->dump("\t=> ")<< endreq;
          continue;
        }
        ppucands->insert(l0mcand);
      }
    }
  }
  
  // Data received by the PUs
  if (m_procDataFlag){
    location = LHCb::L0MuonDataLocation::Default + extension;
    LHCb::L0MuonDatas* pdata = new LHCb::L0MuonDatas();
    put(pdata , location );
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES -------------------------"<< endreq;
      debug() << "writeOnTES at "<< location << endreq;
    }
    for (int i= 0; i<4; ++i) {
      if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: quarter "<<i<< endreq;
      std::vector<LHCb::MuonTileID>  pus = m_procData[i].pus();
      for (std::vector<LHCb::MuonTileID>::iterator itpu=pus.begin(); itpu!=pus.end(); ++itpu){
        //debug() << "writeOnTES: pu = "<<itpu->toString()<< endreq;
        std::vector<LHCb::MuonTileID> ols = m_procData[i].ols(*itpu);
        //debug() << "writeOnTES: ols length = "<<ols.size()<< endreq;
        std::vector<LHCb::MuonTileID> neighs = m_procData[i].neighs(*itpu);
        //debug() << "writeOnTES: neighs length = "<<neighs.size()<< endreq;
        LHCb::L0MuonData *l0muondata = new LHCb::L0MuonData(*itpu, ols, neighs);
        pdata->insert(l0muondata);
      }
    }
  }
  
//   location = LHCb::L0MuonInfoLocation::Default + extension;
//   LHCb::L0MuonInfo* pl0minfo = new LHCb::L0MuonInfo();
//   pl0minfo->setL0EventNumber(m_l0EventNumber);
//   pl0minfo->setL0_B_Id(m_l0_B_Id);
//   put(pl0minfo, location );
 
  return StatusCode::SUCCESS;
}
StatusCode L0MuonOutputs::monitorBanks(){

  int nCandFinal  = 0;
  double sumPt = 0;
  double sumCh = 0;
 
  std::vector<L0Muon::PMuonCandidate> cands ;
  std::vector<L0Muon::PMuonCandidate>::iterator itcand;
  std::string location;
  if (msgLevel( MSG::DEBUG )) {
    
    // Candidates selected by the controller boards
    // L0Muon (always there - light, standard and debug modes)
    debug() << "monitorBanks -------------------------"<< endreq;
    for (int i= 0; i<2; ++i) {
      debug() << "monitorBanks: side "<<i<< endreq;
      if (msgLevel( MSG::DEBUG )) m_ctrlCand[i].dump("\t=> ");
      cands = m_ctrlCand[i].muonCandidates();
      nCandFinal+=cands.size();
      debug() << "monitorBanks: => "<<cands.size()<<" candidates found"<< endreq;
      for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
        debug() << "monitorBanks:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      }
    }
    if (m_nCandFinalTot+nCandFinal>0) {
      m_averagePt  = ((m_averagePt*m_nCandFinalTot)+sumPt)/(m_nCandFinalTot+nCandFinal); 
      m_averageCh  = ((m_averageCh*m_nCandFinalTot)+sumCh)/(m_nCandFinalTot+nCandFinal); 
    }
    m_nCandFinalTot+=nCandFinal;
    if (nCandFinal>0) ++m_nTriggeredEvt;
    ++m_nCandFinalNorm;

    // Candidates selected by the BCSUs
    debug() << "monitorBanks -------------------------"<< endreq;
    for (int i= 0; i<2; ++i) {
      debug() << "monitorBanks: side "<<i<< endreq;
      cands = m_ctrlCand[i].muonCandidatesBCSU();
      debug() << "monitorBanks: => "<<cands.size()<<" candidates found (BCSU)"<< endreq;
      for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
        debug() << "monitorBanks:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      }
    }

    // Candidates found by the PUs
    debug() << "monitorBanks -------------------------"<< endreq;
    for (int i= 0; i<4; ++i) {
      debug() << "monitorBanks: quarter "<<i<< endreq;
      cands = m_procCand[i].muonCandidatesPU();
      debug() << "monitorBanks: => "<<cands.size()<<" candidates found (PU)"<< endreq;
      for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
        debug() << "monitorBanks:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      }
    }
  
    // Data received by the PUs
    debug() << "monitorBanks -------------------------"<< endreq;
    for (int i= 0; i<4; ++i) {
      debug() << "monitorBanks: quarter "<<i<< endreq;
      std::vector<LHCb::MuonTileID>  pus = m_procData[i].pus();
      for (std::vector<LHCb::MuonTileID>::iterator itpu=pus.begin(); itpu!=pus.end(); ++itpu){
        //debug() << "monitorBanks: pu = "<<itpu->toString()<< endreq;
        std::vector<LHCb::MuonTileID> ols = m_procData[i].ols(*itpu);
        //debug() << "monitorBanks: ols length = "<<ols.size()<< endreq;
        std::vector<LHCb::MuonTileID> neighs = m_procData[i].neighs(*itpu);
        //debug() << "monitorBanks: neighs length = "<<neighs.size()<< endreq;
      }
    }

  }
  
  return StatusCode::SUCCESS;
}


StatusCode L0MuonOutputs::writeL0ProcessorData(std::string extension){

  unsigned long cu[4],su[4];
  unsigned long  index[4];
  for (int iq=0;iq<4;iq++) {
    cu[iq] = L0DUBase::EmptyData ;
    su[iq] = L0DUBase::EmptyData ;
    index[iq] = 0;
  }
  
  for (int i= 0; i<2; ++i) {
    std::vector<L0Muon::PMuonCandidate> cands = m_ctrlCand[i].muonCandidates();
    for ( std::vector<L0Muon::PMuonCandidate>::iterator itcand = cands.begin();itcand!=cands.end();++itcand ) {      
      int quarter = (*itcand)->quarter();
      int addM3 = ((*itcand)->colM3()&0x3) + (((*itcand)->rowM3()<<5)&0x7C);
      if (msgLevel( MSG::DEBUG )) {
        debug() << "writeL0ProcessorData: \tquarter= "<< quarter  << endreq;
        debug() << "writeL0ProcessorData: \tpT= "<< (*itcand)->pT()  << endreq;
        debug() << "writeL0ProcessorData: \tPU= "<< (*itcand)->pu()  << endreq;
        debug() << "writeL0ProcessorData: \tboard= "<< (*itcand)->board()  << endreq;
        debug() << "writeL0ProcessorData: \taddM3= "<< addM3  << endreq;
        debug() << "writeL0ProcessorData: \tcharge= "<< (*itcand)->charge()  << endreq;
        debug() << "writeL0ProcessorData: --- "<< endreq;
      }
    
      if (index[quarter]==0) {
        cu[quarter] += ( (*itcand)->pT()     << L0DUBase::Muon::Pt1::Shift) & L0DUBase::Muon::Pt1::Mask;
        cu[quarter] += (      addM3          << L0DUBase::Muon::Address1::Shift) & L0DUBase::Muon::Address1::Mask;
        su[quarter] += ( (*itcand)->pu()     << L0DUBase::Muon::Pu1::Shift) & L0DUBase::Muon::Pu1::Mask;
        su[quarter] += ( (*itcand)->board()  << L0DUBase::Muon::Pb1::Shift) & L0DUBase::Muon::Pb1::Mask;
        su[quarter] += ( (*itcand)->charge() << L0DUBase::Muon::Sign1::Shift) & L0DUBase::Muon::Sign1::Mask;  
      } else {
        cu[quarter] += ( (*itcand)->pT()     << L0DUBase::Muon::Pt2::Shift) & L0DUBase::Muon::Pt2::Mask;
        cu[quarter] += (      addM3          << L0DUBase::Muon::Address2::Shift) & L0DUBase::Muon::Address2::Mask;
        su[quarter] += ( (*itcand)->pu()     << L0DUBase::Muon::Pu2::Shift) & L0DUBase::Muon::Pu2::Mask;
        su[quarter] += ( (*itcand)->board()  << L0DUBase::Muon::Pb2::Shift) & L0DUBase::Muon::Pb2::Mask;
        su[quarter] += ( (*itcand)->charge() << L0DUBase::Muon::Sign2::Shift) & L0DUBase::Muon::Sign2::Mask;  
      }
      index[quarter]++;
    }
  }
  
  if (msgLevel( MSG::DEBUG )) {
    for (int iq=0;iq<4;iq++) {
      debug() << "writeL0ProcessorData: number of candidates = "<< index[iq] << endreq;;
      debug() << "writeL0ProcessorData: cu["<<iq<<"] = " << cu[iq] << endreq;;
      debug() << "writeL0ProcessorData: su["<<iq<<"] = " << su[iq] << endreq;;
    }
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
  put(  l0MuonDatas ,  LHCb::L0ProcessorDataLocation::Muon + extension );

  return StatusCode::SUCCESS;
}

StatusCode L0MuonOutputs::releaseRegisters(){

  for (int i= 0; i<2; ++i) m_ctrlCand[i].release();
  for (int i= 0; i<4; ++i) m_procCand[i].release();
  for (int i= 0; i<4; ++i) m_procData[i].release();

  return StatusCode::SUCCESS;
}

std::vector<unsigned int> L0MuonOutputs::DC06RawBanks(){
  //
  // Special encoder for DC06 since only 5 TELLS were foreseen at this time
  //
  
  int size= 31;
  int max = 32;

  std::vector<unsigned int> data;
  int word=0;

  // 1st word 
  // [13- 0] : evt number -> to be filled !!
  // [27-14] : evt number -> to be filled !!
  // [31-28] : error (CU) = 0

  data.push_back(word);

  // 2nd word
  // [ 3- 0] : error (SU) = 0
  // [ 7- 4] : number of candidates
  // [32- 8] : beginning of first candidate
  int ncandidates=0;
  for (int i= 0; i<2; ++i) {
    ncandidates += m_ctrlCand[i].muonCandidates().size();
  }
  word=( (ncandidates<<4) & 0xF0);

  // End of 2nd word and next words ...
  int cursor = 8;
  bool finished=false;
  // Loop over the 2 converters
  for (int i= 0; i<2; ++i) {
    std::vector<unsigned int> cands;
    m_ctrlCand[i].rawBankDC06(cands);
    // Loop over the candidates
    for ( std::vector<unsigned int>::iterator itcand = cands.begin();itcand!=cands.end();++itcand ) {
      unsigned int shifted = ( ( (*itcand) <<     cursor   ) & 0xFFFFFFFF );
      unsigned int left    = ( ( (*itcand) >> (max-cursor) ) & 0xFFFFFFFF );
      word+=shifted;
      cursor+=size;
      finished=true;
      if (cursor>=max) {
        data.push_back(word);
        finished=false;
        cursor-=max;
        word = left;
      }
    }
  }
  if (!finished) data.push_back(word);

  return data;
}

LHCb::L0MuonCandidate* L0MuonOutputs::l0muoncandidate(L0Muon::PMuonCandidate cand,int procVersion)
{
  bool debug = false;
  if (msgLevel( MSG::VERBOSE )) debug=true;
  
  //   std::vector<LHCb::MuonTileID> pads = cand->pads(procVersion,true);
  //   std::vector<double> kine = L0Muon::kine(pads[0],pads[1],procVersion,true);
  //   LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], pads,cand->pT());
  //   return pl0muoncandidate;
  std::vector<LHCb::MuonTileID> pads = L0Muon::add2pads(cand->quarter(),cand->board(),cand->pu(),
                                                        cand->colM3(),cand->rowM3(), cand->offM2(),cand->offM1(),
                                                        procVersion,debug);
  if (pads.size()==0) {
    error()<<"wrong address for candidate "<<endmsg;
    return NULL;
  }
  std::vector<double> kine = L0Muon::kine(pads[0],pads[1],procVersion,debug);
  LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], pads,cand->pT());
  return pl0muoncandidate;
}

// // LHCb::L0MuonCandidate* L0MuonOutputs::l0muoncandidate(L0Muon::CandRegisterHandler handler, int icand, int procVersion)
// // {
// //   std::vector<LHCb::MuonTileID> pads = L0Muon::add2pads(handler->getCandQuarter(icand),
// //                                                         handler->getCandBoard(  icand),
// //                                                         handler->getCandPU(     icand),
// //                                                         handler->getCandColM3(  icand),
// //                                                         handler->getCandRowM3(  icand),
// //                                                         handler->getCandOffM2(  icand),
// //                                                         handler->getCandOffM1(  icand),
// //                                                         procVersion,true);
// //   std::vector<double> kine = L0Muon::kine(pads[0],pads[1],procVersion,true);
// //   LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], 
// //                                                                       pads,handler->getCandPTCharged(icand));
  
// //   return pl0muoncandidate;
// // }


void L0MuonOutputs::statTot(MsgStream & os) const{
  if (0!=m_nCandFinalNorm)
  {  
    os << "- Total number of candidates                 : "
       <<format("%8d",m_nCandFinalTot) <<endmsg;
    os << "- Number of events with at least 1 candidate : "
       <<format("%8d",m_nTriggeredEvt) 
       <<format("  (%6.2f\%)",(100.*m_nTriggeredEvt)/m_nCandFinalNorm) << endmsg;
    os << "- Number of candidate per event              : "
       <<format("      %5.2f",(1.*m_nCandFinalTot)/m_nCandFinalNorm) << endmsg;
    os << "- Average Pt of the candidates               : "
       <<format("    %7.2f MeV",m_averagePt)<<endmsg;
    os << "- Average charge of the candidates           : "
       <<format("      %5.2f (+/-%4.2f)",m_averageCh,sqrt((1-(m_averageCh*m_averageCh))/m_nCandFinalNorm))<<endmsg;
  }
  
  if (0!=m_rawBankNorm)
  {  
    os << "- Average bank size                          : "
       <<format("  %8.1f Bytes",(4.*m_rawBankSizeTot)/m_rawBankNorm)<<endmsg;
  }

}
