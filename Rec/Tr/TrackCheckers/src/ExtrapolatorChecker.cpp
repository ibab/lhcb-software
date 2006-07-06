// $Id: ExtrapolatorChecker.cpp,v 1.1 2006-07-06 13:14:59 jvantilb Exp $
// Include files 

// local
#include "ExtrapolatorChecker.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

// from Event/TrackEvent
#include "Event/Track.h"

// from Event/MCEvent
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

using namespace Gaudi;
using namespace Gaudi::Units;
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( ExtrapolatorChecker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExtrapolatorChecker::ExtrapolatorChecker( const std::string& name,
                            ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator )
{
  declareProperty( "TrackSelector",
                   m_trackSelectorName = "TrackCriteriaSelector" );

  m_dets.push_back( "Velo" );
  m_dets.push_back( "TT"   );
  m_dets.push_back( "IT"   );
  m_dets.push_back( "OT"   );
}

//=============================================================================
// Destructor
//=============================================================================
ExtrapolatorChecker::~ExtrapolatorChecker() {}; 

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode ExtrapolatorChecker::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_trackSelector = tool<ITrackCriteriaSelector>( m_trackSelectorName,
                                                  "TrackSelector", this );
  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator"       );
  m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator" );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ExtrapolatorChecker::execute()
{  
  // Get the MCParticles
  MCParticles* particles = get<MCParticles>( MCParticleLocation::Default );
  
  // Loop over the MCParticles
  MCParticles::const_iterator iPart;
  for( iPart = particles->begin(); particles->end() != iPart; ++iPart ) {
    MCParticle* mcPart = *iPart;
    // Decide whether the MCParticle will be checked
    if ( m_trackSelector->select( mcPart ) ) {

      // Get the state at the vertex
      State* stateVtx;
      m_stateCreator -> createStateVertex( mcPart, stateVtx );

      // Find first MCHit
      std::string detName;
      MCHit* mcHit;
      int detID = findNextHit( mcPart, stateVtx->z(), mcHit, detName );
      
      // Get the entry point pf th MCHit
      XYZPoint entryP = mcHit -> entry();

      // Extrapolate through RF foil
      m_extrapolator -> propagate( *stateVtx, entryP.z() );
      TrackVector vec    = stateVtx -> stateVector();
      TrackSymMatrix cov = stateVtx -> covariance();

      // Determine the resolutions
      double dx  = vec(0) - entryP.x();
      double dy  = vec(1) - entryP.y();
      double dtx = vec(2) - mcHit -> dxdz();
      double dty = vec(3) - mcHit -> dydz();

      // fill the histograms
      plot1D( dx, 1, "X resolution at 1st meas", -0.5, 0.5, 100 );
      plot1D( dy, 2, "Y resolution at 1st meas", -0.5, 0.5, 100 );
      plot1D( dtx, 3, "Tx resolution at 1st meas", -0.01, 0.01, 100 );
      plot1D( dty, 4, "Ty resolution at 1st meas", -0.01, 0.01, 100 );
      plot1D( stateVtx->p() - mcHit->p(), 5,"dp at 1st meas", 
	      -0.3, 0.3, 100 );
      if ( cov(0,0) != 0 && cov(1,1) != 0 && cov(2,2) != 0 && cov(3,3) != 0){
	plot1D( dx / sqrt(cov(0,0)), 11,"X pull at 1st meas", -5., 5., 100 );
	plot1D( dy / sqrt(cov(1,1)), 12,"Y pull at 1st meas", -5., 5., 100 );
	plot1D( dtx / sqrt(cov(2,2)), 13,"Tx pull at 1st meas", -5., 5., 100);
	plot1D( dty / sqrt(cov(3,3)), 14,"Ty pull at 1st meas", -5., 5., 100);
      }
      delete stateVtx;

      const Gaudi::TrackSymMatrix zeroCov;
      State state;
      state.setState( entryP.x(),  entryP.y(), entryP.z(),
		      mcHit -> dxdz(), mcHit -> dydz(), qOverP(mcPart, mcHit));
      state.setCovariance( zeroCov );

      detID = findNextHit( mcPart, state.z(), mcHit, detName );

      while( 0 != mcHit ) {

	entryP = mcHit -> entry(); 
	
	// Extrapolate to next MCHit
	m_extrapolator -> propagate( state, entryP.z() );
	TrackVector vec    = state.stateVector();
	TrackSymMatrix cov = state.covariance();

	// Determine the resolutions
	double dx  = vec(0) - entryP.x();
	double dy  = vec(1) - entryP.y();
	double dtx = vec(2) - mcHit -> dxdz();
	double dty = vec(3) - mcHit -> dydz();

	// fill the histograms
	int ID = 100 * detID; 
	std::string title = detName + " hits" ;
	plot1D( dx, ID+1, "X resolution at "+title, -0.5, 0.5, 100 );
	plot1D( dy, ID+2, "Y resolution at "+title, -0.5, 0.5, 100 );
	plot1D( dtx, ID+3, "Tx resolution at "+title, -0.01, 0.01, 100 );
	plot1D( dty, ID+4, "Ty resolution at "+title, -0.01, 0.01, 100 );
	plot1D( state.p() - mcHit->p(), ID+5,"dp at "+title, 
		-0.3, 0.3, 100 );
	if ( cov(0,0) != 0 && cov(1,1) != 0 && cov(2,2) != 0 && cov(3,3) != 0){
	  plot1D( dx/sqrt(cov(0,0)), ID+11, "X pull at "+title, -5., 5.,100);
	  plot1D( dy/sqrt(cov(1,1)), ID+12, "Y pull at "+title, -5., 5.,100);
	  plot1D( dtx/sqrt(cov(2,2)), ID+13, "Tx pull at "+title, -5., 5.,100);
	  plot1D( dty/sqrt(cov(3,3)), ID+14, "Ty pull at "+title, -5., 5.,100);
	}
	
	state.setState( entryP.x(),  entryP.y(), entryP.z(),
			mcHit -> dxdz(), mcHit -> dydz(),qOverP(mcPart,mcHit));
	state.setCovariance( zeroCov );

	detID = findNextHit( mcPart, entryP.z(), mcHit, detName );
      }
    }
  } // End loop over MCParticles

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ExtrapolatorChecker::finalize()
{
  return GaudiHistoAlg::finalize();
}


//=============================================================================
// Find the next MCHit starting from a given z
// looping over the hits in all the tracking detectors
//=============================================================================
int ExtrapolatorChecker::findNextHit( const MCParticle* mcPart,
				      const double zRec,
				      MCHit*& closestHit,
				      std::string& detectorName) const
{
  detectorName = "Not found!";
  int detID = 0;
  MCHit* tmpClosestHit = 0;
  closestHit = tmpClosestHit;
  double closestZ = 100000.;

  std::vector<std::string>::const_iterator itDets;
  for( itDets = m_dets.begin(); itDets < m_dets.end(); ++itDets ) {
    findNextXxxHit( mcPart, zRec,
		    MCParticleLocation::Default + "2MC" + *itDets + "Hits",
		    tmpClosestHit );
    if ( tmpClosestHit != 0 &&
	 tmpClosestHit->entry().z() > zRec && 
	 tmpClosestHit->entry().z() < closestZ ) {
      closestHit = tmpClosestHit;
      closestZ = tmpClosestHit->entry().z();
      detectorName = *itDets;
      detID = 1 + itDets - m_dets.begin();
    }
  }
  return detID;
}

//=============================================================================
// Find the next MCHit of type Xxx starting from a given z
//=============================================================================
void ExtrapolatorChecker::findNextXxxHit( const MCParticle* mcPart,
					  const double zRec,
					  std::string linkPath,
					  MCHit*& closestHit ) const
{
  // Retrieve MCParticle to MCHit linker tables
  LinkedFrom<MCHit,MCParticle> mcp2mchitLink( evtSvc(), msgSvc(), linkPath );

  double closestZ = 100000.;

  MCHit* mcHit = mcp2mchitLink.first( mcPart );
  while( 0 != mcHit ) {
    double zOfHit = mcHit -> entry().z();
    // get the closest hit
    if ( zOfHit > zRec + 0.1 && zOfHit < closestZ ) {
      closestHit = mcHit;
      closestZ   = zOfHit ;
    }
    mcHit = mcp2mchitLink.next();
  }
}

//=============================================================================
//  
//=============================================================================
double ExtrapolatorChecker::qOverP( const MCParticle* mcPart,
				    const MCHit* mcHit ) const
{
  double momentum = mcPart -> p();
  if ( mcHit != NULL ) momentum = mcHit -> p();
  double charge = ( mcPart -> particleID().threeCharge() ) / 3.;
  // TODO: hack as it happens that some MCHits have p = 0!
  if ( mcHit != NULL &&  mcHit -> p() == 0. ) momentum = mcPart -> p();
  if( momentum > TrackParameters::lowTolerance ) {
    return charge / momentum;
  }
  else { return 0.; }
}

