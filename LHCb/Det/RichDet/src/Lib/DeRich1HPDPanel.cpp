// $Id: DeRich1HPDPanel.cpp,v 1.12 2003-11-21 22:27:40 jonesc Exp $
#define DERICH1HPDPANEL_CPP

// Include files
#include "RichDet/DeRich1HPDPanel.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// CLHEP files
#include "CLHEP/Units/SystemOfUnits.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"



//----------------------------------------------------------------------------
//
// Implementation of class :  DeRich1HPDPanel
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRich1HPDPanel = 12011;  // User defined

// Standard Constructor
DeRich1HPDPanel::DeRich1HPDPanel() {}

// Standard Destructor
DeRich1HPDPanel::~DeRich1HPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich1HPDPanel::classID() { return CLID_DeRich1HPDPanel; }

StatusCode DeRich1HPDPanel::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRich1HPDPanel" );
  log << MSG::VERBOSE <<"Starting initialisation of DeRich1HPDPanel" << endreq;
  this->printOut(log);

  if ( !DeRichHPDPanel::initialize() ) return StatusCode::FAILURE;

  // specific initialization for Rich1HPDPanel
  m_columnPitch = m_HPD1Centre.x() - m_HPD0Centre.x();
  m_rowPitch = m_HPDNSCentre.y() - m_HPD0Centre.y();
  log << MSG::DEBUG <<"columnPitch:"<< m_columnPitch << " rowPitch:"
      << m_rowPitch << endreq;

  if (fabs(m_rowPitch) < m_activeRadius*2) {
    log << MSG::WARNING << "The active area is bigger by:" 
        << (m_activeRadius*2 - fabs(m_rowPitch))/mm
        << " mm than the row pitch.  There could be loss of photons" 
        << endreq;
  }

  // get the Horizontal Edge of the HPD cover area. When the HPD centre
  // coordinate is positive the pitch is negative and vise versa
  m_panelHorizEdgeEven = m_HPD0Centre.x() - 0.5*m_columnPitch;
  m_panelHorizEdgeOdd = m_HPDNSCentre.x() - 0.5*m_columnPitch;

  m_detPlaneHorizEdge = fabs(m_panelHorizEdgeEven);

  if (m_detPlaneHorizEdge < fabs(m_panelHorizEdgeOdd) )
    m_detPlaneHorizEdge = fabs(m_panelHorizEdgeOdd);

  //get the Vertical Edge for the two types of columns
  //numbers start at 0
  m_panelVerticalEdge = m_HPD0Centre.y() - 0.5*m_rowPitch;
      
  m_detPlaneVertEdge = fabs(m_panelVerticalEdge);

  log << MSG::DEBUG <<"panelHorizEdgeEven:"<< m_panelHorizEdgeEven
      << " panelHorizEdgeOdd:" << m_panelHorizEdgeOdd
      << " panelVerticalEdge:" << m_panelVerticalEdge 
      << " m_detPlaneHorizEdge:" << m_detPlaneHorizEdge
      << endreq;

  log << MSG::DEBUG <<"Finished initialisation for DeRich1HPDPanel"<< endreq;

  return sc;
}

// ===========================================================================

bool DeRich1HPDPanel::findHPDRowCol (const HepPoint3D& inPanel,
                                          RichSmartID& id)
{

  unsigned int HPDRow = 
    static_cast<unsigned int>(floor((inPanel.y() - m_panelVerticalEdge) /
                                    m_rowPitch));
  
  if (HPDRow >= m_HPDRows) return false;
  id.setPDRow( HPDRow );
  
  unsigned int HPDColumn = 0;
  if (0 == HPDRow%2) {
    HPDColumn = static_cast<unsigned int> (floor((inPanel.x() - m_panelHorizEdgeEven)
                                                 / m_columnPitch));
  } else {
    HPDColumn = static_cast<unsigned int> (floor((inPanel.x() - m_panelHorizEdgeOdd)
                                                 / m_columnPitch));
  }

  if (HPDColumn >= m_HPDColumns) return false;
  id.setPDCol(HPDColumn);
  
  return true;
}


