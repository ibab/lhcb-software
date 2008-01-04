
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.cpp
 *
 *  Implementation file for algorithm class : RichCherenkovAngleMonitor
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngleMonitor.cpp,v 1.13 2008-01-04 16:15:14 jonrob Exp $
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
  : Rich::Rec::TupleAlgBase ( name, pSvcLocator ),
    m_richPartProp      ( NULL ),
    m_richRecMCTruth    ( NULL ),
    m_ckAngle           ( NULL ),
    m_trSelector        ( NULL )
{
  // min beta
  declareProperty( "MinBeta",     m_minBeta   = 0.999 );
  // number of bins
  declareProperty( "NumberBins1D",  m_nBins1D = 200 );
  declareProperty( "NumberBins2D",  m_nBins2D = 100 );
}

// Destructor
CherenkovAngleMonitor::~CherenkovAngleMonitor() {}

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
              minCkTheta[rad], maxCkTheta[rad], m_nBins1D );
      plot1D( thetaExpTrue, hid(rad,"ckThetaExp"), "Expected Cherenkov theta",
              minCkTheta[rad], maxCkTheta[rad], m_nBins1D );
      plot1D( phiRec, hid(rad,"ckPhi"), "Reconstructed Cherenkov phi", 0, 2*M_PI, m_nBins1D );
      plot1D( delTheta, hid(rad,"ckDiffAll"), "Rec-Exp CK theta all",
              -ckRange[rad], ckRange[rad], m_nBins1D );
      plot2D( phiRec, delTheta, hid(rad,"ckDiffAllVPhi"), "Rec-Exp CK theta all V CK Phi",
              0, 2*M_PI, -ckRange[rad], ckRange[rad], m_nBins2D, m_nBins2D );

      // theta versus phi plots
      plot2D( phiRec, thetaRec, hid(rad,"thetaVphi"), "CK theta V phi",
              0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
      if ( hitPnt.y() < 0 && hitPnt.x() < 0 )
      {
        plot2D( phiRec, thetaRec, hid(rad,"thetaVphiR1"), "CK theta V phi : y<0 x<0",
                0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
      }
      else if ( hitPnt.y() > 0 && hitPnt.x() < 0 )
      {
        plot2D( phiRec, thetaRec, hid(rad,"thetaVphiR2"), "CK theta V phi : y>0 x<0",
                0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
      }
      else if ( hitPnt.y() < 0 && hitPnt.x() > 0 )
      {
        plot2D( phiRec, thetaRec, hid(rad,"thetaVphiR3"), "CK theta V phi : y<0 x>0",
                0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
      }
      else if ( hitPnt.y() > 0 && hitPnt.x() > 0 )
      {
        plot2D( phiRec, thetaRec, hid(rad,"thetaVphiR4"), "CK theta V phi : y>0 x>0",
                0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
      }

      if ( mcType != Rich::Unknown )
      {
        // true CK photon ?
        const LHCb::MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton(photon);
        if ( photonParent )
        {

          // CK angles for true photons
          plot1D( thetaRec, hid(rad,mcType,"ckThetaTrue"), "Reconstructed Cherenkov theta : true",
                  minCkTheta[rad], maxCkTheta[rad], m_nBins1D );
          plot1D( thetaExpTrue, hid(rad,"ckThetaExpTrue"), "Expected Cherenkov theta : true",
                  minCkTheta[rad], maxCkTheta[rad], m_nBins1D );
          plot1D( phiRec,   hid(rad,mcType,"ckPhiTrue"), "Cherenkov phi : true", 0, 2*M_PI, m_nBins1D );
          plot1D( delTheta, hid(rad,"ckDiffTrue"), "Rec-Exp CK theta all : true",
                  -ckRange[rad],ckRange[rad], m_nBins1D );
          plot2D( phiRec, delTheta, hid(rad,"ckDiffTrueVPhi"), "Rec-Exp CK theta all V CK Phi : true",
                  0, 2*M_PI, -ckRange[rad], ckRange[rad], m_nBins2D, m_nBins2D );

          profile1D( pTot, delTheta, hid(rad,"ckDiffTrueVP"), "Rec-Exp CK theta Versus Ptot all : true",
                     minP, maxP, 50 );

          // theta versus phi plots
          plot2D( phiRec, thetaRec, hid(rad,"thetaVphiTrue"), "CK theta V phi : true CK photons",
                  0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
          if ( hitPnt.y() < 0 && hitPnt.x() < 0 )
          {
            plot2D( phiRec, thetaRec, hid(rad,"thetaVphiR1True"), "CK theta V phi : y<0 x<0 : true CK photons",
                    0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
          }
          else if ( hitPnt.y() > 0 && hitPnt.x() < 0 )
          {
            plot2D( phiRec, thetaRec, hid(rad,"thetaVphiR2True"), "CK theta V phi : y>0 x<0 : true CK photons",
                    0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
          }
          else if ( hitPnt.y() < 0 && hitPnt.x() > 0 )
          {
            plot2D( phiRec, thetaRec, hid(rad,"thetaVphiR3True"), "CK theta V phi : y<0 x>0 : true CK photons",
                    0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
          }
          else if ( hitPnt.y() > 0 && hitPnt.x() > 0 )
          {
            plot2D( phiRec, thetaRec, hid(rad,"thetaVphiR4True"), "CK theta V phi : y>0 x>0 : true CK photons",
                    0, 2*M_PI, minCkTheta[rad], maxCkTheta[rad], m_nBins2D, m_nBins2D );
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
                    minCkTheta[rad], maxCkTheta[rad], m_nBins1D );
            plot1D( delThetaMC, hid(rad,"mcckDiffTrue"), "Rec-MC CK theta true",
                    -ckRange[rad],ckRange[rad], m_nBins1D );
            plot1D( delThetaExpMC, hid(rad,"mcExpDiff"), "MC-Exp CK theta true",
                    -ckRange[rad],ckRange[rad], m_nBins1D );
            plot2D( phiMC, delThetaMC, hid(rad,"mcckDiffTrueVPhi"), "Rec-MC CK theta true V true CK Phi",
                    0, 2*M_PI, -ckRange[rad], ckRange[rad], m_nBins2D, m_nBins2D );

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

        }
        else // fake photon
        {
          
          // CK angles for fake photons
          plot1D( thetaRec, hid(rad,mcType,"ckThetaFake"), "Cherenkov theta : fake",
                  minCkTheta[rad], maxCkTheta[rad], m_nBins1D );
          plot1D( thetaExpTrue, hid(rad,"ckThetaExpFake"), "Expected Cherenkov theta : fake",
                  minCkTheta[rad], maxCkTheta[rad], m_nBins1D );
          plot1D( phiRec,   hid(rad,mcType,"ckPhiFake"), "Cherenkov phi : fake", 0, 2*M_PI, m_nBins1D );
          plot1D( delTheta, hid(rad,"ckDiffFake"), "Rec-Exp CK theta all : fake",
                  -ckRange[rad],ckRange[rad], m_nBins1D );
          profile1D( pTot, delTheta, hid(rad,"ckDiffFakeVP"), "Rec-Exp CK theta Versus Ptot all : fake",
                     minP, maxP, 50 ); 

        }

      } // mc type known

    } // loop over segment photons

  } // loop over segments

  return StatusCode::SUCCESS;
}
