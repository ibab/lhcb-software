// $Id: TrackMonitor.cpp,v 1.1 2007-05-08 06:49:00 mneedham Exp $
// Include files 
#include "TrackMonitor.h"

//event
#include "Event/Track.h"
#include "Event/State.h"

#include "GaudiKernel/PhysicalConstants.h"

// gsl
#include "gsl/gsl_math.h"

#include "GaudiKernel/AlgFactory.h"

DECLARE_ALGORITHM_FACTORY( TrackMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMonitor::TrackMonitor(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
TrackMonitorBase( name , pSvcLocator ){

  declareProperty("ReferenceZ", m_refZ = 0.0);
}

//=============================================================================
// Destructor
//=============================================================================
TrackMonitor::~TrackMonitor() {}; 

StatusCode TrackMonitor::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackMonitorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // guess that the size of the histograms at ref is ~ 0.35 by 0.3
  m_xMax = GSL_MAX(5.0*Gaudi::Units::cm, 0.35*m_refZ);
  m_yMax = GSL_MAX(5.0*Gaudi::Units::cm, 0.3*m_refZ);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackMonitor::execute()
{

  // get the input data
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());

  // # number of tracks
  plot(tracks->size(),1, "# tracks", 0., 500., 50);

  // histograms per track
  unsigned int nSel = 0;
  LHCb::Tracks::const_iterator iterT = tracks->begin();
  for (; iterT != tracks->end(); ++iterT){
    if (selector()->accept(**iterT) == true){
      ++nSel;
      fillHistograms(*iterT);
    }
  } // iterT

  counter("Tracks") += tracks->size();
  counter("Selected") += nSel;

  return StatusCode::SUCCESS;
};

void TrackMonitor::fillHistograms(const LHCb::Track* aTrack) {

  // plots we should always make...
  plot(aTrack->probChi2(),2,"probChi2", -0.01, 1.01, 51);
  plot(aTrack->chi2PerDoF(),3,"chi2/ndof", -2., 202., 51);
  plot(aTrack->nLHCbIDs(),4,"#nLHCbIDs", -0.5, 60.5, 61);
  plot(aTrack->p()/Gaudi::Units::GeV, 5 ,"momentum", -5., 205., 21);
  plot(aTrack->pt()/Gaudi::Units::GeV,6, "pt", -0.1, 10.1, 51);
  plot(aTrack->pseudoRapidity(),7, "eta", 0.95 , 6.05, 50);

  // expert checks  
  if (fullDetail() == true){
    plot(aTrack->nMeasurements(),100,"#nMeas",  -0.5, 60., 61);
    plot(aTrack->nMeasurementsRemoved(),101,"#outliers", -0.5, 10.5, 61);
    plot(aTrack->type(),102, "type" ,-0.5, 10.5, 10);

    // track parameters at some reference z
    LHCb::State aState;
    StatusCode sc = extrapolator()->propagate(*aTrack, m_refZ,
                                              aState );
    plot2D(aState.x()/Gaudi::Units::cm,
           aState.y()/Gaudi::Units::cm, 200 ,"x vs y", 
           -m_xMax,m_xMax, -m_yMax, m_yMax, 50, 50);
    plot(aState.tx(),103,"tx", -1.,1., 50);
    plot(aState.ty(),104, "ty", -1.,1., 50);
  }
}

