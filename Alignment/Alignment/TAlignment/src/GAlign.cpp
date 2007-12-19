//-----------------------------------------------------------------------------
/** @file GAlign.cpp
 *
 *  Implementation file for RICH reconstruction tool : GAlign
 *
 *  CVS Log :-
 *  $Id: GAlign.cpp,v 1.8 2007-12-19 13:13:07 jblouw Exp $
 *
 *  @author J.Blouw Johan.Blouw@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "Kernel/LHCbID.h"

// Event
#include "Event/StateTraj.h"
#include "Event/Track.h"
#include "TAlignment/GAlign.h"

#include "STDet/DeSTLayer.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeITSector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeTTDetector.h"
#include "STDet/DeTTHalfModule.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTModule.h"
#include "VeloDet/DeVelo.h"

// Conditions
#include "DetDesc/IAlignment.h"
#include "DetDesc/AlignmentCondition.h"

//#include "Event/STLiteCluster.h"
//#include "Event/VeloLiteCluster.h"

// gsl stuff
#include "gsl/gsl_cdf.h"


using namespace LHCb;
using namespace Gaudi;



DECLARE_ALGORITHM_FACTORY( GAlign );

//-----------------------------------------------------------------------------

GAlign::GAlign( const std::string& name,
                ISvcLocator* pSvcLocator ):
  GaudiTupleAlg ( name , pSvcLocator ),
  m_converged(false),
  velo_detector(false),
  tt_detector(false),
  it_detector(false),
  ot_detector(false),
  m_evtsPerRun(0),
  m_MaxIterations(1),
  m_ForceIterations(false),
  m_iterations(0),
  m_chi2(0.0),
  m_ntr(0)
{
  // define track containers
  declareProperty("TAlignment_Config_Tool", m_AlignConfTool = "TAConfig");
  declareProperty("InputContainer", m_inputcontainer = TrackLocation::Default );
  declareProperty("Detectors", m_detectors);

  //define maximum number of iterations
  declareProperty("MaxIterations", m_MaxIterations = 5);
  declareProperty("evtsPerRun", m_evtsPerRun = 2000);
  declareProperty("forceIterations", m_ForceIterations = false );
}

GAlign::~GAlign() { 
  info() << "GAlign destructor" << endreq;
}

StatusCode GAlign::initialize() {
  info() << "Initialising GAlign alignment..." << endreq;
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) {
    error() << "Error initialising GAlign" << endreq;
    return sc;
  }
  // Retrieve measurement provider tool:
  m_measProvider = tool<IMeasurementProvider>( "MeasurementProvider");
  // Retrieve configuration tool
  m_taConfig = tool<ITAConfigTool>( "TAConfig" );
  sc = m_taConfig->Initialize( m_detectors );
  sc = m_taConfig->CacheDetElements();
  if ( sc.isFailure() ) { 
    error() << "Error caching detector elements" << endreq;
    return StatusCode::FAILURE;
  }
  // get the number of alignment (== global) parameters
  m_nGlPars = m_taConfig->NumAlignPars();
  m_align_err.resize( m_nGlPars );
  m_align.resize( m_nGlPars );
  m_estimated.resize( m_nGlPars );
  info() << "Number of global parameters = " << m_nGlPars << endreq;
  info() << "Size of m_estimated = " << m_estimated.size() << endreq;
  if ( sc.isFailure() ) {
    error() << "Error initalizing TAConfig" << endreq;
    return StatusCode::FAILURE;
  }
  std::string s("Velo");
  velo_detector = m_taConfig->AlignDetector( s );
  s = "TT";
  tt_detector = m_taConfig->AlignDetector( s );
  s = "OT";
  ot_detector = m_taConfig->AlignDetector( s );
  s = "IT";
  it_detector = m_taConfig->AlignDetector( s );

  // add listener to incident svc
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) {
     return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  }
  incSvc->addListener(this, "GlobalMPedeFit");
  incSvc->addListener(this, "Converged");
  return StatusCode::SUCCESS;
}

void GAlign::handle( const Incident &incident ) {
  if ( "GlobalMPedeFit" == incident.type() ) 
    this->GloFit();
  if ( "Converged" == incident.type() )
    this->Converged();
}

StatusCode GAlign::queryInterface( const InterfaceID &id, void **ppI ) {
   // check the placeholder
  if ( 0 == ppI ) 
     return StatusCode::FAILURE;
  // check ID of the interface
  if ( IIncidentListener::interfaceID() == id ) {
    *ppI = static_cast<IIncidentListener*>(this);
  } else {
    return Algorithm::queryInterface( id, ppI );
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool GAlign::Converged() {
  info() << "Convergence = " << m_converged << endreq;
  /*
  std::vector<double>::iterator i;
  double ave = 0.0;
  for( i = m_align.begin(); i < m_align.end(); i++ ) {
    info() << "m_align = " << *i << " ave = " << ave << endreq;
    ave += *i;
  }
  ave /= (double) ( m_align.size() );
  info() << "Average = " << ave << endreq;
  if ( ave < 0.0001 ) m_converged = true;
  */
  return m_converged;
}


StatusCode GAlign::execute() {
  debug() << "Into GAlign execute (1)" << endreq;
  if ( ! exist<Tracks>( m_inputcontainer ) ) {
    info() << "Error: no tracks in container " << m_inputcontainer << endreq;
    return StatusCode::FAILURE;
  } else {
    Tracks *inCont = get<Tracks>(m_inputcontainer);
    Tracks::const_iterator iterT;
    
    int tr_cnt = 0;
    bool crossed = false;
    
    for ( iterT = inCont->begin(), tr_cnt = 0; iterT != inCont->end(); iterT++, tr_cnt++) {
      Track *atrack = *iterT;
      if ( atrack->nStates() < 1 ) continue;
      if ( atrack->nMeasurements() == 0 ) {
	// load the measurements on the track
//	m_measProvider->load();
	// calculate from the track what the measurement was
	StatusCode mc = m_measProvider->load( *atrack );
	if ( ! mc ) {
	  error() << "Error re-creating measurements!" << endreq;
	  return StatusCode::FAILURE;
	}
      }
      // Reset MilleTool variables etc.
      StatusCode sc =  m_taConfig->ResetGlVars();
      // ************************
      // 
      // Loop over the LHCbIDs 
      // 
      // ************************
      int rank; // position in c-matrix for a detector element
      std::vector<LHCb::LHCbID> IDs;
      std::vector<LHCb::LHCbID>::const_iterator itID;
      double weight = 1.0;
      crossed = false;
      double trPar[4];
      double chiMat[4][4];
      m_taConfig->ZeroMatrVec( chiMat, trPar );
      for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
	// check if this LHCbID is really on the track
	LHCb::LHCbID id = *itID;
	if ( atrack->isOnTrack( id ) &&  atrack->isMeasurementOnTrack( id )) {
	  debug() << "LHCbID " << id.detectorType() << " is on track!" << " "
		  << "and has Measurement " << atrack->isMeasurementOnTrack( id ) << endreq;
	  double Z_position = 0.0;
	  double stereo_angle = 0.0;
	  double measMP = 0.0;
	  LHCb::State trState;
	  sc = m_taConfig->Rank( id, rank );
	  if ( sc.isFailure() ) {
	    debug() << "Not processing any data from tracks!" << endreq;
	    debug() << "while tt = " << tt_detector << " and it = " 
		    << it_detector << " and ot = " << ot_detector << endreq;
	    debug() << "This hit: isOT(): " << id.isOT() 
		    << " isIT(): " << id.isIT() << " isTT(): " 
		    << id.isTT() << " id.isVelo(): " << id.isVelo() << endreq;
	  }
	  if ( sc.isSuccess() ) {
	    m_taConfig->CalcResidual( *atrack, 
				      id, 
				      rank, 
				      weight, 
				      measMP,
				      Z_position, 
				      stereo_angle, 
				      trState ); 
	    m_taConfig->ConfMatrix( stereo_angle,
				    measMP,
				    weight,
				    Z_position,
				    trPar,
				    chiMat );
	  }
	} 
      } // First loop over hits
      // Do a local track fit to get an estimate of the track parameters
      int rrank = m_taConfig->InvMatrix( chiMat, trPar, 4 );
      double Z_position = -1000.0;
      // Start second loop over hits
      for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
	// check if this LHCbID is really on the track
	LHCb::LHCbID id = *itID;
	if ( atrack->isOnTrack( id ) &&  atrack->isMeasurementOnTrack( id )) {
	  debug() << "LHCbID " << id.detectorType() << " is on track!" << " "
		  << "and has Measurement " << atrack->isMeasurementOnTrack( id ) << endreq;
	  double stereo_angle = 0.0;
	  double measMP = 0.0;
	  Z_position = -1000.0;
	  LHCb::State trState;
	  sc = m_taConfig->Rank( id, rank );
	  if ( sc.isFailure() ) {
	    debug() << "Not processing any data from tracks!" << endreq;
	    debug() << "while tt = " << tt_detector << " and it = " 
		    << it_detector << " and ot = " << ot_detector << endreq;
	    debug() << "This hit: isOT(): " << id.isOT() 
		    << " isIT(): " << id.isIT() << " isTT(): " 
		    << id.isTT() << " id.isVelo(): " << id.isVelo() << endreq;
	  }
	  sc = m_taConfig->CalcResidual( *atrack,
				      id, 
				      rank, 
				      weight, 
				      measMP,
				      Z_position, 
				      stereo_angle, 
				      trState ); 
	  if ( sc.isFailure() )
	    continue;
	  // Configure the 'large' C-matrix
	  sc = m_taConfig->FillMatrix( rank, 
				       trPar,
				       measMP, 
				       Z_position, 
				       stereo_angle );
	}
      }
      /**************************************************************
       *  do local track fit with MP only when there was a track going through
       *  the detector part one wants to align.
       **********************************************************************/
      if ( Z_position > 0.0 ) {
	vector<double> trpar( 2 * m_taConfig->NumTrPars(), 0.0 );
	double chi2 = 0;
	//local track fit
	double residual = -99999.9;
        m_ntr++;
	sc = m_taConfig->LocalTrackFit(tr_cnt,trpar,0,m_estimated, chi2, residual);
	plot2D( m_iterations, chi2, "Chi2 vs iteration", 0.0, 20.0,0.0,10000,21,100);
	if ( chi2 > 0.0 ) m_chi2 += 1.0/chi2;
	//	info() << "average chi2 = " << 1/m_chi2 << " instantenous chi2 = " << chi2 << endreq;
	if ( sc != StatusCode::SUCCESS ) {
	  error() << "Error in LocalTrackFit: bailing out..." << endreq;
	  return StatusCode::FAILURE;
	}
	
	//	info()<< "------------------------------------" << endreq;
      }//long tracks 
    } 
  }
  return StatusCode::SUCCESS;
}

StatusCode GAlign::GloFit() {
  m_iterations++;
  /*************************
   ****    GLOBAL FIT  *****
   **************************/
  info() << "Doing global fit" << endreq;
  m_pull.resize( m_taConfig->NumAlignPars() );
  StatusCode sc = m_taConfig->GlobalFit( m_align, m_align_err, m_pull );
  if ( sc.isFailure() ) {
     error() << "Error in Millepede Global Fit" << endreq;
     return StatusCode::FAILURE;
  }
  std::vector<double> cp(m_estimated);
  if ( cp != m_estimated ) {
      error() << "Error copying vector..." << endreq;
      return StatusCode::FAILURE;
  } 
  info() << "m_estimated =  " << m_estimated << endreq;
  add(m_estimated, cp , m_align);
  info() << "m_align = " << m_align << endreq;
  info() << "m_estimated =  " << m_estimated << endreq;
  // check various convergence criteria.
  // One criterium could be a check whether the values 
  // of the m_align vector are less than a certain minimum:
  // here, the average is calculated
  double ave = 0.0;
  int n = 0;
  std::vector<double>::iterator it;
  for( it = m_align.begin(); it < m_align.end(); it++ ) {
    if ( fabs(*it) > 0.0 ) {
      ave += fabs(*it);
      n++;
    }
  }
  info() << "Number of tracks: " << m_ntr << " ave chi2 " << 1.0/m_chi2 << endreq;
  info() << "Iteration nr: " << m_iterations 
	 << " average misalignment " << ave/n 
	 << " average chi2 " << 1.0/m_chi2 
         << " nr of track used in fir : " << m_ntr << endreq;
  plot2D( m_iterations, ave/n, "Average misalignment vs iteration", 0.0, 20,-1.0,1.0,21,100 );
  //  plot2D( m_iterations, chi2, "Average chi2 of global fit vs iteration", 0.0, 10,-1.0,1.0,11,50 );
  // if the average deviation of each alignable component is less than 10^-4
  // bail out: we are satisfied with this accuracy.
  if ( ave/n < 0.0001 ) 
    m_converged = true;
  m_ntr = 0;
  m_chi2 = 0.0;
  return StatusCode::SUCCESS;
}


StatusCode GAlign::finalize(){
  debug() << "==> Finalize" << endreq;
  //  StatusCode sc = this->GloFit();
  //  info() << "Alignment parameters = " << m_align << endreq;
  return GaudiAlgorithm::finalize();
}



