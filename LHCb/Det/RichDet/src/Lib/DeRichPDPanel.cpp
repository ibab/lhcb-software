// $Id: DeRichPDPanel.cpp,v 1.1 2003-10-22 10:48:28 papanest Exp $
// Include files 

#include "DetDesc/IGeometryInfo.h"

// local
#include "RichDet/DeRichPDPanel.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeRichPDPanel
//
// 2003-10-07 : Antonis Papanestis
//-----------------------------------------------------------------------------

//=============================================================================

bool DeRichPDPanel::detPlanePoint( const HepPoint3D& pGlobal,
                                   const HepVector3D& vGlobal,
                                   HepPoint3D& hitPosition,
                                   traceMode mode) {

  // transform point and vector to the MaPMT Panel coordsystem.
  HepPoint3D pLocal( geometry()->toLocal(pGlobal) );
  HepVector3D vLocal( vGlobal );
  vLocal.transform( m_vectorTransf );

  double scalar1 = vLocal*m_localPlaneNormal;
  double distance = 0.0;

  if ( scalar1 == 0.0 ) return false;

  distance = -(m_localPlane.d() + pLocal*m_localPlaneNormal) / scalar1;
  HepPoint3D hitInPanel( pLocal + distance*vLocal );

  hitPosition = geometry()->toGlobal( hitInPanel );

  if ( mode == tight) {
    if ( fabs(hitInPanel.x()) >= m_detPlaneHorizEdge ||
         fabs(hitInPanel.y()) >= m_detPlaneVertEdge ) {
      return false;
    }
    
  }
  return true;
  
}
//=============================================================================
