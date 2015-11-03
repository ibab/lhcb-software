#include "Event/PrimaryVertex.h"
#include "Event/RecVertex.h"
#include "GaudiAlg/GaudiAlgorithm.h"

class PVToRecConverter : public GaudiAlgorithm
{
public:
  PVToRecConverter( const std::string&, ISvcLocator*) ;
  StatusCode execute() ;
private:
  std::string m_inputLocation ;
  std::string m_outputLocation ;
  std::string m_inputTrackLocation ;
  bool m_rescaleWeights ;
} ;

DECLARE_ALGORITHM_FACTORY( PVToRecConverter )

//-----------------------------------------------------------------------------
// Implementation file for class : PVToRecConverter
//
// 2015-10-30 : Wouter Hulsbergen
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVToRecConverter::PVToRecConverter( const std::string& name,
						  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputLocation" , m_inputLocation  = LHCb::PrimaryVertexLocation::Default );
  declareProperty( "OutputLocation", m_outputLocation = LHCb::RecVertexLocation::Primary );
  declareProperty( "InputTrackLocation", m_inputTrackLocation = LHCb::TrackLocation::Default ) ;
  declareProperty( "RescaleWeights", m_rescaleWeights = true ) ;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode PVToRecConverter::execute() 
{
  LHCb::PrimaryVertex::Range pvs = get<LHCb::PrimaryVertex::Range>(m_inputLocation) ;
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_inputTrackLocation) ;

  // create a map from veloIDs to tracks in the best track list. only
  // store the first one (since long tracks are first!)
  std::map< LHCb::PrimaryVertex::VeloSegmentID, std::vector<const LHCb::Track*> > idToTracks ;
  for( const auto& tr: tracks ) {
    auto id = LHCb::PrimaryVertex::uniqueVeloSegmentID( *tr ) ;
    idToTracks[id].push_back( tr ) ;
  }
  
  // now loop over input vertices and create RecVertices
  auto recvertices = new LHCb::RecVertex::Container() ;
  for( const auto& pv: pvs ) {
    // it would have been nice to have a constructor from a VertexBase!
    auto recvertex = new LHCb::RecVertex( pv->position() ) ;
    recvertex->VertexBase::operator=( *pv ) ;
    recvertex->setTechnique(LHCb::RecVertex::Primary) ;
    // add the daughters
    for( const auto& trk : pv->tracks() ) {
      auto trkinmap = idToTracks.find( trk.id() ) ;
      if( trkinmap != idToTracks.end() ) {
	// add them all!
	for( const auto& itrk : trkinmap->second ) {
	  double weight = trk.weight() ;
	  if( m_rescaleWeights ) {
	    // track cov matrix may have changed due to track
	    // fit. rescale weights such that track gets effectively
	    // still the same weight. the unbiasing will then still
	    // work.
	    LHCb::State newstate = itrk->firstState() ;
	    newstate.linearTransportTo( pv->refZ() ) ;
	    // compute the ratio of the covariances of the transverse
	    // IP: dx * diry - dy * dirx. (don't need to divide by
	    // sqrt(tx^2+ty^2) since we take ratio anyway)
	    const double tx = newstate.tx() ;
	    const double ty = newstate.ty() ;
	    auto oldcov = trk.invcov() ;
	    oldcov.Invert() ;
	    const auto& newcov =  newstate.covariance() ;
	    const double covA = oldcov(0,0) * ty * ty + oldcov(1,1) * tx * tx - 2 * oldcov(1,0) * tx * ty ;
	    const double covB = newcov(0,0) * ty * ty + newcov(1,1) * tx * tx - 2 * newcov(1,0) * tx * ty ;
	    weight *= covB / covA ;
	  }
	  recvertex->addToTracks( itrk, weight ) ;
	}
      }
    }
    recvertices->insert( recvertex ) ;
    // info() << "Created RecVertex with daughters: "
    //        << pv->tracks().size() << " " << recvertex->tracks().size() << endreq ;
  }
  put(recvertices,m_outputLocation) ;

  return StatusCode::SUCCESS ;
}
