#include "CreateFastTrackCollection.h"

#include "GaudiKernel/SharedObjectsContainer.h"
#include "Event/Track.h"

#include <boost/foreach.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : CreateFastTrackCollection
//
// 2009-02-25 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CreateFastTrackCollection )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CreateFastTrackCollection::CreateFastTrackCollection(const std::string& name,
	ISvcLocator* pSvcLocator) :
    GaudiAlgorithm(name, pSvcLocator)
{
    declareProperty("InputLocations", m_inputLocations);
    declareProperty("OutputLocation", m_outputLocation);
    declareProperty("SlowContainer", m_slowContainer = false);
}

//=============================================================================
// Destructor
//=============================================================================
CreateFastTrackCollection::~CreateFastTrackCollection() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CreateFastTrackCollection::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiAlgorithm

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

    // verify job options
    if (m_inputLocations.empty()) {
	error() << "No input locations specified." << endmsg;
	return StatusCode::FAILURE;
    }
    if (m_outputLocation.empty()) {
	error() << "No output locations specified." << endmsg;
	return StatusCode::FAILURE;
    }
    unsigned pos = 0;
    bool allok = true;
    BOOST_FOREACH(const std::string& src, m_inputLocations) {
	++pos;
	if (src.empty()) {
	    error() << "Invalid input location specified (position " <<
		pos << ")." << endmsg;
	    allok = false;
	}
    }
    if (!allok)
	return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateFastTrackCollection::execute()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;
  if (!m_slowContainer) {
	// create output container and put it on TES
    SharedObjectsContainer<LHCb::Track> *out =
	    new SharedObjectsContainer<LHCb::Track>;
    put(out, m_outputLocation);
	// get all input containers in turn and put track pointers into output
    BOOST_FOREACH(const std::string& src, m_inputLocations) {
	    LHCb::Tracks *input = get<LHCb::Tracks>(src);
	    out->insert(input->begin(), input->end());
    }
  } else {
	// count tracks so that we can make an output container of the
	// right size
	std::size_t ntracks = 0;
	BOOST_FOREACH(const std::string& src, m_inputLocations) {
	    const LHCb::Tracks* input = get<LHCb::Tracks>(src);
	    ntracks += input->size();
	}
	// copy tracks the old way, using keyed containers
	LHCb::Tracks *out = new LHCb::Tracks();
	out->reserve(ntracks);
	put(out, m_outputLocation);
	BOOST_FOREACH(const std::string& src, m_inputLocations) {
	    const LHCb::Tracks* input = get<LHCb::Tracks>(src);
	    BOOST_FOREACH(const LHCb::Track* tr, *input) {
		// hopefully, I copy everything
		LHCb::Track *ntr = tr->clone();
		ntr->setHistory(tr->history());
		ntr->setType(tr->type());
		out->insert(ntr);
	    }
	}
    }

    return StatusCode::SUCCESS;
}
