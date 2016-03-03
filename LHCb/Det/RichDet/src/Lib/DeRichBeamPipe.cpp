
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
DeRichBeamPipe::DeRichBeamPipe(const std::string & name) :
  DeRichBase  ( name )
{
  setMyName("DeRichBeamPipe");
}

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
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Initialize " << name() << endmsg;
    debug() << geometry()->lvolume()->name() << endmsg;
  }

  m_solid = geometry()->lvolume()->solid();
  if ( m_solid )
  {
    if ( m_solid->typeName() == "SolidCons" )
    {
      const auto * coneSolid = dynamic_cast<const SolidCons*>(m_solid);
      if ( coneSolid )
      {
        m_zHalfLength = coneSolid->zHalfLength();
        m_localCone.reset( new SolidCons( "LocalRichCone",
                                          m_zHalfLength,
                                          coneSolid->outerRadiusAtMinusZ(),
                                          coneSolid->outerRadiusAtPlusZ() ) );
        // get the start and end radius square + some tolerence factor
        m_startRad2 = 2 * std::pow( coneSolid->outerRadiusAtMinusZ(), 2 );
        m_endRad2   = 2 * std::pow( coneSolid->outerRadiusAtPlusZ(),  2 );
        m_startP    = Gaudi::XYZPoint( 0, 0, -m_zHalfLength );
        m_endP      = Gaudi::XYZPoint( 0, 0,  m_zHalfLength );
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
DeRichBeamPipe::intersectionPoints( const Gaudi::XYZPoint&  position,
                                    const Gaudi::XYZVector& direction,
                                    Gaudi::XYZPoint& entryPoint,
                                    Gaudi::XYZPoint& exitPoint ) const
{
  // get the position and point in local coordinates as a line object
  const LINE lPhot ( geometry()->toLocal(position),
                     geometry()->toLocalMatrix()*direction );

  // Try first fast test to see if we are close enough to the beampipe
  const bool inter = isCloseBy( lPhot );
  if ( inter )
  {
    // OK, try the full check ...
    ISolid::Ticks ticks;
    const auto noTicks = m_localCone->intersectionTicks( lPhot.beginPoint(),
                                                         lPhot.direction(),
                                                         ticks );
    if ( 0 != noTicks )
    {
      // set entry point
      const auto entryLocal = lPhot.beginPoint() + ticks[0] * lPhot.direction();
      const bool frontFaceHit = ( entryLocal.z() < -m_zHalfLength + 1*Gaudi::Units::mm );
      entryPoint = geometry()->toGlobal( entryLocal );

      // set exit point
      const auto exitLocal = lPhot.beginPoint() + ticks[noTicks-1] * lPhot.direction();
      const bool backFaceHit = ( exitLocal.z() > m_zHalfLength - 1*Gaudi::Units::mm );
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
DeRichBeamPipe::testForIntersection( const Gaudi::XYZPoint&  position,
                                     const Gaudi::XYZVector& direction ) const
{

  // get the position and point in local coordinates as a line object
  const LINE lPhot ( geometry()->toLocal(position),
                     geometry()->toLocalMatrix()*direction );

  // Try first fast test to see if we are close enough to the beampipe
  bool inter = isCloseBy( lPhot );
  if ( inter )
  {
    // OK, try the full check ...
    ISolid::Ticks ticks;
    inter = 0 != m_localCone->intersectionTicks( lPhot.beginPoint(),
                                                 lPhot.direction(),
                                                 ticks );
  }

  // // test against running the full test
  // ISolid::Ticks ticks;
  // const bool testinter = ( 0 != m_localCone->intersectionTicks( lPhot.beginPoint(),
  //                                                               lPhot.direction(),
  //                                                               ticks ) );
  // if ( testinter != inter )
  // {
  //   info() << "Mis-match : " << testinter << " " << inter << m_zHalfLength << " " << lPhot << endmsg;
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
