
/** @file DeRichMultiSolidRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichMultiSolidRadiator
 *
 *  $Id: DeRichMultiSolidRadiator.cpp,v 1.11 2005-12-14 09:34:52 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHSINGLESOLIDRADIATOR_CPP

// Include files
#include "RichDet/DeRichMultiSolidRadiator.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// MathCore files
#include "Kernel/Vector3DTypes.h"

/// Detector description classes
#include "DetDesc/SolidBoolean.h"
#include "DetDesc/SolidTrd.h"

//----------------------------------------------------------------------------

const CLID& CLID_DeRichMultiSolidRadiator = 12041;  // User defined

// Standard Constructor
DeRichMultiSolidRadiator::DeRichMultiSolidRadiator() {}

// Standard Destructor
DeRichMultiSolidRadiator::~DeRichMultiSolidRadiator() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichMultiSolidRadiator::classID() 
{
  return CLID_DeRichMultiSolidRadiator;
}

StatusCode DeRichMultiSolidRadiator::initialize()
{
  if ( DeRichRadiator::initialize().isFailure() ) return StatusCode::FAILURE;
 
  MsgStream log( msgSvc(), "DeRichMultiSolidRadiator" );
  log << MSG::DEBUG <<"Starting initialisation for DeRichMultiSolidRadiator "
      << name() << endmsg;

  // multi solid specific initialisation
  const ILVolume* topLV = geometry()->lvolume();

  // look for Aerogel container left
  const IPVolume* contR = topLV->pvolume("pvRich1AerogelContainerRight:0");
  if ( contR )
    if ( !addVolumes(contR->lvolume(), "AerogelT", contR->matrix() ) ) return StatusCode::FAILURE;
  
  // look for Aerogel container right
  const IPVolume* contL = topLV->pvolume("pvRich1AerogelContainerLeft:1");
  if ( contL ) {
    if ( !addVolumes(contL->lvolume(), "AerogelT", contL->matrix() ) ) return StatusCode::FAILURE;
  }
  // old geometry with aerogel quarters
  else 
    if ( !addVolumes(topLV, "AerogelQuad", Gaudi::Transform3D() ) ) return StatusCode::FAILURE;
  
  if ( m_refIndices.empty() ) 
  {
    log << MSG::ERROR << "Radiator " << name() << " without refractive index"
        << endmsg;
    return StatusCode::FAILURE;
  }
  // for the time being use the refIndex of the first volume for the whole 
  // radiator
  m_refIndex = m_refIndices[0];

  if ( !m_rayleighVector.empty() ) m_rayleigh = m_rayleighVector[0];
  
  return StatusCode::SUCCESS;
}


//=========================================================================
// add physical volumes to multi solid radiator  
//=========================================================================
StatusCode DeRichMultiSolidRadiator::addVolumes (const ILVolume* lv, 
                                                 const std::string volName,
                                                 const Gaudi::Transform3D& toLowerLevel) {

  MsgStream log( msgSvc(), "DeRichMultiSolidRadiator" );

  // while string volumes also store the total transformation to 
  // get to/from the low level volume to the top level volume
  ILVolume::PVolumes::const_iterator pviter;
  for (pviter=lv->pvBegin(); pviter!=lv->pvEnd(); ++pviter) {
    if( (*pviter)->name().find(volName) != std::string::npos ) {
      m_pVolumes.push_back( (*pviter) );
      m_toLowLevel.push_back( toLowerLevel*(*pviter)->matrix() );
      m_toTopLevel.push_back( toLowerLevel.Inverse()*(*pviter)->matrixInv() );
      m_solids.push_back( (*pviter)->lvolume()->solid() );
      log << MSG::DEBUG << "Storing pvolume " << (*pviter)->name();

      const Material::Tables& myTabProp = (*pviter)->lvolume()->material()->
        tabulatedProperties();
      Material::Tables::const_iterator tabIter;
      for (tabIter=myTabProp.begin(); tabIter!=myTabProp.end(); ++tabIter) {
        if( (*tabIter) ){
          if ( (*tabIter)->type() == "RINDEX" ) {
            log << MSG::DEBUG << " with property " << (*tabIter)->name()
                << endreq;
            m_refIndices.push_back( (*tabIter) );
          }
          if ( (*tabIter)->type() == "RAYLEIGH" ) {
            log << MSG::DEBUG << " and with property " << (*tabIter)->name()
                << endreq;
            m_rayleighVector.push_back( (*tabIter) );
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  returns the next intersection point
//=========================================================================
StatusCode
DeRichMultiSolidRadiator::nextIntersectionPoint( const Gaudi::XYZPoint&  pGlobal,
                                                 const Gaudi::XYZVector& vGlobal,
                                                 Gaudi::XYZPoint&  returnPoint ) const
{

  const Gaudi::XYZPoint pLocal( geometry()->toLocal(pGlobal) );
  Gaudi::XYZVector vLocal( geometry()->matrix()*vGlobal );

  ISolid::Ticks ticks;
  Gaudi::XYZPoint localNextPoint(0.0, 0.0, 1e6);
  Gaudi::XYZPoint localNextTempPoint;
  //Gaudi::XYZVector solidLocalVector;
  bool foundTick(false);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid) {
    Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    Gaudi::XYZPoint solidLocalPoint( m_toLowLevel[solid]*pLocal );

    if ( m_solids[solid]->
         intersectionTicks(solidLocalPoint,solidLocalVector,ticks) ) {
      Gaudi::XYZPoint localNext( solidLocalPoint+solidLocalVector*ticks[0] );
      localNextTempPoint = m_toTopLevel[solid]*localNext;
      if ( localNextTempPoint.z() < localNextPoint.z() )
        localNextPoint = localNextTempPoint;
      foundTick = true;
    }
  }

  if (!foundTick)  return StatusCode::FAILURE;

  returnPoint = geometry()->toGlobal(localNextPoint);
  return StatusCode::SUCCESS;

}

//=========================================================================
// return the entry and exit points of radiator
//=========================================================================
StatusCode
DeRichMultiSolidRadiator::intersectionPoints( const Gaudi::XYZPoint&  position,
                                              const Gaudi::XYZVector& direction,
                                              Gaudi::XYZPoint& entryPoint,
                                              Gaudi::XYZPoint& exitPoint ) const 
{

  const Gaudi::XYZPoint pLocal( geometry()->toLocal(position) );
  Gaudi::XYZVector vLocal( geometry()->matrix()*direction );

  ISolid::Ticks ticks;
  Gaudi::XYZPoint localEntryPoint(0.0, 0.0, 1e6);
  Gaudi::XYZPoint localEntryTempPoint;
  Gaudi::XYZPoint localExitPoint(0.0, 0.0, -1e6);
  Gaudi::XYZPoint localExitTempPoint;
  //Gaudi::XYZVector solidLocalVector;
  bool foundTick(false);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid) {
    Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    Gaudi::XYZPoint solidLocalPoint( m_toLowLevel[solid]*pLocal );
    
    if ( m_solids[solid]->
         intersectionTicks(solidLocalPoint,solidLocalVector,ticks) ) {
      Gaudi::XYZPoint localEntryStep1( solidLocalPoint+solidLocalVector*ticks[0] );
      localEntryTempPoint = m_toTopLevel[solid]*localEntryStep1;
      Gaudi::XYZPoint localExitStep1 ( solidLocalPoint+solidLocalVector*ticks[ticks.size()-1]);
      localExitTempPoint = m_toTopLevel[solid]*localExitStep1;

      if ( localEntryTempPoint.z() < localEntryPoint.z() )
        localEntryPoint = localEntryTempPoint;
      if ( localExitTempPoint.z() > localExitPoint.z() )
        localExitPoint = localExitTempPoint;
      foundTick = true;
    }
  }


  if (!foundTick)  return StatusCode::FAILURE;

  entryPoint = geometry()->toGlobal( localEntryPoint );
  exitPoint = geometry()->toGlobal( localExitPoint );
  return StatusCode::SUCCESS;

}


//=========================================================================
//  return a vector with all intersection points with solid
//=========================================================================
unsigned int
DeRichMultiSolidRadiator::intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                              const Gaudi::XYZVector& vGlobal,
                                              std::vector<Gaudi::XYZPoint>&
                                              points) const 
{

  const Gaudi::XYZPoint pLocal = geometry()->toLocal(pGlobal);
  Gaudi::XYZVector vLocal( geometry()->matrix()*vGlobal );

  ISolid::Ticks ticks;
  unsigned int noTicks;
  unsigned int totalTicks(0);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid) {
    Gaudi::XYZPoint solidLocalPoint( m_toLowLevel[solid]*pLocal);
    Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    noTicks = m_solids[solid]->intersectionTicks(solidLocalPoint,
                                                 solidLocalVector,
                                                 ticks);
    if (noTicks != 0) {
      totalTicks += noTicks;
      for (ISolid::Ticks::iterator tick_it = ticks.begin();
           tick_it != ticks.end();
           ++tick_it) {
        //Gaudi::XYZPoint intersect(  );
        points.push_back( geometry()->toGlobal
                          (m_toTopLevel[solid]*(solidLocalPoint+solidLocalVector*(*tick_it))));
      }
    }
  }

  return totalTicks;
}
