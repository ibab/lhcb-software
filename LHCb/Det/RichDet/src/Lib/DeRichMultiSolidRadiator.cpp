/** @file DeRichMultiSolidRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichMultiSolidRadiator
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

// Include files
#include "RichDet/DeRichMultiSolidRadiator.h"

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// MathCore files
#include "GaudiKernel/Vector3DTypes.h"

/// Detector description classes
#include "DetDesc/SolidBoolean.h"
#include "DetDesc/IPVolume.h"

//----------------------------------------------------------------------------

const CLID CLID_DeRichMultiSolidRadiator = 12041;  // User defined

// Standard Constructor
DeRichMultiSolidRadiator::DeRichMultiSolidRadiator(const std::string & name)
  : DeRichRadiator ( name ),
    m_firstUpdate  ( true ) 
{ 
  // reserve size in vectors for max aerogel (sub)tiles
  m_solids.reserve(300);
  m_pVolumes.reserve(300);
  m_toTopLevel.reserve(300);
  m_toLowLevel.reserve(300);
  m_radiators.reserve(300);
}

// Standard Destructor
DeRichMultiSolidRadiator::~DeRichMultiSolidRadiator() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichMultiSolidRadiator::classID()
{
  return CLID_DeRichMultiSolidRadiator;
}

StatusCode DeRichMultiSolidRadiator::initialize()
{
  StatusCode sc = DeRichRadiator::initialize();
  if ( sc.isFailure() ) return sc;

  // Declare UMS dependencies
  updMgrSvc()->registerCondition( this, geometry(),
                                  &DeRichMultiSolidRadiator::geometryUpdate );

  // Trigger first update
  sc = updMgrSvc()->update(this);
  if ( sc.isFailure() ) { fatal() << "UMS updates failed" << endmsg; }

  // return
  return sc;
}

//=========================================================================
// UMS update on geometry changes
//=========================================================================
StatusCode DeRichMultiSolidRadiator::geometryUpdate()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Geometry update" << endmsg;

  // clear up first
  m_solids.clear();
  m_pVolumes.clear();
  m_toTopLevel.clear();
  m_toLowLevel.clear();
  m_radiators.clear();
  m_fullTiles.clear();

  // multi solid specific initialisation
  const ILVolume* topLV = geometry()->lvolume();
  if ( !topLV )
  {
    fatal() << "Cannot access ILVolume" << endmsg;
    return StatusCode::FAILURE;
  }

  // look for Aerogel container left
  const IPVolume* contR = topLV->pvolume("pvRich1AerogelContainerRight:0");
  if ( contR )
  {
    if ( !addVolumes(contR->lvolume(), "AerogelT", contR->matrix(), 0 ) )
      return StatusCode::FAILURE;
  }
  
  // look for Aerogel container right
  const IPVolume* contL = topLV->pvolume("pvRich1AerogelContainerLeft:1");
  if ( contL ) 
  {
    if ( !addVolumes(contL->lvolume(), "AerogelT", contL->matrix(), 1 ) )
      return StatusCode::FAILURE;
  }
  
  // old geometry with aerogel quarters
  if ( !contR && !contL )
  {
    if ( !addVolumes(topLV, "AerogelQuad", Gaudi::Transform3D(), 0 ) )
      return StatusCode::FAILURE;
  }

  m_firstUpdate = false;

  return StatusCode::SUCCESS;
}

//=========================================================================
// add physical volumes to multi solid radiator
//=========================================================================
StatusCode
DeRichMultiSolidRadiator::addVolumes(const ILVolume* lv,
                                     const std::string& volName,
                                     const Gaudi::Transform3D& toLowerLevel, 
                                     const int VolIndex )
{
  StatusCode sc =  StatusCode::SUCCESS;

  // First determine if the subtiles exist or not. Depending upon this activate adding the
  // full tile volumes or sub tile volumes. For one may use any full tile and its subtile.
  // so here full tile 2 for right container and tile 10 for the left container are used.

  std::string aTilePvName = "pvRich1AerogelTile:2";
  if ( 1 == VolIndex ) aTilePvName = "pvRich1AerogelTile:10" ;

  const IPVolume* aFullTilePv = lv->pvolume(aTilePvName);
  if ( aFullTilePv )
  {

    const int aNumSubTile = (int)( aFullTilePv->lvolume()->noPVolumes() );
    if      ( 0 == aNumSubTile )
    {
      // old geometry with no sub tiles. use the full tiles as before
      sc = addFullTileVolumes( lv, volName, toLowerLevel );
    }
    else if ( 0 <  aNumSubTile )
    {
      // new geometry with sub tiles.
      sc = addSubTileVolumes( lv, volName, toLowerLevel );
    }
    else
    {
      error() << "InAdmissible number of subtiles in Aerogel tile 2 = " << aNumSubTile << endreq;
      sc = StatusCode::FAILURE;
    }

  }
  else
  {
    error() << "No Aerogel full tile from DeRichMultiSolidRadiator" << endreq;
    sc = StatusCode::FAILURE;
  }

  return sc;
}

//=========================================================================
// add sub tiles det elements
//=========================================================================

StatusCode
DeRichMultiSolidRadiator::addSubTileVolumes ( const ILVolume* lv,
                                              const std::string& volName,
                                              const Gaudi::Transform3D& toLowerLevel )
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Adding sub tile volumes" << endmsg;

  const std::string& aAgelLocation = DeRichLocations::Aerogel;
  std::string aAgelSubTileMasterLocation = aAgelLocation.substr(0,aAgelLocation.size()-7);

  for ( ILVolume::PVolumes::const_iterator pviter = lv->pvBegin(); 
        pviter != lv->pvEnd(); ++pviter )
  {

    if ( (*pviter)->name().find(volName) != std::string::npos )
    {
      const Gaudi::Transform3D & tileTrans    = (*pviter)->matrix();
      const Gaudi::Transform3D & tileTransInv = (*pviter)->matrixInv();
      const ILVolume* tileLv = (*pviter)->lvolume();
      for ( ILVolume::PVolumes::const_iterator stpviter = tileLv->pvBegin(); 
            stpviter != tileLv->pvEnd(); ++stpviter ) 
      {
        m_pVolumes.push_back( (*stpviter) );
        m_toLowLevel.push_back( ( (*stpviter)->matrix()) * tileTrans * toLowerLevel );
        m_toTopLevel.push_back( (toLowerLevel.Inverse()) * tileTransInv * ((*stpviter)->matrixInv()));
        m_solids.push_back( (*stpviter)->lvolume()->solid() );
        const std::string::size_type pvolNameColPos = (*stpviter)->name().find(":");
        const std::string pvolNameSuffixA  = (*stpviter)->name().substr(pvolNameColPos-4,4);
        const std::string pvolNameSuffixB  = (*stpviter)->name().substr(pvolNameColPos);
        const std::string curSubTileDeName = aAgelSubTileMasterLocation + "Rich1AerogelSubTileDeT"+pvolNameSuffixA+pvolNameSuffixB;
        SmartDataPtr<DeRichRadiator> deRad( dataSvc(), curSubTileDeName );
        if ( !deRad ) 
        {
          error() << "Cannot find DeRichRadiator for " << curSubTileDeName <<"  "
                  << (*stpviter)->name() << endmsg;
          return StatusCode::FAILURE;
        }
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Saving sub-tile " << curSubTileDeName << " to radiator list" << endmsg;
        m_radiators.push_back( deRad );

      } // end loop over sub tiles

      // Save list of full tiles
      // get the volume number
      const std::string::size_type numPos = (*pviter)->name().find(':');
      if ( numPos == std::string::npos )
      {
        error() << "Cannot find aerogel tile number " << (*pviter)->name() << endmsg;
        return StatusCode::FAILURE;
      }
      const std::string tileNumStr = (*pviter)->name().substr(numPos+1);
      const std::string radLoc = DeRichLocations::Aerogel+"T"+tileNumStr+":"+tileNumStr;
      SmartDataPtr<DeRichRadiator> deFullRad( dataSvc(), radLoc );
      if ( !deFullRad )
      {
        error() << "Cannot find DeRichRadiator " << radLoc << " "
                << (*pviter)->name() << endmsg;
        return StatusCode::FAILURE;
      }
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Loading " << radLoc << " " << tileNumStr << endmsg;
      m_fullTiles.push_back( deFullRad );

    } // end if , checking for Agel full tile

  } // end loop over container daughters

  return StatusCode::SUCCESS;
}

//=========================================================================
// add full tiles det elements
//=========================================================================
StatusCode
DeRichMultiSolidRadiator::addFullTileVolumes ( const ILVolume* lv,
                                               const std::string& volName,
                                               const Gaudi::Transform3D& toLowerLevel )
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Adding full tile volumes" << endmsg;

  // while string volumes also store the total transformation to
  // get to/from the low level volume to the top level volume
  for ( ILVolume::PVolumes::const_iterator pviter = lv->pvBegin();
        pviter != lv->pvEnd(); ++pviter )
  {
    if( (*pviter)->name().find(volName) != std::string::npos )
    {
      m_pVolumes.push_back( (*pviter) );
      m_toLowLevel.push_back( ((*pviter)->matrix()) * (toLowerLevel) );
      m_toTopLevel.push_back( (toLowerLevel.Inverse()) * ((*pviter)->matrixInv()) );
      m_solids.push_back( (*pviter)->lvolume()->solid() );
      if ( msgLevel(MSG::DEBUG) )
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
      const std::string tileNumStr = (*pviter)->name().substr(numPos+1);
      const std::string radLoc = DeRichLocations::Aerogel+"T"+tileNumStr+":"+tileNumStr;
      SmartDataPtr<DeRichRadiator> deRad( dataSvc(), radLoc );
      if ( !deRad )
      {
        error() << "Cannot find DeRichRadiator " << radLoc << " "
                << (*pviter)->name() << endmsg;
        return StatusCode::FAILURE;
      }
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Loading " << radLoc << " " << tileNumStr << endmsg;
      m_radiators.push_back( deRad );
      m_fullTiles.push_back( deRad );

      // Declare UMS dependencies
      // CRJ Probably not needed ...
      // if ( m_firstUpdate )
      //  updMgrSvc()->registerCondition( this, deRad->geometry(),
      //                                  &DeRichMultiSolidRadiator::geometryUpdate );

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

  const Gaudi::XYZPoint  pLocal( geometry()->toLocal(pGlobal) );
  const Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

  ISolid::Ticks ticks;
  Gaudi::XYZPoint localNextPoint(0.0, 0.0, 1e6);
  Gaudi::XYZPoint localNextTempPoint;
  //Gaudi::XYZVector solidLocalVector;
  bool foundTick(false);

  for ( unsigned int solid=0; solid<m_solids.size(); ++solid )
  {
    const Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    const Gaudi::XYZPoint  solidLocalPoint( m_toLowLevel[solid]*pLocal );

    if ( m_solids[solid]->
         intersectionTicks(solidLocalPoint,solidLocalVector,ticks) )
    {
      const Gaudi::XYZPoint localNext( solidLocalPoint+solidLocalVector*ticks[0] );
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

  const Gaudi::XYZPoint  pLocal( geometry()->toLocal(position) );
  const Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*direction );

  ISolid::Ticks ticks;
  Gaudi::XYZPoint localEntryPoint(0.0, 0.0, 1e6);
  Gaudi::XYZPoint localEntryTempPoint;
  Gaudi::XYZPoint localExitPoint(0.0, 0.0, -1e6);
  Gaudi::XYZPoint localExitTempPoint;

  bool foundTick(false);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid)
  {
    const Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    const Gaudi::XYZPoint solidLocalPoint( m_toLowLevel[solid]*pLocal );

    if ( m_solids[solid]->
         intersectionTicks(solidLocalPoint,solidLocalVector,ticks) )
    {
      const Gaudi::XYZPoint localEntryStep1( solidLocalPoint+solidLocalVector*ticks[0] );
      localEntryTempPoint = m_toTopLevel[solid]*localEntryStep1;
      const Gaudi::XYZPoint localExitStep1 ( solidLocalPoint+solidLocalVector*ticks[ticks.size()-1]);
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
DeRichMultiSolidRadiator::
intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                    const Gaudi::XYZVector& vGlobal,
                    std::vector<Gaudi::XYZPoint>& points ) const
{

  const Gaudi::XYZPoint pLocal = geometry()->toLocal(pGlobal);
  const Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

  ISolid::Ticks ticks;
  unsigned int noTicks;
  unsigned int totalTicks(0);

  points.reserve( m_solids.size() * 2 );

  for ( unsigned int solid = 0; solid < m_solids.size(); ++solid )
  {

    const Gaudi::XYZPoint solidLocalPoint( m_toLowLevel[solid]*pLocal);
    const Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    noTicks = m_solids[solid]->intersectionTicks(solidLocalPoint,
                                                 solidLocalVector,
                                                 ticks);
    if ( 0 != noTicks )
    {
      totalTicks += noTicks;
      for ( ISolid::Ticks::iterator tick_it = ticks.begin();
            tick_it != ticks.end(); ++tick_it )
      {
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
               Rich::RadIntersection::Vector& intersections ) const
{
  const Gaudi::XYZPoint pLocal = geometry()->toLocal(pGlobal);
  const Gaudi::XYZVector vLocal( geometry()->toLocalMatrix()*vGlobal );

  ISolid::Ticks ticks;
  unsigned int noTicks;
  unsigned int totalTicks(0);

  for (unsigned int solid=0; solid<m_solids.size(); ++solid)
  {
    const Gaudi::XYZPoint solidLocalPoint( m_toLowLevel[solid]*pLocal);
    const Gaudi::XYZVector solidLocalVector( m_toLowLevel[solid]*vLocal );
    noTicks = m_solids[solid]->intersectionTicks(solidLocalPoint,
                                                 solidLocalVector,
                                                 ticks);
    if ( 0 != noTicks )
    {
      totalTicks += noTicks;
      for ( unsigned int tick = 0; tick < noTicks; tick += 2 )
      {
        intersections.push_back(Rich::RadIntersection
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
DeRichMultiSolidRadiator::refractiveIndex( const double energy,
                                           const bool hlt ) const
{
  double refIn(0);
  // Loop over all tiles and form an average
  for ( DeRichRadiator::Vector::const_iterator iRad = radiators().begin();
        iRad != radiators().end(); ++iRad )
  {
    // Should this be a weighted average of some form ?
    refIn += (*((*iRad)->refIndex(hlt)))[energy*Gaudi::Units::eV];
  }
  return ( radiators().empty() ? refIn : refIn/radiators().size() );
}
