
/** @file DeRichSingleSolidRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichSingleSolidRadiator
 *
 *  $Id: DeRichSingleSolidRadiator.cpp,v 1.13 2005-12-14 09:34:52 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHSINGLESOLIDRADIATOR_CPP

// Include files
#include "RichDet/DeRichSingleSolidRadiator.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// MathCore files
#include "Kernel/Transform3DTypes.h"
#include "Kernel/Vector3DTypes.h"

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
const CLID& DeRichSingleSolidRadiator::classID() 
{
  return CLID_DeRichSingleSolidRadiator;
}

StatusCode DeRichSingleSolidRadiator::initialize()
{
  if ( DeRichRadiator::initialize().isFailure() ) return StatusCode::FAILURE;

  MsgStream log( msgSvc(), "DeRichSingleSolidRadiator" );
  log << MSG::DEBUG << "Starting initialisation for DeRichSingleSolidRadiator "
      << name() << endreq;

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

  const Gaudi::XYZPoint zero(0.0, 0.0, 0.0);
  log << MSG::DEBUG << "Found TabProp " << m_refIndex->name() << " type "
      << m_refIndex->type() << endmsg;
  if ( m_rayleigh )
    log << MSG::DEBUG << "Found TabProp " << m_rayleigh->name() << " type "
        << m_rayleigh->type() << endmsg;
  log << MSG::DEBUG <<" Centre:" << geometry()->toGlobal(zero) << endreq;

  return StatusCode::SUCCESS;
}

StatusCode
DeRichSingleSolidRadiator::nextIntersectionPoint( const Gaudi::XYZPoint&  pGlobal,
                                                  const Gaudi::XYZVector& vGlobal,
                                                  Gaudi::XYZPoint&  returnPoint ) const
{

  const Gaudi::XYZPoint pLocal( geometry()->toLocal(pGlobal) );
  Gaudi::XYZVector vLocal( geometry()->matrix()*vGlobal );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

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
DeRichSingleSolidRadiator::intersectionPoints( const Gaudi::XYZPoint&  position,
                                               const Gaudi::XYZVector& direction,
                                               Gaudi::XYZPoint& entryPoint,
                                               Gaudi::XYZPoint& exitPoint ) const
{

  const Gaudi::XYZPoint pLocal( geometry()->toLocal(position) );
  Gaudi::XYZVector vLocal( geometry()->matrix()*direction );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks)  return StatusCode::FAILURE;

  entryPoint = geometry()->toGlobal( pLocal + ticks[0] * vLocal );
  exitPoint  = geometry()->toGlobal( pLocal + ticks[noTicks-1] * vLocal );
  return StatusCode::SUCCESS;

}


//=========================================================================
//
//=========================================================================
unsigned int
DeRichSingleSolidRadiator::intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                               const Gaudi::XYZVector& vGlobal,
                                               std::vector<Gaudi::XYZPoint>&
                                               points) const
{

  const Gaudi::XYZPoint pLocal( geometry()->toLocal(pGlobal) );
  Gaudi::XYZVector vLocal( geometry()->matrix()*vGlobal );

  ISolid::Ticks ticks;
  unsigned int noTicks = m_solid->intersectionTicks(pLocal, vLocal, ticks);

  if (noTicks != 0) {
    for (ISolid::Ticks::iterator tick_it = ticks.begin();
         tick_it != ticks.end();
         ++tick_it) {
      points.push_back( Gaudi::XYZPoint( geometry()->toGlobal( pLocal + (*tick_it) * vLocal) ) );
    }
  }
  return noTicks;
}
