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
  m_stepSize1 = (m_minNumTracks+1)/2;
  m_stepSize2 = (m_minNumTracks)/4;
  m_minTracksToAccept = m_minNumTracks - m_stepSize2;
  m_maxNumExtends = 10; //hard fixed
  // "sigma bad" parameters (hard fixed)
  m_sigmaBadSlope  = 56.e-3;
  m_sigmaBadConst1 = 44.;
  m_sigmaBadConst2 = 16.;
  m_degradationFactor = 0.2;

  if (msgLevel(MSG::DEBUG)) { 
          debug() << "==> Initialize\n" << endreq 
          << "========== Algorithm parameters ========="   << endreq
          << "OutputSelection = " << m_outputSelectionName << endreq
          << "zTracksMin      = " << m_zTrMin              << endreq
          << "zTracksMax      = " << m_zTrMax              << endreq
          << "zTracksExcludeLumiRegionLow = " << m_zTrExclLRLow << endreq
          << "zTracksExcludeLumiRegionUp  = " << m_zTrExclLRUp  << endreq
          << "MinimumNumberOfTracks       = " << m_minNumTracks << endreq
          << "MainStepSize = "      << m_stepSize1 << endreq
          << "ExtensionStepSize = " << m_stepSize2 << endreq
          << "MinTracksToAccept = " << m_minTracksToAccept << endreq << "\n" << endreq;
  }
  return StatusCode::SUCCESS;
};


//=============================================================================
// Define Member Functions
//=============================================================================

//### function to get 1D vector out of a vector of XYZ points
VectorD BeamGasProtoVertex::get1DVector(const Vector3DPoints& vect3DPoints, int coord) const {
  VectorD vector1D;
  vector1D.reserve( vect3DPoints.size() );
  for( Vector3DPoints::const_iterator pIter=vect3DPoints.begin(); pIter!=vect3DPoints.end(); ++pIter) {
    if      (coord == 0) vector1D.push_back( pIter->x() );
    else if (coord == 1) vector1D.push_back( pIter->y() );
    else if (coord == 2) vector1D.push_back( pIter->z() );
  }
  return vector1D;
}

//### function to calculate the mean and sigma of list of z values
void BeamGasProtoVertex::getMeanAndSigma(const VectorD& zValues, double& sMean, double& sSigma) const {
  double sSize = zValues.size();
  if (!sSize) { debug() << "Function getMeanAndSigma received empty vector" << endmsg; }
  else {
    // calculate the sum and the mean
    double sum_vals = std::accumulate(zValues.begin(), zValues.end(), 0.);
    sMean = sum_vals / sSize;
    // calculate the standard deviation
    double sum_distToMeanSquare = 0.;
    for( VectorD::const_iterator zIter=zValues.begin(); zIter!=zValues.end(); ++zIter) {
      sum_distToMeanSquare += pow((*zIter - sMean),2);
    }
    sSigma = sqrt(sum_distToMeanSquare/sSize);
  }
  debug() << "\nz values properties:"   << "\n   Entries = " << sSize
          << "\n   Mean    = " << sMean << "\n   Sigma   = " << sSigma << "\n" << endmsg;
  return;
}

//### function to calculate the z-dependent "bad sigma"
double BeamGasProtoVertex::sigmaBad(double z) const {
  return (z < 250) ? (-1*m_sigmaBadSlope*z + m_sigmaBadConst1) : (m_sigmaBadSlope*z + m_sigmaBadConst2) ;
}


void BeamGasProtoVertex::printVector(const VectorD& vectZ, const std::string& theText) const {
  debug() << theText << "\nVector Size = " << vectZ.size() << "  Contents:" << "\n";
  for(unsigned i=0; i<vectZ.size(); ++i) debug() << "   " << vectZ[i] << "\n"; 
  debug() << endmsg;
}

//### function to look for a proto vertex
bool BeamGasProtoVertex::findProtoVertex(const VectorD& zValues) {

  //Init
  VectorD::const_iterator indStartMS = zValues.begin();
  assert(m_stepSize1>0);
  VectorD  vect_sample;
  vect_sample.reserve(m_minNumTracks+m_stepSize1);
  bool boolDecision = false;

  //----------------------------------------------------------
  // Loop "Main Step" on listZ until no more steps can be made
  //----------------------------------------------------------
  int iloop = 0;
  while (indStartMS + m_minNumTracks <= zValues.end()) { // minNumTracks or minTracksToaccept !!! #2  Note: <= should be < I think...
    ++iloop; 
    debug() << " loop # " << iloop << endmsg;
    assert(indStartMS>=zValues.begin());
    assert(indStartMS+m_stepSize1<=zValues.end());

    vect_sample.assign( indStartMS, indStartMS+m_stepSize1);
    if (msgLevel(MSG::DEBUG)) printVector(vect_sample, "*** Main Step ***");

    assert(!vect_sample.empty());
    double   mean_sample  = -9999.;
    double   sigma_sample = -9999.;
    getMeanAndSigma(vect_sample, mean_sample, sigma_sample);
    debug() << "Mean And Sigma: " << mean_sample << " / " << sigma_sample << endmsg;

    double SIGMA_BAD = sigmaBad(mean_sample);
    debug() << "###SigmaBad is " << SIGMA_BAD << endmsg;

    if (sigma_sample > SIGMA_BAD || sigma_sample < -9998.) {
      debug() << "Sigma is too big. Starting new Main Step ..." << endmsg;
      indStartMS += m_stepSize1;
      continue;
    }    
    //----------------------------------------------------------
    // ### LOOP to look for extension in the FWD direction
    //----------------------------------------------------------
    debug() << "Sigma is good. Starting to extend ..." << endmsg;
    double sigma_MIN = sigma_sample;
    double sigma_ext = -9999.;
    double mean_ext  = -9999.;
    VectorD::const_iterator indEndExt = zValues.end();
    unsigned numProtoVtxTracks = 0;

    assert(indStartMS>=zValues.begin());
    assert(indStartMS+m_stepSize1<=zValues.end());

    for (unsigned iExtend=1; iExtend<m_maxNumExtends; ++iExtend){
      bool AccOK = false;
      debug() << "====== Extend FWD iteration "<< iExtend << " ======" << endmsg;
      indEndExt = indStartMS+m_stepSize1 + iExtend*m_stepSize2;
      debug() << "indEndExt = " << indEndExt-zValues.begin() << endmsg;
      if (indEndExt+1 >= zValues.end() ) {  // why the +1 ??
        debug() << "Reached Last Element - Breaking FWD Extension Loop ...\nMoving starting index one S2 back" << endmsg;
        indEndExt -= m_stepSize2;
        break;
      }
      // now that we are sure that there are enough elements 
      // append m_stepSize2 values to the sample vector
      assert( indStartMS+m_stepSize1 >= zValues.begin() );
      assert( indStartMS+m_stepSize1+m_stepSize2 <= zValues.end() );
      vect_sample.insert(vect_sample.end(),  indStartMS+m_stepSize1, indStartMS+m_stepSize1+m_stepSize2) ;
      if (msgLevel(MSG::DEBUG)) printVector(vect_sample, "*** After Extension ***");

      numProtoVtxTracks = vect_sample.size();
      getMeanAndSigma(vect_sample, mean_ext, sigma_ext);
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
        indEndExt -= m_stepSize2;
        break;
      }

      //----------------------------------------------------------
      // Condition to Accept the Event
      //----------------------------------------------------------
      debug() << "Currently #tracks in the proto vertex = " << numProtoVtxTracks << endmsg;
      if (numProtoVtxTracks > m_minTracksToAccept && AccOK) {
        debug() << "\n\nEVENT ACCEPTED" << endmsg;
        debug() << "Track Indices of found Peak (first/last) : " << indStartMS-zValues.begin() <<  " / " << indEndExt-zValues.begin() << endmsg;
        debug() << "Their z positions (1): " << *indStartMS <<  " / " << *indEndExt << endmsg;
        debug() << "Their z positions (2): " << vect_sample.front() <<  " / " << vect_sample.back() << endmsg;

        // Fill the output_selection. Write down the tracks of the selected z-sample
        // need the following, to handle both the increasing and decreasing sorting
        double zSelMin = std::min( vect_sample.front(), vect_sample.back() ) - 1.;
        double zSelMax = std::max( vect_sample.front(), vect_sample.back() ) + 1.;
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
        indEndExt = zValues.end();
        break; //to skip next extension steps
      }

    }//END for loop (trying extensions)

    // After the Extension loop is over set the starting point up to where we reached after the extensions
    assert(indEndExt>indStartMS); // make sure we go forward and don't hang around forever...
    indStartMS = indEndExt;
    debug() << "\nExtension Loop is over. sigma_min / sigma_last = " << sigma_MIN << " / " << sigma_ext << endmsg;
    debug() << "Index starting position = " << indStartMS-zValues.begin() << " with Last Possible Ind = " << zValues.size()-1 << "\n\n" << endmsg;


  }//END while loop

  return boolDecision;
}//END definition of findProtoVertex()



//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasProtoVertex::execute() {
  debug() << "==> Execute" << endmsg;

  const Hlt::TSelection<LHCb::Track>* BGtracks = m_trackSelection.input<1>();
  debug() << "Number of tracks in the BG Tracks Container = " << BGtracks->size() << endmsg;

  Vector3DPoints vect3DPos;
  vect3DPos.reserve(400);
  
  // Loop over the tracks and fill the vector with the XYZ positions; use the tracks state ClosestToBeam
  // Write down only the tracks which pass the z-position cuts (different for the different lines)
  for ( Hlt::TSelection<LHCb::Track>::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) {
    if ((*itT)->hasStateAt(LHCb::State::ClosestToBeam)) {
      const LHCb::State* firstState = (*itT)->stateAt(LHCb::State::ClosestToBeam);
      if ( passZCuts(firstState->z()) ) vect3DPos.push_back( firstState->position() );
    }
  }

  // In case there are not enough tracks -> reject the event (minNumTracks or minTracksToaccept!!!)
  if (vect3DPos.size() < m_minTracksToAccept) {
    debug() << "Rejecting the event because of not enough good tracks" << endmsg;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Get a 1D vector holding the z-positions (second arg of the function is the coordinate)
  VectorD vectZPos = get1DVector(vect3DPos, 2);
  debug() << "3D vector size = " << vect3DPos.size() << endmsg;
  debug() << " Z vector size = " << vectZPos.size()  << endmsg;

  //-------------------------------------------------------------
  // Execute the proto-vertex searching function twice - 
  // once with sorted vector and once with inversly sorted vector
  //-------------------------------------------------------------

  VectorD vectZPosAscending = vectZPos;
  std::sort(vectZPosAscending.begin(), vectZPosAscending.end());

  VectorD vectZPosDescending = vectZPos;
  std::sort(vectZPosDescending.begin(), vectZPosDescending.end(), std::greater<double>());

  //for (int i=0; i<v0.size(); ++i){ std::cout << v0[i] << "   " << vs1[i] << "   " << vs2[i] << std::endl; }
  if (msgLevel(MSG::DEBUG)) { printVector(vectZPosAscending, "======================= All Good Z ============================"); }

  debug() << "\n\n=================== Running First Time (ascending sort) ===========================" << endmsg;
  bool acc1 = findProtoVertex( vectZPosAscending );
  debug() << "\n\n=================== Running Second Time (descending sort) ===========================" << endmsg;
  bool acc2 = findProtoVertex( vectZPosDescending );
  debug() << "\n\n=================== DECISIONS = " << acc1 << " / " << acc2 << " ===========================" << endmsg;

  // Now fill the output selection (tracks) with the 

  /// !!! instead of setting filter passed we need to fill the output selection container !!!, like

  return StatusCode::SUCCESS;
}  

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BeamGasProtoVertex::finalize() {
  return HltAlgorithm::finalize();
}




