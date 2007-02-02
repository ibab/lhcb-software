
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.cpp
 *
 *  Implementation file for algorithm class : RichCherenkovAngleMonitor
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngleMonitor.cpp,v 1.10 2007-02-02 10:07:11 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichCherenkovAngleMonitor.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CherenkovAngleMonitor );

// Standard constructor, initializes variables
CherenkovAngleMonitor::CherenkovAngleMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : Rich::Rec::TupleAlgBase     ( name, pSvcLocator ),
    m_richPartProp      ( NULL ),
    m_richRecMCTruth    ( NULL ),
    m_ckAngle           ( NULL ),
    m_trSelector        ( NULL )
{
  // min beta
  declareProperty( "MinBeta",     m_minBeta   = 0.999 );
}

// Destructor
CherenkovAngleMonitor::~CherenkovAngleMonitor() {};

//  Initialize
StatusCode CherenkovAngleMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::TupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "TrackSelector",      m_trSelector, this );

  return sc;
}

// Main execution
StatusCode CherenkovAngleMonitor::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const Rich::HistoID hid;
  //            Radiator          Aerogel  Rich1Gas    Rich2Gas
  const double ckRange[]      = { 0.015,   0.01,    0.005   };
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;

  // min and max P for histos
  const double maxP = m_trSelector->maxPCut() * Gaudi::Units::GeV;
  const double minP = m_trSelector->minPCut() * Gaudi::Units::GeV;

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // segment momentum
    const double pTot = sqrt(segment->trackSegment().bestMomentum().Mag2());

    // beta for pion
    const double pionbeta = m_richPartProp->beta( pTot, Rich::Pion );
    if ( pionbeta < m_minBeta ) continue; // skip non-saturated tracks

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // Expected Cherenkov theta angle for true particle type
    // if MC type is unknown, assume pion (maybe type should be job option ???)
    const double thetaExpTrue = ( mcType == Rich::Unknown ?
                                  m_ckAngle->avgCherenkovTheta( segment, Rich::Pion ) :
                                  m_ckAngle->avgCherenkovTheta( segment, mcType ) );

    // Get photons for this segment
    const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    verbose() << " Found " << photons.size() << " photon candidates" << endreq;

    // loop over photons
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot )
    {
      LHCb::RichRecPhoton * photon = *iPhot;

      // Hit point associated to photon
      const Gaudi::XYZPoint & hitPnt = photon->richRecPixel()->globalPosition();

      // Cherenkov angles
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      const double phiRec   = photon->geomPhoton().CherenkovPhi();
      const double delTheta = thetaRec-thetaExpTrue;

      plot1D( thetaRec, hid(rad,"ckTheta"), "Reconstructed Cherenkov theta",
              minCkTheta[rad], maxCkTheta[rad] );
      plot1D( thetaExpTrue, hid(rad,"ckThetaExp"), "Expected Cherenkov theta",
              minCkTheta[rad], maxCkTheta[rad] );
      plot1D( phiRec, hid(rad,"ckPhi"), "Reconstructed Cherenkov phi", 0, 2*M_PI );
      plot1D( delTheta, hid(rad,"ckDiffAll"), "Rec-Exp CK theta all",
              -ckRange[rad],ckRange[rad]);

      // theta versus phi plots
      if ( hitPnt.y() < 0 && hitPnt.x() < 0 )
      {
        plot2D( thetaRec, phiRec, hid(rad,"thetaVphiR1"), "CK phi V theta : y<0 x<0",
                minCkTheta[rad], maxCkTheta[rad], 0, 2*M_PI, 100,100 );
      }
      else if ( hitPnt.y() > 0 && hitPnt.x() < 0 )
      {
        plot2D( thetaRec, phiRec, hid(rad,"thetaVphiR2"), "CK phi V theta : y>0 x<0",
                minCkTheta[rad], maxCkTheta[rad], 0, 2*M_PI, 100,100 );
      }
      else if ( hitPnt.y() < 0 && hitPnt.x() > 0 )
      {
        plot2D( thetaRec, phiRec, hid(rad,"thetaVphiR3"), "CK phi V theta : y<0 x>0",
                minCkTheta[rad], maxCkTheta[rad], 0, 2*M_PI, 100,100 );
      }
      else if ( hitPnt.y() > 0 && hitPnt.x() > 0 )
      {
        plot2D( thetaRec, phiRec, hid(rad,"thetaVphiR4"), "CK phi V theta : y>0 x>0",
                minCkTheta[rad], maxCkTheta[rad], 0, 2*M_PI, 100,100 );
      }

      if ( mcType != Rich::Unknown )
      {
        // true CK photon ?
        const LHCb::MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton(photon);
        if ( photonParent )
        {

          // CK angles for true photons
          plot1D( thetaRec, hid(rad,mcType,"ckThetaTrue"), "Cherenkov theta : true",
                  minCkTheta[rad], maxCkTheta[rad] );
          plot1D( thetaExpTrue, hid(rad,"ckThetaExpTrue"), "Expected Cherenkov theta : true",
                  minCkTheta[rad], maxCkTheta[rad] );
          plot1D( phiRec,   hid(rad,mcType,"ckPhiTrue"), "Cherenkov phi : true", 0, 2*M_PI );
          plot1D( delTheta, hid(rad,"ckDiffTrue"), "Rec-Exp CK theta all : true",
                  -ckRange[rad],ckRange[rad]);
          profile1D( delTheta, pTot, hid(rad,"ckDiffTrueVP"), "Rec-Exp CK theta Versus Ptot all : true",
                     minP, maxP, 50 );

          // theta versus phi plots
          if ( hitPnt.y() < 0 && hitPnt.x() < 0 )
          {
            plot2D( thetaRec, phiRec, hid(rad,"thetaVphiR1True"), "CK phi V theta : y<0 x<0 : true CK photons",
                    minCkTheta[rad], maxCkTheta[rad], 0, 2*M_PI, 100,100 );
          }
          else if ( hitPnt.y() > 0 && hitPnt.x() < 0 )
          {
            plot2D( thetaRec, phiRec, hid(rad,"thetaVphiR2True"), "CK phi V theta : y>0 x<0 : true CK photons",
                    minCkTheta[rad], maxCkTheta[rad], 0, 2*M_PI, 100,100 );
          }
          else if ( hitPnt.y() < 0 && hitPnt.x() > 0 )
          {
            plot2D( thetaRec, phiRec, hid(rad,"thetaVphiR3True"), "CK phi V theta : y<0 x>0 : true CK photons",
                    minCkTheta[rad], maxCkTheta[rad], 0, 2*M_PI, 100,100 );
          }
          else if ( hitPnt.y() > 0 && hitPnt.x() > 0 )
          {
            plot2D( thetaRec, phiRec, hid(rad,"thetaVphiR4True"), "CK phi V theta : y>0 x>0 : true CK photons",
                    minCkTheta[rad], maxCkTheta[rad], 0, 2*M_PI, 100,100 );
          }

          // Associated MCRichOpticalPhoton
          const LHCb::MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(photon);
          double thetaMC       = -999;
          double phiMC         = -999;
          double delThetaMC    = -999;
          double delThetaExpMC = -999;
          if ( mcPhot )
          {
            thetaMC       = mcPhot->cherenkovTheta();
            phiMC         = mcPhot->cherenkovPhi();
            delThetaMC    = thetaRec-mcPhot->cherenkovTheta();
            delThetaExpMC = thetaExpTrue-mcPhot->cherenkovTheta();

            plot1D( thetaMC, hid(rad,"ckThetaMC"), "MC Cherenkov theta",
                    minCkTheta[rad], maxCkTheta[rad] );
            plot1D( delThetaMC, hid(rad,"mcckDiffTrue"), "Rec-MC CK theta true",
                    -ckRange[rad],ckRange[rad]);
            plot1D( delThetaExpMC, hid(rad,"mcExpDiff"), "MC-Exp CK theta true",
                    -ckRange[rad],ckRange[rad]);
          } // mc photon

            // make a tuple

          Tuple tuple = nTuple( hid(rad,"ckResTuple"), "CKTuple" ) ;

          tuple->column( "RecoPtot", pTot );
          tuple->column( "RecoCKtheta" , thetaRec );
          tuple->column( "RecoCKphi" ,   phiRec );
          tuple->column( "McCKtheta" , thetaMC );
          tuple->column( "McCKphi" ,  phiMC );
          tuple->column( "ExpCKtheta", thetaExpTrue );

          tuple->write();

        } // true photon

      } // mc type known

    } // loop over segment photons

  } // loop over segments

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode CherenkovAngleMonitor::finalize()
{
  // Execute base class method
  return Rich::Rec::TupleAlgBase::finalize();
}

