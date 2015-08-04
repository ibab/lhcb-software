// Include files 

// Event
#include "Event/ODIN.h"
#include "Event/L0MuonError.h"
#include "Event/RawBankReadoutStatus.h"

// ROOT
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1D.h"
#include "TAxis.h"

// local
#include "L0MuonMonitorError.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorError
//
// 2010-02-16 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitorError )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitorError::L0MuonMonitorError( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_summary(0)
  , m_sync_ctrl_src(0)
  , m_decoding_src(0)
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

  // Book histograms and set labels 
  char buf[4096];
  TH1D * hist;

  // Summary
  m_summary = book1D("Summary_of_L0Muon_errors",-0.5,5.5,6);
  hist = Gaudi::Utils::Aida2ROOT::aida2root( m_summary );
  if (hist==0) Error("Can not get TH1D for Summary_of_L0Muon_errors",StatusCode::SUCCESS).ignore();
  else{
    TAxis* axis = hist -> GetXaxis();
    axis -> SetBinLabel(1,"Processed");
    axis -> SetBinLabel(2,"Tx");
    axis -> SetBinLabel(3,"Sync");
    axis -> SetBinLabel(4,"de-rnd");
    axis -> SetBinLabel(5,"tell1");
    axis -> SetBinLabel(6,"decoding");
  }
  
  for (int iq = 0; iq < NQuarters; ++iq){ // loop over quarters

    // Transmission 
    sprintf(buf,"Summary_of_transmission_errors_Q%d",iq+1);
    m_tx_summary[iq] = book1D(buf,-0.5,12.5,13);
    hist = Gaudi::Utils::Aida2ROOT::aida2root( m_tx_summary[iq] );
    if (hist==0) Error("Can not get TH1D for Summary_of_transmission_errors_Qi",StatusCode::SUCCESS).ignore();
    else{
      TAxis* axis = hist -> GetXaxis();
      for (int ipb=0; ipb<12; ++ipb){
        sprintf(buf,"PB%d",ipb);
        axis -> SetBinLabel(ipb+1,buf);
      }
      axis -> SetBinLabel(13,"CB");
    }

    // Synchronisation
    sprintf(buf,"Summary_of_synchronisation_errors_Q%d",iq+1);
    m_sync_summary[iq] = book1D(buf,-0.5,12.5,13);
    hist = Gaudi::Utils::Aida2ROOT::aida2root( m_sync_summary[iq] );
    if (hist==0) Error("Can not get TH1D for Summary_of_synchronisation_errors_Qi",StatusCode::SUCCESS).ignore();
    else{
      TAxis* axis = hist -> GetXaxis();
      for (int ipb=0; ipb<12; ++ipb){
        sprintf(buf,"PB%d",ipb);
        axis -> SetBinLabel(ipb+1,buf);
      }
      axis -> SetBinLabel(13,"CB");
    }
    
    // Derandomizer
    sprintf(buf,"Summary_of_derandomiser_errors_Q%d",iq+1);
    m_derandom_summary[iq] = book1D(buf,-0.5,11.5,12);
    hist = Gaudi::Utils::Aida2ROOT::aida2root( m_derandom_summary[iq] );
    if (hist==0) Error("Can not get TH1D for Summary_of_derandomiser_errors_Qi",StatusCode::SUCCESS).ignore();
    else{
      TAxis* axis = hist -> GetXaxis();
      for (int ipb=0; ipb<12; ++ipb){
        sprintf(buf,"PB%d",ipb);
        axis -> SetBinLabel(ipb+1,buf);
      }
    }

  } // end of loop over quarters

  // CU-SU synchro
  m_sync_ctrl_src = book1D("CU-SU_synchronisation_error",-0.5,3.5,4);
  hist = Gaudi::Utils::Aida2ROOT::aida2root( m_sync_ctrl_src );
  if (hist==0) Error("Can not get TH1D for CU-SU_synchronisation_error",StatusCode::SUCCESS).ignore();
  else{
    TAxis* axis = hist -> GetXaxis();
    for (int iq=0; iq<4; ++iq){
      sprintf(buf,"Q%d",(iq+1));
      axis -> SetBinLabel(iq+1,buf);
    }
  }

  for (int iq = 0; iq < NQuarters; ++iq){ // loop over quarters

    // Internal_error_TELL1 (PB)
    sprintf(buf,"Internal_error_TELL1_Q%d",iq+1);
    m_tell1_int_src[iq] = book1D(buf,-0.5,23.5,24);
    hist = Gaudi::Utils::Aida2ROOT::aida2root( m_tell1_int_src[iq] );
    if (hist==0) Error("Can not get TH1D for Internal_error_TELL1_Qi",StatusCode::SUCCESS).ignore();
    else{
      TAxis* axis = hist -> GetXaxis();
      for (int ipb=0; ipb<12; ++ipb){
        sprintf(buf,"PB%d_J",ipb);
        axis -> SetBinLabel(ipb*2+1,buf);
        sprintf(buf,"PB%d_K",ipb);
        axis -> SetBinLabel(ipb*2+2,buf);
      }
    }

    // Reference_error_TELL1 (PB)
    sprintf(buf,"Reference_error_TELL1_Q%d",iq+1);
    m_tell1_ref_src[iq] = book1D(buf,-0.5,23.5,24);
    hist = Gaudi::Utils::Aida2ROOT::aida2root( m_tell1_ref_src[iq] );
    if (hist==0) Error("Can not get TH1D for Reference_error_TELL1_Qi",StatusCode::SUCCESS).ignore();
    else{
      TAxis* axis = hist -> GetXaxis();
      for (int ipb=0; ipb<12; ++ipb){
        sprintf(buf,"PB%d_J",ipb);
        axis -> SetBinLabel(ipb*2+1,buf);
        sprintf(buf,"PB%d_K",ipb);
        axis -> SetBinLabel(ipb*2+2,buf);
      }
    }
  }  // end of loop over quarters

  for (int is=0; is<2; ++is) { // loop over A and C sides

    // Internal_error_TELL1 (CB)
    sprintf(buf,"Internal_error_TELL1_Q%d%d",(2*is)+1,(2*is)+2);
    m_tell1_int_src[4+is] = book1D(buf,-0.5,3.5,4);
    hist = Gaudi::Utils::Aida2ROOT::aida2root( m_tell1_int_src[4+is] );
    if (hist==0) Error("Can not get TH1D for Internal_error_TELL1_Qij",StatusCode::SUCCESS).ignore();
    else{
      TAxis* axis = hist -> GetXaxis();
      for (int icb=0; icb<2; ++icb){
        sprintf(buf,"CB_%d_J",icb);
        axis -> SetBinLabel(icb*2+1,buf);
        sprintf(buf,"CB_%d_K",icb);
        axis -> SetBinLabel(icb*2+2,buf);
      }
    }
    
    // Reference_error_TELL1 (CB)
    sprintf(buf,"Reference_error_TELL1_Q%d%d",(2*is)+1,(2*is)+2);
    m_tell1_ref_src[4+is] = book1D(buf,-0.5,3.5,4);
    hist = Gaudi::Utils::Aida2ROOT::aida2root( m_tell1_ref_src[4+is] );
    if (hist==0) Error("Can not get TH1D for Reference_error_TELL1_Qij",StatusCode::SUCCESS).ignore();
    else{
      TAxis* axis = hist -> GetXaxis();
      for (int icb=0; icb<2; ++icb){
        sprintf(buf,"CB_%d_J",icb);
        axis -> SetBinLabel(icb*2+1,buf);
        sprintf(buf,"CB_%d_K",icb);
        axis -> SetBinLabel(icb*2+2,buf);
      }
    }

  } // end of loop over A and C sides

  // Decoding
  m_decoding_src = book1D("Decoding_error",-0.5,9.5,10);
  hist = Gaudi::Utils::Aida2ROOT::aida2root( m_decoding_src );
  if (hist==0) Error("Can not get TH1D for Decoding_error",StatusCode::SUCCESS).ignore();
  else{
    TAxis* axis = hist -> GetXaxis();
    axis -> SetBinLabel(1,"CtrlCandQ12");
    axis -> SetBinLabel(2,"CtrlCandQ34");
    for (int iq = 0; iq < 4; ++iq){
      sprintf(buf,"ProcCandQ%d",iq+1);
      axis -> SetBinLabel(3+iq,buf);
    }
    for (int iq = 0; iq < 4; ++iq){
      sprintf(buf,"ProcDataQ%d",iq+1);
      axis -> SetBinLabel(7+iq,buf);
    }
  }
  
  for (int iq = 0; iq < NQuarters; ++iq){  // loop over quarters

    for (int ibcsu = 0; ibcsu < NBCSUs; ++ibcsu){ // loop over processing boards

      // BCSU
      sprintf(buf,"BCSU_error_Q%dPB%d",iq+1,ibcsu);
      m_bcsu_src[iq][ibcsu] = book1D(buf,-0.5,12.5,13);
      hist = Gaudi::Utils::Aida2ROOT::aida2root( m_bcsu_src[iq][ibcsu] );
      if (hist==0) Error("Can not get TH1D for BCSU_error_QiPBj",StatusCode::SUCCESS).ignore();
      else{
        TAxis* axis = hist -> GetXaxis();
        axis -> SetBinLabel(1,"BCSU_bcid");
        for (int ipu = 0; ipu < 4; ++ipu){
          sprintf(buf,"PU%d_bcid",ipu);
          axis -> SetBinLabel(2+ipu,buf);
        }
        for (int ipu = 0; ipu < 4; ++ipu){
          sprintf(buf,"PU%d_H",ipu);
          axis -> SetBinLabel(6+ipu,buf);
        }
        for (int ipu = 0; ipu < 4; ++ipu){
          sprintf(buf,"PU%d_F",ipu);
          axis -> SetBinLabel(10+ipu,buf);
        }
      }

      for (int ipu=0; ipu < NPUs ; ++ipu) { // loop over PU
        // PU
        sprintf(buf,"PU_error_Q%dPB%dPU%d",iq+1,ibcsu,ipu);
        m_pu_src[iq][ibcsu][ipu] = book1D(buf,-0.5,23.5,24);
        hist = Gaudi::Utils::Aida2ROOT::aida2root( m_pu_src[iq][ibcsu][ipu] );
        if (hist==0) Error("Can not get TH1D for PU_error_QiPBjPUk",StatusCode::SUCCESS).ignore();
        else{
          TAxis* axis = hist -> GetXaxis();
          axis -> SetBinLabel(1,"PAR_X");
          axis -> SetBinLabel(2,"PAR_H");
          for (int i=0;i<5;++i){
            sprintf(buf,"PAR_BP%d",i);
            axis -> SetBinLabel(3+i,buf);
          }
          axis -> SetBinLabel(8,"-");
          for (int i=0;i<2;++i){
            sprintf(buf,"SER_V%d",i);
            axis -> SetBinLabel(9+i,buf);
          }
          for (int i=0;i<6;++i){
            sprintf(buf,"SER_BP%d",i);
            axis -> SetBinLabel(11+i,buf);
          }
          axis -> SetBinLabel(17,"M1a");
          axis -> SetBinLabel(18,"M1b");
          axis -> SetBinLabel(19,"M2a");
          axis -> SetBinLabel(20,"M2b");
          axis -> SetBinLabel(21,"M3a");
          axis -> SetBinLabel(22,"M3b");
          axis -> SetBinLabel(23,"M4");
          axis -> SetBinLabel(24,"M5");
        }

      } // end of loop over PU

    } // end of loop over processing boards

  } // end of loop over quarters

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
  
//       int bcid = err->bcid();
//       if (bcid>0) {
//         err->index(quarter,board,pu);
//         fill(m_bcsu_src[quarter][board],pu+1,1.);
//       }
      
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
      if (bcid>0) {
        err->index(quarter,board,pu);
        fill(m_bcsu_src[quarter][board],0,1.);
      }
      
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
        tell1_summary |= (J|K|J2|K2);
      }
    }
    
    // Loop over ctrl boards error
    for (iterr=ctrlCUSU->begin(); iterr!=ctrlCUSU->end(); ++iterr){
      const LHCb::L0MuonError *err = (*iterr);
  
//       int bcid = err->bcid();
//       if (bcid>0) {
//         err->index(quarter,board,pu);
//         fill(m_sync_ctrl_src,quarter+4,1.); // requires to change histogram range
//       }

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
        tell1_summary |= (J|K|J2|K2);
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
