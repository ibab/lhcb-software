
/** @file DeRichSingleSolidRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichSingleSolidRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichSingleSolidRadiator.cpp,v 1.5 2004-10-18 09:21:49 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2004/09/01 15:20:19  papanest
 *  added functions for TabProps
 *
 *  Revision 1.3  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHSINGLESOLIDRADIATOR_CPP

// Include files
#include "RichDet/DeRichSingleSolidRadiator.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "Kernel/CLHEPStreams.h"

// CLHEP files
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

/// Detector description classes
#include "DetDesc/SolidBoolean.h"
#include "DetDesc/SolidTrd.h"

//----------------------------------------------------------------------------

const CLID& CLID_DeRichSingleSolidRadiator = 12040;  // User defined

// Standard Constructor
DeRichSingleSolidRadiator::DeRichSingleSolidRadiator() {}

// Standard Destructor
DeRichSingleSolidRadiator::~DeRichSingleSolidRadiator() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichSingleSolidRadiator::classID() {
  return CLID_DeRichSingleSolidRadiator;
}

StatusCode DeRichSingleSolidRadiator::initialize() {

  MsgStream log(msgSvc(), "DeRichSingleSolidRadiator" );
  log << MSG::DEBUG << "Starting initialisation for DeRichSingleSolidRadiator "
      << name() << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  if ( DeRichRadiator::initialize().isFailure() ) return StatusCode::FAILURE;
  
  m_solid = geometry()->lvolume()->solid();

  const Material::Tables& myTabProp = geometry()->lvolume()->material()->tabulatedProperties();
  Material::Tables::const_iterator matIter;
  for ( matIter = myTabProp.begin(); matIter!=myTabProp.end(); ++matIter ) {
    if ( (*matIter) ) {
      if ( (*matIter)->type() == "RINDEX" ) {
        m_refIndex = (*matIter);
      }
      if ( (*matIter)->type() == "RAYLEIGH" ) {
        m_rayleigh = (*matIter);
      }
    }
  }

  if (!m_refIndex) {
    log << MSG::ERROR << "Radiator " << name() << " without refractive index"
        << endmsg;
    return StatusCode::FAILURE;
  }

  HepPoint3D zero(0.0, 0.0, 0.0);
  log << MSG::DEBUG << "Found  TabProp " << m_refIndex->name() << " type " 
      << m_refIndex->type() << endmsg;
  if ( m_rayleigh ) 
    log << MSG::DEBUG << "Found  TabProp " << m_rayleigh->name() << " type " 
        << m_rayleigh->type() << endmsg;
  log << MSG::DEBUG <<" Centre:" << geometry()->toGlobal(zero) << endreq;
  log << MSG::DEBUG <<"Finished initialisation for "  << name() << endreq;

  return sc;
}

StatusCode 
DeRichSingleSolidRadiator::nextIntersectionPoint( const HepPoint3D&  pGlobal,
                                                  const HepVector3D& vGlobal,
                                                  HepPoint3D&  returnPoint ) {

  HepPoint3D pLocal( geometry()->toLocal(pGlobal) );
  HepVector3D vLocal( vGlobal );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks) {
    return StatusCode::FAILURE;
  }
  else {
    returnPoint = geometry()->toGlobal( pLocal + ticks[0] * vLocal );
    return StatusCode::SUCCESS;
  }

}

//=========================================================================
//  
//=========================================================================
StatusCode 
DeRichSingleSolidRadiator::intersectionPoints( const HepPoint3D&  position,
                                               const HepVector3D& direction,
                                               HepPoint3D& entryPoint,
                                               HepPoint3D& exitPoint ) {

  HepPoint3D pLocal( geometry()->toLocal(position) );
  HepVector3D vLocal( direction );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks)  return StatusCode::FAILURE;
  
  entryPoint = geometry()->toGlobal( pLocal + ticks[0] * vLocal );
  exitPoint = geometry()->toGlobal( pLocal + ticks[noTicks-1] * vLocal );
  return StatusCode::SUCCESS;

}


//=========================================================================
//  
//=========================================================================
unsigned int 
DeRichSingleSolidRadiator::intersectionPoints( const HepPoint3D& pGlobal,
                                               const HepVector3D& vGlobal,
                                               std::vector<HepPoint3D>& 
                                               points) {

  HepPoint3D pLocal( geometry()->toLocal(pGlobal) );
  HepVector3D vLocal( vGlobal );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (noTicks != 0) {
    for (ISolid::Ticks::iterator tick_it = ticks.begin();
         tick_it != ticks.end();
         ++tick_it) {
      HepPoint3D tempPointGlobal( geometry()->toGlobal( pLocal + (*tick_it)
                                                        * vLocal) );
      points.push_back(tempPointGlobal);
    }
  }
  return noTicks;
}
