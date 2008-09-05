// $Id: L0MuonErrorHistos.cpp,v 1.3 2008-09-05 09:07:09 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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
DECLARE_TOOL_FACTORY( L0MuonErrorHistos );


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
 
   for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
      m_hquarter[ih][quarter]=NULL;
    }
    
   for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::NErrors; ++ih){
     for (int board=0; board<12; ++board){
       m_hboard[ih][quarter][board]=NULL;
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

void L0MuonErrorHistos::bookHistos_gen(bool shortname)
{
  
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
    hname=L0Muon::MonUtilities::hname_error_ctrl1(ih,toolname);
    m_hgen[ih]=book1D(hname,-0.5,3.5,4);
  }
  
}

void L0MuonErrorHistos::bookHistos_quarter(int quarter, bool shortname)
{
  
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
    hname=L0Muon::MonUtilities::hname_error_ctrl2(ih,quarter,toolname);
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
      hname=L0Muon::MonUtilities::hname_error_proc(ih,quarter,board,toolname);
      m_hboard[ih][quarter][board]=book1D(hname,-0.5,5.5,6);
    }
  }

}

void L0MuonErrorHistos::fillHistos()
{
  
  std::string location;
  location=LHCb::L0MuonErrorLocation::Default+context();
  _fillHistos(location,L0Muon::MonUtilities::Ctrl);

  location=LHCb::L0MuonErrorLocation::CtrlBCSU+context();
  _fillHistos(location,L0Muon::MonUtilities::Ctrl);

  location=LHCb::L0MuonErrorLocation::ProcBCSU+context();
  _fillHistos(location,L0Muon::MonUtilities::Proc);

  location=LHCb::L0MuonErrorLocation::ProcPU+context();
  _fillHistos(location,L0Muon::MonUtilities::Proc);
  
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

