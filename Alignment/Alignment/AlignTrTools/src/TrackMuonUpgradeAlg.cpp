// $Id: TrackMuonUpgradeAlg.cpp,v 1.1 2010/04/19 22:35:56 wouter Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"

// event
#include "Event/Track.h"
#include "Event/TrackFitResult.h"
#include <string>
#include <boost/foreach.hpp>

class TrackMuonUpgradeAlg: public GaudiAlgorithm {

public:
  
  // Constructors and destructor
  TrackMuonUpgradeAlg(const std::string& name,
		      ISvcLocator* pSvcLocator);
  virtual ~TrackMuonUpgradeAlg();
  virtual StatusCode execute();

private:
  std::string m_inputLocation;
  std::string m_trackInputLocation;
  std::string m_outputLocation;
};


DECLARE_ALGORITHM_FACTORY( TrackMuonUpgradeAlg );

TrackMuonUpgradeAlg::TrackMuonUpgradeAlg(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "MuonInputLocation",   m_inputLocation  = LHCb::TrackLocation::Muon ) ;
  declareProperty( "TrackInputLocation",  m_trackInputLocation  = LHCb::TrackLocation::Default ) ;
  declareProperty( "OutputLocation",      m_outputLocation  = "Rec/Track/LongMuons" ) ;
}

TrackMuonUpgradeAlg::~TrackMuonUpgradeAlg()
{
  // destructor
}

StatusCode TrackMuonUpgradeAlg::execute()
{
  // get them as a range .... then const-cast:-)
  LHCb::Track::Range muontracks  = get<LHCb::Track::Range>(m_inputLocation) ;

  LHCb::Tracks* outtracks(0) ;
  if( m_inputLocation != m_outputLocation ) {
    outtracks = new LHCb::Tracks() ;
    put(outtracks,m_outputLocation) ;
  } else {
    error() << "upgrade not yet inimplemented" << endreq ;
    return StatusCode::FAILURE ;
  }

  const LHCb::Tracks* besttracks(0) ;
  BOOST_FOREACH(const LHCb::Track* muontrack, muontracks) {
    const LHCb::Track* origtrack(0) ;
    
    if( muontrack->ancestors().empty() ) {
      debug() << "Problem: muon track has no ancestors!" << endreq ;
      if( besttracks==0 ) besttracks = get<LHCb::Tracks>( m_trackInputLocation ) ;
      origtrack = static_cast<const LHCb::Track*>(besttracks->containedObject(muontrack->key())) ;
    } else {
      origtrack = muontrack->ancestors().front() ;
    }
    if(origtrack && 
       origtrack->type() == LHCb::Track::Long ) {
      // for now, let's make a new track in a new container
      if( outtracks ) {
	// clone
	LHCb::Track* track = origtrack->clone() ;
	// add the muon hits
	track->addSortedToLhcbIDs( muontrack->lhcbIDs() ) ;
	// invalidate the fit
	track->setFitResult(0) ;
	outtracks->insert( track ) ;
      } else {
	// upgrade an existing track. to be done ...
      }
    }
  }

  return StatusCode::SUCCESS ;
}
