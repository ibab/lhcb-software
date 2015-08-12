// $Id: TrackCheckerNT.cpp,v 1.14 2009-10-20 07:47:21 mschille Exp $
// Include files 

// local
#include "TrackCheckerNT.h"

#include <boost/foreach.hpp>

// from Event/TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"

// from Event/MCEvent
#include "Event/MCVertex.h"

// from Event/DigiEvent
#include "Kernel/OTChannelID.h"
#include "Event/OTLiteTime.h"
#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

// from Tr/TrackFitEvent
#include "Event/STMeasurement.h"
#include "Event/OTMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

// Detector stuff
#include <OTDet/DeOTDetector.h>
#include <OTDet/DeOTModule.h>
#include <STDet/DeSTDetector.h>
#include <STDet/DeSTLayer.h>
#include <VeloDet/DeVelo.h>
#include <OTDAQ/IOTRawBankDecoder.h>

// STL sets
#include <set>
#include <algorithm>

#include "gsl/gsl_cdf.h"

#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

using namespace Gaudi;
using namespace Gaudi::Units;
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackCheckerNT )

typedef std::vector<float> Array;
typedef std::vector<Array> Matrix;

// In this source file, I use the C preprocessor quite a bit, which may
// may seem unconventional for c++ code these days. I did this to avoid
// typing very similar things very often, and because one tends to lose
// the context if one has several screens full of variable definitions
// (e.g. array which are to be filled in)
// Generally, I'm against this type of obfuscation, but it seemed sensible
// to do it to stress the important parts of the code by making less
// important things occupy less space...
//
// I'd also like to leave in the multiplication/division by units (e.g.
// mm) even when the conversion factor happens to be 1.0, just to make
// sure that it's absolutely clear in which units the variables in the
// NTuple were written...
// 
// - Manuel

// the following is used to monitor track states at fixed z positions and
// at the true vertex which will be measurement 0, the fixed z positions will
// come afterwards - the positions to monitor can be set via option files
static const unsigned int FixedZ = (unsigned int) 42;
// we won't monitor more than 10 fixed z positions (including the true vertex)
static const int maxFixedZ = 10;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCheckerNT::TrackCheckerNT( const std::string& name,
                            ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg( name , pSvcLocator ),
  m_linkerInTable(""), 
  m_nMCEvt(0),
  m_nEvt(0), 
  m_maxTracks(0),
  m_maxMCParticles(0)
{

  // default z-positions
  m_zPositions.clear();
  m_zPositions.push_back( StateParameters::ZBegRich1 );
  m_zPositions.push_back( StateParameters::ZEndRich1 );
  m_zPositions.push_back( StateParameters::ZBegRich2 );
  m_zPositions.push_back( StateParameters::ZEndRich2 );
  
  declareProperty( "TracksInContainer",
                   m_tracksInContainer = TrackLocation::Default  );
  declareProperty( "LinkerInTable",   m_linkerInTable = ""       );
  declareProperty( "ZPositions",      m_zPositions               );
  
  // if you change these, you have to think of changing the header files
  // of the ROOT macros as well - the defaults should be fine, though
  declareProperty("maxTracks", m_maxTracks = 1000);
  declareProperty("maxMCParticles", m_maxMCParticles = 10000);
  declareProperty("OTDecoderName", m_otDecoderName = "OTRawBankDecoder");

  // warn the user if he tries to monitor more fixed z positions than we
  // write to our ntuple...
  if (m_zPositions.size() > (maxFixedZ - 1)) {
    warning() << "You are trying to monitor more fixed Z positions than "
      "I'm going to write to the ntuple, I'll write the first few - "
      "please investigate!" << endmsg;
  }
}

//=============================================================================
// Destructor
//=============================================================================
TrackCheckerNT::~TrackCheckerNT() {}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackCheckerNT::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  info() << "maxTracks = " << m_maxTracks << " maxMCParticles = " <<
	  m_maxMCParticles << endmsg;

  // Set the path for the linker table Track - MCParticle
  if ( m_linkerInTable == "" ) m_linkerInTable = m_tracksInContainer;


  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator"       );
  m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator" );
  m_projectorSelector = tool<ITrackProjectorSelector>( "TrackProjectorSelector",
                                          "Projector", this );
  m_otdecoder = tool<IOTRawBankDecoder>(m_otDecoderName);
  
  // get number of channels per detector - this may be interesting
  // if you look at occupancies, for example, so save the user from having
  // to do the counting by hand...
  // we do not save this to the NTuple because it changes so infrequently,
  // but we print it nevertheless...
  DeVelo *Velo = getDet<DeVelo>(DeVeloLocation::Default);
  DeSTDetector *TT = getDet<DeSTDetector>(DeSTDetLocation::TT);
  DeSTDetector *IT = getDet<DeSTDetector>(DeSTDetLocation::IT);
  DeOTDetector *OT = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if ((0 == Velo) || (0 == TT) || (0 == IT) || (0 == OT))
	  error() << "Can't retrieve one of the detector components" << endmsg;
  long nstrips = 0;
  for (std::vector<DeVeloSensor*>::const_iterator it = Velo->sensorsBegin();
       it != Velo->sensorsEnd(); ++it)
    nstrips += (*it)->numberOfStrips();
  info() << "number of channels in Velo: " << nstrips << endmsg;
  info() << "number of channels in   TT: " << TT->nStrip() << endmsg;
  info() << "number of channels in   IT: " << IT->nStrip() << endmsg;
  info() << "number of channels in   OT: " << OT->nChannels() << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackCheckerNT::execute()
{  
  StatusCode sc;
  // Get the Tracks
  Tracks* tracks = get<Tracks>( m_tracksInContainer );

  // Get the MCParticles
  MCParticles* particles = get<MCParticles>( MCParticleLocation::Default );
  
  // Retrieve the Linker table
  LinkedTo<MCParticle,Track> directLink( evtSvc(), msgSvc(), m_linkerInTable );
  if ( directLink.notFound() ) {
    error() << "Linker table not found" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve the reverse direction Linker table
  LinkedFrom<Track,MCParticle> revLink( evtSvc(), msgSvc(), m_linkerInTable );

  // ok, build mappings from Track* and MCParticle* to matrix rows
  // make sure we can tell if a Track or MCParticle did not end up in our
  // ntuple due to the fixed array/matrix size constraint which is present
  // here in Brunel
  int i = 0;
  std::map<Track*, int> trackIdx;
  {
    Tracks::const_iterator iTrack = tracks->begin();
    for(; (iTrack != tracks->end()) && (i < m_maxTracks); ++iTrack )
      trackIdx[*iTrack] = i++;
    for(; iTrack != tracks->end(); ++iTrack )
      trackIdx[*iTrack] = -2; // Track is available but can not be saved
  }
  trackIdx[(Track*) 0] = -1;
  std::map<MCParticle*, int> mcPartIdx;
  i = 0;
  {
    MCParticles::const_iterator mcp = particles->begin();
    for(; (mcp != particles->end()) && (i < m_maxMCParticles); ++mcp)
      mcPartIdx[*mcp] = i++;
    for(; mcp != particles->end(); ++mcp)
      mcPartIdx[*mcp] = -2; // MCParticle is available but can not be saved
  }
  mcPartIdx[(MCParticle*) 0] = -1;
  
  // book tuple
  Tuples::Tuple tuple = nTuple(4242, "Events");

  // ok, save the number of MCParticles and Tracks we have here in Brunel,
  // and also save the number of MCParticles/Tracks saved to the NTuple
  tuple->column("nMCParticles", particles->size());
  tuple->column("nSavedMCParticles", std::min((long)particles->size(), m_maxMCParticles));
  tuple->column("nTracks", tracks->size());
  tuple->column("nSavedTracks", std::min((long)tracks->size(), m_maxTracks));
  
  // warn if we lose Tracks_MCParticles
  if ((long)particles->size() > m_maxMCParticles)
    warning() << "Event has " << particles->size() << " MCParticles, "
      "saving only " << m_maxMCParticles << " to avoid NTuple array/matrix "
      "overflows." << endmsg;
  if ((long)tracks->size() > m_maxTracks)
    warning() << "Event has " << tracks->size() << " Tracks, "
      "saving only " << m_maxTracks << " to avoid NTuple array/matrix "
      "overflows." << endmsg;
  // write number of channels hit per subdetector
  VeloClusters* VeloCont = get<VeloClusters>("Raw/Velo/Clusters");
  STClusters *TTCont = get<STClusters>("Raw/TT/Clusters");
  STClusters *ITCont = get<STClusters>("Raw/IT/Clusters");
  tuple->column("nVeloHits", VeloCont->size());
  tuple->column("nTTHits", TTCont->size());
  tuple->column("nITHits", ITCont->size());
  tuple->column("nOTHits", m_otdecoder->totalNumberOfHits());

  // handle the tracks in our event
  // the loop over tracks is done in the different methods
  
  // fill in "global" track parameters (e.g. track type, chi^2, ndof,
  // associated MCParticles, ...)
  fillGlobalTrackParameters(tracks, mcPartIdx, directLink, tuple);

  // fill in figures needed for hit purity/efficiency/ghost rate
  sc = fillHitPurEff(tuple, tracks, directLink);
  if (sc.isFailure()) return sc;

  // fill in track parameters at the measurements in subdetectors
  // for all tracks in our event
#define fDTPAM fillDetTrackParametersAtMeasurements
  fDTPAM(maxFixedZ, FixedZ              , tracks, mcPartIdx, directLink, tuple);
  fDTPAM(25       , Measurement::VeloR  , tracks, mcPartIdx, directLink, tuple);
  fDTPAM(25       , Measurement::VeloPhi, tracks, mcPartIdx, directLink, tuple);
  fDTPAM(12       , Measurement::TT     , tracks, mcPartIdx, directLink, tuple);
  fDTPAM(20       , Measurement::IT     , tracks, mcPartIdx, directLink, tuple);
  fDTPAM(32       , Measurement::OT     , tracks, mcPartIdx, directLink, tuple);
#undef fDTPAM
    
  // done with the tracks, do the MCParticles now
  sc = fillMCGeneralInfo(tuple, trackIdx, mcPartIdx, particles, revLink);
  if (sc.isFailure()) return sc;

  // ok, now we have a look at the hits in the detector caused by
  // MCParticles - this is more involved, so we split it off into a
  // seperate method
  sc = fillMCHitStatistics(tuple, particles);
  if (sc.isFailure()) return sc;
  
  // ok, write tuple row
  tuple->write();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackCheckerNT::finalize()
{
  return GaudiHistoAlg::finalize();
}

//=============================================================================
//
//=============================================================================

//=============================================================================
//  Ok, here come all the little helpers
//=============================================================================

// fill a track state into a given Array
// this format is used for saving the first state on a track so we
// don't have to look through all subdetectors to find it (this
// simplifies quick cuts on e.g. momentum quite a bit)
void TrackCheckerNT::fillTrackState(
    Array& statevec,
    LHCb::State &st)
{
  // base units used to save data are mm and GeV, respectively
  // ok, save track state
  statevec[ 0] = (float) (st.z() / mm);
  statevec[ 1] = (float) (st.x() / mm);
  statevec[ 2] = (float) (st.y() / mm);
  statevec[ 3] = (float) (st.tx());
  statevec[ 4] = (float) (st.ty());
  statevec[ 5] = (float) (st.qOverP() * GeV);
  // now comes the covariance matrix
  statevec[ 6] = (float) (st.covariance()(0,0) / mm / mm);
  statevec[ 7] = (float) (st.covariance()(0,1) / mm / mm);
  statevec[ 8] = (float) (st.covariance()(0,2) / mm);
  statevec[ 9] = (float) (st.covariance()(0,3) / mm);
  statevec[10] = (float) (st.covariance()(0,4) / mm * GeV);
  statevec[11] = (float) (st.covariance()(1,1) / mm / mm);
  statevec[12] = (float) (st.covariance()(1,2) / mm);
  statevec[13] = (float) (st.covariance()(1,3) / mm);
  statevec[14] = (float) (st.covariance()(1,4) / mm * GeV);
  statevec[15] = (float) (st.covariance()(2,2));
  statevec[16] = (float) (st.covariance()(2,3));
  statevec[17] = (float) (st.covariance()(2,4) * GeV);
  statevec[18] = (float) (st.covariance()(3,3));
  statevec[19] = (float) (st.covariance()(3,4) * GeV);
  statevec[20] = (float) (st.covariance()(4,4) * GeV * GeV);
}

// get MCParticle associated with a Measurement
MCParticle* TrackCheckerNT::getMCParticle(const Measurement *mm)
{
  // try to avoid work when there's nothing to do
  if (0 == mm)
    return 0;

  // try to get MCParticle belonging to Measurement
  if (mm->checkType(Measurement::VeloPhi)) {
    const VeloPhiMeasurement *m = dynamic_cast<const VeloPhiMeasurement*>(mm);
    LinkedTo<MCParticle, VeloCluster> veloLink(evtSvc(),msgSvc(),
	LHCb::VeloClusterLocation::Default);
    if ( veloLink.notFound() )
      error() << "Unable to retrieve VeloCluster-MCParticle linker table" << endmsg;
    else
      return veloLink.first(m->cluster());
  } else if (mm->checkType(Measurement::VeloR)) {
    const VeloRMeasurement *m = dynamic_cast<const VeloRMeasurement*>(mm);
    LinkedTo<MCParticle, VeloCluster> veloLink(evtSvc(),msgSvc(),
	LHCb::VeloClusterLocation::Default);
    if ( veloLink.notFound() )
      error() << "Unable to retrieve VeloCluster-MCParticle linker table" << endmsg;
    else
      return veloLink.first(m->cluster());
  } else if (mm->checkType(Measurement::OT)) {
    const OTMeasurement *m = dynamic_cast<const OTMeasurement*>(mm);
    LinkedTo<MCParticle> otLink(evtSvc(),msgSvc(),
	LHCb::OTTimeLocation::Default);
    if ( otLink.notFound() )
      error() << "Unable to retrieve OTTime-MCParticle linker table" << endmsg;
    else
      return otLink.first(m->channel());
  } else if (mm->checkType(Measurement::TT)) {
    const STMeasurement *m = dynamic_cast<const STMeasurement*>(mm);
    LinkedTo<MCParticle, STCluster> ttLink(evtSvc(),msgSvc(),
	LHCb::STClusterLocation::TTClusters);
    if ( ttLink.notFound() )
      error() << "Unable to retrieve TT STCluster-MCParticle linker table" << endmsg;
    else
      return ttLink.first(m->cluster());
  } else if (mm->checkType(Measurement::IT)) {
    const STMeasurement *m = dynamic_cast<const STMeasurement*>(mm);
    LinkedTo<MCParticle, STCluster> itLink(evtSvc(),msgSvc(),
	LHCb::STClusterLocation::ITClusters);
    if ( itLink.notFound() )
      error() << "Unable to retrieve IT STCluster-MCParticle linker table" << endmsg;
    else 
      return itLink.first(m->cluster());
  }
  // ok, we were unable to get a pointer to the particle, so we say so...
  return 0;
}

// get the MeasType for a Measurement
// a MeasType will tell if the measurement is of x, u, v, r, phi flavour
// we have several methods to find this piece of information, one
// for Measurements (just below), and for OTTimes/STClusters (a bit
// further down)
TrackCheckerNT::MeasType TrackCheckerNT::getMeasType(const Measurement* m)
{
  // we use a threshold to determine if we are in a stereo layer
  // at the moment, this roughly corresponds to 2.5 degrees
  // the idea is that slightly rotated stereo layers can't fool us with
  // the thresholding method (this may be an issue with a misaligned
  // detector)
  const double stereoThreshold = 0.04363323; // radians
  // ok, extract the different types from the measurement
  if (Measurement::OT == m->type()) {
    const OTMeasurement *otm = dynamic_cast<const OTMeasurement*>(m);
    if (otm->stereoAngle() < -stereoThreshold)
      return V;
    if (otm->stereoAngle() > stereoThreshold)
      return U;
    return X;
  }
  if ((Measurement::TT == m->type()) ||
      (Measurement::IT == m->type())) {
    const STMeasurement *stm = dynamic_cast<const STMeasurement*>(m);
    return getMeasType(stm->cluster());
  }
  if (Measurement::VeloR == m->type()) 
    return R;
  if (Measurement::VeloPhi == m->type()) 
    return Phi;
  
  return Unknown;
}

// get the MeasType for STClusters
TrackCheckerNT::MeasType TrackCheckerNT::getMeasType(const STCluster* stc)
{
  // we use a threshold to determine if we are in a stereo layer
  // at the moment, this roughly corresponds to 2.5 degrees
  // the idea is that slightly rotated stereo layers can't fool us with
  // the thresholding method (this may be an issue with a misaligned
  // detector)
  const double stereoThreshold = 0.04363323; // radians
  
  DeSTDetector *ST;
  if (stc->isTT())
    // it's a TT cluster
    ST = getDet<DeSTDetector>(DeSTDetLocation::TT);
  else
    // must be a IT cluster
    ST = getDet<DeSTDetector>(DeSTDetLocation::IT);
  if (0 == ST) {
	error() << "Can't retrieve ST detector geometry." << endmsg;
	return Unknown;
  }
  DeSTLayer *l = ST->findLayer(stc->channelID());
  if (l->angle() < -stereoThreshold)
    return V;
  if (l->angle() > stereoThreshold)
    return U;
  return X;
}

// get the MeasType for OTTimes
TrackCheckerNT::MeasType TrackCheckerNT::getMeasType(const OTChannelID ott)
{
  // we use a threshold to determine if we are in a stereo layer
  // at the moment, this roughly corresponds to 2.5 degrees
  // the idea is that slightly rotated stereo layers can't fool us with
  // the thresholding method (this may be an issue with a misaligned
  // detector)
  const double stereoThreshold = 0.04363323; // radians

  DeOTDetector *OT = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if (0 == OT) {
	error() << "Can't retrieve OT detector geometry." << endmsg;
	return Unknown;
  }
  DeOTModule *m = OT->findModule(ott);
  if (m->angle() < -stereoThreshold)
    return V;
  if (m->angle() > stereoThreshold)
    return U;
  return X;
}

// fill the NTuple with "global" track parameters, i.e. track type,
// state at first measurement, associated MCParticles, chi^2, ndof, etc.
StatusCode TrackCheckerNT::fillGlobalTrackParameters(
    Tracks *tracks,
    std::map<MCParticle*, int>& mcPartIdx,
    LinkedTo<MCParticle, Track>& directLink,
    Tuples::Tuple& tuple)
{
  int nTracks = std::min(m_maxTracks, (long)tracks->size());
  // ok, now we declare the vectors to hold our data
  Array type(nTracks, -1.);	// track type
  Array chi2(nTracks, -1.);	// chi square
  Array nDoF(nTracks, -1.);	// degrees fo freedom
  Array Pchi2(nTracks, -1.);	// chi sqare probability
  Array History(nTracks, -1.);	// track history
  // we save no more than 10 associated MCParticles for each track
  const int maxAssocMCParticles = 10;
  // associated MC particles (idx into ntuple MCParticles, negative if unav.)
  Matrix assocMCParts(nTracks, Array(maxAssocMCParticles, -1.));
  // weights of assoc. MC particles (as above)
  Matrix assocMCPartWeights(nTracks, Array(maxAssocMCParticles, -1.));
  // first (reconstructed) state of the track (for quick cuts on p etc.)
  Matrix firstState(nTracks, Array(21, -1.));
  
  // we loop over all tracks in our event
  int nTrack = 0;
  for (Tracks::const_iterator t = tracks->begin();
      (t != tracks->end()) && (nTrack < nTracks); ++t, ++nTrack) {
    // a pointer is faster than deferencing the iterator every time
    Track *track = *t;
    
    if (LHCb::Track::FitStatusUnknown == track->fitStatus()) {
	warning() << "Track with unknown fit status, skipping!" << endmsg;
	continue;
    }
    if (LHCb::Track::FitFailed == track->fitStatus()) {
	debug() << "Track with fit failure, trying to continue!" << endmsg;
    }
    
    type[nTrack] = (float) track->type();
    chi2[nTrack] = (float) track->chi2();
    nDoF[nTrack] = (float) track->nDoF();
    Pchi2[nTrack] = (float) gsl_cdf_chisq_Q(track->chi2(),track->nDoF());
    History[nTrack] = (float) track->history();
    // fill in a vector of associated MCParticles
    {
      MCParticle *p = directLink.first(track);
      // fill up to maxAssocMCParticles into our array
      for (int i = 0; (0 != p) && (i < maxAssocMCParticles);
		      p = directLink.next(), ++i) {
	assocMCParts[nTrack][i] = (float) mcPartIdx[p];
	assocMCPartWeights[nTrack][i] = (float) directLink.weight();
      }
    }
    // track state at first state
    fillTrackState(firstState[nTrack], track->firstState());
  }

  // the preprocessor was meant to save a little typing work and to generally
  // come in handy for all sorts of tasks, and that's what we'll use it for
#define SAVEARR(x) tuple->farray("Tracks_"#x, x, "nMaxSavedTracks", \
		 m_maxTracks)
#define SAVEMAT(x, y) tuple->fmatrix("Tracks_"#x, x, nTracks, y, \
		"nMaxSavedTracks", m_maxTracks)
  // ok, fill our tuple
  SAVEARR(type);
  SAVEARR(chi2);
  SAVEARR(nDoF);
  SAVEARR(Pchi2);
  SAVEARR(History);
  SAVEMAT(assocMCParts, maxAssocMCParticles);
  SAVEMAT(assocMCPartWeights, maxAssocMCParticles);
  SAVEMAT(firstState, 21);
#undef SAVEMAT
#undef SAVEARR

  return StatusCode::SUCCESS;
}

// convert a Measurement to an index in an array (see below for usage)
// the idea is to have an entry in an array for every hit that can possibly
// be on a track, excluding detector overlap and/or multiple hits per
// detector (mono-)layer
int TrackCheckerNT::convertMeasurementToID(const Measurement *m)
{
  double zz = m->z();
  if (Measurement::OT == m->type()) {
    // ok, OT: try to get the layer/module info from the Measurement
    const OTMeasurement *otm = dynamic_cast<const OTMeasurement*>(m);
    // get OT Module which took the hit to find the Monolayer
    const DeOTDetector *OT = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
    const DeOTModule *mod = OT->findModule(otm->channel());
    if(mod==0) {
      error() << "Cannot find module for channel: " << otm->channel() << endmsg ;
      return -1 ;
    }
    int k = (mod->monoLayerA(otm->channel().straw())?0:1) +
      2 * otm->channel().layer() +
      8 * (otm->channel().station() - 1);
    return k;
  }
  if (Measurement::TT == m->type()) {
    // ok, TT: try to get the layer/module info from the Measurement
    const STMeasurement *stm = dynamic_cast<const STMeasurement*>(m);
    int k = 1 * (stm->cluster()->channelID().layer() - 1) +
      2 * (stm->cluster()->channelID().station() - 1);
    return k;
    DeSTDetector *TT = getDet<DeSTDetector>(DeSTDetLocation::TT);
    DeSTLayer *l = TT->findLayer(stm->cluster()->channelID());
    plot2D(zz/mm, l->angle(), 424258, "TT: z vs angle", 2200., 2800., -1., 1., 800, 300);
  }
  if (Measurement::IT == m->type()) {
    // ok, IT: try to get the layer/module info from the Measurement
    const STMeasurement *stm = dynamic_cast<const STMeasurement*>(m);
    int k = 1 * (stm->cluster()->channelID().layer() - 1) +
      4 * (stm->cluster()->channelID().station() - 1);
    return k;
  }
  if (Measurement::VeloR == m->type()) {
    // ok, try to get the layer/module info from the Measurement
    const VeloRMeasurement *vrm = dynamic_cast<const VeloRMeasurement*>(m);
    int k = vrm->cluster()->channelID().sensor() / 2;
    return k;
  }
  if (Measurement::VeloPhi == m->type()) {
    // ok, try to get the layer/module info from the Measurement
    const VeloPhiMeasurement *vpm = dynamic_cast<const VeloPhiMeasurement*>(m);
    int k = (vpm->cluster()->channelID().sensor() - 64) / 2;
    return k;
  }
  // the FixedZ case with its fake measurements is easy to do...
  if (FixedZ == m->type())
    return (int) m->measure();

  return -1;
}

// build a large table of reconstructed and true track states at measurements,
// and also add the measurements themselves and the corresponding MC truth
// yes, this routine is ugly, but it will at least work for all subdetectors
// given the right parameters
void TrackCheckerNT::fillDetTrackParametersAtMeasurements(
    int arrmax,		// number of measurements to save in this subdetector
    unsigned int type,	// (detector) type of measurement to monitor
    			// a value of (unsigned int) -42 means to monitor the
			// true primary vertex and/or FixedZ positions
    Tracks* tracks,	// tracks to handle
    std::map<MCParticle*, int>& mcPartIdx,	// lookup table for indices
    						// into the NTuple's
						// MCParticle table
    LinkedTo<MCParticle, Track>& directLink,	// Track to MCParticle linker
    Tuples::Tuple &tuple)	// tuple to write things to
{
  int nTracks = std::min(m_maxTracks, (long)tracks->size());

  // the preprocessor was meant to save a little typing work and to generally
  // come in handy for all sorts of tasks, and that's what we'll use it for
  // it also has the advantage that one can group things more easily in a
  // layout that captures what is going to be saved, making things a little
  // clearer (I hope, see the covariance matrix entries below as an example)
#define DECL(x) Matrix x(nTracks, Array(arrmax, -1.));
  // we have matrices for every interesting quantity per track (row) and
  // detector measurement (column)

  // z of measurement
  DECL(z);
  // state at measurement
  DECL(x); DECL(y); DECL(tx); DECL(ty); DECL(qp);
  // covariance matrix
  DECL(Cxx); DECL(Cxy); DECL(Cxtx ); DECL(Cxty ); DECL(Cxqp );
	     DECL(Cyy); DECL(Cytx ); DECL(Cyty ); DECL(Cyqp );
  			DECL(Ctxtx); DECL(Ctxty); DECL(Ctxqp);
  				     DECL(Ctyty); DECL(Ctyqp);
  						  DECL(Cqpqp);
  // true state at measurement
  DECL(Tx); DECL(Ty); DECL(Ttx); DECL(Tty); DECL(Tqp);
  // true covariance matrix
  DECL(TCxx); DECL(TCxy); DECL(TCxtx ); DECL(TCxty ); DECL(TCxqp );
	      DECL(TCyy); DECL(TCytx ); DECL(TCyty ); DECL(TCyqp );
			  DECL(TCtxtx); DECL(TCtxty); DECL(TCtxqp);
					DECL(TCtyty); DECL(TCtyqp);
						      DECL(TCqpqp);
  // detector layer (matrix row where this hit should have gone)
  // (some hits are relocated to spare rows inside our matrices because
  // their position in the matrix is already taken by a different hit
  // due to detector overlaps etc.)
  DECL(layer);
  // measurement type as returned by getMeasType
  DECL(meastype);
  // residuals and measurement error (error is negative if unavailable)
  // res. calculated by producing measurement from MC truth
  DECL(measres); DECL(measerr);
  // index into MCParticles (in the ntuple) to find the MCParticle which
  // caused the hit (negative if unavailable)
  DECL(mcp);
#undef DECL
  
  // make a string from the desired measurement type, which will be used
  // for messages and as prefix for our NTuple columns to disambiguate
  // by measurement type
  std::string pfx("Tracks_");
  pfx.append(measType(type));
  
  // we loop over all tracks in our event
  int nTrack = 0;
  for (Tracks::const_iterator t = tracks->begin();
      (t != tracks->end()) && (nTrack < nTracks); ++t, ++nTrack) {
    // a pointer is faster than deferencing the iterator every time
    Track *tr = *t;
    
    if (LHCb::Track::FitStatusUnknown == tr->fitStatus()) {
	warning() << "Track with unknown fit status, skipping!" << endmsg;
	continue;
    }
    if (LHCb::Track::FitFailed == tr->fitStatus()) {
	debug() << "Track with fit failure, trying to continue!" << endmsg;
    }
    
    // try to get associated MCParticle
    MCParticle *mcPart = directLink.first(tr);
    
    // pointer to array with fake measurements
    std::vector<const Measurement*> *fakes = 0;

    // get the measurements on the track
    std::vector<const Measurement*> measurements(tr->measurements()) ;
    
    // we'll soon loop over all track measurements (and we cheat a lot
    // in case we want data for the true vertex position or at fixed z
    // to avoid duplicate code)

    // in case we run for the FixedZ pseudo measurement type, we need to
    // fake a measurement vector here which we can loop over later
    if (FixedZ == type) {
      // dynamically allocate vector and fake measurements
      // don't forget to delete them once you're done!
      fakes = new std::vector<const Measurement*>(maxFixedZ, (Measurement*) 0);
      // handle the Vertex case
      if (0 != mcPart) {
	// we have MC truth available, so we use it
	// build a measurement for the true vertex (if available)
	const MCVertex *vtx = mcPart->originVertex();
	if (0 == vtx) {
	  // this should never happen - MC truth and still no vertex?!
	  // we won't die quietly then
	  error() << "Track with associated MCParticle does not have "
	    "primary vertex in " << pfx << "?!" << endmsg;
	  // if it still does happen, we can't supply the vertex info,
	  // so we won't
	} else {
	  // ok, we have the vertex info...
	  // we just clone the first measurement here and overwrite the
	  // members we are going to use
	  Measurement* meas = measurements.front()->clone();
	  // we set Measurement parameters
	  meas->setType((Measurement::Type) FixedZ);
	  meas->setMeasure(0.);
	  meas->setZ(vtx->position().z());
	  (*fakes)[0] = meas ;
	  // ok, done in the true vertex case...
	}
      } else {
	  // no MC truth available, we just extrapolate to z = 0
	  // on the NTuple side, we can tell if we have MC truth available,
	  // so it may make some sense to provide the information, even if
	  // it's not what it's supposed to be...
	  // 
	  // we just clone the first measurement here and overwrite the
	  // members we are going to use, just like above
	  Measurement* meas = measurements.front()->clone();
	  // we set Measurement parameters
	  meas->setType((Measurement::Type) FixedZ);
	  meas->setMeasure(0.);
	  meas->setZ(0.);
	  (*fakes)[0] = meas ;
	  // ok, done in the true vertex case...
      }
      // ok, now fill in the real fixed z positions at which we want to
      // monitor things
      std::vector<double>::const_iterator p = m_zPositions.begin();
      for (int k = 1; k < maxFixedZ; ++k) {
	// get a new fixed z, if available
	if (p == m_zPositions.end()) break;
	// we just clone the first measurement here and overwrite the
	// members we are going to use
	Measurement* meas = measurements.front()->clone();
	// we set Measurement parameters
	meas->setType((Measurement::Type) FixedZ);
	meas->setMeasure((double) k);
	meas->setZ(*p);
	(*fakes)[k] = meas ;
	++p;
      }
    }
    // ok, fake measurements created (if applicable)
    
    // get measurements (fake or real ones, depending on type)
    const std::vector<const Measurement*>& measures =
	    ((FixedZ == type)?(*fakes):(tr->measurements()));
    
    // now comes the mean main loop over all measurements (which may or
    // may not be fake ones...)
    for (std::vector<const Measurement*>::const_iterator mmm = measures.begin();
	mmm != measures.end(); ++mmm) {
      const Measurement *m = *mmm;
      // the fake measurement container may contain NULL pointers, so we
      // skip them...
      if (0 == m) continue;
      // ok, check that we look at the correct type of measurement
      if ((unsigned int)m->type() != type) continue;

      // figure out the column in which to save this measurement in our
      // matrices
      int i = convertMeasurementToID(m);
      // make sure we don't write past the end of our column...
      // this is mainly there to guard against programming errors (e.g.
      // bugs in convertMeasurementToID or mismatch between measurement
      // type and matrix size (arrmax parameter))
      if ((i >= arrmax) || (i < 0)) {
	error() << "Encountered impossible ID in " << pfx << "." << endmsg;
	continue;
      }
      
      double zz = m->z();
      
      // ok, get a state at measurement z
      State state;
      StatusCode sc = m_extrapolator->propagate(*tr, zz, state);
      if (sc.isFailure()) {
	error() << "Can't extrapolate track in " << pfx << "." << endmsg;
	continue;
      }
      
      // check if we would overwrite things - one possible reason why this
      // overwriting business may happen are detector overlaps (e.g. in the
      // IT)
      if (Cxx[nTrack][i] >= 0.) {
	// overlapping detector parts and/or two hits per detection
	// (mono-)layer and track are a problem
	// we have the following solution: we make our per detector arrays
	// a bit larger and keep "overflow" columns to catch those hits
	// that way, we also can easily discard tracks without hits in the
	// overlap region (or multiple hits per (mono-)layer) if we are
	// interested in such tracks
	int j = i;
	// first, we figure out where we start to look for a new slot
	switch (type) {
	  case Measurement::IT:
	    i = 12; break;
	  case Measurement::OT:
	    i = 24; break;
	  case Measurement::TT:
	    i = 8; break;
	  case Measurement::VeloR:
	  case Measurement::VeloPhi:
	    i = 21; break;
	  default:
	    // make sure we do the right thing for new measurement types
	    // if someone forgot to adapt this switch block (which is to
	    // complain loudly, see below)
	    i = arrmax; break;
	}
	// find a free slot
	for (; i < arrmax; ++i)
	  if (Cxx[nTrack][i] < 0.) break;
	// complain if we don't find one
	if ((i >= arrmax) || (Cxx[nTrack][i] >= 0.)) {
	  warning() << "Refusing to overwrite previous measurement at (" <<
	    x[nTrack][j]/mm << "/" << y[nTrack][j]/mm << "/" <<
	    z[nTrack][j]/mm << ") in " << pfx << ", new one is at (" <<
	    state.x()/mm << "/" << state.y()/mm << "/" << zz/mm << ")."
	    << endmsg;
	  continue;
	}
	// we can use the slot we found
	// save the real layer...
	layer[nTrack][i] = (float) j;
      } else {
	// save the real layer even for those measurements where no
	// relocation is needed...
	layer[nTrack][i] = (float) i;
      }
      
      // make entries for reconstructed track parameters
      z[nTrack][i] = (float) (zz / mm);
      
      // we define a macro to do the work because we need nearly identical
      // code below for the true state
#define copyState(mypfx, mystate) \
      mypfx##x[nTrack][i] = (float) (mystate.x() / mm); \
      mypfx##y[nTrack][i] = (float) (mystate.y() / mm); \
      mypfx##tx[nTrack][i] = (float) (mystate.tx()); \
      mypfx##ty[nTrack][i] = (float) (mystate.ty()); \
      mypfx##qp[nTrack][i] = (float) (mystate.qOverP()*GeV); \
      mypfx##Cxx[nTrack][i] = (float) (mystate.covariance()(0,0)/mm/mm); \
      mypfx##Cxy[nTrack][i] = (float) (mystate.covariance()(0,1)/mm/mm); \
      mypfx##Cxtx[nTrack][i] = (float) (mystate.covariance()(0,2)/mm); \
      mypfx##Cxty[nTrack][i] = (float) (mystate.covariance()(0,3)/mm); \
      mypfx##Cxqp[nTrack][i] = (float) (mystate.covariance()(0,4)/mm*GeV); \
      mypfx##Cyy[nTrack][i] = (float) (mystate.covariance()(1,1)/mm/mm); \
      mypfx##Cytx[nTrack][i] = (float) (mystate.covariance()(1,2)/mm); \
      mypfx##Cyty[nTrack][i] = (float) (mystate.covariance()(1,3)/mm); \
      mypfx##Cyqp[nTrack][i] = (float) (mystate.covariance()(1,4)/mm*GeV); \
      mypfx##Ctxtx[nTrack][i] = (float) (mystate.covariance()(2,2)); \
      mypfx##Ctxty[nTrack][i] = (float) (mystate.covariance()(2,3)); \
      mypfx##Ctxqp[nTrack][i] = (float) (mystate.covariance()(2,4)*GeV); \
      mypfx##Ctyty[nTrack][i] = (float) (mystate.covariance()(3,3)); \
      mypfx##Ctyqp[nTrack][i] = (float) (mystate.covariance()(3,4)*GeV); \
      mypfx##Cqpqp[nTrack][i] = (float) (mystate.covariance()(4,4)*GeV*GeV)

      // ok, copy our entries for the reconstructed state
      copyState(, state); // mypfx must be empty here!

      // try to get MCParticle which caused the hit, but only for real
      // measurements (i.e. not fake ones)
      MCParticle* mcPartResponsibleForHit = 0;
      if (FixedZ != type)
	      mcPartResponsibleForHit = getMCParticle(m);
      mcp[nTrack][i] = (float) mcPartIdx[mcPartResponsibleForHit];

      // get measurement type
      meastype[nTrack][i] = (float) getMeasType(m);
      
      // project true state of the particle that caused the measurement
      // into a measurement to monitor unbiased resolutions
      // this can only work for real measurements, but not for true vertex
      // info or states at fixed z
      do {
        State trueState;
        if ((FixedZ == type) || (0 == mcPartResponsibleForHit))
	  break;
	sc = m_stateCreator->createState(mcPartResponsibleForHit, zz, trueState);
	if (sc.isFailure()) {
	  warning() << "Can't create true state for MCParticle that "
		  "caused a hit in " << pfx << "." << endmsg;
	  break;
	}
	ITrackProjector *proj = m_projectorSelector->projector(*m);
	if (0 != proj) {
		sc = proj->project(trueState, *m);
		if (sc.isFailure()) {
			warning() << "Can't project true state onto measurement in " <<
				pfx << "." << endmsg;
			break;
	}
	
		// unbiased measurement residuals/errors
		measres[nTrack][i] = (float) (proj->residual() / mm);
		measerr[nTrack][i] = (float) (proj->errMeasure() / mm);
	} else {
		warning() << "Can't obtain projector for a measurement in " <<
			pfx << "." << endmsg;
		break;
	}
      } while (false);
      
      // check if we have an mcPart (if not, we can't gather more
      // information below, so we skip the rest to the things to do -
      // this also means that anything which does not need an associated
      // MCParticle must be done above!)
      if (0 == mcPart)
	continue;
      // make entries for true state
      State trueState;
      // make sure we create a state at the vertex if that is what we are
      // supposed to do, otherwise we extrapolate to the given z
      if ((FixedZ == type) && (0 == i))
	sc = m_stateCreator->createStateVertex(mcPart, trueState);
      else
	sc = m_stateCreator->createState(mcPart, zz, trueState);
      
      if (sc.isFailure()) {
	warning() << "Can't create (true) state for MCParticle in " <<
		pfx << "." << endmsg;
	continue;
      }
      //     State &truestate = *trueState;
      // set a flag indicating that we have the true state
      // copy true state parameters to our array
      copyState(T, trueState); // true state variables begin with a 'T'
      // we don't need the copyState macro any longer, so we get rid of it
#undef copyState
    }		// End loop Measurements
    
    // in case of FixedZ measurements, we need to delete the stuff we
    // allocated for the fake measurements we created...
    if (FixedZ == type) {
      // delete the fake measurements
      std::vector<const Measurement*>::const_iterator p = measures.begin();
      while (p != measures.end()) {
	if (0 != *p)
	  delete *p;
	++p;
      }
      // delete the vector itself
      delete &measures;
    }
  }		// End loop Tracks
  
  // the preprocessor was meant to save a little typing work and to generally
  // come in handy for all sorts of tasks, and that's what we'll use it for
#define SAVE(x) tuple->fmatrix(std::string(pfx).append(#x), x, nTracks, \
		arrmax, "nMaxSavedTracks", m_maxTracks)
#define SAVESTR(x,y) tuple->fmatrix(std::string(pfx).append(y), x, \
		nTracks, arrmax, "nMaxSavedTracks", m_maxTracks)
  // ok, we'll fill our NTuple now...
  SAVE(z);

  SAVE(x); SAVE(y); SAVE(tx); SAVE(ty); SAVE(qp);

  SAVE(Cxx); SAVE(Cxy); SAVE(Cxtx ); SAVE(Cxty ); SAVE(Cxqp );
	     SAVE(Cyy); SAVE(Cytx ); SAVE(Cyty ); SAVE(Cyqp );
		        SAVE(Ctxtx); SAVE(Ctxty); SAVE(Ctxqp);
				     SAVE(Ctyty); SAVE(Ctyqp);
						  SAVE(Cqpqp);

  SAVE(Tx); SAVE(Ty); SAVE(Ttx); SAVE(Tty); SAVE(Tqp);

  SAVE(TCxx); SAVE(TCxy); SAVE(TCxtx ); SAVE(TCxty ); SAVE(TCxqp );
	      SAVE(TCyy); SAVE(TCytx ); SAVE(TCyty ); SAVE(TCyqp );
		          SAVE(TCtxtx); SAVE(TCtxty); SAVE(TCtxqp);
				        SAVE(TCtyty); SAVE(TCtyqp);
						      SAVE(TCqpqp);
  SAVESTR(measres,  "MeasResidual");
  SAVESTR(measerr,  "MeasError");
  SAVESTR(meastype, "MeasType");
  SAVESTR(layer,    "Layer");
  SAVESTR(mcp,      "MCParticle");
#undef SAVE
#undef SAVESTR
}

// this routine gathers data to determine hit purities, efficiencies and ghost
// rates
StatusCode TrackCheckerNT::fillHitPurEff(
    Tuples::Tuple& tuple,
    Tracks* tracks,
    LinkedTo<MCParticle, Track>& directLink)
{
  int nTracks = std::min(m_maxTracks, (long)tracks->size());
  Array TotalHits(nTracks), MCTotalHits(nTracks), GoodHits(nTracks);
  Array TotalVelo(nTracks), TotalTT(nTracks), TotalIT(nTracks),
	TotalOT(nTracks);
  Array MCTotalVelo(nTracks), MCTotalTT(nTracks), MCTotalIT(nTracks),
	MCTotalOT(nTracks);
  Array GoodVelo(nTracks), GoodTT(nTracks), GoodIT(nTracks), GoodOT(nTracks);
  
  // Loop over all Tracks
  int nTrack = 0;
  for (Tracks::const_iterator t = tracks->begin();
      (t != tracks->end()) && (nTrack < nTracks); ++t, ++nTrack) {
    Track *track = *t;
    // 
    LHCb::Track::MeasurementContainer measurements = track->measurements() ;
    
    if (LHCb::Track::FitStatusUnknown == track->fitStatus()) {
	warning() << "Track with unknown fit status, skipping!" << endmsg;
	continue;
    }
    if (LHCb::Track::FitFailed == track->fitStatus()) {
	debug() << "Track with fit failure, trying to continue!" << endmsg;
    }

    // zero counters
    int nTotalVelo   = 0, nTotalTT   = 0, nTotalIT   = 0, nTotalOT   = 0;
    int nGoodVelo    = 0, nGoodTT    = 0, nGoodIT    = 0, nGoodOT    = 0;
    int nMCTotalVelo = 0, nMCTotalTT = 0, nMCTotalIT = 0, nMCTotalOT = 0;
    // get VeloClusters and count correct and total number of clusters
    // Count number of Measurements of the different types
    {
      for(LHCb::Track::MeasurementContainer::const_iterator itMeas = measurements.begin() ;
	  itMeas != measurements.end(); ++itMeas) {
	if (      (*itMeas)->type() == Measurement::VeloPhi ) { ++nTotalVelo; }
	else if ( (*itMeas)->type() == Measurement::VeloR )   { ++nTotalVelo; }
	else if ( (*itMeas)->type() == Measurement::TT )      { ++nTotalTT;   }
	else if ( (*itMeas)->type() == Measurement::IT )      { ++nTotalIT;   }
	else if ( (*itMeas)->type() == Measurement::OT )      { ++nTotalOT;   }
      }
    }
    
    // retrieve most probable MCParticle
    MCParticle *mcPart = directLink.first(track);
    if (0 != mcPart) {
      // get VeloClusters and count correct and total number of clusters
      // Get the linker table MCParticle => VeloCluster
      LinkedFrom<VeloCluster,MCParticle> veloLink(evtSvc(),msgSvc(),
	  LHCb::VeloClusterLocation::Default);
      if ( veloLink.notFound() )
	return Error( "Unable to retrieve MCParticle-VeloCluster linker table" );
      
      const VeloCluster* veloCluster = veloLink.first( mcPart );
      while ( 0 != veloCluster ) {
        bool found = false;
	++nMCTotalVelo;
	LHCb::Track::MeasurementContainer::const_iterator iMeas   = measurements.begin();
	LHCb::Track::MeasurementContainer::const_iterator endMeas = measurements.end();
	while ( !found && ( iMeas != endMeas ) ) {
	  if ( (*iMeas)->type() == Measurement::VeloR ) {
	    const VeloRMeasurement* meas = dynamic_cast<const VeloRMeasurement*>( *iMeas );
	    found = ( veloCluster == meas->cluster() );
	  }
	  if ( (*iMeas)->type() == Measurement::VeloPhi ) {
	    const VeloPhiMeasurement* meas = dynamic_cast<const VeloPhiMeasurement*>( *iMeas );
	    found = ( veloCluster == meas->cluster() );
	  }
	  ++iMeas;
	}
	if ( found ) { ++nGoodVelo; }
	veloCluster = veloLink.next();
      }
    
      // get STClusters and count correct and total number of clusters
      // Get the linker table MCParticle => TT STCluster
      LinkedFrom<STCluster,MCParticle> ttLink(evtSvc(),msgSvc(),
	  LHCb::STClusterLocation::TTClusters);
      if ( ttLink.notFound() )
	return Error( "Unable to retrieve MCParticle-TT STCluster linker table" );
      
      // Get the linker table MCParticle => IT STCluster
      LinkedFrom<STCluster,MCParticle> itLink(evtSvc(),msgSvc(),
	  LHCb::STClusterLocation::ITClusters);
      if ( itLink.notFound() )
	return Error( "Unable to retrieve MCParticle-IT STCluster linker table" );
    
      // TT
      const STCluster* ttCluster = ttLink.first ( mcPart );
      while ( 0 != ttCluster ) {
        bool found = false;
	++nMCTotalTT;
	LHCb::Track::MeasurementContainer::const_iterator iMeas   = measurements.begin();
	LHCb::Track::MeasurementContainer::const_iterator endMeas = measurements.end();
	while ( !found && ( iMeas != endMeas ) ) {
	  if ( (*iMeas)->type() == Measurement::TT ) {
	    const STMeasurement* meas = dynamic_cast<const STMeasurement*>( *iMeas );
	    found = ( ttCluster == meas->cluster() );
	  }
	  ++iMeas;
	}
	if ( found ) { ++nGoodTT; }
	ttCluster = ttLink.next();
      }
    
      //IT
      const STCluster* itCluster = itLink.first ( mcPart );
      while ( 0 != itCluster ) {
        bool found = false;
	++nMCTotalIT;
	LHCb::Track::MeasurementContainer::const_iterator iMeas   = measurements.begin();
	LHCb::Track::MeasurementContainer::const_iterator endMeas = measurements.end();
	while ( !found && ( iMeas != endMeas ) ) {
	  if ( (*iMeas)->type() == Measurement::IT ) {
	    const STMeasurement* meas = dynamic_cast<const STMeasurement*>( *iMeas );
	    found = ( itCluster == meas->cluster() );
	  }
	  ++iMeas;
	}
	if ( found ) { ++nGoodIT; }
	itCluster = itLink.next();
      }
    
      // get OTTimes and count correct and total number of clusters
      // Get the linker table MCParticle => OTTime
      LinkedFromKey<MCParticle, OTChannelID> otLink(evtSvc(),msgSvc(),
	  LHCb::OTTimeLocation::Default);
      if ( itLink.notFound() )
	return Error( "Unable to retrieve MCParticle-OTChannelID linker table" );
    
      std::vector<OTChannelID> otids(otLink.keyRange(mcPart));
      BOOST_FOREACH(OTChannelID otid, otids) {
        bool found = false;
	++nMCTotalOT;
	LHCb::Track::MeasurementContainer::const_iterator iMeas   = measurements.begin();
	LHCb::Track::MeasurementContainer::const_iterator endMeas = measurements.end();
	while ( !found && ( iMeas != endMeas ) ) {
	  if ( (*iMeas)->type() == Measurement::OT ) {
	    const OTMeasurement* meas = dynamic_cast<const OTMeasurement*>( *iMeas );
	    found = ( otid == meas->channel() );
	  }
	  ++iMeas;
	}
	if ( found ) { ++nGoodOT; }
      }
    } // end if (0 != mcPart)

    // Sum of Velo, TT, IT and OT hits
    int nTotalHits   = nTotalVelo   + nTotalTT   + nTotalIT   + nTotalOT;
    int nMCTotalHits = nMCTotalVelo + nMCTotalTT + nMCTotalIT + nMCTotalOT;
    int nGoodHits    = nGoodVelo    + nGoodTT    + nGoodIT    + nGoodOT;
    
    // ok, fill in the vectors from way above
    TotalHits[nTrack]	= (float) nTotalHits;
    MCTotalHits[nTrack]	= (float) nMCTotalHits;
    GoodHits[nTrack]	= (float) nGoodHits;
  
    TotalVelo[nTrack]	= (float) nTotalVelo;
    MCTotalVelo[nTrack]	= (float) nMCTotalVelo;
    GoodVelo[nTrack]	= (float) nGoodVelo;
  
    TotalIT[nTrack]	= (float) nTotalIT;
    MCTotalIT[nTrack]	= (float) nMCTotalIT;
    GoodIT[nTrack]	= (float) nGoodIT;
    
    TotalOT[nTrack]	= (float) nTotalOT;
    MCTotalOT[nTrack]	= (float) nMCTotalOT;
    GoodOT[nTrack]	= (float) nGoodOT;
  
    TotalTT[nTrack]	= (float) nTotalTT;
    MCTotalTT[nTrack]	= (float) nMCTotalTT;
    GoodTT[nTrack]	= (float) nGoodTT;
  } // end loop over tracks

  // ok, we use the preprocessor again to make things easier to type
  // (and it also looks nicer this way)
#define SAVE(x) tuple->farray("Tracks_n"#x, x, "nMaxSavedTracks", \
		m_maxTracks)
  // ok, now fill our vectors into our Tuple
  SAVE(TotalHits); SAVE(MCTotalHits); SAVE(GoodHits);
  SAVE(TotalVelo); SAVE(MCTotalVelo); SAVE(GoodVelo);
  SAVE(TotalTT  ); SAVE(MCTotalTT  ); SAVE(GoodTT  );
  SAVE(TotalIT  ); SAVE(MCTotalIT  ); SAVE(GoodIT  );
  SAVE(TotalOT  ); SAVE(MCTotalOT  ); SAVE(GoodOT  );
#undef SAVE
  
  return StatusCode::SUCCESS;
}

// write general information on a MCParticle to the NTuple
StatusCode TrackCheckerNT::fillMCGeneralInfo(
		Tuples::Tuple& tuple,
		std::map<Track*, int>& trackIdx,
		std::map<MCParticle*, int>& mcPartIdx,
		MCParticles* particles,
		LinkedFrom<Track,MCParticle>& revLink)
{
  // Loop over the MCParticles - we're doing rather simple stuff here
  const int maxAssocTracks = 10;
  int nParts = std::min(m_maxMCParticles, (long)particles->size());
  // PID of MC particle
  Array PID(nParts, -1.);
  // mother of MC particle (idx into NTuple MCParticles, neg. if unav.)
  Array mother(nParts, -1.);
  // momentum of MC particle
  Array momentum(nParts, -1.);
  Matrix _4momentum(nParts, Array(4, -1.));
  // spatial coordinated of origin vertex
  Matrix _origVtxPos(nParts, Array(3, -1.));
  // the first few assoc. Tracks (idx into NTuple, neg. if unav.)
  Matrix _assocTracks(nParts, Array(maxAssocTracks, -1.));
  // corresponding weights (see above...)
  Matrix _assocTrackWeights(nParts, Array(maxAssocTracks, -1.));
  MCParticles::const_iterator iPart;
  int nPart = 0;
  for(iPart = particles->begin();
      (particles->end() != iPart) && (nPart < nParts);
      ++iPart, ++nPart) {
    MCParticle* mcPart = *iPart;
    // PID information for our particle and its mother (if available)
    PID[nPart] = (float) mcPart->particleID().pid();
    // save index of mother particle
    mother[nPart] = (float) mcPartIdx[(MCParticle *) mcPart->mother()];
    // we may want to place a cut on particle momentum, so we save that as well
    momentum[nPart] = (float) (mcPart->p() / GeV);
    // fourmomentum in GeV
    _4momentum[nPart][0] = (float) (mcPart->momentum().E() / GeV);
    _4momentum[nPart][1] = (float) (mcPart->momentum().Px() / GeV);
    _4momentum[nPart][2] = (float) (mcPart->momentum().Py() / GeV);
    _4momentum[nPart][3] = (float) (mcPart->momentum().Pz() / GeV);
    // get origin vertex
    const MCVertex *mcvtx = mcPart->originVertex();
    if (mcvtx) {
      const Gaudi::XYZPoint &point = mcvtx->position();
      _origVtxPos[nPart][0] = (float) (point.X() / mm);
      _origVtxPos[nPart][1] = (float) (point.Y() / mm);
      _origVtxPos[nPart][2] = (float) (point.Z() / mm);
    }
    // ok, write up to maxAssocTracks associated Tracks
    Track* track = revLink.first(mcPart);
    // fill up to maxAssocTracks into our array
    for (int j = 0; (0 != track) && (j < maxAssocTracks);
	track = revLink.next(), ++j) {
      _assocTracks[nPart][j] = (float) trackIdx[track];
      _assocTrackWeights[nPart][j] = (float) revLink.weight();
    }
  } // End loop over MCParticles
  
  // ok, we use the preprocessor again to make things easier to type
  // (and it also looks nicer this way)
#define SAVEARR(x) tuple->farray("MCParticles_"#x, x, \
		"nMaxSavedMCParticles", m_maxMCParticles)
#define SAVEMAT(x, y) tuple->fmatrix("MCParticles"#x, x, \
		nParts, y, "nMaxSavedMCParticles", m_maxMCParticles)
  // ok, fill corresponding Tuple columns
  SAVEARR(PID); SAVEARR(mother); SAVEARR(momentum);
  SAVEMAT(_assocTracks, maxAssocTracks);
  SAVEMAT(_assocTrackWeights, maxAssocTracks);
  SAVEMAT(_origVtxPos, 3);
  SAVEMAT(_4momentum, 4);
#undef SAVEMAT
#undef SAVEARR

  return StatusCode::SUCCESS;
}

// the following routine gathers hit statistics on the MCParticle level which
// are needed to check for reconstructible particles etc.
// basically, we count hits in the subdetectors
// we'll also write out hit masks for each subdetector so we do at least know
// which layers have been hit
StatusCode TrackCheckerNT::fillMCHitStatistics(
    Tuples::Tuple& tuple,
    MCParticles* particles)
{
  int nParts = std::min(m_maxMCParticles, (long)particles->size());
  // again, we rely on the preprocessor to do some dirty work for us
#define DECL(det, type, thing) Array n##det##type##thing(nParts, 0.)
#define DECLVELO(type) DECL(Velo, type, Hits); DECL(Velo, type, Stations)
#define DECLTR0(det, thing) DECL(det,X,thing); DECL(det,U,thing); \
  		DECL(det,V,thing); DECL(det,,thing)
#define DECLTR(det) DECLTR0(det,Hits); DECLTR0(det,Stations); DECL(det,St,Stations)
#define DECLHM(x) Array HitMask##x(nParts, 0.)
  // declare arrays to hold VeloR, VeloPhi and VeloStatistics
  DECLVELO(R); DECLVELO(Phi); DECLVELO(Total);
  // arrays for X,U,V-Hits/Stations, St-Stations and total Hits/Stations
  // in TT, IT, OT
  DECLTR(TT); DECLTR(IT); DECLTR(OT);
  // arrays for Stereo and total T Stations
  DECLTR0(T,Stations); DECL(T,St,Stations);
  // hit masks
  DECLHM(VeloR); DECLHM(VeloPhi); DECLHM(TT); DECLHM(IT); DECLHM(OT);
#undef DECLHM
#undef DECLTR
#undef DECLTR0
#undef DECLVELO
#undef DECL
  
  // get a few linker tables first...
  LinkedFrom<VeloCluster,MCParticle> veloLink(evtSvc(),msgSvc(),
      LHCb::VeloClusterLocation::Default);
  if (veloLink.notFound())
    return Error("Unable to retrieve MCParticle-VeloCluster linker table");
  LinkedFrom<STCluster,MCParticle> ttLink(evtSvc(),msgSvc(),
      LHCb::STClusterLocation::TTClusters);
  if (ttLink.notFound())
    return Error("Unable to retrieve MCParticle-TT STCluster linker table");
  LinkedFrom<STCluster,MCParticle> itLink(evtSvc(),msgSvc(),
      LHCb::STClusterLocation::ITClusters);
  if (itLink.notFound())
    return Error("Unable to retrieve MCParticle-IT STCluster linker table");
  LinkedFromKey<MCParticle, OTChannelID> otLink(evtSvc(),msgSvc(),
      LHCb::OTTimeLocation::Default);
  if (otLink.notFound())
    return Error("Unable to retrieve MCParticle-OTTime linker table");
  
  // loop over MCParticles
  int nPart = 0;
  for (MCParticles::const_iterator mcp = particles->begin();
      (mcp != particles->end()) && (nPart < nParts); ++mcp, ++nPart) {
    const MCParticle *mcPart = *mcp;
    
    // ok, get Velo clusters and count them
    {
      int nR = 0, nPhi = 0;
      std::set<int> rStations, phiStations;
      for (const VeloCluster* veloCluster = veloLink.first(mcPart);
	  0 != veloCluster; veloCluster = veloLink.next()) {
	// determine if it's an r or phi cluster
	if (veloCluster->isRType()) {
	  ++nR;
	  rStations.insert(veloCluster->channelID().sensor());
	  HitMaskVeloR[nPart] = (float)
		(((int) HitMaskVeloR[nPart]) |
		 (1 << (veloCluster->channelID().sensor() / 2)));
	} else {
	  ++nPhi;
	  phiStations.insert(veloCluster->channelID().sensor());
	  HitMaskVeloPhi[nPart] = (float)
		(((int) HitMaskVeloPhi[nPart]) |
		 (1 << ((veloCluster->channelID().sensor() - 64) / 2)));
	}
      }
      nVeloRHits[nPart] = (float) nR;
      nVeloPhiHits[nPart] = (float) nPhi;
      nVeloTotalHits[nPart] = (float) (nR + nPhi);
      nVeloRStations[nPart] = (float) (rStations.size());
      nVeloPhiStations[nPart] = (float) phiStations.size();
      nVeloTotalStations[nPart] = (float) (rStations.size() + phiStations.size());
    }
    
    // ok, now it's the TT clusters' turn...
    {
      int nX = 0, nU = 0, nV = 0;
      std::set<int> Stations, xStations, uStations, vStations, StStations;
      for (const STCluster* TTCluster = ttLink.first(mcPart);
	  0 != TTCluster; TTCluster = ttLink.next()) {
	Stations.insert(TTCluster->channelID().station());
	// determine if it's an r or phi cluster
	MeasType type;
	type = getMeasType(TTCluster);
	if (X == type) {
	  // x cluster
	  ++nX;
	  xStations.insert(TTCluster->channelID().station());
	} else if (U == type) {
	  // u cluster
	  ++nU;
	  uStations.insert(TTCluster->channelID().station());
	  StStations.insert(TTCluster->channelID().station());
	} else {
	  // v cluster
	  ++nV;
	  vStations.insert(TTCluster->channelID().station());
	  StStations.insert(TTCluster->channelID().station());
	}
	HitMaskTT[nPart] = (float)
		(((int) HitMaskTT[nPart]) |
		 (1 << (2 * (TTCluster->channelID().station() - 1) +
			(TTCluster->channelID().layer() - 1))));
      }
      nTTXHits[nPart] = (float) nX;
      nTTUHits[nPart] = (float) nU;
      nTTVHits[nPart] = (float) nV;
      nTTHits[nPart] =(float) ( nX + nU + nV);
      nTTXStations[nPart] = (float) xStations.size();
      nTTUStations[nPart] = (float) uStations.size();
      nTTVStations[nPart] = (float) vStations.size();
      nTTStStations[nPart] = (float) StStations.size();
      nTTStations[nPart] = (float) Stations.size();
    }

    // ok, now it's the IT clusters' turn...
    {
      int nX = 0, nU = 0, nV = 0;
      std::set<int> Stations, xStations, uStations, vStations, StStations;
      for (const STCluster* ITCluster = itLink.first(mcPart);
	  0 != ITCluster; ITCluster = itLink.next()) {
	Stations.insert(ITCluster->channelID().station());
	// determine if it's an r or phi cluster
	MeasType type;
	type = getMeasType(ITCluster);
	if (X == type) {
	  // x cluster
	  ++nX;
	  xStations.insert(ITCluster->channelID().station());
	} else if (U == type) {
	  // u cluster
	  ++nU;
	  uStations.insert(ITCluster->channelID().station());
	  StStations.insert(ITCluster->channelID().station());
	} else {
	  // v cluster
	  ++nV;
	  vStations.insert(ITCluster->channelID().station());
	  StStations.insert(ITCluster->channelID().station());
	}
	HitMaskIT[nPart] = (float)
		(((int) HitMaskIT[nPart]) |
		 (1 << (4 * (ITCluster->channelID().station() - 1) +
			(ITCluster->channelID().layer() - 1))));
      }
      nITXHits[nPart] = (float) nX;
      nITUHits[nPart] = (float) nU;
      nITVHits[nPart] = (float) nV;
      nITHits[nPart] = (float) (nX + nU + nV);
      nITXStations[nPart] = (float) xStations.size();
      nITUStations[nPart] = (float) uStations.size();
      nITVStations[nPart] = (float) vStations.size();
      nITStStations[nPart] = (float) StStations.size();
      nITStations[nPart] = (float) Stations.size();
    }
    
    // ok, now it's the OT times' turn...
    {
      int nX = 0, nU = 0, nV = 0;
      std::set<int> Stations, xStations, uStations, vStations, StStations;
      std::vector<OTChannelID> otids(otLink.keyRange(mcPart));
      BOOST_FOREACH(OTChannelID otid, otids) {
	Stations.insert(otid.station());
	// determine if it's an r or phi cluster
	MeasType type = getMeasType(otid);
	if (X == type) {
	  // x cluster
	  ++nX;
	  xStations.insert(otid.station());
	} else if (U == type) {
	  // u cluster
	  ++nU;
	  uStations.insert(otid.station());
	  StStations.insert(otid.station());
	} else {
	  // v cluster
	  ++nV;
	  vStations.insert(otid.station());
	  StStations.insert(otid.station());
	}
	// get OT Module which took the hit to find the Monolayer
	DeOTDetector *OT = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
	DeOTModule *mod = OT->findModule(otid);
	// although the mantisse of a float is 23 bits only, it is just the
	// "fractional part" of the number represented, so a leading set bit
	// is implicitly assumed. hence, we will not run into trouble with a
	// 24 bit hit mask: if msb is not set, we have 23 bits, so all is well
	// if msb is set, it becomes the suppressed leading bit, and the
	// remaining 23 are safe in the mantissa - again, all is well
	int mask = (int) HitMaskOT[nPart];
	// ok, or our new bit onto mask
	mask |= (1 << ((mod->monoLayerA(otid.straw())?0:1) +
			8 * (otid.station() - 1) +
			2 * otid.layer()));
	// ok, convert back to float...
	HitMaskOT[nPart] = (float) mask;
      }
      nOTXHits[nPart] = (float) nX;
      nOTUHits[nPart] = (float) nU;
      nOTVHits[nPart] = (float) nV;
      nOTHits[nPart] = (float) (nX + nU + nV);
      nOTXStations[nPart] = (float) xStations.size();
      nOTUStations[nPart] = (float) uStations.size();
      nOTVStations[nPart] = (float) vStations.size();
      nOTStStations[nPart] = (float) StStations.size();
      nOTStations[nPart] = (float) Stations.size();
    }

    // now, we treat IT and OT as a unit (for stations only, hits are
    // additive!)
    {
      std::set<int> Stations, xStations, uStations, vStations, StStations;
      for (const STCluster* ITCluster = itLink.first(mcPart);
	  0 != ITCluster; ITCluster = itLink.next()) {
	Stations.insert(ITCluster->channelID().station());
	// determine if it's an r or phi cluster
	MeasType type;
	type = getMeasType(ITCluster);
	if (X == type) {
	  // x cluster
	  xStations.insert(ITCluster->channelID().station());
	} else if (U == type) {
	  // u cluster
	  uStations.insert(ITCluster->channelID().station());
	  StStations.insert(ITCluster->channelID().station());
	} else {
	  // v cluster
	  vStations.insert(ITCluster->channelID().station());
	  StStations.insert(ITCluster->channelID().station());
	}
      }
      std::vector<OTChannelID> otids(otLink.keyRange(mcPart));
      BOOST_FOREACH(OTChannelID otid, otids) {
	Stations.insert(otid.station());
	// determine if it's an r or phi cluster
	MeasType type = getMeasType(otid);
	if (X == type) {
	  // x cluster
	  xStations.insert(otid.station());
	} else if (U == type) {
	  // u cluster
	  uStations.insert(otid.station());
	  StStations.insert(otid.station());
	} else {
	  // v cluster
	  vStations.insert(otid.station());
	  StStations.insert(otid.station());
	}
      }
      
      nTXStations[nPart] = (float) xStations.size();
      nTUStations[nPart] = (float) uStations.size();
      nTVStations[nPart] = (float) vStations.size();
      nTStStations[nPart] = (float) StStations.size();
      nTStations[nPart] =(float)  Stations.size();
    }
  } // end loop over MCParticles

  // ok, we use the preprocessor again to make things easier to type
  // (and it also looks nicer this way)
#define SAVE(x) tuple->farray("MCParticles_"#x, x, \
		"nMaxSavedMCParticles", m_maxMCParticles)
  // ok, write tuple columns
  SAVE(nVeloRHits    ); SAVE(nVeloPhiHits    ); SAVE(nVeloTotalHits    );
  SAVE(nVeloRStations); SAVE(nVeloPhiStations); SAVE(nVeloTotalStations);

  SAVE(nTTXHits    ); SAVE(nTTUHits     ); SAVE(nTTVHits    );
  SAVE(nTTXStations); SAVE(nTTUStations ); SAVE(nTTVStations);
  SAVE(nTTStations ); SAVE(nTTStStations); SAVE(nTTHits     );
  
  SAVE(nITXHits    ); SAVE(nITUHits     ); SAVE(nITVHits    );
  SAVE(nITXStations); SAVE(nITUStations ); SAVE(nITVStations);
  SAVE(nITStations ); SAVE(nITStStations); SAVE(nITHits     );
  
  SAVE(nOTXHits    ); SAVE(nOTUHits     ); SAVE(nOTVHits    );
  SAVE(nOTXStations); SAVE(nOTUStations ); SAVE(nOTVStations);
  SAVE(nOTStations ); SAVE(nOTStStations); SAVE(nOTHits     );
  
  SAVE(nTXStations ); SAVE(nTUStations  ); SAVE(nTVStations );
  SAVE(nTStStations); SAVE(nTStations   );

  SAVE(HitMaskVeloR); SAVE(HitMaskVeloPhi);
  SAVE(HitMaskTT); SAVE(HitMaskIT); SAVE(HitMaskOT);
#undef SAVE

  // all went well...
  return StatusCode::SUCCESS;
}

//=============================================================================
// TODO: remove this helper as soon as G.O.D. has functionality incorporated
// FIXME: not sure if the comment above still applies in the TrackCheckerNT
// 	  version of the routine
//=============================================================================
std::string TrackCheckerNT::measType ( unsigned int type )
{
  switch ( type ) {
    case LHCb::Measurement::Unknown : return "Unknown";
    case LHCb::Measurement::VeloR   : return "VeloR";
    case LHCb::Measurement::VeloPhi : return "VeloPhi";
    case LHCb::Measurement::TT      : return "TT";
    case LHCb::Measurement::IT      : return "IT";
    case LHCb::Measurement::OT      : return "OT";
    case LHCb::Measurement::Muon    : return "Muon";
    case FixedZ			    : return "FixedZ";
    default : return "ERROR wrong value for enum LHCb::Measurement::Type";
  }
}
