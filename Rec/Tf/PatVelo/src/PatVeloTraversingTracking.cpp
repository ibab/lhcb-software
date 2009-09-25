// $Id: PatVeloTraversingTracking.cpp,v 1.4 2009-09-25 16:45:35 gersabec Exp $
// Include files
#include "PatVeloTraversingTracking.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

namespace Tf {
  DECLARE_ALGORITHM_FACTORY( PatVeloTraversingTracking );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Tf::PatVeloTraversingTracking::
PatVeloTraversingTracking( const std::string& name,
			   ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  ,  m_veloDet ( 0 )
{
  //Theta criterion
  declareProperty( "ThetaCut",            m_thetaCut=0.01);
  //Sigma criterion
  declareProperty( "SigmaCut",            m_sigmaCut=1);
  //Impact parameter cut in mm
  declareProperty( "IPCut",               m_IPCut=4*Gaudi::Units::mm);
  //Include tracks from luminous region?
  declareProperty( "IncludeLuminousRegion", m_includeLR=false);
  //Initial guess of opening. Needed to exclude luminous region
  declareProperty( "DistanceGuess",       m_distanceGuess=0);
  //Theta criterion
  declareProperty( "MakeHistogram",       m_makeHistogram=false);
  //Input location of tracks
  declareProperty( "InputTracksLocation" ,
		   m_inputTracksLocation = LHCb::TrackLocation::Velo );
  //Output location of merged track
  declareProperty( "OutputTracksLocation" ,
		   m_outputTracksLocation = "Rec/Track/Traversing" );
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode Tf::PatVeloTraversingTracking::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_debugLevel = msgLevel( MSG::DEBUG ) ;
  m_verboseLevel = msgLevel( MSG::VERBOSE ) ;

  if(m_debugLevel) debug() << "==> Initialize" << endmsg;
  setHistoTopDir( "Velo/" );

  m_halfDistance = book1D("HalfDistance", "distance between velo halfs", 0, 5, 100 );

  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Tf::PatVeloTraversingTracking::finalize() {

  if(m_debugLevel==true) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Tf::PatVeloTraversingTracking::execute() {

  if(m_debugLevel==true) debug() << "==> Execute" << endmsg;

  if ( m_debugLevel ) {
    debug()<< "==> getTrackContainer() "
	   << "Retrieving " << m_inputTracksLocation << endmsg;
  }
  if ( !exist<LHCb::Tracks>(m_inputTracksLocation ) ) {
    return Error(" ----  No track container retrieved from " +
		 m_inputTracksLocation, StatusCode::FAILURE);
  }

  const LHCb::Tracks * trackContainer=get<LHCb::Tracks>(m_inputTracksLocation);
  if(m_debugLevel) debug() << "---- VeloTrack Container retrieved from "
			   << m_inputTracksLocation << endmsg;

  double distance=0;
  TraversingTrackSet set;
  std::vector< std::vector<LHCb::Track*> > traversingTrackContainer;
  findTracks(distance,set,trackContainer,traversingTrackContainer);

  //Book place in TES:
  LHCb::Tracks* outputTracks;
  if ( exist<LHCb::Tracks>( m_outputTracksLocation ) ) {
    outputTracks = get<LHCb::Tracks>( m_outputTracksLocation );
  } else {
    outputTracks = new LHCb::Tracks();
    put(outputTracks, m_outputTracksLocation );
  }

  //Plot x-distance and save to TES:
  if(distance>-900){ // less than "magic" -999 number
    if(m_debugLevel) debug() << "Distance " << distance << endmsg;
    if(m_makeHistogram) plot1D(distance, "x_distance",
			       "Distance between velo halves [mm]",
			       -1+0.05,20+0.05,210);
    if(m_makeHistogram) m_halfDistance->fill(distance);
    saveTrackInTES(set.mergedTrack());
    traversingTrackContainer.clear();
    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }
 
  traversingTrackContainer.clear();
  setFilterPassed(false);

  return StatusCode::SUCCESS;
}

//==========================================================================
// Method that calculates the signed impact parameter between a track and
// the z-axis
//==========================================================================
double Tf::PatVeloTraversingTracking::impactParameter(LHCb::Track *track)
{

  Gaudi::XYZVector slope = track->slopes();
  Gaudi::XYZVector zaxis(0,0,1);
  Gaudi::XYZVector projvector = slope.Cross( zaxis ).Unit();

  Gaudi::XYZPoint position = track->position();
  Gaudi::XYZVector connector(position.x(),position.y(),position.z());

  return projvector.Dot(connector ) ; // signed distance
}

//==========================================================================
// Main method. Finds traversing tracks based on the 3 criteria listed in
// the class description
//==========================================================================
void Tf::PatVeloTraversingTracking::
findTracks(double &distance,
	   TraversingTrackSet& set,
	   const LHCb::Tracks * trackContainer,
	   std::vector< std::vector<LHCb::Track*> > & traversingTrackContainer)
{
  distance = -999; // "magic" value if no track pairs

  if(trackContainer->size()<2) return;

  /* STEP 1: SORT TRACK CONTAINER W.R.T THETA. USE STL SORT BY STORING THETA
     AND TRACK IN PAIRS*/
  std::vector< std::pair<double, LHCb::Track*> > sortedTrackVector;

  LHCb::Tracks::const_iterator itTrg;
  for( itTrg=trackContainer->begin(); itTrg!=trackContainer->end(); ++itTrg){
    LHCb::Track* track = (*itTrg);
    Gaudi::XYZVector slopes = track->slopes();
    sortedTrackVector.push_back( std::make_pair(slopes.theta(), track) );
  }
  // then use first of pair (theta) to sort
  std::sort(sortedTrackVector.begin(),sortedTrackVector.end());

  /* STEP2: Find combinations of tracks with small delta_theta: */
  std::vector<LHCb::Track*> traversingTrackCandidates;
  // start loop at 2nd position
  std::vector<std::pair<double, LHCb::Track*> >::iterator it = 
    sortedTrackVector.begin()+1;
  for(; it!=sortedTrackVector.end();++it){

    LHCb::Track* track = (*it).second;
    LHCb::Track* previous_track = (*(it-1)).second;
    double theta = (*it).first;
    double previous_theta = (*(it-1)).first;

    if(fabs(theta-previous_theta)<m_thetaCut){
      if(traversingTrackCandidates.size() == 0){
	traversingTrackCandidates.push_back(previous_track);
      }
      traversingTrackCandidates.push_back(track);
    } else {
      if(traversingTrackCandidates.size()!=0){
	traversingTrackContainer.push_back(traversingTrackCandidates);
	traversingTrackCandidates.clear();
      }
    }
  }
  if(traversingTrackCandidates.size()!=0){
    traversingTrackContainer.push_back(traversingTrackCandidates);
    traversingTrackCandidates.clear();
  }

  /* STEP3: Calculate sigma (measure of deviation from parallel lines)
     for all combinations of tracks with small delta_theta */
  std::vector< double > trackX, trackY;
  std::vector <LHCb::Track*> IP_vector;
  std::vector< std::vector<LHCb::Track*> >::iterator it_container;
  for( it_container = traversingTrackContainer.begin();
       it_container != traversingTrackContainer.end(); ++it_container){

    double bestSigmaX=1000000;
    std::vector<LHCb::Track*> candidates = *it_container;
    if(m_verboseLevel) verbose() << "Looping over candidates" << endmsg;
    // first candidate
    std::vector<LHCb::Track*>::iterator it_candidate0;
    for (it_candidate0 = candidates.begin();
	 it_candidate0!=candidates.end();++it_candidate0){
      
      LHCb::Track* track0 = *it_candidate0;

      double x0 = track0->position().x();
      double y0 = track0->position().y();
      double z0 = track0->position().z();
      double tx0 = track0->slopes().x();
      double ty0 = track0->slopes().y();

      int sensorID0 = track0->lhcbIDs()[0].veloID().sensor();
      bool track0half = m_veloDet->sensor(sensorID0)->isLeft();
      double xcorr=0;
      if(track0half == true){
	xcorr= m_distanceGuess/2;}
      else{
	xcorr= -1*m_distanceGuess/2;
      }

      trackX.clear();
      trackY.clear();

      // next candidate
      std::vector<LHCb::Track*>::iterator it_candidate1;
      for( it_candidate1 = it_candidate0 + 1;
	   it_candidate1 != candidates.end() ; ++it_candidate1){

	LHCb::Track* track1 = *it_candidate1;

	//Check if the tracks are in different halves:
	int sensorID1 = track1->lhcbIDs()[0].veloID().sensor();
	bool track1half = m_veloDet->sensor(sensorID1)->isLeft();
	if( track1half == track0half) continue;

	double x1 = track1->position().x();
	double y1 = track1->position().y();
	double z1 = track1->position().z();
	double tx1 = track1->slopes().x();
	double ty1 = track1->slopes().y();

	double average_z = (z0+z1)/2;

	double average_x = 0;
	double average_y = 0;
	//Extrapolate track candidates to 5 points in z. 
	//Calculate sigma to check how parallel the tracks are in X:
	for(double Z=average_z-800;Z<=average_z+800;Z+=400){
	  // unsigned dist in x between tracks at Z
	  double X = fabs((x0+tx0*(Z-z0)) - (x1+tx1*(Z-z1)));
	  double Y = fabs((y0+ty0*(Z-z0)) - (y1+ty1*(Z-z1)));
	  trackX.push_back(X);
	  trackY.push_back(Y);
	  average_x +=X;
	  average_y +=Y;
	}

	average_x /= trackX.size();
	average_y /= trackY.size();

	double sigmaX =  0;
	for(std::vector<double>::iterator iter=trackX.begin();
	    iter!=trackX.end();++iter){
	  sigmaX += pow(*iter - average_x,2);
	}
	sigmaX = sqrt(sigmaX/trackX.size());

	double sigmaY =  0;
	for(std::vector<double>::iterator iter=trackY.begin();
	    iter!=trackY.end();++iter){
	  sigmaY += pow(*iter - average_y,2);
	}
	sigmaY = sqrt(sigmaY/trackY.size());

	// If tracks are parallel (sigma < sigmaCut), 
	// check if they come from the luminous region:
	if ( ( sigmaX < m_sigmaCut ) && ( sigmaY < m_sigmaCut ) ) {
	  IP_vector.clear();
	  IP_vector.reserve(2);	
	  IP_vector.push_back(track0->clone());
	  IP_vector.push_back(track1->clone());

	  bool IPflag = false;
	  for(std::vector<LHCb::Track* >::iterator IP_it = IP_vector.begin();
	      IP_it!=IP_vector.end();++IP_it){
	    LHCb::Track* track = (*IP_it);
	    if(IP_it == IP_vector.begin())
	      track->position().SetX(track->position().x() +xcorr);
	    else
	      track->position().SetX(track->position().x() -xcorr);

	    double IP = fabs(impactParameter(*IP_it));
	    if(IP < m_IPCut){ IPflag=true; }
	  }
	  if(sigmaX<bestSigmaX){
	    if(m_includeLR==true || (m_includeLR==false && IPflag==false)){
	      set.Init(average_x,sigmaX,track0,track1);
	      distance = average_x;
	      bestSigmaX = sigmaX;
	    }

	  } // closes 	  if(sigma<bestSigma)

	} // closes 	if(sigma<m_sigmaCut)

      } //closes loop over candidate1

    } //closes loop over candidate0

  } //closes loop over all candidates
  trackX.clear();
  IP_vector.clear();

  return;
}

void Tf::PatVeloTraversingTracking::saveTrackInTES(LHCb::Track* track){
  if(m_debugLevel==true) debug() << "==> saveTrackInTES()" << endmsg;
  LHCb::Tracks * outputTracks;
  if ( exist<LHCb::Tracks>( m_outputTracksLocation ) ) {
    outputTracks = get<LHCb::Tracks>( m_outputTracksLocation );
  } else {
    outputTracks = new LHCb::Tracks();
    put(outputTracks, m_outputTracksLocation );
  }
  outputTracks->reserve(outputTracks->size() + 1);
  outputTracks->insert(track);

  return;
}


