// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/DeMuonStation.cpp,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
#define DDEXAMPLE_DEMUONSTATION_CPP

// Include files
#include "DeMuonStation.h"


//------------------------------------------------------------------------------
//
// Implementation of class :  DeMuonStation
//
//------------------------------------------------------------------------------


const CLID& CLID_DEMuonStation = 9990;  // User defined

// Standard Constructors
DeMuonStation::DeMuonStation()
: m_thickness( 0.0 ), m_padx( 0.0 ), m_pady( 0.0 )
{
}
  
DeMuonStation::DeMuonStation( double t, double x, double y )
: m_thickness( t ), m_padx( x ), m_pady( y )
{
}

// Standard Destructor
DeMuonStation::~DeMuonStation()
{
}
  
// Retrieve Pointer to class defininition structure
const CLID& DeMuonStation::classID()
{
  return CLID_DEMuonStation;
}

// Get/Set thickness
double DeMuonStation::thickness()
{
  return m_thickness;
}

void   DeMuonStation::setThickness( double t )
{
  m_thickness = t;
}

// Get/Set pad dimensions
double DeMuonStation::padx()
{
  return m_padx;
}

double DeMuonStation::pady()
{
  return m_pady;
}

void DeMuonStation::setPadx( double x )
{
  m_padx = x;
}

void DeMuonStation::setPady( double y )
{
  m_pady = y;
}
