
/** @file DeRich1HPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRich1HPDPanel
 *
 *  CVS Log :-
 *  $Id: DeRich1HPDPanel.cpp,v 1.18 2004-10-20 22:41:55 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.17  2004/10/20 17:02:44  jonrob
 *  Updates for windows
 *
 *  Revision 1.16  2004/10/20 16:16:36  jonrob
 *  More minor updates to functions (adding const etc.)
 *
 *  Revision 1.15  2004/10/18 09:21:49  jonrob
 *  Minor updates to functions (adding const etc.)
 *
 *  Revision 1.14  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

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
                                     RichSmartID& id) const
{

  unsigned int HPDRow =
    static_cast<unsigned int>(floor((inPanel.y() - m_panelVerticalEdge) /
                                    m_rowPitch));

  if (HPDRow >= m_HPDRows) return false;
  id.setPDRow( HPDRow );

  unsigned int HPDColumn = 0;
  if (0 == HPDRow%2) {
    HPDColumn = static_cast<unsigned int>
      (floor((inPanel.x() - m_panelHorizEdgeEven) / m_columnPitch));
  } else {
    HPDColumn = static_cast<unsigned int>
      (floor((inPanel.x() - m_panelHorizEdgeOdd) / m_columnPitch));
  }

  if (HPDColumn >= m_HPDColumns) return false;
  id.setPDCol(HPDColumn);

  return true;
}


//=========================================================================
//  convert a point from the panel to the global coodinate system
//=========================================================================
HepPoint3D DeRich1HPDPanel::globalPosition( const HepPoint3D& localPoint,
                                            const Rich::Side side) const
{
  const int sign = ( side == Rich::top ? -1 : 1 );
  return (geometry()->
          toGlobal(HepPoint3D(localPoint.x(),
                              localPoint.y()+sign*m_detPlaneVertEdge,
                              localPoint.z() + m_detPlaneZ )));
}

//============================================================================

const double DeRich1HPDPanel::localOffset() const
{
  return m_detPlaneVertEdge;
}

unsigned int DeRich1HPDPanel::PDMax() const
{
  return  m_HPDColumns * m_HPDRows;
}

unsigned int DeRich1HPDPanel::PDRow(const unsigned int PD) const 
{
  return PD/m_HPDColumns;
}

unsigned int DeRich1HPDPanel::PDCol(const unsigned int PD) const
{
  return PD%m_HPDColumns;
}

unsigned int DeRich1HPDPanel::HPDForNS() const 
{
  return m_HPDColumns;
}

unsigned int DeRich1HPDPanel::HPDForB() const 
{
  return m_HPDColumns -1;
}

unsigned int DeRich1HPDPanel::HPDForC() const 
{
  return m_HPDRows*m_HPDColumns - static_cast<unsigned int>(0.5*m_HPDRows);
}

unsigned int DeRich1HPDPanel::HPDRowColToNum(const unsigned int HPDRow,
                                             const unsigned int HPDCol ) const 
{
  return HPDRow * m_HPDColumns + HPDCol;
}
