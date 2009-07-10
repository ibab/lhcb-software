/*
 *
 *  Implementation file for Alignment algorithm : GAlign
 *
 *  CVS Log :-
 *  $Id: GAlign.cpp,v 1.15 2009-07-10 07:34:14 wouter Exp $
 *
 *  @author J.Blouw Johan.Blouw@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "Kernel/LHCbID.h"

// Event
#include "TrackKernel/StateTraj.h"
#include "Event/Track.h"
#include "Event/MuonMeasurement.h"
#include "GAlign.h"

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

#include "Math/SMatrix.h"

using namespace LHCb;
using namespace Gaudi;



DECLARE_ALGORITHM_FACTORY( GAlign );

//-----------------------------------------------------------------------------

GAlign::GAlign( const std::string& name,
                ISvcLocator* pSvcLocator ):
  GaudiTupleAlg ( name , pSvcLocator ),
  m_converged(false),
  m_taConfig(0),
  m_measProvider(0),
  m_AlignConfTool(""),
  m_nGlPars(0),
  velo_detector(false),
  tt_detector(false),
  it_detector(false),
  ot_detector(false),
  muon_detector(false),
  m_evtsPerRun(0),
  m_MaxIterations(1),
  m_ForceIterations(false),
  m_iterations(0),
  m_chi2(0.0),
  m_chi2B4(0.0),
  m_ntr(0),
  m_ntrB4(0)
{
  // define track containers
  declareProperty("TAlignment_Config_Tool", m_AlignConfTool = "TAConfig");
  declareProperty("InputContainer", m_inputcontainer = TrackLocation::Default );
  declareProperty("Detectors", m_detectors );

  //define maximum number of iterations
  declareProperty("MaxIterations", m_MaxIterations = 5);
  declareProperty("evtsPerRun", m_evtsPerRun = 2000);
  declareProperty("forceIterations", m_ForceIterations = false );
  declareProperty("MinIterations", m_iterationsMin = 5 );
}

GAlign::~GAlign() { 
  info() << "GAlign destructor" << endreq;
}

StatusCode GAlign::initialize() {
  info() << "Initialising GAlign alignment..." << endreq;
  info() << "Properties:" << endreq;
  info() << "            TAlignment_Config_Tool: " << m_AlignConfTool << endreq;
  info() << "            InputContainer        : " << m_inputcontainer << endreq;
  info() << "            Detectors             : " << m_detectors << endreq;
  info() << "            MaxIterations         : " << m_MaxIterations << endreq;
  info() << "            evtsPerRun            : " << m_evtsPerRun << endreq;
  info() << "            forceIterations       : " << m_ForceIterations << endreq;
  info() << "            MinIterations         : " << m_iterationsMin << endreq;
   
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
  m_align_err.resize( m_nGlPars, 0.0 );
  m_align.resize( m_nGlPars, 0.0 );
  m_estimated.resize( m_nGlPars, 0.0 );
  m_ntrack_pars = m_taConfig->NumTrPars();
  // set track counter
  tr_cnt = 0;
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
  s = "Muon";
  muon_detector = m_taConfig->AlignDetector( s );
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
    error() << "Error: no tracks in container " << m_inputcontainer << endreq;
    return StatusCode::FAILURE;
  } else {
    Tracks *inCont = get<Tracks>(m_inputcontainer);
    Tracks::const_iterator iterT;
    bool crossed = false;
    for ( iterT = inCont->begin(); iterT != inCont->end(); iterT++ ) {
      // create & initialize the track parameter vector with zeros
      if ( m_iterations == 0 )
	m_taConfig->MakeTrackParVec();
      Track *atrack = *iterT;
      if ( atrack->nStates() < 1 ) continue;
      if ( atrack->nMeasurements() == 0 ) {
	// load the measurements on the track
	StatusCode mc = m_measProvider->load( *atrack );
	if ( ! mc ) {
	  error() << "Error re-creating measurements!" << endreq;
	  return StatusCode::FAILURE;
	}
      }
      tr_cnt++;
      debug() << "Looping... track nr: " << tr_cnt << endreq;
      // Reset MilleTool variables etc.
      m_taConfig->ResetGlVars();
      m_taConfig->ResetLVars();
      // ************************
      // 
      // Loop over the LHCbIDs 
      // 
      // ************************
      std::vector<LHCb::LHCbID>::const_iterator itID;
      crossed = false;
      std::vector<double> trPar( m_ntrack_pars, 0.0 ); // set size & contents of vector for track parameters
      m_taConfig->SetTrackPar( trPar, tr_cnt );
      m_taConfig->ZeroMatrix();
      int hit_cnt = 0;
      for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
	// check if this LHCbID is really on the track
	LHCb::LHCbID id = *itID;
	if ( atrack->isOnTrack( id ) &&  atrack->isMeasurementOnTrack( id )
	     && (( id.isTT() && tt_detector ) ||
                 ( id.isOT() && ot_detector ) ||
                 ( id.isIT() && it_detector ) ||
                 ( id.isMuon() && muon_detector) ) ) {
	  hit_cnt++;
	  //	  info() << "LHCbID " << id.detectorType() << " is on track!" << " "
	  //		 << "and has Measurement " << atrack->isMeasurementOnTrack( id ) << endreq;
	  double measMP = 0.0;
	  StatusCode sc = m_taConfig->Rank( id ); // note, this sets also the variable m_rank_nr, local to TAConfig
	  if ( sc.isFailure() ) {
            error() << "Not processing any data from tracks!" << endreq;
            error() << "while tt = " << tt_detector << " and it = "
                    << it_detector << " and ot = " << ot_detector << endreq;
            error() << "This hit: isOT(): " << id.isOT()
                    << " isIT(): " << id.isIT() << " isTT(): "
                    << id.isTT() << " isMuon() " << id.isMuon() << " id.isVelo(): " << id.isVelo() << endreq;
	  }
          // in case we have more than one measurement per LHCbID (e.g. for the Muonchambers)
  	  // loop over the measurements that belong to this LHCbID.
	  int numTrajs = 1;
  	  if ( id.isMuon() ) numTrajs++;
 	  for ( int i = 0; i < numTrajs; i++ ) {
	    LHCb::Measurement *trMeas = m_measProvider->measurement( id, i );
 	    LHCb::MuonMeasurement *muMeas;
            if ( trMeas->type() == LHCb::Measurement::Muon ) {
 		muMeas = dynamic_cast<MuonMeasurement*>(trMeas);
                info() << "Muon measuremnt type = " << muMeas->muonProjection() << endreq;
            }
	    if ( sc.isSuccess() ) {
	      sc = m_taConfig->CalcResidual( tr_cnt,
					     *atrack, 
					     trMeas, 
					     id,
  					     true,
					     measMP ); 
              if ( sc.isFailure() ) {
                error() << "1: Error calculating residual" << endreq;
		continue;
              }
	      sc = m_taConfig->ConfMatrix( tr_cnt, 
					   measMP,
					   trPar );
              if ( sc.isFailure()  ) {
                error() << "1: Error configuring matrix" << endreq;
		continue;
	      }
            }
          } // loop over measurements which belong to lhcbid.
	}
      } // First loop over hits
      if ( hit_cnt == 0 ) 
	// there were no usefull hits on this track, continue with next
	continue; 
      // Do a local track fit to get an estimate of the track parameters
      int rrank = m_taConfig->InvMatrix( trPar, m_ntrack_pars );
      // histogramize the track parameteres
      plot( trPar[0], "x0", -20,20,200);
      plot( trPar[1], "tx", -0.25,0.25,400);
      plot( trPar[2], "y0", -300, 300, 200);
      plot( trPar[3], "ty", -0.4, 0.4, 400);
      if ( rrank == -1 )
	return StatusCode::FAILURE;
      // store the track parameters
      m_taConfig->SetTrackPar( trPar, tr_cnt );
      // start second loop over hits
      m_taConfig->ResetLVars(); 
      int nmes = 0;
      bool flagC = true;
      bool flagC2 = true;
      for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
        LHCb::LHCbID id = *itID;
        if ( atrack->isOnTrack( id ) &&  atrack->isMeasurementOnTrack( id )
             && (( id.isTT() && tt_detector ) ||
                 ( id.isOT() && ot_detector ) ||
                 ( id.isIT() && it_detector ) ||
                 ( id.isMuon() && muon_detector) ) ) {
          double measMP = 0.0;
          LHCb::State trState;
          StatusCode sc = m_taConfig->Rank( id );
	  if ( sc.isFailure() ) {
            error() << "Not processing any data from tracks!" << endreq;
	    error() << "while tt = " << tt_detector << " and it = "
		  << it_detector << " and ot = " << ot_detector << endreq;
            error() << "This hit: isOT(): " << id.isOT()
                    << " isIT(): " << id.isIT() << " isTT(): "
                    << id.isTT() << " id.isVelo(): " << id.isVelo() << endreq;
	    return sc;
          }
          int numTrajs = 1;
          if ( id.isMuon() ) numTrajs++;
          for ( int i = 0; i < numTrajs; i++ ) {
            LHCb::Measurement *trMeas = m_measProvider->measurement( id, i );
            // get the angle of the trajectory wrt to the 'standard' X-axis...
            nmes++;
	    sc = m_taConfig->CalcResidual( tr_cnt,
					   *atrack,
                                           trMeas,
                                           id,
                                           false,
                                           measMP );
	    if ( sc.isFailure() ) {
	      error() << "Error in CalcResidual" << endreq;
	      continue;
	    }
	  }
	}
	if ( m_taConfig->GetLChi2() > 3.0 )
	  flagC2 = flagC2 && (false);
      }
      bool neXt = false;
      flagC=true;
      if ( tr_cnt == 0 && m_iterations > 9 ) 
         neXt = true;
      m_taConfig->CheckLChi2( m_ntrack_pars, nmes, flagC, neXt );
      if ( m_iterations > 15 )
         flagC = flagC && flagC2;
      m_taConfig->SetFlag( flagC, tr_cnt );
      if ( flagC == false ) 
	 nfalse++;
      else 
         ntrue++;
      if ( flagC == true )
         m_chi2 += m_taConfig->GetLChi2(); 
      m_taConfig->ResetLVars();
      
      // Start third loop over hits
      for ( itID = atrack->lhcbIDs().begin(); atrack->lhcbIDs().end() != itID; ++itID ) {
        if ( flagC == false ) // go to next track if flag of chi2-cut-pass-test is false
           break;
	// check if this LHCbID is really on the track
	LHCb::LHCbID id = *itID;
	if ( atrack->isOnTrack( id ) &&  atrack->isMeasurementOnTrack( id )
	     && (( id.isTT() && tt_detector ) ||
                 ( id.isOT() && ot_detector ) ||
                 ( id.isIT() && it_detector ) ||
                 ( id.isMuon() && muon_detector) ) ) {
	  double measMP = 0.0;
	  LHCb::State trState;
	  StatusCode sc = m_taConfig->Rank( id );
	  if ( ! sc ) {
	    error() << "Not processing any data from tracks!" << endreq;
	  }
          // in case we have more than one measurement per LHCbID (e.g. for the Muonchambers)
          // loop over the measurements that belong to this LHCbID.
          int numTrajs = 1;
          if ( id.isMuon() ) numTrajs++;
          for ( int i = 0; i < numTrajs; i++ ) {
            LHCb::Measurement *trMeas = m_measProvider->measurement( id, i );
	    // get the angle of the trajectory wrt to the 'standard' X-axis...
	    measMP = -99999.99;
	    sc = m_taConfig->CalcResidual( tr_cnt,
					   *atrack,
				           trMeas, 
 					   id,
					   false,
				           measMP ); 
	    if ( fabs(measMP) > 1000.0 ) {
	      info() << "Measurement = " << measMP << endreq;
	      info() << "tr_cnt = " << tr_cnt << endreq;
	    }
	    if ( sc.isFailure() ) {
	      error() << "Error in CalcResidual, will continue!" << endreq;
	      continue;
	    }
	    // Configure the 'large' C-matrix
	    sc = m_taConfig->FillMatrix(  tr_cnt,
				           measMP );
	    if ( sc.isFailure() ) {
	      error() << "Error in FillMatrix" << endreq;
	      continue;
	    }
	  }
        }
      }
      /**************************************************************
       *  do local track fit with MP only when there was a track going through
       *  the detector part one wants to align.
       **********************************************************************/
      if ( nmes > 5 ) { // at least 5 hits on the track
	std::vector<double> trpar( 2 * m_taConfig->NumTrPars(), 0.0 );
	double chi2 = 0;
	double residual = -99999.9;
        if ( flagC == true )
           m_ntr++;
	std::vector<double> estimated( m_estimated.size(), 0.0);
	//        std::vector<double> estimated = m_estimated;
	StatusCode sc = m_taConfig->LocalTrackFit( tr_cnt, trpar, estimated, /*chi2,*/ residual );
	plot2D( m_iterations, chi2, "Chi2 vs iteration", 0.0, 20.0,0.0,10000,21,100);
	if ( chi2 > 0.0 ) m_chi2 += 1.0/chi2;
	if ( sc.isFailure() ) {
	  error() << "Error in LocalTrackFit: bailing out..." << endreq;
	  return StatusCode::FAILURE;
	}
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
  info() << "m_align_err = " << m_align_err <<"\n" << endreq;
  info() << "ntrue= "<< ntrue<< endl;
  info() << "nfalse= "<< nfalse<< endl;
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
  m_taConfig->SetAlignmentPar( m_estimated );
  
  m_Tdof = m_ntr * m_taConfig->NumTrPars();
  m_Tdof += m_estimated.size();
  if ( m_chi2B4 != 0.0 )
     m_converged = true;
  for ( it = m_align.begin(); it < m_align.end(); ++it ) {
    if ( fabs( *it ) > 0.0 ) {
       if ( fabs( *it )/fabs(m_estimated[n]) > 0.01 ) {
          n++;
          m_converged = m_converged && false;
       }
    }
  }
  m_chi2 = m_chi2/m_Tdof;
  m_chi2B4 = m_chi2;
  m_ntrB4 = m_ntr;
  
  ntrue = 0;
  nfalse = 0;
  m_ntr = 0;
  m_chi2 = 0.0;
  tr_cnt = 0;
  return StatusCode::SUCCESS;
}


StatusCode GAlign::finalize(){
  debug() << "==> Finalize" << endreq;
  StatusCode sc = this->GloFit();
  if ( sc.isFailure() )
    error() << "Error calculating alignment parameters"<< endreq;
  sc = m_taConfig->StoreParameters( m_align );
  if ( sc.isFailure() )
    error() << "Error storing alignment parameters in memory"<< endreq;
  info() << "Alignment parameters = " << m_align << endreq;
  return GaudiAlgorithm::finalize();
}



