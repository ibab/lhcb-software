// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// event model
#include "Event/Track.h"
// boost
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

// local
#include "BeamGasProtoVertex.h"
#include <algorithm>

//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasProtoVertex
// 2008-06-16 : Plamen Hopchev <plamen.hopchev@cern.ch>
// 2011-27-02 : Plamen Hopchev <plamen.hopchev@cern.ch>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasProtoVertex );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasProtoVertex::BeamGasProtoVertex(const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm(name, pSvcLocator, false)
  , m_trackSelection(*this)
{
  m_trackSelection.declareProperties();
  declareProperty( "zTracksMin", m_zTrMin = -1200.);
  declareProperty( "zTracksMax", m_zTrMax =  1200.);
  declareProperty( "zTracksExcludeLumiRegionLow", m_zTrExclLRLow = 0.);
  declareProperty( "zTracksExcludeLumiRegionUp",  m_zTrExclLRUp  = 0.);
  declareProperty( "MinimumNumberOfTracks", m_minNumTracks = 8);
  declareProperty( "MaxNumberOfVeloTracks", m_maxNumOfVeloTracks = 600);
}

//=============================================================================
// Destructor
//=============================================================================
BeamGasProtoVertex::~BeamGasProtoVertex() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode BeamGasProtoVertex::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  m_trackSelection.retrieveSelections();
  m_trackSelection.registerSelection();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel(MSG::DEBUG)) {
          debug() << "==> Initialize\n" << endreq
          << "========== Algorithm parameters ========="   << endreq
          << "zTracksMin      = " << m_zTrMin              << endreq
          << "zTracksMax      = " << m_zTrMax              << endreq
          << "zTracksExcludeLumiRegionLow = " << m_zTrExclLRLow << endreq
          << "zTracksExcludeLumiRegionUp  = " << m_zTrExclLRUp  << endreq
          << "MinimumNumberOfTracks       = " << m_minNumTracks << endreq
          << "StepSize1                   = " << stepSize1() << endreq
          << "StepSize2                   = " << stepSize2() << endreq
          << "MinTracksToAccept           = " << minTracksToAccept() << endreq
	  << "MaxNumberOfVeloTracks       = " << m_maxNumOfVeloTracks << endreq << "\n" << endreq;
  }

  // Check that all steps have sensible sizes
  assert( m_minNumTracks > 1 );
  assert( stepSize1() > 0 );
  assert( stepSize2() > 0 );
  assert( minTracksToAccept() > 0 );

  return StatusCode::SUCCESS;
};


//=============================================================================
// Define Member Functions
//=============================================================================


//### function to calculate the mean and sigma of list of z values
template <typename ITER>
void BeamGasProtoVertex::getMeanAndSigma(ITER begin, ITER end , double& sMean, double& sSigma) const {
  namespace ba = boost::accumulators;
  typedef ba::accumulator_set<double, ba::stats<ba::tag::mean, ba::tag::variance, ba::tag::count> > m012;
  m012 acc = std::for_each( begin,end, m012() );
  if (ba::count(acc)!=0) {
      sMean  = ba::mean(acc);
      sSigma = sqrt(ba::variance(acc));
  } else {
      debug() << "Function getMeanAndSigma received empty vector" << endmsg;
  }
  debug() << "\nz values properties:"   << "\n   Entries = " << ba::count(acc)
          << "\n   Mean    = " << sMean << "\n   Sigma   = " << sSigma << "\n" << endmsg;
}

//### function to calculate the z-dependent "bad sigma"
double BeamGasProtoVertex::sigmaBad(double z) const {
  // "sigma bad" parameters (hard fixed)
  static const double m_sigmaBadConst1 = 44.;
  static const double m_sigmaBadConst2 = 16.;
  static const double m_sigmaBadSlope  = 56.e-3; // asume straight lines symmetric around z=250mm (sigma(250) = sigmaMin = 30. mm)
  return (z < 250) ? (-1*m_sigmaBadSlope*z + m_sigmaBadConst1) : (m_sigmaBadSlope*z + m_sigmaBadConst2) ;
}


template <typename ITER>
void BeamGasProtoVertex::printVector(ITER begin, ITER end, const std::string& theText) const {
  debug() << theText << "\nVector Size = " << end-begin << "  Contents:" << "\n";
  while (begin!=end)  debug() <<"   " << *begin++ << "\n";
  debug() << endmsg;
}

//### function to look for a proto vertex
template <typename ITER>
void BeamGasProtoVertex::findProtoVertex(ITER begin, ITER end) {
  //Init
  ITER indStartMS = begin;
  //----------------------------------------------------------
  // Loop "Main Step" on range until no more steps can be made
  //----------------------------------------------------------
  unsigned iloop = 0;
  unsigned maxNumMainSteps = std::distance(begin, end) / stepSize1(); //in initialize() assert that the denominator is > 0 
  while (indStartMS + minTracksToAccept() <= end) { // Note: <= should be < I think...
    ++iloop;
    debug() << " loop # " << iloop << "/" << maxNumMainSteps << endmsg;
    // ensure that we'll not fall into infinite loop
    if ( iloop > maxNumMainSteps ) {
      warning() << "Too Many Executions of MainStep while-loop. Breaking it." << endmsg;
      break;
    }

    // avoid looking at samples with too distant ends
    double distMinMax = abs( *indStartMS - *(indStartMS+stepSize1()-1) ); 
    debug() << "Distance between min and max z in the sample : " << distMinMax << endmsg;
    if ( distMinMax > 300 ) {
      debug() << "spread of z sample higher than 300 mm. Rejecting the event" << endmsg;
      indStartMS += stepSize1();
      continue;
    }

    if ( msgLevel(MSG::DEBUG) ) printVector(indStartMS, indStartMS+stepSize1(), "*** Main Step ***");
    double   mean_sample  = -9999.;
    double   sigma_sample = -9999.;
    getMeanAndSigma(indStartMS, indStartMS+stepSize1(), mean_sample, sigma_sample);
    debug() << "Main Step Mean And Sigma: " << mean_sample << " / " << sigma_sample << endmsg;

    double SIGMA_BAD = sigmaBad(mean_sample);
    debug() << "###SigmaBad is " << SIGMA_BAD << endmsg;

    if ( sigma_sample > SIGMA_BAD || sigma_sample < -9998. ) {
      debug() << "Sigma not good. Starting new Main Step." << endmsg;
      indStartMS += stepSize1();
      continue;
    }

    //-----------------------------------------------------------------------
    // Sigma is good. Try to extend ONCE in the forward or backward direction
    //-----------------------------------------------------------------------
    debug() << "Sigma is good. Trying to extend ..." << endmsg;
    ITER indStartExt = begin;
    ITER indEndExt = begin;
    bool extendOK = false;
    bool AccOK = false;

    // try fwd:
    if ( indStartMS+stepSize1()+stepSize2() <= end ) {
      indStartExt = indStartMS;
      indEndExt   = indStartMS+stepSize1()+stepSize2();
      debug() << "Found possible extension FWD" << endmsg;
      extendOK = true;
    // try bkw:
    } else if ( indStartMS - stepSize2() >= begin ) {
      indStartExt = indStartMS-stepSize2();
      indEndExt   = indStartMS+stepSize1();
      debug() << "Found possible extension BKW" << endmsg;
      extendOK = true;
    }

    // if extension is not possible make next Main Step; else check if sigma is good
    if ( !extendOK ) {
      debug() << "Not sufficient elements to do extension. This should not happen ! /nStarting new Main Step" << endmsg;
      indStartMS += stepSize1();
      continue;
    }
    // get the mean and sigma of the extended sample
    else {
      debug() << "Extension: going to use start/end indices" << indStartExt-begin << "/" << indEndExt-begin  << endmsg;
      debug() << "End index =" << end - begin << endmsg;
      debug() << ">>><<< end-1 and end elements :" << *(end-1) << "   " << *end << endmsg;

      if ( msgLevel(MSG::DEBUG) ) printVector(indStartExt, indEndExt, "*** Extended Sample ***");
      getMeanAndSigma(indStartExt, indEndExt, mean_sample, sigma_sample);
      debug() << "Extension Mean And Sigma: " << mean_sample << " / " << sigma_sample << endmsg;

      // if sigma is bad - reject, if it is good - set the AccOK flag
      double SIGMA_BAD = sigmaBad(mean_sample);
      debug() << "###SigmaBad is " << SIGMA_BAD << endmsg;
      if ( sigma_sample > SIGMA_BAD || sigma_sample < 0 ) {
	debug() << "Sigma not good. Starting new Main Step." << endmsg;                                                                                                          
	indStartMS += stepSize1();
	continue;
      }
      else {
	AccOK = true;
	indStartMS = end; // stop the while loop
	// by construction the sample size is = minTracksToAcc() = stepSize1()+stepSize2() 
	debug() << "Good sigma after the extension." << endmsg;
        debug() << "\n\nEVENT ACCEPTED" << endmsg;
      }
    }

    // print some debug, make more checks and fill the outputSelection
    if (AccOK) {
      debug() << "Currently #tracks in the proto vertex = " << std::distance(indStartExt,indEndExt) << " / " << minTracksToAccept() << endmsg;
      if ( std::distance(indStartExt, indEndExt) == minTracksToAccept() ) {
        debug() << "Track Indices of found Peak (first/last) : " << std::distance(begin,indStartExt) <<  " / " << std::distance(begin,indEndExt) << endmsg;
        debug() << "Their z positions : " << *indStartExt <<  " / " << *(indEndExt-1)<< endmsg;

        // In case the output track selection is not filled yet
	// add the tracks of the selected z-sample
	if ( m_trackSelection.output()->size() < minTracksToAccept() ) {
	  // Need the following, to handle both the increasing and decreasing sorting
	  double zSelMin = std::min( *indStartExt, *(indEndExt-1) ) - 0.001;
	  double zSelMax = std::max( *indStartExt, *(indEndExt-1) ) + 0.001;
	  const Hlt::TSelection<LHCb::Track>* BGtracks = m_trackSelection.input<1>();
	  for ( Hlt::TSelection<LHCb::Track>::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) {
	    if ( (*itT) != NULL ) {
	      if ((*itT)->hasStateAt(LHCb::State::ClosestToBeam)) {
		double zOfTrack = (*itT)->stateAt(LHCb::State::ClosestToBeam)->z();
		if (zOfTrack > zSelMin && zOfTrack < zSelMax) m_trackSelection.output()->push_back( *itT );
	      }
	    }
	  }
	  debug() << "Filled outputSelection. Size = " << m_trackSelection.output()->size() << endmsg;
	  debug() << "Z Sel Min and Max = " << zSelMin << " / " << zSelMax << endmsg;
	}
      }
    }

    // make sure we go forward and don't hang around forever...
    debug() << "\n================\nEnd of loop #" << iloop << "\nMain Step Starting index = " << indStartMS-begin << endmsg;
    assert(indStartMS>begin); 

  }//END while loop

}//END definition of findProtoVertex()


//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasProtoVertex::execute() {
  debug() << "==> Execute" << endmsg;

  const Hlt::TSelection<LHCb::Track>* BGtracks = m_trackSelection.input<1>();
  debug() << "Number of tracks in the BG Tracks Container = " << BGtracks->size() << endmsg;
  debug() << "Number of objects in outputSelection = " << m_trackSelection.output()->size() << endmsg;

  if ( BGtracks->size() > m_maxNumOfVeloTracks ) {
    debug() << "Rejecting the event because of too many tracks" << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Loop over the tracks and fill the vector with the Z positions (use the tracks state ClosestToBeam)
  // Write down only the tracks which pass the z-position cuts (can be different for the different lines)
  std::vector<double> vectZPos;
  for ( Hlt::TSelection<LHCb::Track>::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) {
    if ( (*itT) != NULL ) {
      if ( (*itT)->hasStateAt(LHCb::State::ClosestToBeam) ) {
	const LHCb::State* firstState = (*itT)->stateAt(LHCb::State::ClosestToBeam);
	if ( passZCuts(firstState->z()) ) vectZPos.push_back( firstState->z() );
      }
    }
  }
  debug() << "Number of tracks passing the z-cut = " << vectZPos.size()  << endmsg;

  // In case there are not enough tracks : reject the event
  if (vectZPos.size() < minTracksToAccept()) {
    debug() << "Rejecting the event because of not enough tracks poassing the z-cut" << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // sort the vector with the z positions
  std::sort(vectZPos.begin(), vectZPos.end());
  if (msgLevel(MSG::DEBUG)) { printVector(vectZPos.begin(),vectZPos.end(), "========== All Good Z =========="); }

  //------------------------------------------------------------------
  // Execute the proto-vertex searching function.
  // If ascending sorted pass is not successful try with descenig sort
  //------------------------------------------------------------------
  debug() << "\n\n=================== Running with sorted vector (1) ===========================" << endmsg;
  findProtoVertex( vectZPos.begin(), vectZPos.end() );
  if ( m_trackSelection.output()->size() < minTracksToAccept() ) {
    debug() << "\n\n=================== Running with reverse_iterator (2) ===========================" << endmsg;
    findProtoVertex( vectZPos.rbegin(), vectZPos.rend() );
  }

  return StatusCode::SUCCESS;
}
