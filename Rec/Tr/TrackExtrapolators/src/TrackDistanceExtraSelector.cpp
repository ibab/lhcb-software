// $Id: TrackDistanceExtraSelector.cpp,v 1.5 2006-05-31 12:50:57 erodrigu Exp $

// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// local
#include "TrackDistanceExtraSelector.h" 

DECLARE_TOOL_FACTORY( TrackDistanceExtraSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

TrackDistanceExtraSelector::TrackDistanceExtraSelector(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent):
  GaudiTool(type, name, parent)
{
  declareInterface<ITrackExtraSelector>( this );

  declareProperty( "shortDist"
                   , m_shortDist = 100.0*Gaudi::Units::mm );
  declareProperty( "ShortFieldExtrapolatorName",
                   m_shortFieldExtrapolatorName = "TrackParabolicExtrapolator" );
  declareProperty( "LongFieldExtrapolatorName",
                   m_longFieldExtrapolatorName  = "TrackHerabExtrapolator" );
  declareProperty( "ShortFieldExtrapolatorType",
                   m_shortFieldExtrapolatorType = "TrackParabolicExtrapolator" );
  declareProperty( "LongFieldExtrapolatorType",
                   m_longFieldExtrapolatorType  = "TrackHerabExtrapolator" ); 
}

//=============================================================================
// Destructor
//=============================================================================
TrackDistanceExtraSelector::~TrackDistanceExtraSelector() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackDistanceExtraSelector::initialize()
{
  // initialize
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize", sc );

  // request a short distance magnetic field extrapolator
  m_shortFieldExtrapolator = tool<ITrackExtrapolator>( m_shortFieldExtrapolatorType,
                                                       m_shortFieldExtrapolatorName );

  // request extrapolator for going short distances in magnetic field
  m_longFieldExtrapolator = tool<ITrackExtrapolator>( m_longFieldExtrapolatorType,
                                                      m_longFieldExtrapolatorName );

  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
ITrackExtrapolator* TrackDistanceExtraSelector::select( const double zStart,
                                                        const double zEnd ) const
{
  return ( fabs(zEnd-zStart) < m_shortDist ?
           m_shortFieldExtrapolator : m_longFieldExtrapolator );
}

//=============================================================================
