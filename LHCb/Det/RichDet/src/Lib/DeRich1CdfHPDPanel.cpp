
/** @file DeRich1CdfHPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRich1CdfHPDPanel
 *     (COLUMN wise panel)
 *  The numbering in this class increases in columns and odd cloumns have
 *  one less HPD than even columns
 *
 *  CVS Log :-
 *  $Id: DeRich1CdfHPDPanel.cpp,v 1.23 2005-02-22 14:33:51 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.22  2004/10/27 14:18:04  jonrob
 *  more minor additions
 *
 *  Revision 1.21  2004/10/21 08:55:42  jonrob
 *  minor update
 *
 *  Revision 1.20  2004/10/20 22:41:55  jonrob
 *  Tidy up inline and virtual functions (whilst solving a windows problem)
 *
 *  Revision 1.19  2004/10/20 17:02:44  jonrob
 *  Updates for windows
 *
 *  Revision 1.18  2004/10/20 16:16:36  jonrob
 *  More minor updates to functions (adding const etc.)
 *
 *  Revision 1.17  2004/10/18 09:21:49  jonrob
 *  Minor updates to functions (adding const etc.)
 *
 *  Revision 1.16  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICH1CDFHPDPANEL_CPP

// Include files
#include "RichDet/DeRich1CdfHPDPanel.h"

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

const CLID& CLID_DeRich1CdfHPDPanel = 12013;  // User defined

// Standard Constructor
DeRich1CdfHPDPanel::DeRich1CdfHPDPanel() {}

// Standard Destructor
DeRich1CdfHPDPanel::~DeRich1CdfHPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich1CdfHPDPanel::classID() { return CLID_DeRich1CdfHPDPanel; }


//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRich1CdfHPDPanel::initialize() 
{
  if ( !DeRichHPDPanel::initialize() ) return StatusCode::FAILURE;

  MsgStream log(msgSvc(), myName() );
  log << MSG::VERBOSE <<"Starting initialisation of " << myName() << endreq;
  this->printOut(log);

  // specific initialization for Rich1CdfHPDPanel
  // the number of rows (m_HPDRows) in this panel is 2*rows-1
  // of what you would expect from a normal grid

  log << MSG::DEBUG <<"HPDsInBigCol:" << m_HPDsInBigCol
      << " PDMax:" << PDMax() << endreq;

  m_rowPitch = m_HPD1Centre.y() - m_HPD0Centre.y();
  m_columnPitch = m_HPDNSCentre.x() - m_HPD0Centre.x();
  log << MSG::DEBUG <<"columnPitch:"<< m_columnPitch << " rowPitch:"
      << m_rowPitch << endreq;

  if ( fabs(m_columnPitch) < m_activeRadius*2 ) {
    log << MSG::WARNING << "The active area is bigger by:"
        << (m_activeRadius*2 - fabs(m_columnPitch))/mm
        << " mm than the column pitch.  There could be loss of photons"
        << endreq;
  }

  // get the Horizontal Edge of the HPD cover area. When the HPD centre
  // coordinate is positive the pitch is negative and vise versa
  m_panelHorizEdge = m_HPD0Centre.x() - 0.5*m_columnPitch;

  //  m_detPlaneHorizEdge = m_fabs_panelHorizEdge;
  m_detPlaneHorizEdge = fabs(m_panelHorizEdge);

  //get the Vertical Edge for the two types of columns
  //numbers start at zero (so the first is even)
  m_panelVerticalEdgeEven = m_HPD0Centre.y() - 0.5*m_rowPitch;
  m_panelVerticalEdgeOdd = m_HPDNSCentre.y() - 0.5*m_rowPitch;

  m_detPlaneVertEdge = fabs(m_panelVerticalEdgeEven);
  if (fabs(m_panelVerticalEdgeOdd) > m_detPlaneVertEdge)
    m_detPlaneVertEdge = fabs(m_panelVerticalEdgeOdd);

  log << MSG::DEBUG <<"panelHorizEdge:"<< m_panelHorizEdge
      << " panelVerticalEdgeEven:" << m_panelVerticalEdgeEven
      << " panelVerticalEdgeOdd:" << m_panelVerticalEdgeOdd
      << "  m_detPlaneVertEdge:" <<  m_detPlaneVertEdge
      << endreq;

  log << MSG::INFO << "Initialized Successfully" << endreq;

  return StatusCode::SUCCESS;
}

// ===========================================================================

bool DeRich1CdfHPDPanel::findHPDRowCol ( const HepPoint3D& inPanel,
                                         RichSmartID& id ) const
{

  // find the correct HPD inside the panel

  const unsigned int HPDColumn =
    static_cast<unsigned int>(floor((inPanel.x() - m_panelHorizEdge)
                                    / m_columnPitch));

  if (HPDColumn >= m_HPDColumns)  return false;
  id.setPDCol(HPDColumn);

  unsigned int HPDRow = 0;
  if (0 == HPDColumn%2) {
    HPDRow=static_cast<unsigned int>
      (floor((inPanel.y() - m_panelVerticalEdgeEven)/m_rowPitch));
    if (HPDRow >= m_HPDsInBigCol) return false;

  } else {
    HPDRow=static_cast<unsigned int>
      (floor((inPanel.y() - m_panelVerticalEdgeOdd)/m_rowPitch));
    if (HPDRow >= m_HPDsInBigCol-1) return false;
  }

  id.setPDRow(HPDRow);
  return true;
}

//=========================================================================
//  convert a point from the panel to the global coodinate system
//=========================================================================
HepPoint3D DeRich1CdfHPDPanel::globalPosition( const HepPoint3D& localPoint,
                                               const Rich::Side side) const
{
  const int sign = ( side == Rich::top ? -1 : 1 );

  return (geometry()->
          toGlobal(HepPoint3D(localPoint.x(),
                              localPoint.y()+sign*m_detPlaneVertEdge,
                              localPoint.z() + m_detPlaneZ )));
}
//============================================================================


const double DeRich1CdfHPDPanel::localOffset() const
{
  return m_detPlaneVertEdge;
}

unsigned int DeRich1CdfHPDPanel::PDMax() const
{
  return static_cast<int>(0.5*m_HPDColumns)*m_HPDsIn2Cols + m_HPDsInBigCol;
}

unsigned int DeRich1CdfHPDPanel::PDRow(const unsigned int PD) const
{
  const unsigned int HPDsLeft = PD%m_HPDsIn2Cols;
  return ( HPDsLeft >= m_HPDsInBigCol ? HPDsLeft - m_HPDsInBigCol : HPDsLeft );
}

unsigned int DeRich1CdfHPDPanel::PDCol(const unsigned int PD) const
{
  const unsigned int HPDsLeft = PD%m_HPDsIn2Cols;
  return ( HPDsLeft >= m_HPDsInBigCol ? 2*(PD/m_HPDsIn2Cols)+1 : 2*(PD/m_HPDsIn2Cols) );
}

unsigned int DeRich1CdfHPDPanel::HPDForNS() const
{
  return static_cast<int>(ceil(m_HPDRows/2.0));
}

unsigned int DeRich1CdfHPDPanel::HPDForB() const
{
  return static_cast<int>(ceil(m_HPDRows/2.0)) -1;
}

unsigned int DeRich1CdfHPDPanel::HPDForC() const
{
  return static_cast<int>(0.5*m_HPDColumns)*m_HPDRows;
}

unsigned int DeRich1CdfHPDPanel::HPDRowColToNum(const unsigned int HPDRow,
                                                const unsigned int HPDCol ) const
{
  unsigned int HPDNumber;
  const unsigned int HPDColDiv2 = HPDCol/2;
  if (0 == HPDCol%2) {
    HPDNumber = HPDColDiv2 * m_HPDsIn2Cols + HPDRow;
  } else {
    HPDNumber = HPDColDiv2 * m_HPDsIn2Cols + HPDRow + m_HPDsInBigCol;
  }
  return HPDNumber;
}
