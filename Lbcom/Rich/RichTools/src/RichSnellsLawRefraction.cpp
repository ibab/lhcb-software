
//-----------------------------------------------------------------------------
/** @file RichSnellsLawRefraction.cpp
 *
 * Implementation file for class : Rich::SnellsLawRefraction
 *
 * @author Antonis Papanestis
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2003-10-28
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichSnellsLawRefraction.h"

using namespace Rich;

DECLARE_TOOL_FACTORY( SnellsLawRefraction );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SnellsLawRefraction::SnellsLawRefraction( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : ToolBase       ( type, name, parent ),
    m_radiatorTool ( NULL ),
    m_refIndex     ( NULL ),
    m_radiators    ( Rich::NRadiatorTypes ),
    m_hltMode      ( false                   )
{
  // interface
  declareInterface<ISnellsLawRefraction>(this);
}

//=============================================================================
// Destructor
//=============================================================================
SnellsLawRefraction::~SnellsLawRefraction() {}
//=============================================================================

//=============================================================================
// Initialise
//=============================================================================
StatusCode SnellsLawRefraction::initialize()
{
  // Initialise base class
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // HLT mode or not ?
  m_hltMode = contextContains("HLT");

  // get tools
  acquireTool( "RichRadiatorTool", m_radiatorTool );
  acquireTool( "RichRefractiveIndex", m_refIndex );

  // get three points in exit plane
  RichRadIntersection::Vector intersections;
  Gaudi::XYZPoint p1(100,100,0), p2(100,-100,0), p3(-100,-100,0);
  const Gaudi::XYZVector v(0,0,1);
  m_radiatorTool->intersections( p1, v, Rich::Aerogel, intersections );
  if ( intersections.empty() ) return Error( "No intersections found for p1" );
  p1 = intersections.back().exitPoint();
  m_minZaero = p1.z();
  intersections.clear();
  m_radiatorTool->intersections( p2, v, Rich::Aerogel, intersections );
  if ( intersections.empty() ) return Error( "No intersections found for p2" );
  p2 = intersections.back().exitPoint();
  if ( p2.z() < m_minZaero ) m_minZaero = p2.z();
  intersections.clear();
  m_radiatorTool->intersections( p3, v, Rich::Aerogel, intersections );
  if ( intersections.empty() ) return Error( "No intersections found for p3" );
  p3 = intersections.back().exitPoint();
  if ( p3.z() < m_minZaero ) m_minZaero = p3.z();

  // make a plane using these points
  m_aeroExitPlane = Gaudi::Plane3D(p1,p2,p3);

  // get the normal to the plane
  m_aeroNormVect = m_aeroExitPlane.Normal();

  // Load the radiator detector elements
  m_radiators[Rich::Aerogel]  = getDet<DeRichRadiator>( DeRichLocations::Aerogel  );
  m_radiators[Rich::Rich1Gas] = getDet<DeRichRadiator>( DeRichLocations::Rich1Gas );
  m_radiators[Rich::Rich2Gas] = getDet<DeRichRadiator>( DeRichLocations::Rich2Gas );

  return sc;
}

void SnellsLawRefraction::aerogelToGas( Gaudi::XYZPoint & startPoint,
                                        Gaudi::XYZVector & dir,
                                        const LHCb::RichTrackSegment& trSeg ) const
{
  // check track segment is for aerogel ...
  if ( trSeg.radiator() != Rich::Aerogel )
  {
    Exception( "Track segment is not for aerogel !!"  );
  }

  // photon energy
  const double photonEnergy = trSeg.avPhotonEnergy();
  if ( photonEnergy > 0 )
  {
    if ( startPoint.z() < m_minZaero )
    {

      // get the aerogel ref index for the track segment intersections
      const double refAero = m_refIndex->refractiveIndex( trSeg.radIntersections(), 
                                                          photonEnergy );

      // do the correction
      _aerogelToGas( startPoint, dir, photonEnergy, refAero );

    } // start point OK

  }
  else { Warning( "aerogelToGas:: photonEnergy = 0" ).ignore(); }

}

void SnellsLawRefraction::aerogelToGas( Gaudi::XYZPoint & startPoint,
                                        Gaudi::XYZVector & dir,
                                        const double photonEnergy ) const
{
  if ( photonEnergy > 0 )
  {
    if ( startPoint.z() < m_minZaero )
    {

      // get the average aerogel ref index
      Warning( "Approximate Aerogel refractive index used. Will be wrong..." ).ignore();
      const double refAero = m_radiators[Rich::Aerogel]->refractiveIndex( photonEnergy,
                                                                          m_hltMode );

      // do the correction
      _aerogelToGas( startPoint, dir, photonEnergy, refAero );

    }
  }
  else { Warning( "aerogelToGas:: photonEnergy = 0" ).ignore(); }
}

void SnellsLawRefraction::_aerogelToGas( Gaudi::XYZPoint & startPoint,
                                         Gaudi::XYZVector & dir,
                                         const double photonEnergy,
                                         const double refAero ) const
{
  // Rich1 gas index
  const double refrich1Gas = m_radiators[Rich::Rich1Gas]->refractiveIndex( photonEnergy, m_hltMode );

  // ratio
  const double Rratio      = refAero/refrich1Gas;

  // normalise the direction vector
  dir = dir.Unit();

  // the dot product between the plane normal and the direction
  const double cosT1 = dir.Dot(m_aeroNormVect);

  // update the point extrapolating the photon dir inside the aerogel to exit plane
  const double distance = -(m_aeroExitPlane.Distance(startPoint))/cosT1;
  startPoint += distance*dir;

  // Update the direction due to refraction on exit
  const double cosT2 = std::sqrt( 1.0 - Rratio*Rratio*( 1.0 - cosT1*cosT1 ) );
  if ( cosT1<0 ) { dir = dir*Rratio - m_aeroNormVect*(cosT2+(Rratio*cosT1)); }
  else           { dir = dir*Rratio + m_aeroNormVect*(cosT2-(Rratio*cosT1)); }
}

void SnellsLawRefraction::gasToAerogel( Gaudi::XYZVector & dir,
                                        const LHCb::RichTrackSegment& trSeg ) const
{
  // check track segment is for aerogel ...
  if ( trSeg.radiator() != Rich::Aerogel ) 
  {
    Exception( "Track segment is not for aerogel !!" );
  }

  // photon energy
  const double photonEnergy = trSeg.avPhotonEnergy();
  if ( photonEnergy > 0 )
  {

    // get the aerogel ref index for the track segment intersections
    const double refAero = m_refIndex->refractiveIndex( trSeg.radIntersections(), 
                                                        photonEnergy );

    // do the correction
    _gasToAerogel( dir, photonEnergy, refAero );

  }
  else { Warning( "gasToAerogel:: photonEnergy = 0" ).ignore(); }

}

void SnellsLawRefraction::gasToAerogel( Gaudi::XYZVector & dir,
                                        const double photonEnergy ) const
{

  if ( photonEnergy > 0 )
  {

    // get the average refractive indices for aerogel
    Warning( "Approximate Aerogel refractive index used. Will be wrong..." ).ignore();
    const double refAero = m_radiators[Rich::Aerogel]->refractiveIndex( photonEnergy,m_hltMode );

    // do the correction
    _gasToAerogel( dir, photonEnergy, refAero );

  }
  else { Warning( "gasToAerogel:: photonEnergy = 0" ).ignore(); }

}

void SnellsLawRefraction::_gasToAerogel( Gaudi::XYZVector & dir,
                                         const double photonEnergy,
                                         const double refAero ) const
{
  // Rich1 Gas ref index
  const double refrich1Gas = m_radiators[Rich::Rich1Gas]->refractiveIndex( photonEnergy,m_hltMode );

  // ref index ratio
  const double RratioSq    = (refAero*refAero)/(refrich1Gas*refrich1Gas);

  // normalise the direction vector
  dir = dir.Unit();

  // Apply Snells law and update vector
  Gaudi::XYZVector newDir( 0, 0, std::sqrt(1.-(1.-dir.Z()*dir.Z())/RratioSq) );
  const double R = dir.Y()/dir.X();
  newDir.SetX( std::sqrt( (1.-newDir.Z()*newDir.Z())/(1. + R*R) ) );
  if ( dir.X() < 0 ) newDir.SetX( -newDir.X() );
  newDir.SetY( R*newDir.X() );
  dir = newDir;
}
