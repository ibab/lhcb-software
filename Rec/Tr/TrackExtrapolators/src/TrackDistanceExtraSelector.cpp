// $Id: TrackDistanceExtraSelector.cpp,v 1.3 2006-02-02 14:28:53 erodrigu Exp $

// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// from LHCbDefinitions
#include "Kernel/PhysicalConstants.h"

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
                   m_shortFieldExtrapolatorName = "TrackFastParabolicExtrapolator" );
  declareProperty( "LongFieldExtrapolatorName",
                   m_longFieldExtrapolatorName  = "TrackHerabExtrapolator" );
  declareProperty( "ShortFieldExtrapolatorType",
                   m_shortFieldExtrapolatorType = "TrackFastParabolicExtrapolator" );
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
