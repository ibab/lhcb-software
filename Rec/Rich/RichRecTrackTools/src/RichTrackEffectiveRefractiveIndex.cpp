
//-----------------------------------------------------------------------------
/** @file RichTrackEffectiveRefractiveIndex.cpp
 *
 * Implementation file for class : Rich::Rec::TrackEffectiveRefractiveIndex
 *
 * CVS Log :-
 * $Id: RichTrackEffectiveRefractiveIndex.cpp,v 1.1 2008-11-30 11:02:23 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   27/11/2008
 */
//-----------------------------------------------------------------------------

// local
#include "RichTrackEffectiveRefractiveIndex.h"

using namespace Rich::Rec;

DECLARE_TOOL_FACTORY( TrackEffectiveRefractiveIndex );

// Standard constructor
TrackEffectiveRefractiveIndex::
TrackEffectiveRefractiveIndex ( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : ToolBase    ( type, name, parent ),
    m_refIndex  ( NULL )
{
  // interface
  declareInterface<ITrackEffectiveRefractiveIndex>(this);
}

StatusCode TrackEffectiveRefractiveIndex::initialize()
{
  // Initialise base class
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  acquireTool( "RichRefractiveIndex", m_refIndex );

  return sc;
}

double 
TrackEffectiveRefractiveIndex::
refractiveIndex ( const LHCb::RichRecSegment * segment,
                  const double energy ) const
{
  const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();
  const RichRadIntersection::Vector & rads = tkSeg.radIntersections();
  return m_refIndex->refractiveIndex( rads, energy );
}

double 
TrackEffectiveRefractiveIndex::
refractiveIndex ( const LHCb::RichRecSegment * segment ) const
{
  const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();
  const RichRadIntersection::Vector & rads = tkSeg.radIntersections();
  return m_refIndex->refractiveIndex( rads );
}

double 
TrackEffectiveRefractiveIndex::
refractiveIndexRMS ( const LHCb::RichRecSegment * segment ) const
{
  return m_refIndex->refractiveIndexRMS( segment->trackSegment().radiator() );
}
