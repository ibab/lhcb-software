
/** @file DeRichMultiSolidRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichMultiSolidRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichMultiSolidRadiator.cpp,v 1.5 2004-10-20 16:16:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2004/09/01 15:20:19  papanest
 *  added functions for TabProps
 *
 *  Revision 1.3  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHSINGLESOLIDRADIATOR_CPP

// Include files
#include "RichDet/DeRichMultiSolidRadiator.h"

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

const CLID& CLID_DeRichMultiSolidRadiator = 12041;  // User defined

// Standard Constructor
DeRichMultiSolidRadiator::DeRichMultiSolidRadiator() {}

// Standard Destructor
DeRichMultiSolidRadiator::~DeRichMultiSolidRadiator() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichMultiSolidRadiator::classID() {
  return CLID_DeRichMultiSolidRadiator;
}

StatusCode DeRichMultiSolidRadiator::initialize() {

  MsgStream log(msgSvc(), "DeRichMultiSolidRadiator" );
  log << MSG::DEBUG <<"Starting initialisation for DeRichMultiSolidRadiator "
      << name() << endreq;

  StatusCode fail = StatusCode::FAILURE ;

  if ( DeRichRadiator::initialize().isFailure() ) return fail;

  // multi solid specific initialisation
  const ILVolume* lv = geometry()->lvolume();
  ILVolume::PVolumes::const_iterator pviter;
  for (pviter=lv->pvBegin(); pviter!=lv->pvEnd(); ++pviter) {
    if( (*pviter)->name().find("AerogelQuad") != std::string::npos ) {
      m_pVolumes.push_back( (*pviter) );
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

  if (0 == m_refIndices.size() ) {
    log << MSG::ERROR << "Radiator " << name() << " without refractive index"
        << endmsg;
    return fail;
  }
  // for the time being use the refIndex of the first volume for the whole 
  // radiator
  m_refIndex = m_refIndices[0];

  if ( m_rayleighVector.size() != 0 )
    m_rayleigh = m_rayleighVector[0];
  
  log << MSG::DEBUG <<"Finished initialisation of " << name() << endreq;

  return StatusCode::SUCCESS;
}


//=========================================================================
//  returns the next intersection point
//=========================================================================
StatusCode
DeRichMultiSolidRadiator::nextIntersectionPoint( const HepPoint3D&  pGlobal,
                                                 const HepVector3D& vGlobal,
                                                 HepPoint3D&  returnPoint ) const
{

  HepPoint3D pLocal( geometry()->toLocal(pGlobal) );
  HepVector3D vLocal( vGlobal );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  HepPoint3D localNextPoint(0.0, 0.0, 1e6);
  HepPoint3D localNextTempPoint;
  HepVector3D solidLocalVector;
  bool foundTick(false);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid) {
    solidLocalVector = vLocal;
    if ( m_solids[solid]->
         intersectionTicks(m_pVolumes[solid]->toLocal(pLocal),
                           solidLocalVector.transform(m_pVolumes[solid]->
                                                      matrix()),
                           ticks) ) {
      localNextTempPoint = m_pVolumes[solid]->
        toMother(m_pVolumes[solid]->toLocal(pLocal)+solidLocalVector*ticks[0]);
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
DeRichMultiSolidRadiator::intersectionPoints( const HepPoint3D&  position,
                                              const HepVector3D& direction,
                                              HepPoint3D& entryPoint,
                                              HepPoint3D& exitPoint ) const 
{

  HepPoint3D pLocal( geometry()->toLocal(position) );
  HepVector3D vLocal( direction );
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  HepPoint3D localEntryPoint(0.0, 0.0, 1e6);
  HepPoint3D localEntryTempPoint;
  HepPoint3D localExitPoint(0.0, 0.0, -1e6);
  HepPoint3D localExitTempPoint;
  HepVector3D solidLocalVector;
  bool foundTick(false);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid) {
    solidLocalVector = vLocal;
    if ( m_solids[solid]->
         intersectionTicks(m_pVolumes[solid]->toLocal(pLocal),
                           solidLocalVector.transform(m_pVolumes[solid]->
                                                      matrix()),
                           ticks) ) {
      localEntryTempPoint = m_pVolumes[solid]->
        toMother(m_pVolumes[solid]->toLocal(pLocal)+solidLocalVector*ticks[0]);
      localExitTempPoint = m_pVolumes[solid]->
        toMother(m_pVolumes[solid]->toLocal(pLocal)+solidLocalVector*
                 ticks[ticks.size()-1]);

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
DeRichMultiSolidRadiator::intersectionPoints( const HepPoint3D& pGlobal,
                                              const HepVector3D& vGlobal,
                                              std::vector<HepPoint3D>&
                                              points) const 
{

  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  //HepPoint3D localNextPoint(0.0, 0.0, 1e6);
  //HepPoint3D localNextTempPoint;
  HepVector3D solidLocalVector;
  unsigned int noTicks;
  unsigned int totalTicks(0);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid) {
    solidLocalVector = vLocal;
    noTicks = m_solids[solid]->
      intersectionTicks(m_pVolumes[solid]->toLocal(pLocal),
                        solidLocalVector.transform(m_pVolumes[solid]->matrix())
                        ,ticks);
    if (noTicks != 0) {
      totalTicks += noTicks;
      for (ISolid::Ticks::iterator tick_it = ticks.begin();
           tick_it != ticks.end();
           ++tick_it) {
        // the following line is a little complecated. The physical volume
        // is used to calculate the point in the system of the solid, and then
        // in the system of the mother, before going global.
        points.push_back
          (geometry()->toGlobal
           (m_pVolumes[solid]->toMother
            (m_pVolumes[solid]->toLocal(pLocal) + solidLocalVector.transform
             (m_pVolumes[solid]->matrix()) * (*tick_it) )));
      }
    }
  }

  return totalTicks;
}
