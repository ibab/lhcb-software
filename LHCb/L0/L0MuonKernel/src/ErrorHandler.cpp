// $Id: ErrorHandler.cpp,v 1.2 2008-07-11 15:30:48 jucogan Exp $
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
    m_name="error";
    m_mask=0xFFFF;
}
L0Muon::ErrorHandler::ErrorHandler( std::string fmt, int mask ) {
    m_value=0;
    m_inError=false;
    m_counter=0;
    m_fmt=fmt;
    m_mask=mask;
    m_name="error";
}
L0Muon::ErrorHandler::ErrorHandler( std::string fmt, int mask, std::string name ) {
    m_value=0;
    m_inError=false;
    m_counter=0;
    m_fmt=fmt;
    m_mask=mask;
    m_name=name;
}
//=============================================================================
// Destructor
//=============================================================================
L0Muon::ErrorHandler::~ErrorHandler() {} 

//=============================================================================

std::ostream &L0Muon::operator<<(std::ostream &os, const L0Muon::ErrorHandler &field) {
  if (field.inError()) os<<"\033["<<31<<";1m";
  os <<  boost::format(field.fmt()) % field.value();
  if (field.inError()) os<<"\033[0m";
  return os;
}
  
