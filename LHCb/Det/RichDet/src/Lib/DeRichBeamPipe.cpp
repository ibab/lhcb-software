
//=============================================================================
/** @file DeRichBeamPipe.cpp
 *
 * Implementation file for class : DeRichBeamPipe
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-11-27
 */
//=============================================================================

// local
#include "RichDet/DeRichBeamPipe.h"

//=============================================================================

const CLID CLID_DERichBeamPipe = 12050;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichBeamPipe::DeRichBeamPipe ( const std::string & name )
  : DeRichBase ( name ) { }

//=============================================================================
// Destructor
//=============================================================================
DeRichBeamPipe::~DeRichBeamPipe() { }

// Retrieve Pointer to class defininition structure
const CLID& DeRichBeamPipe::classID()
{
  return CLID_DERichBeamPipe;
}

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRichBeamPipe::initialize ( )
{
  // Which Rich ?
  if      ( name().find("Rich1") != std::string::npos )
  {
    m_rich = Rich::Rich1;
    setMyName("DeRich1BeamPipe");
  }
  else if ( name().find("Rich2") != std::string::npos )
  { 
    m_rich = Rich::Rich2;
    setMyName("DeRich2BeamPipe");
  }
  else
  {
    return Fatal( "Unknown RICH ! " + name() );
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Initialize " << name() << endmsg;
    debug() << geometry()->lvolume()->name() << endmsg;
  }

  const auto * solid = geometry()->lvolume()->solid();
  if ( solid )
  {
    if ( solid->typeName() == "SolidCons" )
    {
      const auto * coneSolid = dynamic_cast<const SolidCons*>(solid);
      if ( coneSolid )
      {
        // make the cone object
        const auto cname = 
          ( Rich::Rich1 == m_rich ? "LocalRich1Cone" : "LocalRich2Cone" );
        m_localCone.reset( new SolidCons( cname, 
                                          coneSolid->zHalfLength(),
                                          coneSolid->outerRadiusAtMinusZ(),
                                          coneSolid->outerRadiusAtPlusZ() ) );

        // beam pipe size tolerance factor for 'fast' check
        const auto tolF = ( Rich::Rich1 == m_rich ? 2.0 : 1.75 );

        // get the start and end radius square + some tolerence factor
        const auto startRad2 = tolF * std::pow( coneSolid->outerRadiusAtMinusZ(), 2 );
        const auto endRad2   = tolF * std::pow( coneSolid->outerRadiusAtPlusZ(),  2 );
        _ri_debug << "Start R^2    = " << startRad2 << endmsg;
        _ri_debug << "End R^2      = " << endRad2 << endmsg;

        // Start and end point of the cone axis in local coordinates
        const auto startPLoc = Gaudi::XYZPoint( 0, 0, -(coneSolid->zHalfLength()) );
        const auto endPLoc   = Gaudi::XYZPoint( 0, 0,  (coneSolid->zHalfLength()) );
        _ri_debug << "Start Local  = " << startPLoc << endmsg;
        _ri_debug << "End Local    = " << endPLoc << endmsg;

        // Start and end point of the cone axis in global coordinates
        m_startPGlo = geometry()->toGlobal( startPLoc );
        m_endPGlo   = geometry()->toGlobal( endPLoc );
        _ri_debug << "Start Global = " << m_startPGlo << endmsg;
        _ri_debug << "End Global   = " << m_endPGlo << endmsg;

        // Compute the y = mx + c parameters for the beam pipe x,y and R^2

        const auto zDiff = ( m_startPGlo.z() - m_endPGlo.z() );

        // x coordinate
        m_m[0] = ( m_startPGlo.x() - m_endPGlo.x() ) / zDiff;
        m_c[0] = m_startPGlo.x() - ( m_m[0] * m_startPGlo.z() );

        // y coordinate
        m_m[1] = ( m_startPGlo.y() - m_endPGlo.y() ) / zDiff;
        m_c[1] = m_startPGlo.y() - ( m_m[1] * m_startPGlo.z() );

        // R^2 
        m_m[2] = ( startRad2 - endRad2 ) / zDiff;
        m_c[2] = startRad2 - ( m_m[2] * m_startPGlo.z() );

      }
      else
      {
        return Fatal( "Problem casting to SolidCons" );
      }
    }
    else
    {
      return Fatal( "Beam pipe solid is not a cone" );
    }
  }
  else
  {
    return Fatal( "Problem loading Geometry" );
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// intersectionPoints
//=========================================================================

DeRichBeamPipe::BeamPipeIntersectionType 
DeRichBeamPipe::intersectionPoints( const Gaudi::XYZPoint& start,
                                    const Gaudi::XYZPoint& end,
                                    Gaudi::XYZPoint& entryPoint,
                                    Gaudi::XYZPoint& exitPoint ) const
{
  // fast test on if the start and end points are close enough to the
  // the beam axis in global coords
  const bool inter = isCloseBy(start,end);
  if ( inter )
  {

    // get point and direction in local coordinates
    const auto pLocal = geometry()->toLocal(start);
    const auto vLocal = geometry()->toLocalMatrix()*(end-start);

    // OK, try the full check ...
    ISolid::Ticks ticks;
    const auto noTicks = 
      m_localCone->intersectionTicks( pLocal, vLocal, ticks );

    if ( 0 != noTicks )
    {
      // set entry point
      const auto entryLocal = pLocal + ticks[0] * vLocal;
      const bool frontFaceHit = 
        ( entryLocal.z() < -(m_localCone->zHalfLength()) + 1*Gaudi::Units::mm );
      entryPoint = geometry()->toGlobal( entryLocal );

      // set exit point
      const auto exitLocal = pLocal + ticks[noTicks-1] * vLocal;
      const bool backFaceHit = 
        ( exitLocal.z() > m_localCone->zHalfLength() - 1*Gaudi::Units::mm );
      exitPoint  = geometry()->toGlobal( exitLocal );

      // return the status
      return ( frontFaceHit && backFaceHit ? FrontAndBackFace :
               frontFaceHit                ? FrontFaceAndCone :
               backFaceHit                 ? BackFaceAndCone  :
               ConeOnly );
    }

  }

  // if get here, no intersection
  return NoIntersection;
}

//=========================================================================
// Test if a given direction intersects the beam-pipe volume at all.
//=========================================================================

bool 
DeRichBeamPipe::testForIntersection( const Gaudi::XYZPoint& start,
                                     const Gaudi::XYZPoint& end ) const
{

  // fast test on if the start and end points are close enough to the
  // the beam axis in global coords
  bool inter = isCloseBy(start,end);
  
  // If close enough, run full test
  if ( inter )
  {
    // get point and direction in local coordinates
    const auto pLocal = geometry()->toLocal(start);
    const auto vLocal = geometry()->toLocalMatrix()*(end-start);
    // run full intersection test
    ISolid::Ticks ticks;
    inter = 0 != m_localCone->intersectionTicks( pLocal, vLocal, ticks );
  }
  
  // // test against running the full test always
  // {
  //   const auto pLocal = geometry()->toLocal(start);
  //   const auto vLocal = geometry()->toLocalMatrix()*(end-start);
  //   ISolid::Ticks ticks;
  //   const bool testinter = ( 0 != m_localCone->intersectionTicks(pLocal,vLocal,ticks) );
  //   if ( testinter != inter )
  //   {
  //     info() << " -> Mis-match : " << testinter << " " << inter
  //            << " " << pLocal << " " << vLocal << endmsg;
  //   }
  //   //else { info() << " Beam OK !!" << endmsg; }
  // }

  return inter;
}

//=========================================================================
//  text conversion
//=========================================================================
std::string
DeRichBeamPipe::text( const DeRichBeamPipe::BeamPipeIntersectionType& type )
{
  switch( type ) {
  case DeRichBeamPipe::NoIntersection:   return "NoIntersection";
  case DeRichBeamPipe::FrontAndBackFace: return "FrontAndBackFace";
  case DeRichBeamPipe::FrontFaceAndCone: return "FrontFaceAndCone";
  case DeRichBeamPipe::BackFaceAndCone:  return "BackFaceAndCone";
  case DeRichBeamPipe::ConeOnly:         return "ConeOnly";
  default:                               return "SHOULD NEVER SEE THIS";
  }

}
