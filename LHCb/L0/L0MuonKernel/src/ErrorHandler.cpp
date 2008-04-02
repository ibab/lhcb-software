// $Id: ErrorHandler.cpp,v 1.1 2008-04-02 12:45:50 jucogan Exp $
// Include files 

// local
#include "L0MuonKernel/ErrorHandler.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ErrorHandler
//
// 2008-03-27 : Julien Cogan
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Muon::ErrorHandler::ErrorHandler(  ) {
    m_value=0;
    m_inError=false;
    m_counter=0;
    m_fmt=" %1d";
    m_mask=0xFFFF;
}
L0Muon::ErrorHandler::ErrorHandler( std::string fmt, int mask ) {
    m_value=0;
    m_inError=false;
    m_counter=0;
    m_fmt=fmt;
    m_mask=mask;
}
//=============================================================================
// Destructor
//=============================================================================
L0Muon::ErrorHandler::~ErrorHandler() {} 

//=============================================================================

std::ostream &L0Muon::operator<<(std::ostream &os, const L0Muon::ErrorHandler &field) {
  os <<  boost::format(field.fmt()) % field.value();
  if (field.inError()) os<<"!";
  else                 os<<" ";
  return os;
}
  
