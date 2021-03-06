#include "Event/TrackFitResult.h"
#include "Event/TrackFunctor.h"
#include "Event/Node.h"

using namespace LHCb ;

//=============================================================================
// Constructor
//=============================================================================
TrackFitResult::TrackFitResult()
  : m_nIter(-1),
    m_pScatter(0)
{
}

//=============================================================================
// Copy constructor. Hidden: use clone method instead
//=============================================================================
TrackFitResult::TrackFitResult(const TrackFitResult& rhs)
  : m_nIter(-1),
    m_pScatter(0)
{
  copy(rhs) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackFitResult::~TrackFitResult()
{
  reset() ;
}

//=============================================================================
// Clone the track
//=============================================================================
TrackFitResult* TrackFitResult::clone() const
{
  TrackFitResult* tr = new TrackFitResult();
  tr -> copy( *this );
  return tr;
}

//=============================================================================
// Add a Measurement to the list associated to the Track
//=============================================================================
void TrackFitResult::addToMeasurements( const LHCb::Measurement& meas )
{
  m_measurements.push_back( meas.clone() ) ;
}

//=============================================================================
// Add a list of measurement to the list associated to the Track. This takes ownership.
//=============================================================================
void TrackFitResult::addToMeasurements( MeasurementContainer& measurements )
{
  m_measurements.insert( m_measurements.end(), measurements.begin(), measurements.end() ) ;
}

//=============================================================================
// Remove a Measurement from the list of Measurements associated to the Track
//=============================================================================
void TrackFitResult::removeFromMeasurements( const Measurement* meas )
{
  TrackFunctor::deleteFromList<Measurement>( m_measurements, meas );
}

//=============================================================================
// Remove a Node from the list of Nodes associated to the Track
//=============================================================================
void TrackFitResult::removeFromNodes( Node* node )
{
  // Also delete from the Track the Measurement corresponding
  // to the deleted Node, if present!
  if ( node -> hasMeasurement() ) {
    Measurement& meas = node -> measurement();
    TrackFunctor::deleteFromList<Node>(m_nodes,node);
    removeFromMeasurements( &meas );
  } else {
    TrackFunctor::deleteFromList<Node>(m_nodes,node);
  }
}

//=============================================================================
// Return the Measurement on the Track corresponding to the input LHCbID
//=============================================================================
const Measurement* TrackFitResult::measurement( const LHCbID& value ) const
{
  auto it = std::find_if( m_measurements.begin(), m_measurements.end(),
                          [&](const Measurement* m) { return m->lhcbID() == value; } );
  return it != m_measurements.end() ? *it : nullptr;
}

//=============================================================================
// Remove all measurements from the track
//=============================================================================
void TrackFitResult::clearMeasurements()
{
  // remove all nodes first
  clearNodes() ;
  // now remove the measurements
  std::for_each(m_measurements.begin(), m_measurements.end(),TrackFunctor::deleteObject()) ;
  m_measurements.clear() ;
}

//=============================================================================
// Remove all nodes from the track
//=============================================================================
void TrackFitResult::clearNodes()
{
  std::for_each(m_nodes.begin(), m_nodes.end(),TrackFunctor::deleteObject()) ;
  m_nodes.clear() ;
}

//=============================================================================
// reset the track
//=============================================================================
void TrackFitResult::reset()
{
  m_nIter = 0 ;
  std::for_each(m_measurements.begin(), m_measurements.end(),TrackFunctor::deleteObject()) ;
  std::for_each(m_nodes.begin(), m_nodes.end(),TrackFunctor::deleteObject()) ;
  m_measurements.clear();
  m_nodes.clear();
}

//=============================================================================
// Copy the info from the argument track into this track
//=============================================================================
void TrackFitResult::copy( const TrackFitResult& rhs )
{
  reset();
  m_nIter    = rhs.m_nIter ;
  m_pScatter = rhs.m_pScatter ;


  // copy the measurements. make a map from old to new measurements.
  std::map<const Measurement*, Measurement*> measurementmap ;
  m_measurements.reserve(rhs.measurements().size()) ;
  for( const auto& meas : rhs.m_measurements) {
    m_measurements.push_back( meas->clone() ) ;
    measurementmap[meas] = m_measurements.back() ;
  }

  // copy the nodes. be sure to remap the measurement.
  m_nodes.reserve(rhs.m_nodes.size()) ;
  for (const auto& node : rhs.m_nodes) {
    m_nodes.push_back(node->clone()) ;
    if( node->hasMeasurement() ) {
      auto it = measurementmap.find( &node->measurement() );
      if(it == measurementmap.end())
        throw GaudiException( "TrackFitResult::copy: found a node pointing to a measurement not on track!",
                              "Track.cpp",StatusCode::FAILURE );
      m_nodes.back()->setMeasurement(*(it->second)) ;
    }
  }
}

//=============================================================================
// Count the number of outliers
//=============================================================================

unsigned int LHCb::TrackFitResult::nOutliers() const
{
  return std::count_if( nodes().begin(), nodes().end(),
                        [](const Node* node) {
                            return node->type() == LHCb::Node::Outlier;
  } );
}

//=============================================================================
// Count the number of outliers
//=============================================================================

unsigned int LHCb::TrackFitResult::nActiveMeasurements() const
{
  return m_measurements.size() - nOutliers() ;
}

//=============================================================================
// Count the number of measurement of a certain type
//=============================================================================

unsigned int LHCb::TrackFitResult::nMeasurements( const LHCb::Measurement::Type& type) const
{
  return std::count_if( m_measurements.begin(), m_measurements.end(),
                        [&](const Measurement* m) { return m->type() == type; } );
}

//=============================================================================
// Count the number of active measurement of a certain type
//=============================================================================

unsigned int LHCb::TrackFitResult::nActiveMeasurements( const LHCb::Measurement::Type& type ) const
{
  return std::count_if( nodes().begin(), nodes().end(),
                        [&](const Node* node) {
    return node->type() == LHCb::Node::HitOnTrack &&
           node->measurement().type() == type;
  });
}
