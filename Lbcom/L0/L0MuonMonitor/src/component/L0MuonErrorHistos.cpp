// Include files 

#include "Event/L0MuonError.h"
#include "Kernel/MuonLayout.h"

// local
#include "L0MuonErrorHistos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonErrorHistos
//
// 2008-07-17 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonErrorHistos )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonErrorHistos::L0MuonErrorHistos( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<L0MuonErrorHistos>(this);
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
    m_hgen[ih]=NULL; 
  }
  
  for (int quarter=0; quarter<4; ++quarter){

    m_hmulti_opt[quarter]=NULL;
    m_hmulti_ser[quarter]=NULL;
    m_hmulti_par[quarter]=NULL;

    for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
      m_hquarter[ih][quarter]=NULL;
    }
    
    for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::NErrors; ++ih){
      for (int board=0; board<12; ++board){
        m_hboard[ih][quarter][board]=NULL;
      }
    }
  }
  
  for (int quarter=0; quarter<4; ++quarter){
    for (int board=0; board<12; ++board){
      for (int ipu=0; ipu<4; ++ipu) {
        m_hpu[quarter][board][ipu]=NULL;
      }
    }
  }

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonErrorHistos::~L0MuonErrorHistos() {} 

//=============================================================================


void L0MuonErrorHistos::bookHistos(bool shortname)
{
  
  bookHistos_gen(shortname);
  bookHistos_multi(shortname);

  for (int quarter=0; quarter<4; ++quarter){
    bookHistos_quarter(quarter, shortname);
  }
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
    for (int quarter=0; quarter<4; ++quarter){
      for (int region=0; region<4; ++region){
        bookHistos_board(quarter, region, shortname);
      }
    }
  }

}

void L0MuonErrorHistos::bookHistos_multi(bool shortname)
{
  
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();
  
  for (int quarter=0; quarter<4; ++quarter){
    hname=L0Muon::MonUtilities::hname_error_multi_opt(quarter,toolname);
    m_hmulti_opt[quarter]=book1D(hname,-0.5,49.5,50);
    hname=L0Muon::MonUtilities::hname_error_multi_ser(quarter,toolname);
    m_hmulti_ser[quarter]=book1D(hname,-0.5,49.5,50);;
    hname=L0Muon::MonUtilities::hname_error_multi_par(quarter,toolname);
    m_hmulti_par[quarter]=book1D(hname,-0.5,49.5,50);;
  }
  
}

void L0MuonErrorHistos::bookHistos_gen(bool shortname)
{
  
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
    hname=L0Muon::MonUtilities::hname_error_gen(ih,toolname);
    m_hgen[ih]=book1D(hname,-0.5,3.5,4);
  }
  
}

void L0MuonErrorHistos::bookHistos_quarter(int quarter, bool shortname)
{
  
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
    hname=L0Muon::MonUtilities::hname_error_quarter(ih,quarter,toolname);
    m_hquarter[ih][quarter]=book1D(hname,-0.5,15.5,16);
  }
  
}


void L0MuonErrorHistos::bookHistos_board(int quarter, bool shortname)
{
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::NErrors; ++ih){
    for (int reg=0; reg<4; ++reg){
      bookHistos_board(quarter,reg,shortname);
    }
  }

}

void L0MuonErrorHistos::bookHistos_board(int quarter, int region, bool shortname)
{
  
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::NErrors; ++ih){
    for (int board=3*region; board<(3*region+3); ++board){
      hname=L0Muon::MonUtilities::hname_error_board(ih,quarter,board,toolname);
      m_hboard[ih][quarter][board]=book1D(hname,-0.5,5.5,6);
    }
  }
  for (int board=3*region; board<(3*region+3); ++board){
    for (int ipu=0; ipu<4; ++ipu) {
      hname=L0Muon::MonUtilities::hname_error_pu(quarter,board,ipu,toolname);
      m_hpu[quarter][board][ipu]=book1D(hname,-0.5,31.5,32);
    }
  }

}

void L0MuonErrorHistos::fillHistos()
{

  for (int quarter=0; quarter<4; ++quarter){
    m_nmulti_opt[quarter]=0;
    m_nmulti_ser[quarter]=0;
    m_nmulti_par[quarter]=0;
  }
  
  std::string location;
  location=LHCb::L0MuonErrorLocation::Default+context();
  _fillHistos(location,L0Muon::MonUtilities::Ctrl);

  location=LHCb::L0MuonErrorLocation::CtrlBCSU+context();
  _fillHistos(location,L0Muon::MonUtilities::Ctrl);

  location=LHCb::L0MuonErrorLocation::ProcBCSU+context();
  _fillHistos(location,L0Muon::MonUtilities::Proc);

  location=LHCb::L0MuonErrorLocation::ProcPU+context();
  _fillHistos(location,L0Muon::MonUtilities::Proc);

  for (int quarter=0; quarter<4; ++quarter){
    fill(m_hmulti_opt[quarter],m_nmulti_opt[quarter],1);
    fill(m_hmulti_ser[quarter],m_nmulti_ser[quarter],1);
    fill(m_hmulti_par[quarter],m_nmulti_par[quarter],1);
  }
  
}

void L0MuonErrorHistos::_fillHistos(std::string location, int type)
{
  if (exist<LHCb::L0MuonErrors> (location)) {
    LHCb::L0MuonErrors *perrors = get<LHCb::L0MuonErrors> (location);
    LHCb::L0MuonErrors::const_iterator iterr;
    for (iterr=perrors->begin(); iterr!=perrors->end(); ++iterr){
      const LHCb::L0MuonError *err = (*iterr);
      int quarter,board,pu;
      err->index(quarter,board,pu);

      if (err->decoding_cand()) fill(m_hgen[L0Muon::MonUtilities::Decoding],quarter,1);
      if (err->internal()) fill(m_hgen[L0Muon::MonUtilities::Internal],quarter,1);
      if (err->overflow()) fill(m_hgen[L0Muon::MonUtilities::Overflow],quarter,1);

      // If the error comes from a controller board
      if ( (type==L0Muon::MonUtilities::Ctrl) && (board==15) ) {

          if (err->decoding_cand()){
            if ( m_hquarter[L0Muon::MonUtilities::Decoding][quarter]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hquarter[L0Muon::MonUtilities::Decoding][quarter],board,1);
            }
          }
          
          if (err->internal()){
            if(m_hquarter[L0Muon::MonUtilities::Internal][quarter]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hquarter[L0Muon::MonUtilities::Internal][quarter],board,1);
            }
            
          }
          
          if (err->overflow()){
            if(m_hquarter[L0Muon::MonUtilities::Overflow][quarter]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hquarter[L0Muon::MonUtilities::Overflow][quarter],board,1);
            }
          }
      } // L0Muon::MonUtilities::Ctrl 
      
      // If the error comes from a processing board
      if (type==L0Muon::MonUtilities::Proc) {

          if (err->decoding_cand()){
            if ( m_hquarter[L0Muon::MonUtilities::Decoding][quarter]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hquarter[L0Muon::MonUtilities::Decoding][quarter],board,1);
            }
            if(m_hboard[L0Muon::MonUtilities::Decoding][quarter][board]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hboard[L0Muon::MonUtilities::Decoding][quarter][board],pu,1);
            }
          }

          if (err->decoding_data()){
            if ( m_hquarter[L0Muon::MonUtilities::Decoding][quarter]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hquarter[L0Muon::MonUtilities::Decoding][quarter],board,1);
            }
            if(m_hboard[L0Muon::MonUtilities::Decoding_data][quarter][board]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hboard[L0Muon::MonUtilities::Decoding_data][quarter][board],pu,1);
            }
          }
          
          if (err->internal()){
            if(m_hquarter[L0Muon::MonUtilities::Internal][quarter]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hquarter[L0Muon::MonUtilities::Internal][quarter],board,1);
            }
            if(m_hboard[L0Muon::MonUtilities::Internal][quarter][board]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hboard[L0Muon::MonUtilities::Internal][quarter][board],pu,1);
              if (pu<4) {
                unsigned int linkError = err->hardware();
                for (int ibit=0; ibit<32; ++ibit){
                  if ( ((linkError>>ibit)&1)==1) fill(m_hpu[quarter][board][pu],ibit,1);
                }
                if (linkError&0xFF0000) ++m_nmulti_opt[quarter];
                if (linkError&0x00FF00) ++m_nmulti_ser[quarter];
                if (linkError&0x0000FF) ++m_nmulti_par[quarter];
              }
            }
          }
          
          if (err->overflow()){
            if(m_hquarter[L0Muon::MonUtilities::Overflow][quarter]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hquarter[L0Muon::MonUtilities::Overflow][quarter],board,1);
            }
            if(m_hboard[L0Muon::MonUtilities::Overflow][quarter][board]==NULL) {
              if (msgLevel(MSG::ERROR) ) error()<<"bad parameters Q"<<quarter<<" PB"<<board<<" PU"<<pu<<endmsg;
            } else {
              fill(m_hboard[L0Muon::MonUtilities::Overflow][quarter][board],pu,1);
            }
          }
      } // type==L0Muon::MonUtilities::Proc
    }
  }
}

