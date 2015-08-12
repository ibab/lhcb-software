// Include files

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// RichKernel
#include "RichKernel/RichHPDIdentifier.h"
// local
#include "RichParticleSearchMain.h"

// from Gaudi
#include "Event/RecVertex.h"
#include "GaudiAlg/GaudiTool.h"
#include "Event/MCHeader.h"

//namespaces

using namespace Rich::Rec::MC;
using std::vector;
//-----------------------------------------------------------------------------
// Implementation file for class : RichParticleSearchMain
//
// 2010-09-22 : Matt Coombes
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichParticleSearchMain )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichParticleSearchMain::RichParticleSearchMain( const std::string& name,
		ISvcLocator* pSvcLocator)
: TupleAlgBase ( name , pSvcLocator ),
  m_trSelector        ( NULL ),
  m_richRecMCTruth    ( NULL ),
  m_richPartProp      ( NULL ),
  m_ckAngle           ( NULL ),
  m_isoTrack          ( NULL ),
  m_tkIndex     ( NULL ),
  m_MuonInformation   ( NULL ),
  m_signal     ( NULL )
{

	// Maximum number of tracks
	declareProperty( "MaxRichRecTracks", m_maxUsedTracks = 200 );
	declareProperty( "UseMCTruth",    m_useMCTruth   = false );
	declareProperty( "Radiator",   m_radTemp = 2 ); // default is Rich2
	declareProperty( "HistoOutputLevel", m_histoOutputLevel = 0 );
	//               (Aerogel) (R1Gas) (R2Gas)
	declareProperty( "MinIsolationCut",     m_IsoCut       = 100, "IsolationCuts for each Radiator in mm");
	declareProperty( "MaxCK_Sig",       m_maxCKcut   = 1,"How many sigma larger than resolution allowed");

	//Cut on Histograms and NTuples
	declareProperty( "CKDevCut",    m_CKDevCut = 0.5, "CK Standard Deviation Cut" );
	declareProperty( "MinCK",     m_minCK = 0.0, "Minimum CK agnle for each radiator");
	declareProperty("PlotPhotons",    m_plotPerPhoton =false, "Enter second photon loop");
	declareProperty( "MinTrackMass",    m_minTrackMass = 0.0, "Lower Mass Cut" );
	declareProperty( "MaxPhotons",    m_maxPhotons = 150, "upper number of photons Cut" );
	declareProperty( "MinPhotons",    m_minPhotons = 0, "lower number of photons Cut" );


	//Use Muon Information
	declareProperty("UseMuonInfo",   m_useMuonInfo = false, "Use Muon PID information");
	declareProperty("findYields",   findYields = false, "FindPhotonYields");

}
//=============================================================================
// Destructor
//=============================================================================
RichParticleSearchMain::~RichParticleSearchMain() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichParticleSearchMain::initialize() {
	StatusCode sc = TupleAlgBase::initialize(); // must be executed first
	if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

	if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

	acquireTool( "RichCherenkovAngle",   m_ckAngle  );
	// get track selector
	acquireTool( "TrackSelector", m_trSelector, this );
	acquireTool( "RichIsolatedTrack", m_isoTrack     );
	acquireTool( "RichTrackEffectiveRefIndex", m_tkIndex   );
	acquireTool( "RichExpectedTrackSignal", m_signal       );


	if ( m_useMCTruth ) {
		if ( msgLevel(MSG::DEBUG) ) debug()<<"Use MC in ParticleSearch"<<endmsg;
		acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
		acquireTool( "RichParticleProperties", m_richPartProp );
	}

	m_MuonInformation = tool<IMuonInformation> ("MuonInformation");
	EvtNum =0;
	m_tkTotal = 0;

	m_radiator = static_cast<Rich::RadiatorType>(m_radTemp);

	if ( msgLevel(MSG::DEBUG) ) {
		debug()<<"IsoCuts "<<m_IsoCut<<endmsg;
		debug()<<"MaxRichRecTracks "<<m_maxUsedTracks<<endmsg;
		debug() << "Radiator:" << m_radiator << " " << m_radTemp << endmsg;
	}

	m_particleType =2;//Pion

	// m_pType = static_cast<Rich::ParticleIDType>(m_particleType);
	//debug() << "Fixed particle type:" << m_pType << endmsg;


	const std::string RAD = Rich::text(m_radiator);

	if (m_useMuonInfo){
		StatusCode scM = m_MuonInformation->initialize();
	}

	if ( msgLevel(MSG::DEBUG) ) debug() << "Finished Initialization" << endmsg;
	return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode RichParticleSearchMain::execute() {

	if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

	// Check Status
	if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

	// If any containers are empty, form them
	if ( richTracks()->empty() ) {
		if ( trackCreator()->newTracks().isFailure() )
		{
			return Error( "Problem Making Tracks" );
		}
		if ( msgLevel(MSG::DEBUG) )
			debug () << "No tracks found : Created " << richTracks()->size()
			<< " RichRecTracks " << richSegments()->size()
			<< " RichRecSegments" << endmsg;
	}
	if ( msgLevel(MSG::DEBUG) )
	{
		debug() << " Found " << richTracks()->size() << " tracks" << endmsg;
	}
	if ( (int)richTracks()->size() > m_maxUsedTracks )
	{
		if ( msgLevel(MSG::DEBUG) )
			debug() << "Found " << richTracks()->size() << ">"
			<< m_maxUsedTracks << " max usable tracks, stopping." << endmsg;
		return StatusCode::SUCCESS;
	}

	if ( richPixels()->empty() ) {
		if ( pixelCreator()->newPixels().isFailure() )
		{
			return Error( "Problem Making Pixels" );
		}
		if ( msgLevel(MSG::DEBUG) )
			debug() << "No Pixels found : Created "
			<< richPixels()->size() << " RichRecPixels" << endmsg;
	}

	if ( richPhotons()->empty() ) {
		photonCreator()->reconstructPhotons().ignore();
		if ( msgLevel(MSG::DEBUG) )
			debug() << "No photons found : Created "
			<< richPhotons()->size() << " RichRecPhotons" << endmsg;
	}
	int trackCounter =0;
	if ( msgLevel(MSG::DEBUG) ) debug()<< "Event "<<EvtNum<<endmsg;

	//Get int for RICH; 0 Aerogel; 1 RICH1Gas; 2 Rich2Gas
	const int RICHint = GetRichInt();

	// Resolution for RICH radiators in mrads
	std::vector<double> RichRes;
	RichRes.push_back(0.00310);
	RichRes.push_back(0.00166);
	RichRes.push_back(0.00068);

	// Variance Cuts
	std::vector<double> RichPhotonCut;
	RichPhotonCut.push_back(0.15);
	RichPhotonCut.push_back(0.018);
	RichPhotonCut.push_back(0.01);


	EvtNum++;
	if ( msgLevel(MSG::DEBUG) ) debug()<< "Event "<<EvtNum<<endmsg;

	// =========================================================================//
	// Iterate over segments
	for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
			iSeg != richSegments()->end(); ++iSeg )
	{
		LHCb::RichRecSegment* segment = *iSeg;
		if ( msgLevel(MSG::DEBUG) ) debug()<< "trackCounter "<<trackCounter<<endmsg;

		// Saturated CK angle for segment
		double SatCK = m_ckAngle->saturatedCherenkovTheta(segment);
		// Maximum allowed angle
		double maxCK = SatCK + RichRes[RICHint]*m_maxCKcut;
		double minCK = SatCK - RichRes[RICHint]*m_maxCKcut;

		// Radiator info
		const Rich::RadiatorType rad = segment->trackSegment().radiator();
		double pathLength = segment->trackSegment().pathLength();
		if ( m_radiator != rad ) continue;
		const std::string RAD = Rich::text(rad);

		// track selection
		if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

		trackCounter++;
		m_tkTotal++;

		// ========================================================================================== //
		//         MC information             //
		int MCtype(-1);
		if ( m_useMCTruth ) {
			const ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
			if ( msgLevel(MSG::DEBUG) ) debug()<< "mcType: "<<mcType<<endmsg;
			//assign nmber to each type of true particle to store in NTuple
			MCtype = (int) mcType;
		}//end if use truth

		// ========================================================================================== //

		m_pType = static_cast<Rich::ParticleIDType>(m_particleType);

		// Momentum information
		double pt=0.0;
		double momentum = std::sqrt(segment->trackSegment().bestMomentum().Mag2());

		int numberOfUsedTracks = 0;

		const LHCb::RichRecTrack * richtrack = segment->richRecTrack();
		const LHCb::Track* track = dynamic_cast<const LHCb::Track*>(richtrack->parentTrack());

		const double trChi2 = track->chi2();

		//track key added by VietNga
		const int trackKey = track->key();

		const int TrackType = (int) track->type();

		numberOfUsedTracks=(int)richTracks()->size();
		const LHCb::RichTrackSegment & trackSegment = segment->trackSegment();
		pt = std::sqrt( std::pow(trackSegment.bestMomentum().x(),2) +
				std::pow(trackSegment.bestMomentum().y(),2) );

		// Get refractive index of radiator required for mass calculations
		double refIndx = m_tkIndex->refractiveIndex(segment);

		const Gaudi::XYZPoint trkentrypoint = segment->trackSegment().entryPoint();
		const Gaudi::XYZPoint trkexitpoint = segment->trackSegment().exitPoint();
		double trkenX = trkentrypoint.X();
		double trkenY = trkentrypoint.Y();
		double trkexX = trkexitpoint.X();
		double trkexY = trkexitpoint.Y();
		double rad_length = trkexitpoint.Z() - trkentrypoint.Z();

		// Separation or isolation of tracks
		double MinimumTrackSeperation = this->MinimumTrackSeparation(segment);
		if (MinimumTrackSeperation>m_IsoCut)
		{

			// Loop over photons to make photon Cut of maximum number of photons per track etc
			// ============================================================================================//
			int PhotonCounter =0;
			int TruePhotonCounter =0;
			int TrueParentAndRadCounter = 0;
			double thetaRecSum = 0.0;
			std::vector<double> CKTheta;
			std::vector<double> trackMass;

			// ============================================================================================//
			//        Photon LOOP               //
			for ( LHCb::RichRecSegment::Photons::const_iterator iPhot2 = segment->richRecPhotons().begin();
					iPhot2 != segment->richRecPhotons().end(); ++iPhot2 )
			{
				LHCb::RichRecPhoton* photon2 = *iPhot2;

				const LHCb::RichGeomPhoton & gPhoton2 = photon2->geomPhoton();
				const LHCb::RichRecPixel * pixel  = photon2->richRecPixel();
				// Get the local coords of the photon detection point (correted for radiator type)

				const double thetaRec2 = gPhoton2.CherenkovTheta();
				double beta2 = 1.0/(std::cos(thetaRec2)*refIndx);
				if (thetaRec2 <= maxCK && beta2 >=1.0/refIndx && thetaRec2 > m_minCK){
					PhotonCounter++;
					CKTheta.push_back(thetaRec2);
					thetaRecSum = thetaRecSum+thetaRec2;
				}

				// Check that photon is from track using truth information
				bool trueParent( false );
				if ( m_useMCTruth ) {
					trueParent = ( NULL != m_richRecMCTruth->trueCherenkovPhoton( photon2 ) );
				}
				// Check that CK is from given radiator
				bool trueRadiator( false );
				if ( m_useMCTruth )
				{
					trueRadiator = ( NULL != m_richRecMCTruth->trueCherenkovRadiation( pixel, rad ) );
				}

				// Only true if radiation is from true parent (track)
				if (trueParent){
					TruePhotonCounter++;
					if (trueRadiator)
					{
						TrueParentAndRadCounter++;
					}
				}

				if (PhotonCounter>m_maxPhotons)
				{
					break;
				} //END photon cut
			}// end photon counter loop
			//         END Photon LOOP               //
			// ============================================================================================//

			if (PhotonCounter>m_minPhotons && PhotonCounter<m_maxPhotons-1)
			{
				double AvThetaRec = thetaRecSum/PhotonCounter; // Average CK per track

				if (AvThetaRec > m_minCK) //  lower cut on average CK
				{
					// Setup Sort by largest variance first
					MattTest mt;
					mt.SetAvTheta(AvThetaRec);
					std::stable_sort(CKTheta.begin(), CKTheta.end(), MattTestOb); // sort in order to remove furthest out-lyers first

					int CKcounter = 0;
					double CKVariance = 0.0;
					double thetaRecSumUpdate = 0.0; //Updated after outlyer removals
					int UsedPhotons = 0; //Updated after outlyer removals
					for (vector<double>::iterator it=CKTheta.begin(); it < CKTheta.end(); ++it){
						// A method to remove outlyers
						if (fabs(*it- AvThetaRec) > 500*RichPhotonCut[RICHint])
						{
							CKcounter++;
							CKTheta.erase(it);// Remove the element outside variance range
							// update ThetaRecAv for next removal
							AvThetaRec = (AvThetaRec*(PhotonCounter - CKcounter +1) - *it)/(PhotonCounter- CKcounter);
							it--;// in order to cover whole range
						}
						else
						{
							UsedPhotons++;
							CKVariance = CKVariance + std::pow((*it- AvThetaRec),2);//Standard deviation squared of CK Theta
						}
						thetaRecSumUpdate = *it + thetaRecSumUpdate;

					}// end iterator

					AvThetaRec = thetaRecSumUpdate/UsedPhotons;
					PhotonCounter = UsedPhotons; // Update photon counter
					// Average beta calculated per track
					double avBetaTk = 1.0/(std::cos(AvThetaRec)*refIndx);
					// Average Mass calculated per track
					double avTrackMass = momentum * std::sqrt((1/(std::pow(avBetaTk,2)))-1.0);

					// CK Standard Deviation per track
					double stdDevCK = std::pow(CKVariance/(PhotonCounter-1), 0.5);


					if (((AvThetaRec-stdDevCK)/AvThetaRec)>m_CKDevCut
							&& avTrackMass>m_minTrackMass )
					{ // Cut on CK standard Deviation weighted by mean CK per track

						if ( m_histoOutputLevel > 1 )
						{
							//Make Plots per track
							plot1D( avTrackMass, "TrackMass", "Mass per Track", 0, 45000, 9000 );
							// Plot inverse Mass per Track
							plot1D( 1/avTrackMass, "InverseMass", "Inverse Mass per track", 0, 0.0008, 8000 );
						}

						//Get Any Muon Information
						int hasMuonPID = 0;
						if (m_useMuonInfo)
						{
							hasMuonPID = m_MuonInformation->HasMuonInformation(track);
						}

						std::vector<float> trackProjection;

						if (findYields)
						{
							const Gaudi::XYZPoint & tkPtLocal = segment->LHCb::RichRecSegment::pdPanelHitPointLocal();

							trackProjection.push_back(tkPtLocal.x());
							trackProjection.push_back(tkPtLocal.y());
							trackProjection.push_back(tkPtLocal.z());
						}

						if ( produceNTuples())
						{
							Tuple TrackTuple = nTuple( "TrackTuple", "Rich Alignment" );
							TrackTuple->column( "avThetaRec"      ,    AvThetaRec     );
							TrackTuple->column( "tkNum",trackCounter);
							TrackTuple->column( "pt"         , pt             );
							TrackTuple->column( "refIndx"       , refIndx        );
							TrackTuple->column( "momentum"      , std::sqrt(segment->trackSegment().bestMomentum().Mag2()));
							TrackTuple->column( "nTracks"       ,  numberOfUsedTracks  );
							TrackTuple->column( "MinimumTrackSeperation" , MinimumTrackSeperation );
							TrackTuple->column( "EvtNum", EvtNum);
							TrackTuple->column( "PhotonsPerTrack", PhotonCounter);
							TrackTuple->column( "averageTrackMass", avTrackMass);
							TrackTuple->column( "stdDevCK", stdDevCK);
							TrackTuple->column( "TrackType", TrackType);
							TrackTuple->column( "pathLength", pathLength);
							TrackTuple->column( "trChi2", trChi2);
							if (findYields){
								TrackTuple->column( "TrackProjectionX", trackProjection[0]);
								TrackTuple->column( "TrackProjectionY", trackProjection[1]);
								TrackTuple->column( "TrackProjectionZ", trackProjection[2]);
								// Photon Yield Info
								TrackTuple->column( "trkenX",trkenX);
								TrackTuple->column( "trkenY",trkenY);
								TrackTuple->column( "trkexX",trkexX);
								TrackTuple->column( "trkexY",trkexY);
							}

							TrackTuple->column( "rad_length",rad_length);


							if ( m_useMCTruth )
							{ // if use Truth information


								//runNumber and eventNumber added by Viet Nga
								const LHCb::MCHeader* header = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
								unsigned long long runNumber = header->runNumber();
								unsigned long long evtNumber = header->evtNumber();

								TrackTuple->column( "trackKey", trackKey);
								TrackTuple->column( "runNumber", (unsigned int)runNumber);
								TrackTuple->column( "evtNumber", (unsigned int)evtNumber);
								TrackTuple->column( "MCParticleType", MCtype);
								TrackTuple->column( "TruePhotonsPerTrack", TruePhotonCounter);
								// TrackTuple->column( "TruePhotonsPerTrackTrueRad",TrueParentAndRadCounter);
							}

							if (m_useMuonInfo)
							{ // If use Muon information
								TrackTuple->column( "hasMuonPID"    , hasMuonPID   );
							}
							TrackTuple->write();
						}//End produce NTuple

						//============================================================================================//
						//      Second Photon LOOP (very CPU intensive        //

						if (m_plotPerPhoton)
						{
							for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
									iPhot != segment->richRecPhotons().end(); ++iPhot )
							{
								LHCb::RichRecPhoton* photon = *iPhot;
								const LHCb::RichGeomPhoton & gPhoton = photon->geomPhoton();

								// Cherenkov angles
								const double thetaRec = gPhoton.CherenkovTheta();
								// Get true track parent if using MCTruth
								bool trueParent2( false );
								if ( m_useMCTruth ) {
									trueParent2 = ( NULL != m_richRecMCTruth->trueCherenkovPhoton( photon ) );
								}
								//track Beta = v/c
								double beta = 1.0/(cos(thetaRec)*refIndx);
								// Mass from photon track pair
								double mass = momentum * std::sqrt((1/(std::pow(beta,2)))-1.0);
								if (thetaRec < maxCK && thetaRec > minCK && beta > 1.0/refIndx && thetaRec > m_minCK)
								{ // Make sure these are same cuts as in first photon loop

									if ( m_histoOutputLevel > 1 ){
										//Make Plots per track
										plot1D( avTrackMass, "MassperPhoton", "Mass per Photon", 0, 45000, 90000 );
										plot1D( 1/avTrackMass, "InverseMassPerPhoton", "Inverse Mass per photon", 0, 0.0008, 8000);
									}

									double Var = fabs(thetaRec- AvThetaRec);
									if (Var < RichPhotonCut[RICHint])
									{
										if ( produceNTuples())
										{

											Tuple photonTuple = nTuple( "massTuplePhoton", "Rich Alignment" );
											photonTuple->column( "tkNum",trackCounter);
											photonTuple->column( "mass"          , mass           );
											photonTuple->column( "TrueParent", trueParent2 );
											photonTuple->column( "Variance", Var );
											photonTuple->column( "EvtNum", EvtNum);
											photonTuple->write();
										}
									}
								}

							}//end photon loop
						}//End if m_plotPhotons
					}//End CKDev Cut
				}// end Average CK cut
			}// end cut on photons
		}// End isolation Cuts
	}//end loop on track segements


	return StatusCode::SUCCESS;
}

//============================================================================


double RichParticleSearchMain::MinimumTrackSeparation(LHCb::RichRecSegment* segment){

	// Minimum track separation initialised as large value
	double MinimumTrackSeperation = 999999999.0;

	// Radiator info
	const Rich::RadiatorType rad = segment->trackSegment().radiator();

	// Hit point of current segment on HPD plane in local coords
	const Gaudi::XYZPoint & tkPtLocal = segment->LHCb::RichRecSegment::pdPanelHitPointLocal();

	//Loop over second Segments
	for ( LHCb::RichRecSegments::const_iterator iSeg2 = richSegments()->begin();
			iSeg2 != richSegments()->end(); ++iSeg2 )
	{
		LHCb::RichRecSegment* segment2 = *iSeg2;
		if ( segment != segment2 ) // check not comparing the segment with itself
		{
			if ( segment2->trackSegment().radiator() == rad )// radiator for segment2 == radiator for segment
			{
				// track hit centre of all other tracks (segment2) on HPD plane
				const Gaudi::XYZPoint & tkPtLocal2 = segment2->LHCb::RichRecSegment::pdPanelHitPointLocal();
				// Compare segment hit point to all other (segment2) hit points
				const double Xdifsq      = std::pow( fabs(tkPtLocal.x()-tkPtLocal2.x()), 2 );
				const double Ydifsq      = std::pow( fabs(tkPtLocal.y()-tkPtLocal2.y()), 2 );
				const double Zdifsq      = std::pow( fabs(tkPtLocal.z()-tkPtLocal2.z()), 2 );
				const double centreXYdif = std::sqrt(Xdifsq+Ydifsq+Zdifsq);

				//Change minimum separation if track separation is less than previous minimum
				if ( centreXYdif < MinimumTrackSeperation )
				{
					MinimumTrackSeperation = centreXYdif;
				}
			}  // end segment comparison loop
		} // End segment != segment2
	}
	return MinimumTrackSeperation;
}
