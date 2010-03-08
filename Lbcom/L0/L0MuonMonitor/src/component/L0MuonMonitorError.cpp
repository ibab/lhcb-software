// $Id: L0MuonMonitorError.cpp,v 1.1 2010-03-08 15:12:53 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Event
#include "Event/ODIN.h"
#include "Event/L0MuonError.h"
#include "Event/RawBankReadoutStatus.h"

// local
#include "L0MuonMonitorError.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorError
//
// 2010-02-16 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitorError );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitorError::L0MuonMonitorError( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "L0Context" , m_l0Context = ""  );
  declareProperty( "EnableTAE" , m_enableTAE = false  );
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitorError::~L0MuonMonitorError() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitorError::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;

  // Book histograms
  char buf[4096];
  m_summary = book1D("Summary_of_L0Muon_errors",-0.5,5.5,6);
  for (int iq = 0; iq < NQuarters; ++iq){
    sprintf(buf,"Summary_of_transmission_errors_Q%d",iq+1);
    m_tx_summary[iq] = book1D(buf,-0.5,12.5,13);
    sprintf(buf,"Summary_of_synchronisation_errors_Q%d",iq+1);
    m_sync_summary[iq] = book1D(buf,-0.5,12.5,13);
    sprintf(buf,"Summary_of_derandomiser_errors_Q%d",iq+1);
    m_derandom_summary[iq] = book1D(buf,-0.5,11.5,12);
  }

  m_sync_ctrl_src = book1D("CU-SU_synchronisation_error",-0.5,3.5,4);

  for (int iq = 0; iq < NQuarters; ++iq){
    sprintf(buf,"Internal_error_TELL1_Q%d",iq+1);
    m_tell1_int_src[iq] = book1D(buf,-0.5,23.5,24);
    sprintf(buf,"Reference_error_TELL1_Q%d",iq+1);
    m_tell1_ref_src[iq] = book1D(buf,-0.5,23.5,24);
  }
  sprintf(buf,"Internal_error_TELL1_Q12");
  m_tell1_int_src[4] = book1D(buf,-0.5,23.5,24);
  sprintf(buf,"Reference_error_TELL1_Q12");
  m_tell1_ref_src[4] = book1D(buf,-0.5,23.5,24);
  sprintf(buf,"Internal_error_TELL1_Q34");
  m_tell1_int_src[5] = book1D(buf,-0.5,23.5,24);
  sprintf(buf,"Reference_error_TELL1_Q34");
  m_tell1_ref_src[5] = book1D(buf,-0.5,23.5,24);

  m_decoding_src = book1D("Decoding_error",-0.5,9.5,10);
  for (int iq = 0; iq < NQuarters; ++iq){
    for (int ibcsu = 0; ibcsu < NBCSUs; ++ibcsu){
      sprintf(buf,"BCSU_error_Q%dPB%d",iq+1,ibcsu);
      m_bcsu_src[iq][ibcsu] = book1D(buf,-0.5,12.5,13);
      for (int ipu=0; ipu < NPUs ; ++ipu) {
        sprintf(buf,"PU_error_Q%dPB%dPU%d",iq+1,ibcsu,ipu);
        m_pu_src[iq][ibcsu][ipu] = book1D(buf,-0.5,23.5,24);
      }
    }
  }
    
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitorError::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  int tae_size = 0;
  if (m_enableTAE) {
    if (exist<LHCb::ODIN>(LHCb::ODINLocation::Default,false)) {
      // TAE size from odin
      LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default,false);
      tae_size = int(odin->timeAlignmentEventWindow());
    } else {
      Warning("ODIN not found at "+LHCb::ODINLocation::Default+", TAE mode requested but not used"
              ,StatusCode::FAILURE,50).ignore();
    }
  }

  int ntae = 0;
  for (int itae = -1*tae_size; itae<=tae_size; ++itae){
    std::string rootInTes = m_tae_items[itae];

    sc=setProperty("RootInTES",rootInTes);
    if (sc.isFailure()) return Error("Can not set RootInTES ",StatusCode::FAILURE,20);

    std::string location;
  
    // Bank read out status
    if(!exist<LHCb::RawBankReadoutStatuss>( LHCb::RawBankReadoutStatusLocation::Default ) ) {
      Warning("RawBankReadoutStatus not found at "+LHCb::RawBankReadoutStatusLocation::Default,StatusCode::SUCCESS,10).ignore();
      continue;
    }
    LHCb::RawBankReadoutStatuss* banksStatus   = get<LHCb::RawBankReadoutStatuss>( LHCb::RawBankReadoutStatusLocation::Default );
    LHCb::RawBankReadoutStatus* ctrlCandStatus = banksStatus->object(LHCb::RawBank::L0Muon);
    LHCb::RawBankReadoutStatus* procCandStatus = banksStatus->object(LHCb::RawBank::L0MuonProcCand);
    LHCb::RawBankReadoutStatus* procDataStatus = banksStatus->object(LHCb::RawBank::L0MuonProcData);
    if (NULL == ctrlCandStatus) {
      Warning("No read out status for bank L0Muon",StatusCode::SUCCESS,10).ignore();
      continue;
    }
    if (NULL == procCandStatus) {
      Warning("No read out status for bank L0MuonProcCand",StatusCode::SUCCESS,10).ignore();
      continue;
    }
    if (NULL == procDataStatus) {
      Warning("No read out status for bank L0MuonProcData",StatusCode::SUCCESS,10).ignore();
      continue;
    }
    // L0MuonErrors on TES
    if (!exist<LHCb::L0MuonErrors>(LHCb::L0MuonErrorLocation::CtrlCUSU + m_l0Context)) {
        Warning("L0MuonErrors not found at "+LHCb::L0MuonErrorLocation::CtrlCUSU + m_l0Context,StatusCode::SUCCESS,10).ignore();
        continue;
      }
    if (!exist<LHCb::L0MuonErrors>(LHCb::L0MuonErrorLocation::ProcBCSU + m_l0Context)) {
      Warning("L0MuonErrors not found at "+LHCb::L0MuonErrorLocation::ProcBCSU + m_l0Context,StatusCode::SUCCESS,10).ignore();
      continue;
    }
    if (!exist<LHCb::L0MuonErrors>(LHCb::L0MuonErrorLocation::ProcPU + m_l0Context)) {
      Warning("L0MuonErrors not found at "+LHCb::L0MuonErrorLocation::ProcPU + m_l0Context,StatusCode::SUCCESS,10).ignore();
      continue;
    }
    LHCb::L0MuonErrors *ctrlCUSU = get<LHCb::L0MuonErrors> (LHCb::L0MuonErrorLocation::CtrlCUSU + m_l0Context);
    LHCb::L0MuonErrors *procBCSU = get<LHCb::L0MuonErrors> (LHCb::L0MuonErrorLocation::ProcBCSU + m_l0Context);
    LHCb::L0MuonErrors *procPU   = get<LHCb::L0MuonErrors> (LHCb::L0MuonErrorLocation::ProcPU + m_l0Context);
  
    // Fill histograms
  
    LHCb::L0MuonErrors::const_iterator iterr;
    int quarter,board,pu;
  
    int status_summary    = 0;
    int dernd_summary     = 0;
    int tell1_summary     = 0;
    int decoding_summary  = 0;
  
    int status_pb[NQuarters]={0,0,0,0};
    
    // Loop over banks (readout status)
    int ctrlTell1Sources[2] = {5,0};
    for (int i=0; i < 2; ++i) {
      if (ctrlCandStatus->status(ctrlTell1Sources[i]) != LHCb::RawBankReadoutStatus::OK){
        decoding_summary |=1;
        fill(m_decoding_src,i,1.); // decoding error
      }
    }
    int procTell1Sources[4] = {3,4,2,1};
    for (int i=0; i < 4; ++i) {
      if (procCandStatus->status(procTell1Sources[i]) != LHCb::RawBankReadoutStatus::OK){
        decoding_summary |=1;
        fill(m_decoding_src,i+2,1.); // decoding error
      }
      if (procDataStatus->status(procTell1Sources[i]) != LHCb::RawBankReadoutStatus::OK){
        decoding_summary |=1;
        fill(m_decoding_src,i+6,1.); // decoding error
      }
    }
  
    // Loop over PUs error
    for (iterr=procPU->begin(); iterr!=procPU->end(); ++iterr){
      const LHCb::L0MuonError *err = (*iterr);
  
////     int bcid = err->bcid();
////     if (bcid>0) fill(m_bcsu_src[quarter][board],0,1.);
  
      int hardware = err->hardware();
      if (hardware>0) {
        err->index(quarter,board,pu);
        for (int i=0; i < 24 ; ++i){
          if ( ((hardware>>i)&0x1) > 0 ) fill(m_pu_src[quarter][board][pu],i,1); // pu link error
        }
      }
    }
    
    // Loop over BCSUs error
    for (iterr=procBCSU->begin(); iterr!=procBCSU->end(); ++iterr){
      const LHCb::L0MuonError *err = (*iterr);
  
      int status = (err->status()&0xC);
      if (status>0) {
        err->index(quarter,board,pu);
        status_pb[quarter] |= status;
        if ( (status&0x8) > 0 ) fill(m_tx_summary[quarter]  ,board,1.); // tx error
        if ( (status&0x4) > 0 ) fill(m_sync_summary[quarter],board,1.); // sync error
      }
      
      int bcid = err->bcid();
      if (bcid>0) fill(m_bcsu_src[quarter][board],0,1.);
  
      int hardware = err->hardware();
      if (hardware>0) {
        err->index(quarter,board,pu);
        int J2 = ( ( hardware>>13 ) & 0x1 );
        int J  = ( ( hardware>>12 ) & 0x1 );
        int K2 = ( ( hardware>> 9 ) & 0x1 );
        int K  = ( ( hardware>> 8 ) & 0x1 );
        int Hs = ( ( hardware>> 4 ) & 0xF );
        int Fs = ( ( hardware>> 0 ) & 0xF );
        if (J2 > 0) fill(m_tell1_ref_src[quarter],2*board  ,1.);
        if (J  > 0) fill(m_tell1_int_src[quarter],2*board  ,1.);
        if (K2 > 0) fill(m_tell1_ref_src[quarter],2*board+1,1.);
        if (K  > 0) fill(m_tell1_int_src[quarter],2*board+1,1.);
        if ((Fs > 0)|| (Hs > 0)) fill(m_derandom_summary[quarter],board,1.);
        for (int ipu = 0; ipu<NPUs ; ++ipu) {
          if ( ((Hs>>ipu)&0x1) >0 ) fill(m_bcsu_src[quarter][board],5+(ipu*4),1.);
          if ( ((Fs>>ipu)&0x1) >0 ) fill(m_bcsu_src[quarter][board],9+(ipu*4),1.);
        }
        dernd_summary |= (Hs|Fs);
        tell1_summary |= (J|K);
      }
    }
    
    // Loop over ctrl boards error
    for (iterr=ctrlCUSU->begin(); iterr!=ctrlCUSU->end(); ++iterr){
      const LHCb::L0MuonError *err = (*iterr);
  
      int status = (err->status()&0xC);
      if (status>0) {
        err->index(quarter,board,pu);
        status_summary |= (err->status()&0xC);
        if ( ((status&0x8) > 0 ) && ((status_pb[quarter]&0x8)==0) ) fill(m_tx_summary[quarter]  ,12,1.); // tx error
        if ( ((status&0x4) > 0 ) && ((status_pb[quarter]&0x4)==0) ) fill(m_sync_summary[quarter],12,1.); // sync error
      }
      
      int hardware = err->hardware();
      if (hardware>0) {
        err->index(quarter,board,pu);
        int J2 = ( ( hardware>>13 ) & 0x1 );
        int J  = ( ( hardware>>12 ) & 0x1 );
        int K2 = ( ( hardware>> 9 ) & 0x1 );
        int K  = ( ( hardware>> 8 ) & 0x1 );
        int iq = 4+(quarter/2);
        if (J2 > 0) fill(m_tell1_ref_src[iq],2*(quarter%2)  ,1.);
        if (J  > 0) fill(m_tell1_int_src[iq],2*(quarter%2)  ,1.);
        if (K2 > 0) fill(m_tell1_ref_src[iq],2*(quarter%2)+1,1.);
        if (K  > 0) fill(m_tell1_int_src[iq],2*(quarter%2)+1,1.);
        if ( ((hardware>>0)&0xF) != ((hardware>>4)&0xF) ) fill(m_sync_ctrl_src,quarter,1.); // BCIDs : SU!=CU
        tell1_summary |= (J|K);
      }
    }
  
    // Summary histogram
    fill(m_summary,0,1.);
    if ( (status_summary&0x8) > 0 ) fill(m_summary,1,1.); // tx error
    if ( (status_summary&0x4) > 0 ) fill(m_summary,2,1.); // sync error
    if ( dernd_summary >0 )         fill(m_summary,3,1.); // derandomiser error
    if ( tell1_summary >0 )         fill(m_summary,4,1.); // tell1 error
    if ( decoding_summary >0 )      fill(m_summary,5,1.); // decoding error

    ++ntae;
  }
  if (ntae==0) return Error( "No valid time slice found",StatusCode::SUCCESS,20);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitorError::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
