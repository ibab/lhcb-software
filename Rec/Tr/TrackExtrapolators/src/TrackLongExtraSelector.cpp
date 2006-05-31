// $Id: TrackLongExtraSelector.cpp,v 1.6 2006-05-31 14:50:04 mneedham Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// Local
#include "TrackLongExtraSelector.h"

DECLARE_TOOL_FACTORY( TrackLongExtraSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackLongExtraSelector::TrackLongExtraSelector( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent ):
  GaudiTool(type, name, parent),
  m_startT(7.5*Gaudi::Units::m)
{
  declareProperty( "ShortDist",
                   m_shortDist = 100.0*Gaudi::Units::mm );
  declareProperty( "ShortFieldExtrapolatorName",
                   m_shortFieldExtrapolatorName = "TrackParabolicExtrapolator ");
  declareProperty( "LongFieldExtrapolatorName",
                   m_longFieldExtrapolatorName  = "TrackHerabExtrapolator" );
  declareProperty( "ShortFieldExtrapolatorType",
                   m_shortFieldExtrapolatorType = "TrackParabolicExtrapolator") ;
  declareProperty( "LongFieldExtrapolatorType",
                   m_longFieldExtrapolatorType  = "TrackHerabExtrapolator" );

 declareInterface<ITrackExtraSelector>( this );
  
}

//=============================================================================
// Destructor
//=============================================================================
TrackLongExtraSelector::~TrackLongExtraSelector() {}

StatusCode TrackLongExtraSelector::initialize()
{
  // initialize
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize", sc );

  // request a short distance magnetic field extrapolator
  m_shortFieldExtrapolator = tool<ITrackExtrapolator>( m_shortFieldExtrapolatorType,
                                                       m_shortFieldExtrapolatorName,
                                                       this );

  // request extrapolator for going short distances in magnetic field
  m_longFieldExtrapolator = tool<ITrackExtrapolator>( m_longFieldExtrapolatorType,
                                                      m_longFieldExtrapolatorName,
                                                      this );
 
  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
ITrackExtrapolator* TrackLongExtraSelector::select( const double zStart,
                                                    const double zEnd ) const
{
  ITrackExtrapolator* tExtra = 0;
  if ( zEnd < m_startT ){ 
    tExtra = m_shortFieldExtrapolator;
  }
  else {
    tExtra = (fabs(zEnd-zStart) < m_shortDist ?  m_shortFieldExtrapolator : m_longFieldExtrapolator );
  }

  return tExtra;
}

//=============================================================================
