
/** @file DeRich2HPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRich2HPDPanel
 *
 *  CVS Log :-
 *  $Id: DeRich2HPDPanel.cpp,v 1.18 2004-10-21 08:55:42 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.17  2004/10/20 22:41:55  jonrob
 *  Tidy up inline and virtual functions (whilst solving a windows problem)
 *
 *  Revision 1.16  2004/10/20 17:02:44  jonrob
 *  Updates for windows
 *
 *  Revision 1.15  2004/10/20 16:16:36  jonrob
 *  More minor updates to functions (adding const etc.)
 *
 *  Revision 1.14  2004/10/18 09:21:49  jonrob
 *  Minor updates to functions (adding const etc.)
 *
 *  Revision 1.13  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH2HPDPANEL_CPP

// Include files
#include "RichDet/DeRich2HPDPanel.h"


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

const CLID& CLID_DeRich2HPDPanel = 12012;  // User defined

// Standard Constructor
DeRich2HPDPanel::DeRich2HPDPanel()
{}

// Standard Destructor
DeRich2HPDPanel::~DeRich2HPDPanel() {
}

// Retrieve Pointer to class defininition structure
const CLID& DeRich2HPDPanel::classID() {
  return CLID_DeRich2HPDPanel;
}


StatusCode DeRich2HPDPanel::initialize() {

  StatusCode sc = StatusCode::FAILURE;

  MsgStream log(msgSvc(), "DeRich2HPDPanel" );
  log << MSG::VERBOSE <<"Starting initialisation of DeRich2HPDPanel" << endreq;
  this->printOut(log);

  if ( !DeRichHPDPanel::initialize() ) return sc;

  // specific initialization for Rich2HPDPanel
  m_rowPitch = m_HPD1Centre.y() - m_HPD0Centre.y();
  m_columnPitch = m_HPDNSCentre.x() - m_HPD0Centre.x();
  log << MSG::DEBUG <<"m_columnPitch:"<< m_columnPitch << " m_rowPitch:"
      << m_rowPitch << endreq;

  if (fabs(m_columnPitch) < m_activeRadius*2) {
    log << MSG::WARNING << "The active area is bigger by:"
        << (m_activeRadius*2 - fabs(m_columnPitch))/mm
        << " mm than the column pitch.  There could be loss of photons"
        << endreq;
  }

  // get the Horizontal Edge of the HPD cover area. When the HPD centre
  // coordinate is positive the pitch is negative and vise versa
  m_panelHorizEdge = m_HPD0Centre.x() - 0.5*m_columnPitch;
  m_detPlaneHorizEdge = fabs(m_panelHorizEdge);

  //get the Vertical Edge for the two types of columns
  //numbers start at 0
  m_panelVerticalEdgeEven = m_HPD0Centre.y() - 0.5*m_rowPitch;
  m_panelVerticalEdgeOdd = m_HPDNSCentre.y() - 0.5*m_rowPitch;

  m_detPlaneVertEdge = fabs(m_panelVerticalEdgeEven);
  if (fabs(m_panelVerticalEdgeOdd) > m_detPlaneVertEdge)
    m_detPlaneVertEdge = fabs(m_panelVerticalEdgeOdd);

  log << MSG::DEBUG <<"m_panelHorizEdge:"<< m_panelHorizEdge
      << " m_panelVerticalEdgeEven:" << m_panelVerticalEdgeEven
      << " m_panelVerticalEdgeOdd:" << m_panelVerticalEdgeOdd
      << "  m_detPlaneVertEdge:" <<  m_detPlaneVertEdge
      << endreq;

  log << MSG::DEBUG <<"Finished initialisation for DeRich2HPDPanel "<< m_name
      <<endreq;

  return StatusCode::SUCCESS;
}

// ===========================================================================

bool DeRich2HPDPanel::findHPDRowCol(const HepPoint3D& inPanel,
                                    RichSmartID& id) const
{

  // find HPD row/column and check if the point is withing the covered area

  const unsigned int HPDColumn =
    static_cast<unsigned int> (floor((inPanel.x()-m_panelHorizEdge)/
                                     m_columnPitch));

  if (HPDColumn >=  m_HPDColumns)  return false;
  id.setPDCol(HPDColumn);

  unsigned int HPDRow = 0;
  if (0 == HPDColumn%2) {
    HPDRow = static_cast<unsigned int>
      (floor((inPanel.y() - m_panelVerticalEdgeEven)/m_rowPitch));
  } else {
    HPDRow = static_cast<unsigned int>
      (floor((inPanel.y() - m_panelVerticalEdgeOdd)/ m_rowPitch));
  }

  if (HPDRow >=  m_HPDRows) return false;

  id.setPDRow( HPDRow );
  return true;

}

//=========================================================================
//  convert a point from the panel to the global coodinate system
//=========================================================================
HepPoint3D DeRich2HPDPanel::globalPosition( const HepPoint3D& localPoint,
                                            const Rich::Side side) const
{
  const int sign = ( side == Rich::left ? -1 : 1 );
  return (geometry()->
          toGlobal(HepPoint3D(localPoint.x()+sign*m_detPlaneHorizEdge,
                              localPoint.y(),
                              localPoint.z() + m_detPlaneZ )));
}

//============================================================================


const double DeRich2HPDPanel::localOffset() const
{
  return m_detPlaneHorizEdge;
}

unsigned int DeRich2HPDPanel::PDMax() const
{
  return m_HPDColumns * m_HPDRows;
}

unsigned int DeRich2HPDPanel::PDRow(const unsigned int PD) const
{
  return PD%m_HPDRows;
}

unsigned int DeRich2HPDPanel::PDCol(const unsigned int PD) const
{
  return PD/m_HPDRows;
}

unsigned int DeRich2HPDPanel::HPDForNS() const
{
  return m_HPDRows;
}

unsigned int DeRich2HPDPanel::HPDForB() const
{
  return m_HPDRows -1;
}

unsigned int DeRich2HPDPanel::HPDForC() const
{
  return m_HPDRows*m_HPDColumns -
    static_cast<unsigned int>(0.5*m_HPDColumns);
}

unsigned int DeRich2HPDPanel::HPDRowColToNum(const unsigned int HPDRow,
                                             const unsigned int HPDCol ) const
{
  return HPDCol * m_HPDRows + HPDRow;
}
