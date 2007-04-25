//-----------------------------------------------------------------------------
/** @file GAlign.cpp
 *
 *  Implementation file for RICH reconstruction tool : GAlign
 *
 *  CVS Log :-
 *  $Id: GAlign.cpp,v 1.2 2007-04-25 15:44:20 jblouw Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "Kernel/LHCbID.h"

// Event
#include "Event/StateTraj.h"
#include "Event/Track.h"
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


using namespace LHCb;
using namespace Gaudi;



DECLARE_ALGORITHM_FACTORY( GAlign );

//-----------------------------------------------------------------------------

GAlign::GAlign( const std::string& name,
                              ISvcLocator* pSvcLocator ):
  GaudiTupleAlg ( name , pSvcLocator ),
  velo_detector(false),
  tt_detector(false),
  it_detector(false),
  ot_detector(false)
{



  // define track containers
  declareProperty("TAlignment_Config_Tool", m_AlignConfTool = "TAConfig");
  declareProperty("InputContainer", m_inputcontainer = TrackLocation::Default );
  declareProperty("Detectors", m_detectors);
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
  m_nGlPars = m_taConfig->NumAlignPars();
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
  return StatusCode::SUCCESS;
}

StatusCode GAlign::execute() {
  info() << "Into GAlign execute (1)" << endreq;
  if ( exist<Tracks>( m_inputcontainer ) ) {
    Tracks *inCont = get<Tracks>(m_inputcontainer);
    Tracks::const_iterator iterT;
    info() << "Number of tracks = " << " in " << m_inputcontainer << " : " << inCont->numberOfObjects() << endreq;
    int tr_cnt = 0;
    bool crossed = false;
    for ( iterT = inCont->begin(), tr_cnt = 0; iterT != inCont->end(); iterT++, tr_cnt++) {
      Track *atrack = *iterT;
      if ( atrack->nStates() < 1 ) continue;
      if ( atrack->nMeasurements() == 0 ) {
	// load the measurements on the track
         m_measProvider->load();
         // calculate from the track what the measurement was
         StatusCode mc = m_measProvider->load( *atrack );
         if ( ! mc ) {
	   error() << "Error re-creating measurements!" << endreq;
	   return StatusCode::FAILURE;
         }
      }
      // Reset MilleTool variables
      StatusCode sc = m_taConfig->ResetGlVars();
      // ************************
      // 
      // Loop over the LHCbIDs to find detector states:
      // 
      // ************************
      int rank; // position in c-matrix for a detector element
      std::vector<LHCb::LHCbID> IDs;
      std::vector<LHCb::LHCbID>::const_iterator itID;
      double weight = 1.0;
      crossed = false;
      //      info() << "Number of nodes on track: " << atrack->nodes().end() - atrack->nodes().begin() << endreq;
      for ( itID = (*iterT)->lhcbIDs().begin(); (*iterT)->lhcbIDs().end() != itID; ++itID ) {
	// check if this LHCbID is really on the track
	LHCb::LHCbID id = *itID; 
	if ( atrack->isOnTrack( id )  && atrack->isMeasurementOnTrack( id ) ) {
          debug() << "LHCbID " << id.detectorType() << " is on track!" << " "
          << "Measurement " << atrack->isMeasurementOnTrack( id ) << endreq;
	  if ( velo_detector && id.isVelo() ) {
	    VeloChannelID Channel = id.veloID();
	    if ( Channel.isRType() || Channel.isPhiType() ) {
	    }
	    sc = m_taConfig->Rank( id, rank );
	    if ( sc.isSuccess() ) {
	      m_res = m_taConfig->Residual( *atrack, id );
	    } else 
	      continue;
	  } else if ( ot_detector && id.isOT() ) {
	    m_res = m_taConfig->Residual( *atrack, id );
	    /*******************************************
	     ********* OT ******************************
	     *******************************************/
	    weight = 1.0; // default 'weight' of hit-error = 1.0
	    crossed = true;
	    sc = m_taConfig->Rank( id, rank );
	    // cluster hits in monolayers
	    if ( sc.isSuccess() ) {
	      m_taConfig->FillMatrix( *atrack, id, rank, weight );
	    } else 
	      continue;
	  } else if ( it_detector && id.isIT() ) {
	    crossed = true;
	    m_res = m_taConfig->Residual( *atrack, id );
	    STChannelID itid = id.stID();
	    sc = m_taConfig->Rank( id, rank );
	    if ( sc.isSuccess() ) {
	      const double weight = 1.0;
	      m_taConfig->FillMatrix( *atrack, id, rank, weight );
	    } else
	      continue;
	  } else if ( tt_detector && id.isTT() ) {
	    crossed = true;
	    m_res = m_taConfig->Residual( *atrack, id );
	    STChannelID ttid = id.stID();
	    sc = m_taConfig->Rank( id, rank );
	    if ( sc.isSuccess() ) {
	      const double weight = 1.0;
	      m_taConfig->FillMatrix( *atrack, id, rank, weight );
	    } else
	      continue;
	  } else {
	    debug() << "Not processing any data from tracks!" << endreq;
	    debug() << "while tt = " << tt_detector << " and it = " << it_detector << " and ot = " << ot_detector << endreq;
	    debug() << "This hit: isOT(): " << id.isOT() 
		   << " isIT(): " << id.isIT() << " isTT(): " 
		   << id.isTT() << " id.isVelo(): " << id.isVelo() << endreq;
	  }
	}
	double doca = m_res.R();
        if ( doca > 0 ) 
        debug() << "Residual = " << doca << endreq;
      }
      /******************
       *  do local track fit with MP only when there was a track going through
       *  the detector part one wants to align.
       **************************************/
      if ( crossed ) {
	vector<double> estimated( m_nGlPars, 0.0 ); // create vector, initalize contents to 0.0
	vector<double> trpar( 2 * m_taConfig->NumTrPars(), 0.0 );
	double chi2 = 0;
	//local track fit
	double residual = -99999.9;
	m_taConfig->LocalTrackFit(tr_cnt,trpar,0,estimated, chi2, residual);
	double z = 7853.42; // at T1
	State &state = atrack->closestState( z );
	Gaudi::XYZVector slope = state.slopes();
	plot( residual ,"Residual from mpede fit",-1000.0,1000.0,200);
	if ( m_taConfig->NumTrPars() > 4 ) {
	  plot2D( trpar[0]+z*trpar[2]+z*z*trpar[4],trpar[6]+z*trpar[8],"trpar x vs y at T1",-2500,2500,-3000,3000,500,600);
	  plot( (trpar[0]+z*trpar[2]+z*z*trpar[4])-state.x(),"x-position diff at T1",-1000.0,1000.0,200);
	  plot( ((trpar[0]+z*trpar[2]+z*z*trpar[4])-state.x())/sqrt(state.errX2()),"x-position pull at T1",-50.0,50.0,200);
	  plot( (trpar[6]+z*trpar[8])-state.y(),"y-position diff at T1",-1000.0,1000.0,200);
	  plot( ((trpar[6]+z*trpar[8])-state.y())/sqrt(state.errY2()),"y-position pull at T1",-50.0,50.0,200);
	} else {
	  plot2D( trpar[0]+z*trpar[2],trpar[4]+z*trpar[6],"trpar x vs y at T1",-2500,2500,-3000,3000,500,600);
	  double diff = ( (trpar[0]+z*trpar[2]) - state.x() )*( (trpar[0]+z*trpar[2]) - state.x() );
	  diff += ( (trpar[4]+z*trpar[6])-state.y() ) * ( (trpar[4]+z*trpar[6])-state.y() );
	  diff = sqrt(diff);
	  plot( diff,"Radial difference at T1", 0.0,1000.0,200);
	  plot( (trpar[0]+z*trpar[2])-state.x(),"x-position diff at T1",-1000.0,1000.0,200);
	  plot( ((trpar[0]+z*trpar[2])-state.x())/sqrt(state.errX2()),"x-position pull at T1",-50.0,50.0,200);
	  plot( (trpar[4]+z*trpar[6])-state.y(),"y-position diff at T1",-1000.0,1000.0,200);
	  plot( ((trpar[4]+z*trpar[6])-state.y())/sqrt(state.errY2()),"y-position pull at T1",-50.0,50.0,200);
	}

        z = 8535.24; // at T2
        state = atrack->closestState( z );
	slope = state.slopes();
        double qoverp = state.qOverP();

	if (  m_taConfig->NumTrPars() > 4 ) {
	  plot2D( trpar[0]+z*trpar[2]+z*z*trpar[4],trpar[6]+z*trpar[8],"trpar x vs y at T2",-2500,2500,-3000,3000,500,600);
	  plot( (trpar[0]+z*trpar[2]+z*z*trpar[4])-state.x(),"x-position diff at T2",-500.0,500.0,200);
	  plot( ((trpar[0]+z*trpar[2]+z*z*trpar[4])-state.x())/sqrt(state.errX2()),"x-position pull at T2",-50.0,50.0,200);
	  plot( (trpar[6]+z*trpar[8])-state.y(),"y-position diff at T2",-500.0,500.0,200);
	  plot( ((trpar[6]+z*trpar[8])-state.y())/sqrt(state.errY2()),"y-position pull at T2",-50.0,50.0,200);
	} else {
	  plot2D( trpar[0]+z*trpar[2],trpar[4]+z*trpar[6],"trpar x vs y at T2",-2500,2500,-3000,3000,500,600);
	  double diff = ( (trpar[0]+z*trpar[2]) - state.x() )*( (trpar[0]+z*trpar[2]) - state.x() );
	  diff += ( (trpar[4]+z*trpar[6])-state.y() ) * ( (trpar[4]+z*trpar[6])-state.y() );
	  diff = sqrt(diff);
	  plot( diff,"Radial difference at T2", 0.0,1000.0,200);
	  plot( (trpar[0]+z*trpar[2])-state.x(),"x-position diff at T2",-500.0,500.0,200);
	  plot( ((trpar[0]+z*trpar[2])-state.x())/sqrt(state.errX2()),"x-position pull at T2",-50.0,50.0,200);
	  plot( (trpar[4]+z*trpar[6])-state.y(),"y-position diff at T2",-500.0,500.0,200);
	  plot( ((trpar[4]+z*trpar[6])-state.y())/sqrt(state.errY2()),"y-position pull at T2",-50.0,50.0,200);
	}
	z = 9409.76; // at T3
	state = atrack->closestState( z );
	slope = state.slopes();

	if ( m_taConfig->NumTrPars() > 4 ) {
	  plot2D( trpar[0]+z*trpar[2]+z*z*trpar[4],trpar[6]+z*trpar[8],"trpar x vs y at T3",-2500,2500,-3000,3000,500,600);
	  plot( (trpar[0]+z*trpar[2]+z*z*trpar[4])-state.x(),"x-position diff at T3",-100.0,100.0,200);
	  plot( ((trpar[0]+z*trpar[2]+z*z*trpar[4])-state.x())/sqrt(state.errX2()),"x-position pull at T3",-50.0,50.0,200);
	  plot( (trpar[6]+z*trpar[8])-state.y(),"y-position diff at T3",-100.0,100.0,200);
	  plot( ((trpar[6]+z*trpar[8])-state.y())/sqrt(state.errY2()),"y-position pull at T3",-50.0,50.0,200);
	} else {
	  plot2D( trpar[0]+z*trpar[2],trpar[4]+z*trpar[6],"trpar x vs y at T3",-2500,2500,-3000,3000,500,600);
	  double diff = ( (trpar[0]+z*trpar[2]) - state.x() )*( (trpar[0]+z*trpar[2]) - state.x() );
	  diff += ( (trpar[4]+z*trpar[6])-state.y() ) * ( (trpar[4]+z*trpar[6])-state.y() );
	  diff = sqrt(diff);
	  plot( diff,"Radial difference at T3", 0.0,1000.0,200);
	  plot( (trpar[0]+z*trpar[2])-state.x(),"x-position diff at T3",-100.0,100.0,200);
	  plot( ((trpar[0]+z*trpar[2])-state.x())/sqrt(state.errX2()),"x-position pull at T3",-50.0,50.0,200);
	  plot( (trpar[4]+z*trpar[6])-state.y(),"y-position diff at T3",-100.0,100.0,200);
	  plot( ((trpar[4]+z*trpar[6])-state.y())/sqrt(state.errY2()),"y-position pull at T3",-50.0,50.0,200);
	}

	plot(chi2,"Chi2 of local fit",0.0,100000,100);
	plot( trpar[0], "track par 0 position", -5000.0,5000.0,400);
	plot( trpar[1], "track par 1 position", 0.55,0.8,200);
	plot( trpar[2], "track par 2 position", -1, 1, 200);
	plot( trpar[3], "track par 3 position", 0.00006, 0.0001, 200);
	plot( trpar[4], "track par 4 position", -500.2, 500.2, 200);
	plot( trpar[5], "track par 5 position", 8.0, 16, 200);
	plot( trpar[6], "track par 6 position", -0.4, 0.4, 100);
	plot( trpar[7], "track par 7 position", 0.001, 0.002, 200);
	if ( m_taConfig->NumTrPars() > 4 ) {
	  plot( trpar[8], "track par 8 position", -0.2, 0.2, 200);
	  plot( trpar[9], "track par 9 position", 0.0, 0.002, 200);
	}
	if ( ot_detector) 
	  plot(trpar[2], "Tx slope mpede fit",-1.5, 1.5, 100);
	else
	  plot(trpar[2], "Tx slope mpede fit",-0.4, 0.4, 100);
	plot(trpar[6], "Ty slope mpede fit",-0.4, 0.4, 100);


	if ( ot_detector ) 
	  plot( slope.x(), "Tx slope trackfit",-1.5, 1.5, 100);
	else
	  plot( slope.x(), "Tx slope trackfit",-.5, .5, 100);
	plot( slope.y(), "Ty slope trackfit",-0.4, 0.4, 100);
	if ( ot_detector) 
	  plot(trpar[2] - slope.x(), "Tx slope difference",-1.2, 1.2, 100);
	else
	  plot(trpar[2] - slope.x(), "Tx slope difference",-0.002, 0.002, 100);
	if ( ot_detector ) { 
	  if ( m_taConfig->NumTrPars() > 4 )
	    plot(trpar[8] - slope.y(), "Ty slope difference",-0.3, 0.3, 100);
	  else
	    plot(trpar[6] - slope.y(), "Ty slope difference",-0.3, 0.3, 100);
	} else {
	  if ( m_taConfig->NumTrPars() > 4 )
	    plot(trpar[8] - slope.y(), "Ty slope difference",-0.03, 0.03, 100);
	  else 
	    plot(trpar[6] - slope.y(), "Ty slope difference",-0.3, 0.3, 100);
	}
	double px, py, pz;
	double si = 1.0;
	m_res.GetCoordinates(px, py, pz);
	if ( px < 0 ) si = -1.0;
	plot( si * sqrt(m_res.Mag2()), "doca ", -100.0,200.0,300);
	plot( residual, "Mpede chi2/ndf", 0.0,2000.0,200);
      }
      //      info()<< "------------------------------------" << endreq;
    }
  } else {
     info() << "Error: no tracks in container " << m_inputcontainer << endreq;
  }

  return StatusCode::SUCCESS;
}



StatusCode GAlign::finalize(){
  debug() << "==> Finalize" << endreq;
  /*************************
   ****  do GLOBAL FIT  *****
   **************************/
  m_align.resize( m_taConfig->NumAlignPars() );
  m_align_err.resize( m_taConfig->NumAlignPars() );
  m_pull.resize( m_taConfig->NumAlignPars() );
  m_taConfig->GlobalFit( m_align, m_align_err, m_pull );
  //  m_Millepede->MakeGlobalFit(align,align_err,pull);
  return GaudiAlgorithm::finalize();
}



