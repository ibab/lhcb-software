
//-----------------------------------------------------------------------------
/** @file RichSnellsLawRefraction.cpp
 *
 * Implementation file for class : Rich::SnellsLawRefraction
 *
 * CVS Log :-
 * $Id: RichSnellsLawRefraction.cpp,v 1.3 2008-08-15 14:27:28 jonrob Exp $
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
  : ToolBase   ( type, name, parent ),
    m_refIndex ( NULL )
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

  // get tools
  acquireTool( "RichRefractiveIndex", m_refIndex );

  // radiator tool
  const IRadiatorTool * radTool;
  acquireTool( "RichRadiatorTool", radTool );

  // get three points in exit plane
  RichRadIntersection::Vector intersections;
  Gaudi::XYZPoint p1(100,100,0), p2(100,-100,0), p3(-100,-100,0);
  const Gaudi::XYZVector v(0,0,1);
  radTool->intersections( p1, v, Rich::Aerogel, intersections );
  p1 = intersections.back().exitPoint();
  m_minZaero = p1.z();
  radTool->intersections( p2, v, Rich::Aerogel, intersections );
  p2 = intersections.back().exitPoint();
  if ( p2.z() < m_minZaero ) m_minZaero = p2.z();
  radTool->intersections( p3, v, Rich::Aerogel, intersections );
  p3 = intersections.back().exitPoint();
  if ( p3.z() < m_minZaero ) m_minZaero = p3.z();
  debug() << "Points " << p1 << " " << p2 << " " << p3 << endreq;

  // make a plane using these points
  m_aeroExitPlane = Gaudi::Plane3D(p1,p2,p3);

  // get the normal to the plane
  m_aeroNormVect = m_aeroExitPlane.Normal();

  // release tools no longer needed
  releaseTool(radTool);

  return sc;
}

void SnellsLawRefraction::aerogelToGas( Gaudi::XYZPoint & startPoint,
                                        Gaudi::XYZVector & dir,
                                        const double photonEnergy ) const
{
  if ( photonEnergy > 0 )
  {
    if ( startPoint.z() < m_minZaero )
    {
      // get refractive indices for aerogel and rich1Gas
      const double refAero     = m_refIndex->refractiveIndex( Rich::Aerogel,  photonEnergy );
      const double refrich1Gas = m_refIndex->refractiveIndex( Rich::Rich1Gas, photonEnergy );
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
  }
  else { Warning( "aerogelToGas:: photonEnergy = 0" ).ignore(); }
}

void SnellsLawRefraction::gasToAerogel( Gaudi::XYZVector & dir,
                                        const double photonEnergy ) const
{
  if ( photonEnergy > 0 )
  {
    // get refractive indices for aerogel and Rich1Gas
    const double refAero     = m_refIndex->refractiveIndex( Rich::Aerogel,  photonEnergy );
    const double refrich1Gas = m_refIndex->refractiveIndex( Rich::Rich1Gas, photonEnergy );
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
  else { Warning( "gasToAerogel:: photonEnergy = 0" ).ignore(); }
}
