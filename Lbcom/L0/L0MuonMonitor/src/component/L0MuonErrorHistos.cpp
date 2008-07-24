// $Id: L0MuonErrorHistos.cpp,v 1.1 2008-07-24 09:36:53 jucogan Exp $
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
    m_hctrl1[ih]=NULL;
  }
  
  for (int quarter=0; quarter<4; ++quarter){
 
   for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
      m_hctrl2[ih][quarter]=NULL;
    }
    
   for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::NErrors; ++ih){
     for (int board=0; board<12; ++board){
       m_hproc[ih][quarter][board]=NULL;
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
  
  std::string hname;
  
  for (int quarter=0; quarter<4; ++quarter){
    bookHistos_ctrl(quarter, shortname);
  }
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::NErrors; ++ih){
    for (int quarter=0; quarter<4; ++quarter){
      for (int region=0; region<4; ++region){
        bookHistos_proc(quarter, region, shortname);
      }
    }
  }

}


void L0MuonErrorHistos::bookHistos_ctrl(int quarter, bool shortname)
{
  
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::Decoding_data; ++ih){
    if (m_hctrl1[ih]==NULL) {
      hname=L0Muon::MonUtilities::hname_error_ctrl1(ih,toolname);
      m_hctrl1[ih]=book1D(hname,-0.5,3.5,4);
    }
    hname=L0Muon::MonUtilities::hname_error_ctrl2(ih,quarter,toolname);
    m_hctrl2[ih][quarter]=book1D(hname,-0.5,15.5,16);
  }
  
}


void L0MuonErrorHistos::bookHistos_proc(int quarter, bool shortname)
{
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::NErrors; ++ih){
    for (int reg=0; reg<4; ++reg){
      bookHistos_proc(quarter,reg,shortname);
    }
  }

}

void L0MuonErrorHistos::bookHistos_proc(int quarter, int region, bool shortname)
{
  
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();
  
  for (int ih=L0Muon::MonUtilities::Overflow; ih<L0Muon::MonUtilities::NErrors; ++ih){
    for (int board=3*region; board<(3*region+3); ++board){
      hname=L0Muon::MonUtilities::hname_error_proc(ih,quarter,board,toolname);
      m_hproc[ih][quarter][board]=book1D(hname,-0.5,5.5,6);
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
      if (type==L0Muon::MonUtilities::Ctrl) _fillHistos_ctrl(*iterr);
      if (type==L0Muon::MonUtilities::Proc) _fillHistos_proc(*iterr);
    }
  }
}

void L0MuonErrorHistos::_fillHistos_ctrl(const LHCb::L0MuonError *err)
{
  int quarter,board,pu;
  err->index(quarter,board,pu);

  if (board==15) {
    if (err->decoding_cand()) fill(m_hctrl1[L0Muon::MonUtilities::Decoding],quarter,1);
    if (err->internal()) fill(m_hctrl1[L0Muon::MonUtilities::Internal],quarter,1);
    if (err->overflow()) fill(m_hctrl1[L0Muon::MonUtilities::Overflow],quarter,1);
  }

  if (err->decoding_cand()) fill(m_hctrl2[L0Muon::MonUtilities::Decoding][quarter],board,1);
  if (err->internal()) fill(m_hctrl2[L0Muon::MonUtilities::Internal][quarter],board,1);
  if (err->overflow()) fill(m_hctrl2[L0Muon::MonUtilities::Overflow][quarter],board,1);
}

void L0MuonErrorHistos::_fillHistos_proc(const LHCb::L0MuonError *err)
{
  int quarter,board,pu;
  err->index(quarter,board,pu);

  if (err->decoding_data()) fill(m_hproc[L0Muon::MonUtilities::Decoding_data][quarter][board],pu,1);
  if (err->decoding_cand()) fill(m_hproc[L0Muon::MonUtilities::Decoding][quarter][board],pu,1);
  if (err->internal()) fill(m_hproc[L0Muon::MonUtilities::Internal][quarter][board],pu,1);
  if (err->overflow()) fill(m_hproc[L0Muon::MonUtilities::Overflow][quarter][board],pu,1);
}


