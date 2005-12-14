
//----------------------------------------------------------------------------
/** @file DeRich2HPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRich2HPDPanel
 *
 *  CVS Log :-
 *  $Id: DeRich2HPDPanel.cpp,v 1.22 2005-12-14 09:34:52 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#define DERICH2HPDPANEL_CPP

// Include files
#include "RichDet/DeRich2HPDPanel.h"


// Gaudi
#include "GaudiKernel/MsgStream.h"

// MathCore files
#include "Kernel/SystemOfUnits.h"

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


StatusCode DeRich2HPDPanel::initialize()
{
  if ( !DeRichHPDPanel::initialize() ) return StatusCode::FAILURE;

  MsgStream log(msgSvc(), myName() );
  log << MSG::DEBUG << "Initializing " << myName() << endreq;
  this->printOut(log);

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

  log << MSG::DEBUG << "m_panelHorizEdge:" << m_panelHorizEdge
      << " m_panelVerticalEdgeEven:" << m_panelVerticalEdgeEven
      << " m_panelVerticalEdgeOdd:" << m_panelVerticalEdgeOdd
      << "  m_detPlaneVertEdge:" <<  m_detPlaneVertEdge
      << endreq;

  return StatusCode::SUCCESS;
}

// ===========================================================================

bool DeRich2HPDPanel::findHPDRowCol(const Gaudi::XYZPoint& inPanel,
                                    LHCb::RichSmartID& id) const
{

  // find HPD row/column and check if the point is withing the covered area

  const unsigned int HPDColumn =
    static_cast<unsigned int> (floor((inPanel.x()-m_panelHorizEdge)/
                                     m_columnPitch));

  if (HPDColumn >=  m_HPDColumns)  return false;
  id.setHPDCol(HPDColumn);

  unsigned int HPDRow = 0;
  if (0 == HPDColumn%2) {
    HPDRow = static_cast<unsigned int>
      (floor((inPanel.y() - m_panelVerticalEdgeEven)/m_rowPitch));
  } else {
    HPDRow = static_cast<unsigned int>
      (floor((inPanel.y() - m_panelVerticalEdgeOdd)/ m_rowPitch));
  }

  if (HPDRow >=  m_HPDRows) return false;

  id.setHPDNumInCol( HPDRow );
  return true;

}

//=========================================================================
//  convert a point from the panel to the global coodinate system
//=========================================================================
Gaudi::XYZPoint DeRich2HPDPanel::globalPosition( const Gaudi::XYZPoint& localPoint,
                                                 const Rich::Side side) const
{
  const int sign = ( side == Rich::left ? -1 : 1 );
  return (geometry()->
          toGlobal(Gaudi::XYZPoint(localPoint.x()+sign*m_detPlaneHorizEdge,
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
