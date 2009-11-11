
/** @file DeRichMultiSolidRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichMultiSolidRadiator
 *
 *  $Id: DeRichMultiSolidRadiator.cpp,v 1.23 2009-11-11 17:27:29 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

// Include files
#include "RichDet/DeRichMultiSolidRadiator.h"

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SystemOfUnits.h"

// MathCore files
#include "GaudiKernel/Vector3DTypes.h"

/// Detector description classes
#include "DetDesc/SolidBoolean.h"
#include "DetDesc/IPVolume.h"

//----------------------------------------------------------------------------

const CLID CLID_DeRichMultiSolidRadiator = 12041;  // User defined

// Standard Constructor
DeRichMultiSolidRadiator::DeRichMultiSolidRadiator(const std::string & name)
  : DeRichRadiator(name) {}

// Standard Destructor
DeRichMultiSolidRadiator::~DeRichMultiSolidRadiator() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichMultiSolidRadiator::classID()
{
  return CLID_DeRichMultiSolidRadiator;
}

StatusCode DeRichMultiSolidRadiator::initialize()
{
  const StatusCode sc = DeRichRadiator::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "Starting initialisation for DeRichMultiSolidRadiator "
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

  return sc;
}

//=========================================================================
// add physical volumes to multi solid radiator
//=========================================================================
StatusCode DeRichMultiSolidRadiator::addVolumes (const ILVolume* lv,
                                                 const std::string volName,
                                                 const Gaudi::Transform3D& toLowerLevel)
{

  // while string volumes also store the total transformation to
  // get to/from the low level volume to the top level volume
  ILVolume::PVolumes::const_iterator pviter;
  for (pviter=lv->pvBegin(); pviter!=lv->pvEnd(); ++pviter)
  {
    if( (*pviter)->name().find(volName) != std::string::npos )
    {
      m_pVolumes.push_back( (*pviter) );
      m_toLowLevel.push_back( toLowerLevel*(*pviter)->matrix() );
      m_toTopLevel.push_back( toLowerLevel.Inverse()*(*pviter)->matrixInv() );
      m_solids.push_back( (*pviter)->lvolume()->solid() );
      debug() << "Storing pvolume " << (*pviter)->name() << endmsg;

      // get the volume number
      const std::string::size_type numPos = (*pviter)->name().find(':');
      if ( numPos == std::string::npos )
      {
        error() << "Cannot find aerogel tile number "
                << (*pviter)->name() << endmsg;
        return StatusCode::FAILURE;
      }

      // get a pointer to DeRichRadiator
      std::string tileNumStr = (*pviter)->name().substr(numPos+1);
      std::string radLoc = DeRichLocations::Aerogel+"T"+tileNumStr+":"+tileNumStr;
      SmartDataPtr<DeRichRadiator> deRad( dataSvc(), radLoc );
      if ( !deRad )
      {
        error() << "Cannot find DeRichRadiator " << radLoc
                << (*pviter)->name() << endmsg;
        return StatusCode::FAILURE;
      }
      debug() << "Loading " << radLoc << endmsg;
      m_radiators.push_back( deRad );
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
  Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

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
  Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*direction );

  ISolid::Ticks ticks;
  Gaudi::XYZPoint localEntryPoint(0.0, 0.0, 1e6);
  Gaudi::XYZPoint localEntryTempPoint;
  Gaudi::XYZPoint localExitPoint(0.0, 0.0, -1e6);
  Gaudi::XYZPoint localExitTempPoint;
  //Gaudi::XYZVector solidLocalVector;
  bool foundTick(false);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid)
  {
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
  Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

  ISolid::Ticks ticks;
  unsigned int noTicks;
  unsigned int totalTicks(0);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid) {
    Gaudi::XYZPoint solidLocalPoint( m_toLowLevel[solid]*pLocal);
    Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    noTicks = m_solids[solid]->intersectionTicks(solidLocalPoint,
                                                 solidLocalVector,
                                                 ticks);
    if (noTicks != 0)
    {
      totalTicks += noTicks;
      for ( ISolid::Ticks::iterator tick_it = ticks.begin();
            tick_it != ticks.end(); ++tick_it)
      {
        //Gaudi::XYZPoint intersect(  );
        points.push_back( geometry()->toGlobal
                          (m_toTopLevel[solid]*(solidLocalPoint+solidLocalVector*(*tick_it))));
      }
    }
  }

  return totalTicks;
}

//=========================================================================
//  intersections
//=========================================================================
unsigned int DeRichMultiSolidRadiator::
intersections( const Gaudi::XYZPoint& pGlobal,
               const Gaudi::XYZVector& vGlobal,
               std::vector<RichRadIntersection>& intersections ) const
{
  const Gaudi::XYZPoint pLocal = geometry()->toLocal(pGlobal);
  const Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

  ISolid::Ticks ticks;
  unsigned int noTicks;
  unsigned int totalTicks(0);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid)
  {
    Gaudi::XYZPoint solidLocalPoint( m_toLowLevel[solid]*pLocal);
    Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    noTicks = m_solids[solid]->intersectionTicks(solidLocalPoint,
                                                 solidLocalVector,
                                                 ticks);
    if (noTicks != 0)
    {
      totalTicks += noTicks;
      for ( unsigned int tick = 0; tick < noTicks; tick += 2 )
      {
        intersections.push_back(RichRadIntersection
                                (geometry()->toGlobal(m_toTopLevel[solid]*
                                                      (solidLocalPoint+solidLocalVector*ticks[tick])),
                                 vGlobal,
                                 geometry()->toGlobal(m_toTopLevel[solid]*
                                                      (solidLocalPoint+solidLocalVector*ticks[tick+1])),
                                 vGlobal,
                                 m_radiators[solid]) );
      }
    }
  }
  return totalTicks/2;
}

//=========================================================================
// Refractive Index
//=========================================================================
double
DeRichMultiSolidRadiator::refractiveIndex( const double energy, bool hlt ) const
{
  double refIn(0);
  // Loop over all tiles and form an average
  for ( std::vector<DeRichRadiator*>::const_iterator iRad = m_radiators.begin();
        iRad != m_radiators.end(); ++iRad )
  {
    // Should this be a weighted average of some form ?
    refIn += (*((*iRad)->refIndex(hlt)))[energy*Gaudi::Units::eV];
  }
  return ( m_radiators.empty() ? refIn : refIn/m_radiators.size() );
}
