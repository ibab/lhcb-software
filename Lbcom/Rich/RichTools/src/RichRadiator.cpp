// $Id: RichRadiator.cpp,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
// Include files

// local
#include "RichRadiator.h"
#include "RichX.h"

#include "DetDesc/IGeometryInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRadiator
//
// 18/05/2002 :
//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichRadiator::~RichRadiator(  ) { }

// Constructor with arguments
RichRadiator::RichRadiator(std::vector<DeRichRadiator*>& segments,
                           RichX* rich,
                           Rich::RadiatorType type) :
  m_type        ( type     ),
  m_rich        ( rich     ),
  m_deRadiators ( segments ) { }

// Copy constructor
RichRadiator::RichRadiator( const RichRadiator& source ) :
  m_type        ( source.m_type        ),
  m_rich        ( source.m_rich        ),
  m_deRadiators ( source.m_deRadiators ) { }

bool RichRadiator::intersectionPoints( const HepPoint3D& position,
                                       const HepVector3D& direction,
                                       HepPoint3D& entryPoint,
                                       HepPoint3D& exitPoint ) {

  std::vector<DeRichRadiator*>::iterator segment_it;
  for (segment_it = m_deRadiators.begin();
       segment_it != m_deRadiators.end(); segment_it++) {

    std::vector<HepPoint3D> points;
    (*segment_it)->intersectionPoints(position, direction, points);

    if ( points.size() > 1 ) {
      // found intersection in this segment

      entryPoint = points[0];
      exitPoint = points[points.size()-1];
      return true; // all ok
    }

  }

  // failed to find points
  entryPoint = exitPoint = HepPoint3D(0,0,0);
  return false;
}

bool RichRadiator::nextIntersectionPoint ( const HepPoint3D& point,
                                                 const HepVector3D& direction,
                                                 HepPoint3D& intersection) {

  std::vector<DeRichRadiator*>::iterator segment_it;
  for ( segment_it = m_deRadiators.begin();
        segment_it != m_deRadiators.end();
        segment_it++ ) {
    if ( (*segment_it)->nextIntersectionPoint( point,
                                               direction,
                                               intersection ) ) return true;
  }

  return false;
}

bool RichRadiator::isInside( const HepPoint3D& point ) {

  for ( std::vector<DeRichRadiator*>::iterator segment_it = m_deRadiators.begin();
        segment_it != m_deRadiators.end();
        ++segment_it ) {
    if ( (*segment_it)->geometry()->isInside(point) ) return true;
  }
  return false;
}

