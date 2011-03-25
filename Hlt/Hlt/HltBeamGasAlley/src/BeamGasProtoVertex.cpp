// from Gaudi 
#include "GaudiKernel/AlgFactory.h"
// event model
#include "Event/Track.h"
// local
#include "BeamGasProtoVertex.h"
// for std::accumulate
#include <numeric>

//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasProtoVertex
// 2008-06-16 : Plamen Hopchev <plamen.hopchev@cern.ch>
// 2011-27-02 : Plamen Hopchev <plamen.hopchev@cern.ch>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasProtoVertex );

typedef std::vector<double> VectorD;
typedef std::vector<Gaudi::XYZPoint> Vector3DPoints;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasProtoVertex::BeamGasProtoVertex(const std::string& name, ISvcLocator* pSvcLocator) 
  : HltAlgorithm(name, pSvcLocator, false)
  , m_trackSelection(*this)
{
  m_trackSelection.declareProperties();
  declareProperty( "OutputSelection", m_outputSelectionName = name );
  declareProperty( "zTracksMin", m_zTrMin = -1200.);
  declareProperty( "zTracksMax", m_zTrMax =  1200.);
  declareProperty( "zTracksExcludeLumiRegionLow", m_zTrExclLRLow = 0.);
  declareProperty( "zTracksExcludeLumiRegionUp",  m_zTrExclLRUp  = 0.);
  declareProperty( "MinimumNumberOfTracks", m_minNumTracks = 8);
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

  // "stepping" parameters
  // m_minNumTracks is a property (job-option) to be set externally
  m_maxNumExtends = 10; //hard fixed
  // "sigma bad" parameters (hard fixed)
  m_sigmaBadSlope  = 56.e-3;
  m_sigmaBadConst1 = 44.;
  m_sigmaBadConst2 = 16.;
  m_degradationFactor = 0.2;

  assert(m_minNumTracks>1);
  if (msgLevel(MSG::DEBUG)) { 
          debug() << "==> Initialize\n" << endreq 
          << "========== Algorithm parameters ========="   << endreq
          << "OutputSelection = " << m_outputSelectionName << endreq
          << "zTracksMin      = " << m_zTrMin              << endreq
          << "zTracksMax      = " << m_zTrMax              << endreq
          << "zTracksExcludeLumiRegionLow = " << m_zTrExclLRLow << endreq
          << "zTracksExcludeLumiRegionUp  = " << m_zTrExclLRUp  << endreq
          << "MinimumNumberOfTracks       = " << m_minNumTracks << endreq
          << "MinTracksToAccept = " << minTracksToAccept() << endreq << "\n" << endreq;
  }
  return StatusCode::SUCCESS;
};


//=============================================================================
// Define Member Functions
//=============================================================================


namespace {
    struct m012 {
        m012(): m0(0),m1(0),m2(0) {}
        void operator()(double x) { ++m0;m1+=x;m2+=x*x; }
        double m0,m1,m2;
    };
}

//### function to calculate the mean and sigma of list of z values
template <typename ITER>
void BeamGasProtoVertex::getMeanAndSigma(ITER begin, ITER end , double& sMean, double& sSigma) const {
  double sSize = end-begin;
  if (!sSize) { debug() << "Function getMeanAndSigma received empty vector" << endmsg; }
  else {
    m012 moments = std::for_each( begin,end, m012() );
    sMean  = moments.m1 / moments.m0;
    sSigma = sqrt( moments.m2/moments.m0 - sMean*sMean );
  }
  debug() << "\nz values properties:"   << "\n   Entries = " << sSize
          << "\n   Mean    = " << sMean << "\n   Sigma   = " << sSigma << "\n" << endmsg;
}

//### function to calculate the z-dependent "bad sigma"
double BeamGasProtoVertex::sigmaBad(double z) const {
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
  // Loop "Main Step" on listZ until no more steps can be made
  //----------------------------------------------------------
  int iloop = 0;
  while (indStartMS + m_minNumTracks <= end) { // minNumTracks or minTracksToaccept !!! #2  Note: <= should be < I think...
    ++iloop; 
    debug() << " loop # " << iloop << endmsg;
    assert(indStartMS+stepSize1()<=end);

    if (msgLevel(MSG::DEBUG)) printVector(indStartMS, indStartMS+stepSize1(), "*** Main Step ***");

    double   mean_sample  = -9999.;
    double   sigma_sample = -9999.;
    getMeanAndSigma(indStartMS, indStartMS+stepSize1(), mean_sample, sigma_sample);
    debug() << "Mean And Sigma: " << mean_sample << " / " << sigma_sample << endmsg;

    double SIGMA_BAD = sigmaBad(mean_sample);
    debug() << "###SigmaBad is " << SIGMA_BAD << endmsg;

    if (sigma_sample > SIGMA_BAD || sigma_sample < -9998.) {
      debug() << "Sigma is too big. Starting new Main Step ..." << endmsg;
      indStartMS += stepSize1();
      continue;
    }    
    //----------------------------------------------------------
    // ### LOOP to look for extension in the FWD direction
    //----------------------------------------------------------
    debug() << "Sigma is good. Starting to extend ..." << endmsg;
    double sigma_MIN = sigma_sample;
    double sigma_ext = -9999.;
    double mean_ext  = -9999.;
    ITER indEndExt = end;

    for (unsigned iExtend=1; iExtend<m_maxNumExtends; ++iExtend){
      bool AccOK = false;
      debug() << "====== Extend FWD iteration "<< iExtend << " ======" << endmsg;
      indEndExt = indStartMS+stepSize1() + iExtend*stepSize2();
      debug() << "indEndExt = " << indEndExt-begin << endmsg;
      if (indEndExt+1 >= end ) {  // why the +1 ??
        debug() << "Reached Last Element - Breaking FWD Extension Loop ...\nMoving starting index one S2 back" << endmsg;
        indEndExt -= stepSize2();
        break;
      }
      // now that we are sure that there are enough elements 
      // append stepSize2 values to the sample vector
      assert( indEndExt <=end );
      assert( indEndExt > indStartMS );
      if (msgLevel(MSG::DEBUG)) printVector(indStartMS, indEndExt, "*** After Extension ***");

      getMeanAndSigma(indStartMS,indEndExt, mean_ext, sigma_ext);
      debug() << "Extension Mean And Sigma: " << mean_ext << " / " << sigma_ext << endmsg;
  
      if (sigma_ext < sigma_MIN) {
        debug() << "Setting New sigma_MIN" << endmsg;
        sigma_MIN = sigma_ext;
        AccOK = true;
      } else if (sigma_ext < (sigma_MIN + m_degradationFactor*SIGMA_BAD)) {
        debug() << "Ext. degrades sigma, but within " << m_degradationFactor*SIGMA_BAD << endmsg;
        AccOK = true;
      } else {    
        debug() << "New sigma not good. Breaking FWD Extenstion Search \nMoving starting index one S2 back" << endmsg;
        indEndExt -= stepSize2();
        break;
      }

      //----------------------------------------------------------
      // Condition to Accept the Event
      //----------------------------------------------------------
      debug() << "Currently #tracks in the proto vertex = " << std::distance(indStartMS,indEndExt) << endmsg;
      if ( std::distance(indStartMS, indEndExt) > minTracksToAccept() && AccOK) {
        debug() << "\n\nEVENT ACCEPTED" << endmsg;
        debug() << "Track Indices of found Peak (first/last) : " << std::distance(begin,indStartMS) <<  " / " << std::distance(begin,indEndExt) << endmsg;
        debug() << "Their z positions : " << *indStartMS <<  " / " << *indEndExt << endmsg;

        // Fill the output_selection. Write down the tracks of the selected z-sample
        // need the following, to handle both the increasing and decreasing sorting
        double zSelMin = std::min( *indStartMS, *indEndExt ) - 1.;
        double zSelMax = std::max( *indStartMS, *indEndExt ) + 1.;
        const Hlt::TSelection<LHCb::Track>* BGtracks = m_trackSelection.input<1>();
        for ( Hlt::TSelection<LHCb::Track>::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) {
            if ((*itT)->hasStateAt(LHCb::State::ClosestToBeam)) {
                double zOfTrack = (*itT)->stateAt(LHCb::State::ClosestToBeam)->z();
                if (zOfTrack > zSelMin && zOfTrack < zSelMax) m_trackSelection.output()->push_back( *itT );
            }
        }
        debug() << "Filled outputSelection. Size = " << m_trackSelection.output()->size() << endmsg;
        debug() << "Z Sel Min and Max = " << zSelMin << " / " << zSelMax << endmsg;

        //In order to stop the Main Steps Loop:
        indEndExt = end;
        break; //to skip next extension steps
      }

    }//END for loop (trying extensions)

    // After the Extension loop is over set the starting point up to where we reached after the extensions
    assert(indEndExt>indStartMS); // make sure we go forward and don't hang around forever...
    indStartMS = indEndExt;
    debug() << "\nExtension Loop is over. sigma_min / sigma_last = " << sigma_MIN << " / " << sigma_ext << endmsg;
    debug() << "Index starting position = " << std::distance(begin,indStartMS) << " with Last Possible Ind = " << std::distance(begin,end)-1 << "\n\n" << endmsg;

  }//END while loop

}//END definition of findProtoVertex()


//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasProtoVertex::execute() {
  debug() << "==> Execute" << endmsg;

  const Hlt::TSelection<LHCb::Track>* BGtracks = m_trackSelection.input<1>();
  debug() << "Number of tracks in the BG Tracks Container = " << BGtracks->size() << endmsg;

  VectorD vectZPos;
  vectZPos.reserve(BGtracks->size());
  
  // Loop over the tracks and fill the vector with the XYZ positions; use the tracks state ClosestToBeam
  // Write down only the tracks which pass the z-position cuts (different for the different lines)
  for ( Hlt::TSelection<LHCb::Track>::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) {
    if ((*itT)->hasStateAt(LHCb::State::ClosestToBeam)) {
      const LHCb::State* firstState = (*itT)->stateAt(LHCb::State::ClosestToBeam);
      if ( passZCuts(firstState->z()) ) vectZPos.push_back( firstState->z() );
    }
  }

  // In case there are not enough tracks -> reject the event (minNumTracks or minTracksToaccept!!!)
  if (vectZPos.size() < minTracksToAccept()) {
    debug() << "Rejecting the event because of not enough good tracks" << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Get a 1D vector holding the z-positions (second arg of the function is the coordinate)
  debug() << " Z vector size = " << vectZPos.size()  << endmsg;

  //-------------------------------------------------------------
  // Execute the proto-vertex searching function twice - 
  // once with sorted vector and once with inversly sorted vector
  //-------------------------------------------------------------

  // ascending sort
  std::sort(vectZPos.begin(), vectZPos.end());

  //for (int i=0; i<v0.size(); ++i){ std::cout << v0[i] << "   " << vs1[i] << "   " << vs2[i] << std::endl; }
  if (msgLevel(MSG::DEBUG)) { printVector(vectZPos.begin(),vectZPos.end(), "======================= All Good Z ============================"); }

  // OOPS: we fill m_trackSelection.input<1> twice, once in the ascending, and once in the descending case...
  debug() << "\n\n=================== Running First Time (ascending sort) ===========================" << endmsg;
  findProtoVertex( vectZPos.begin(), vectZPos.end() );
  debug() << "\n\n=================== Running Second Time (descending sort) ===========================" << endmsg;
  findProtoVertex( vectZPos.rbegin(), vectZPos.rend() );

  // Now fill the output selection (tracks) with the 

  /// !!! instead of setting filter passed we need to fill the output selection container !!!, like

  return StatusCode::SUCCESS;
}
