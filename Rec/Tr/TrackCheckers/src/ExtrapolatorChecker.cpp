// $Id: ExtrapolatorChecker.cpp,v 1.5 2008-02-08 08:13:01 cattanem Exp $
// Include files 

// local
#include "ExtrapolatorChecker.h"

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

// from Event/TrackEvent
#include "Event/Track.h"

// from Event/MCEvent
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// boost
#include <boost/assign/list_of.hpp>

using namespace Gaudi;
using namespace Gaudi::Units;
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( ExtrapolatorChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExtrapolatorChecker::ExtrapolatorChecker( const std::string& name,
                            ISvcLocator* pSvcLocator ) :
  TrackCheckerBase( name , pSvcLocator )
{

  std::vector<std::string> tmp = boost::assign::list_of("IT")("OT")("TT")("Velo");
  declareProperty("Detectors" , m_dets = tmp);
}

//=============================================================================
// Destructor
//=============================================================================
ExtrapolatorChecker::~ExtrapolatorChecker() {} 


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
    if ( selected( mcPart ) ) {

      // Get the state at the vertex
      State stateVtx;
      idealStateCreator() -> createStateVertex( mcPart, stateVtx );

      // Find first MCHit
      std::string detName;
      MCHit* mcHit;
      int detID = findNextHit( mcPart, stateVtx.z(), mcHit, detName );
      
      // Get the entry point of the MCHit
      XYZPoint entryP = mcHit -> entry();

      // Extrapolate through RF foil
      extrapolator() -> propagate( stateVtx, entryP.z() );
      TrackVector vec    = stateVtx.stateVector();
      TrackSymMatrix cov = stateVtx.covariance();

      // Correct tx and ty from the MCHit for the magnetic field
      double tx,ty;
      correctSlopes( mcPart, mcHit, tx, ty );
      
      // Determine the resolutions
      double dx  = vec(0) - entryP.x();
      double dy  = vec(1) - entryP.y();
      double dtx = vec(2) - tx;
      double dty = vec(3) - ty;
      double dqp = vec(4) - qOverP(mcPart, mcHit);

      // fill the histograms
      plot1D( dx, 1, "X resolution at 1st meas", -0.5, 0.5, 100 );
      plot1D( dy, 2, "Y resolution at 1st meas", -0.5, 0.5, 100 );
      plot1D( dtx, 3, "Tx resolution at 1st meas", -0.01, 0.01, 100 );
      plot1D( dty, 4, "Ty resolution at 1st meas", -0.01, 0.01, 100 );
      plot1D( stateVtx.p() - mcHit->p(), 5,"dp at 1st meas", 
              -0.3, 0.3, 100 );
      if ( cov(0,0) != 0 && cov(1,1) != 0 && cov(2,2) != 0 && cov(3,3) != 0){
        plot1D( dx / sqrt(cov(0,0)), 11,"X pull at 1st meas", -5., 5., 100 );
        plot1D( dy / sqrt(cov(1,1)), 12,"Y pull at 1st meas", -5., 5., 100 );
        plot1D( dtx / sqrt(cov(2,2)), 13,"Tx pull at 1st meas", -5., 5., 100);
        plot1D( dty / sqrt(cov(3,3)), 14,"Ty pull at 1st meas", -5., 5., 100);
      }
      if ( cov(4,4) != 0 )
        plot1D( dqp / sqrt(cov(4,4)), 15,"q/p pull at 1st meas", -5., 5., 100);

     
      const Gaudi::TrackSymMatrix zeroCov;
      State state;
      state.setState( entryP.x(),  entryP.y(), entryP.z(),
                      tx, ty, qOverP(mcPart, mcHit));
      state.setCovariance( zeroCov );

      detID = findNextHit( mcPart, state.z(), mcHit, detName );

      while( 0 != mcHit ) {

        entryP = mcHit -> entry(); 
        double dz = entryP.z() - state.z();
	
        // Extrapolate to next MCHit
        extrapolator()->propagate( state, entryP.z() );
        TrackVector vec    = state.stateVector();
        TrackSymMatrix cov = state.covariance();

        // Correct tx and ty from the MCHit for the magnetic field
        correctSlopes( mcPart, mcHit, tx, ty );

        // Determine the resolutions
        double dx  = vec(0) - entryP.x();
        double dy  = vec(1) - entryP.y();
        double dtx = vec(2) - tx;
        double dty = vec(3) - ty;
        double dqp = vec(4) - qOverP(mcPart, mcHit);

        // Define the ranges for the resolution plots
        double xr = 0.04;
        double tr = 0.001;
        double pr = 0.3;

        // Determine the histogram title
        int ID = 100 * detID; 
        std::string title = " at " + detName + " hits" ;
        if ( dz > 4.*m && (detName == "OT" || detName == "IT" )) {
          ID = 1100;
          title = " after magnet extrapolation";
          xr = 5.;
          tr = 0.005;
          pr = 5.;
        } else if ( dz > 1.*m && detName == "TT" ) {          
          ID = 1000;
          title = " after RICH1 extrapolation";
          xr = 5.;
          tr = 0.005;
          pr = 10.;
        }

        // fill the histograms
        plot1D( dx, ID+1, "X resolution"+title, -xr, xr, 100 );
        plot1D( dy, ID+2, "Y resolution"+title, -xr, xr, 100 );
        plot1D( dtx, ID+3, "Tx resolution"+title, -tr, tr, 100 );
        plot1D( dty, ID+4, "Ty resolution"+title, -tr, tr, 100 );
        plot1D( state.p() - mcHit->p(), ID+5,"dp"+title, -pr, pr, 100 );
        if ( cov(0,0) != 0 && cov(1,1) != 0 && cov(2,2) != 0 && cov(3,3) != 0){
          plot1D( dx/sqrt(cov(0,0)), ID+11, "X pull"+title, -5., 5.,100);
          plot1D( dy/sqrt(cov(1,1)), ID+12, "Y pull"+title, -5., 5.,100);
          plot1D( dtx/sqrt(cov(2,2)), ID+13, "Tx pull"+title, -5., 5.,100);
          plot1D( dty/sqrt(cov(3,3)), ID+14, "Ty pull"+title, -5., 5.,100);
        }
	      if ( cov(4,4) != 0 )
          plot1D( dqp / sqrt(cov(4,4)), ID+15,"q/p pull"+title, -5., 5., 100);

        state.setState( entryP.x(),  entryP.y(), entryP.z(),
                        tx, ty, qOverP(mcPart,mcHit));
        state.setCovariance( zeroCov );
        
        detID = findNextHit( mcPart, entryP.z(), mcHit, detName );
      }
    }
  } // End loop over MCParticles

  return StatusCode::SUCCESS;
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
// Determine q/p given an MCHit and a MCParticle
//=============================================================================
double ExtrapolatorChecker::qOverP( const MCParticle* mcPart,
                                    const MCHit* mcHit ) const
{
  double charge = ( mcPart -> particleID().threeCharge() ) / 3.;
  double momentum = mcPart -> p();
  if ( mcHit != NULL && mcHit -> p() != 0. ) momentum = mcHit -> p();
  return ( momentum > TrackParameters::lowTolerance ) ? charge / momentum : 0.;
}

//=============================================================================
// Correct slopes for magnetic field given an MCHit and a MCParticle
//=============================================================================
void ExtrapolatorChecker::correctSlopes( const MCParticle* mcPart,
                                         const MCHit* mcHit,
                                         double& tx, double& ty ) const
{
  // TODO: I hope this method can be removed as soon as the displacement vector
  // in the MCHit is calculated in Gauss using the momentum direction of the
  // *entry point*. (JvT: 27/10/2006).

  // Get magnetic field vector
  Gaudi::XYZVector B;
  fieldSvc()->fieldVector( mcHit -> midPoint() , B );

  // Calculate new displacement vector and tx,ty slopes
  Gaudi::XYZVector d = mcHit -> displacement();
  Gaudi::XYZVector dNew = d - ( 0.5 * d.R() * qOverP(mcPart, mcHit) * 
                                d.Cross(B) * eplus * c_light);
  tx = dNew.x() / dNew.z();
  ty = dNew.y() / dNew.z();  
}


