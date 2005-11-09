// $Id: TrackDistanceExtraSelector.cpp,v 1.1 2005-11-09 14:43:46 erodrigu Exp $

// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// from CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// local
#include "TrackDistanceExtraSelector.h" 

// Declaration of the Tool Factory
static const ToolFactory<TrackDistanceExtraSelector> s_factory;
const IToolFactory& TrackDistanceExtraSelectorFactory = s_factory;

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
                   , m_shortDist = 300.0*mm );
  declareProperty( "ShortFieldExtrapolatorName",
                   m_shortFieldExtrapolatorName = "TrFastParabolicExtrapolator" );
  declareProperty( "LongFieldExtrapolatorName",
                   m_longFieldExtrapolatorName  = "TrHerabExtrapolator" );
  declareProperty( "ShortFieldExtrapolatorType",
                   m_shortFieldExtrapolatorType = "TrFastParabolicExtrapolator" );
  declareProperty( "LongFieldExtrapolatorType",
                   m_longFieldExtrapolatorType  = "TrHerabExtrapolator" ); 
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
  return (fabs(zEnd-zStart) < m_shortDist ?  m_shortFieldExtrapolator : m_longFieldExtrapolator );
}

//=============================================================================
