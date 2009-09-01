// $Id: TrackDistanceExtraSelector.cpp,v 1.6 2009-09-01 20:07:11 wouter Exp $

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
  declareProperty( "ShortDistanceExtrapolatorType",
                   m_shortDistanceExtrapolatorType = "TrackParabolicExtrapolator" );
  declareProperty( "LongDistanceExtrapolatorType",
                   m_longDistanceExtrapolatorType  = "TrackHerabExtrapolator" ); 
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
  m_shortDistanceExtrapolator = tool<ITrackExtrapolator>( m_shortDistanceExtrapolatorType,
							  "ShortDistanceExtrapolator", this ) ;
  // request extrapolator for going short distances in magnetic field
  m_longDistanceExtrapolator = tool<ITrackExtrapolator>( m_longDistanceExtrapolatorType,
							 "LongDistanceExtrapolator", this );
  
  info() << "Short distance extrapolator: " << m_shortDistanceExtrapolator->type() << endreq ;
  info() << "Long distance extrapolator: " << m_longDistanceExtrapolator->type() << endreq ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
ITrackExtrapolator* TrackDistanceExtraSelector::select( const double zStart,
                                                        const double zEnd ) const
{
  return ( std::abs(zEnd-zStart) < m_shortDist ?
           m_shortDistanceExtrapolator : m_longDistanceExtrapolator );
}

//=============================================================================
