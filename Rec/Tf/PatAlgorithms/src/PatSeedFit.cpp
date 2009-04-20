// $Id: PatSeedFit.cpp,v 1.3 2009-04-20 06:24:33 cattanem Exp $
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "Event/STLiteCluster.h"
#include "Event/Track.h"
#include "Event/OTLiteTime.h"
#include "Event/StateParameters.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackMomentumEstimate.h"
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTDetector.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "TfKernel/RecoFuncs.h"

#include "PatSeedFit.h"
#include "PatSeedTool.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PatSeedFit
//
// 2009-01-28 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PatSeedFit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatSeedFit::PatSeedFit(const std::string& type,
                       const std::string& name,
                       const IInterface* parent)
  : GaudiTool(type, name, parent)
{
  declareInterface<IPatSeedFit>(this);
  declareProperty( "StateErrorX2",              m_stateErrorX2          =   4. );
  declareProperty( "StateErrorY2",              m_stateErrorY2          = 400. );
  declareProperty( "StateErrorTX2",             m_stateErrorTX2         = 6e-5 );
  declareProperty( "StateErrorTY2",             m_stateErrorTY2         = 1e-4 );
  declareProperty( "PatSeedDRatio",             m_dRatio          = -3.2265e-4 );
  declareProperty( "PatSeedInitialArrow",       m_initialArrow    = 4.25307e-9 );
  declareProperty( "TrackMaxChi2",              m_maxChi2               = 10.0 );
}

//=============================================================================
// Destructor
//=============================================================================
PatSeedFit::~PatSeedFit() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatSeedFit::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize" << endmsg;

  m_patSeedTool = tool<PatSeedTool>("PatSeedTool", this);
  m_itDet = getDet<DeSTDetector>(DeSTDetLocation::IT);
  m_otDet = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  // Retrieve the raw bank decoder (for recreating the OTLiteTime)
  m_otdecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder") ;
  m_seedTool = tool<IPatSeedFit>( "PatSeedFit" );
  m_momentumTool = tool<ITrackMomentumEstimate>( "FastMomentumEstimate" );
  m_veloFitter = tool<ITrackFitter>( "Tf::PatVeloFitLHCbIDs", "FitVelo", this ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode PatSeedFit::finalize()
{
  debug() << "==> Finalize" << endmsg;


  return GaudiTool::finalize();  // must be called after all other actions
}

/// fit a PatSeedTrack
StatusCode PatSeedFit::fitSeed( const std::vector<LHCb::LHCbID> lhcbIDs, 
				std::vector<LHCb::State> *states) const
{

  // these are the z-positions for the output states
  std::vector<double> m_zOutputs  =
    boost::assign::list_of(StateParameters::ZBegT)(StateParameters::ZMidT)(StateParameters::ZEndT);
  double m_zReference = StateParameters::ZMidT ;

  //
  LHCb::STLiteCluster::FastContainer*  m_stLiteContainer =
    get<LHCb::STLiteCluster::FastContainer>(LHCb::STLiteClusterLocation::ITClusters);

  // loop over LHCbIDs, extract T-station measurements
  std::vector< PatFwdHit* > hits ;
  std::vector< Tf::STHit* > sthits ;
  std::vector< Tf::OTHit* > othits ;
  
  for( std::vector<LHCb::LHCbID>::const_iterator ihit = lhcbIDs.begin(); 
       ihit != lhcbIDs.end(); ++ihit ) { 
    if(  ihit->detectorType()==LHCb::LHCbID::IT ) {
      LHCb::STChannelID stChan = ihit->stID() ;
      const DeSTSector* stSector = m_itDet->findSector( stChan );
      if(stSector==0) {
          return Error( "No sector found for IT hit!" );
      }

      LHCb::STLiteCluster::FastContainer::const_iterator iclus =  
          m_stLiteContainer->find< LHCb::STLiteCluster::findPolicy >( stChan ) ;

      if( iclus != m_stLiteContainer->end() ) {
          Tf::STHit* sthit = new Tf::STHit(*stSector, *iclus ) ;
          sthits.push_back( sthit ) ;
          hits.push_back( new PatFwdHit( *sthit ) ) ;
      } else {
          return Error("Cannot find lite cluster!");
      }
      
    } else if( ihit->detectorType()==LHCb::LHCbID::OT ) {

      LHCb::OTChannelID otid = ihit->otID() ;
      const DeOTModule* module = m_otDet->findModule( otid ) ;
      if(module==0) {
        return Error("No module found for OT hit!");
      }

      LHCb::OTLiteTime otlitetime = m_otdecoder->time( otid );// *module ) ;
      Tf::OTHit* othit = new  Tf::OTHit( *module, otlitetime ) ;
      othits.push_back( othit) ;
      hits.push_back( new PatFwdHit( *othit ) ) ;
    }
  }
  // create the PatSeedTrack
  if( hits.size() >= 4 ) {
    std::vector< PatFwdHit* > seedhits(4,static_cast<PatFwdHit*>(0)) ;
    size_t numStereo(0) ;
    BOOST_FOREACH( PatFwdHit* ihit, hits ) {
      ihit->setSelected(true) ;
      if( ihit->hit()->isX() ) {
          if( seedhits[0] == 0 ||
              ihit->hit()->zAtYEq0() < seedhits[0]->hit()->zAtYEq0() )
              seedhits[0] = ihit ;
          if( seedhits[3] == 0 ||
              ihit->hit()->zAtYEq0() > seedhits[3]->hit()->zAtYEq0() )
              seedhits[3] = ihit ;
      } else {
          ++numStereo ;
          if( seedhits[1] == 0 ||
              ihit->hit()->zAtYEq0() < seedhits[1]->hit()->zAtYEq0() )
              seedhits[1] = ihit ;
          if( seedhits[2] == 0 ||
              ihit->hit()->zAtYEq0() > seedhits[2]->hit()->zAtYEq0() )
              seedhits[2] = ihit ;
      }
    }
    //for(int i=0; i<4; ++i) seedhits[i] ;

    PatSeedTrack * pattrack = 
      new PatSeedTrack(seedhits[0],seedhits[1],seedhits[2],seedhits[3], m_zReference, m_dRatio, m_initialArrow ) ;
    
    BOOST_FOREACH( PatFwdHit* ihit, hits ) {
      if( std::find(seedhits.begin(), seedhits.end(), ihit ) == seedhits.end() ) {
	 updateHitForTrack( ihit, pattrack->yAtZ(ihit->z()), 0);
	pattrack->addCoord( ihit ) ;
      }
    }
    
    StatusCode sc = fitTrack( *pattrack, m_maxChi2, 0, false, false).ignore() ;

    BOOST_FOREACH( PatFwdHit* ihit, hits ) {
    if( std::find(seedhits.begin(), seedhits.end(), ihit ) == seedhits.end() ) 
    updateHitForTrack( ihit, pattrack->yAtZ(ihit->z()), 0);
    }

    sc = fitTrack( *pattrack, m_maxChi2, 0, false, false).ignore() ;

    
    LHCb::State temp(Gaudi::TrackVector(pattrack->xAtZ(m_zReference), 
					pattrack->yAtZ(m_zReference),
					pattrack->xSlope(m_zReference), 
					pattrack->ySlope(m_zReference), 0.),
		     m_zReference, LHCb::State::AtT);

    double qOverP, sigmaQOverP;
    sc = m_momentumTool->calculate(&temp, qOverP, sigmaQOverP, true) ;
    /*
    if(states.size()>0 && states.begin().location()<=LHCb::State::EndVelo) {
      //success = m_momentumTool->calculate(&(track.firstState()), &temp, qOverP, sigmaQOverP, true) ;
      //qOverP = -qOverP;
      for(  std::vector<LHCb::State>::iterator istate = states.begin();
	    istate != states.end(); ++istate) {
	if(istate->location() <= LHCb::State::EndVelo) {
	  istate->setQOverP( qOverP ) ;
	  istate->setErrQOverP2(sigmaQOverP*sigmaQOverP) ;
        }
      }
    }
    */
    //else success = m_momentumTool->calculate(&temp, qOverP, sigmaQOverP, true) ;

    if(sc.isFailure()) {
      // if our momentum tool doesn't succeed, we have to try ourselves
      qOverP = pattrack->curvature() ;
      sigmaQOverP = qOverP; // be conservative
    }
    
    temp.setQOverP(qOverP);
    //== overestimated covariance matrix, as input to the Kalman fit
    Gaudi::TrackSymMatrix& cov = temp.covariance();
    cov(0,0) = m_stateErrorX2;
    cov(1,1) = m_stateErrorY2;
    cov(2,2) = m_stateErrorTX2;
    cov(3,3) = m_stateErrorTY2;
    cov(4,4) = sigmaQOverP * sigmaQOverP;

    BOOST_FOREACH( const double z, m_zOutputs ) {
      temp.setX(pattrack->xAtZ(z));
      temp.setY(pattrack->yAtZ(z));
      temp.setZ(z);
      temp.setTx(pattrack->xSlope(z));
      temp.setTy(pattrack->ySlope(z));
      states->push_back( temp );
    }
    // cleanup the track
    delete pattrack ;
  } else {
    info() << "Not enough SEED hits! "<<hits.size()  << endmsg ;
  }
  
  // deletthe hits
  
  BOOST_FOREACH( PatFwdHit* ihit, hits ) delete ihit ;
  BOOST_FOREACH( Tf::STHit* ihit, sthits ) delete ihit ;
  BOOST_FOREACH( Tf::OTHit* ihit, othits ) delete ihit ;
  //std::cout<<states->size()<<std::endl;
  return StatusCode::SUCCESS ;
}

/// fit a PatSeedTrack
StatusCode PatSeedFit::fitTrack( PatSeedTrack& track, double maxChi2,
		unsigned minPlanes, bool xOnly, bool forceDebug ) const
{
  if (m_patSeedTool->fitTrack(track, maxChi2, minPlanes, xOnly, forceDebug))
    return StatusCode::SUCCESS;
  else
    return StatusCode::FAILURE;
}

