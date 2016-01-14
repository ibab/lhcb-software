
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.cpp
 *
 *  Implementation file for algorithm class : RichCherenkovAngleMonitor
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

// Standard constructor, initializes variables
CherenkovAngleMonitor::CherenkovAngleMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : Rich::Rec::TupleAlgBase ( name, pSvcLocator )
{
  // min beta
  declareProperty( "MinBeta",     m_minBeta   = 0.999 );
  // number of bins
  setProperty( "NBins1DHistos", 100 );
  setProperty( "NBins2DHistos", 100 );
  // # CK phi regions
  declareProperty( "NumPhiRegions", m_nPhiRegions = 4 );
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
  acquireTool( "RichMassHypoRings",   m_massHypoRings   );
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
  //            Radiator          Aerogel  Rich1Gas  Rich2Gas
  const double ckRange[]      = { 0.015,   0.01,     0.005   };
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;
  const double xLimit[]       = { 400,     200,      1400    };
  const double yLimit[]       = { 400,     200,      1400    };

  // min and max P for histos
  //const double maxP = m_trSelector->maxPCut() * Gaudi::Units::GeV;
  //const double minP = m_trSelector->minPCut() * Gaudi::Units::GeV;
  const double maxP = 100 * Gaudi::Units::GeV;
  const double minP = 0   * Gaudi::Units::GeV;

  // Iterate over segments
  for ( auto * segment : *richSegments() )
  {

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // segment momentum
    const double pTot = std::sqrt(segment->trackSegment().bestMomentum().Mag2());

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

    // Nominal saturated CK theta for this radiator
    const auto nomSatCKTheta = m_ckAngle->nominalSaturatedCherenkovTheta(rad);

    // How far from CK theta saturation (for kaon) is this segment ?
    const auto fracCKThetaSat =
      ( m_ckAngle->avgCherenkovTheta(segment,Rich::Kaon) / nomSatCKTheta );

    // Loop over hypos for this segment
    unsigned int nRingPointTrue = 0;
    for ( const auto pid : m_richPartProp->particleTypes() )
    {
      // get the mass hypo ring for this pid
      const auto * ring = m_massHypoRings->massHypoRing( segment, pid );
      if ( ring )
      {
        // Number of run points
        const auto nPts = ring->ringPoints().size();
        // Fill info for this hypo ring
        richHisto1D( hid(rad,pid,"nMassHypoRingPnts"), "# Mass hypo ring points",
                     0.5, 200.5, 200 )->fill( nPts );
        richProfile1D( hid(rad,pid,"nMassHypoRingPntsVP"), "# Mass hypo ring points V Ptot",
                       minP, maxP, nBins1D() )->fill( pTot, nPts );
        richProfile1D( hid(rad,pid,"nMassHypoRingPntsVCKTheta"),
                       "# Mass hypo ring points V CK theta Exp",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )
          -> fill( m_ckAngle->avgCherenkovTheta(segment,pid), nPts );
        if ( pid == mcType ) { nRingPointTrue = nPts; }
      }
    }

    // Get photons for this segment
    const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    verbose() << " Found " << photons.size() << " photon candidates" << endmsg;

    // loop over photons
    for ( auto * photon : photons )
    {

      // Hit point associated to photon
      const Gaudi::XYZPoint & hitPnt = photon->richRecPixel()->globalPosition();

      // Cherenkov angles
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      const double phiRec   = photon->geomPhoton().CherenkovPhi();
      const double delTheta = thetaRec-thetaExpTrue;

      // phi 'region'
      const PhiDesc & phiR = phiRegionDesc( phiRec );

      richHisto1D( hid(rad,"ckTheta"), "Reconstructed Cherenkov theta",
                   minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec);
      richHisto1D( hid(rad,"ckThetaExp"), "Expected Cherenkov theta",
                   minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaExpTrue);
      richHisto1D( hid(rad,"ckPhi"), "Reconstructed Cherenkov phi", 0, 2*M_PI, nBins1D() )->fill(phiRec);
      richHisto1D( hid(rad,"ckDiffAll"), "Rec-Exp CK theta all",
                   -ckRange[rad], ckRange[rad], nBins1D() )->fill(delTheta);
      richHisto1D( hid(rad,"ckDiffAll"+phiR.first), "Rec-Exp CK theta all : "+phiR.second,
                   -ckRange[rad], ckRange[rad], nBins1D() )->fill(delTheta);
      richHisto2D( hid(rad,"ckDiffAllVPhi"), "Rec-Exp CK theta all V CK Phi",
                   0, 2*M_PI, nBins2D(), -ckRange[rad], ckRange[rad], nBins2D() )->fill(phiRec,delTheta);

      // only fill these if above Kaon threshold
      if ( fracCKThetaSat > 0 )
      {
        richHisto1D( hid(rad,"fracCKThetaSat"), "Fraction of nominal saturated Kaon CK Theta",
                     0.0, 1.0, nBins1D() )->fill(fracCKThetaSat);
        richProfile1D( hid(rad,"fracCKThetaSatVP"), "Fraction of nominal saturated Kaon CK Theta V Momentum",
                       minP, maxP, nBins1D() )->fill(pTot,fracCKThetaSat);
      }

      // theta versus phi plots
      richHisto2D( hid(rad,"thetaVphi"), "CK theta V phi",
                   0, 2*M_PI, nBins2D(),
                   minCkTheta[rad], maxCkTheta[rad], nBins2D() )->fill(phiRec,thetaRec);
      if ( hitPnt.y() < 0 && hitPnt.x() < 0 )
      {
        richHisto2D( hid(rad,"thetaVphiR1"), "CK theta V phi : y<0 x<0",
                     0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad],
                     nBins2D() )->fill(phiRec,thetaRec);
      }
      else if ( hitPnt.y() > 0 && hitPnt.x() < 0 )
      {
        richHisto2D( hid(rad,"thetaVphiR2"), "CK theta V phi : y>0 x<0",
                     0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad],
                     nBins2D() )->fill(phiRec,thetaRec);
      }
      else if ( hitPnt.y() < 0 && hitPnt.x() > 0 )
      {
        richHisto2D( hid(rad,"thetaVphiR3"), "CK theta V phi : y<0 x>0",
                     0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad],
                     nBins2D() )->fill(phiRec,thetaRec);
      }
      else if ( hitPnt.y() > 0 && hitPnt.x() > 0 )
      {
        richHisto2D( hid(rad,"thetaVphiR4"), "CK theta V phi : y>0 x>0",
                     0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad],
                     nBins2D() )->fill(phiRec,thetaRec);
      }

      richProfile1D( HID("avgCKThetaVGloX",rad),
                     "Average CK theta V Global X in radiator : Isolated Tracks",
                     -xLimit[rad], xLimit[rad], nBins1D() )
        ->fill( segment->trackSegment().middlePoint().x(), thetaRec );

      richProfile1D( HID("avgCKThetaVGloY",rad),
                     "Average CK theta V Global Y in radiator : Isolated Tracks",
                     -yLimit[rad], yLimit[rad], nBins1D() )
        ->fill( segment->trackSegment().middlePoint().y(), thetaRec );

      if ( mcType != Rich::Unknown )
      {
        // true CK photon ?
        const LHCb::MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton(photon);
        if ( photonParent )
        {

          // CK angles for true photons
          richHisto1D( hid(rad,"ckThetaTrue"), "Reconstructed Cherenkov theta : true",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec);
          richHisto1D( hid(rad,mcType,"ckThetaTrue"), "Reconstructed Cherenkov theta : true",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec);
          richHisto1D( hid(rad,"ckThetaExpTrue"), "Expected Cherenkov theta : true",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaExpTrue);
          richHisto1D( hid(rad,mcType,"ckPhiTrue"), "Cherenkov phi : true", 0, 2*M_PI, nBins1D() )->fill(phiRec);
          richHisto1D( hid(rad,"ckDiffTrue"), "Rec-Exp CK theta all : true",
                       -ckRange[rad],ckRange[rad], nBins1D() )->fill(delTheta);
          richHisto1D( hid(rad,mcType,"ckDiffTrue"), "Rec-Exp CK theta all : true",
                       -ckRange[rad],ckRange[rad], nBins1D() )->fill(delTheta);
          richHisto1D( hid(rad,"ckDiffTrue"+phiR.first), "Rec-Exp CK theta all : true : "+phiR.second,
                       -ckRange[rad],ckRange[rad], nBins1D() )->fill(delTheta);
          richHisto2D( hid(rad,"ckDiffTrueVPhi"), "Rec-Exp CK theta all V CK Phi : true",
                       0, 2*M_PI, nBins2D(), -ckRange[rad], ckRange[rad], nBins2D() )->fill(phiRec, delTheta);

          richProfile1D( hid(rad,"ckDiffTrueVP"), "Rec-Exp CK theta Versus Ptot all : true",
                         minP, maxP, nBins1D() )->fill(pTot,delTheta);
          richProfile1D( hid(rad,"absckDiffTrueVP"), "| Rec - Exp CK theta | Versus Ptot all : true",
                         minP, maxP, nBins1D() )->fill(pTot,fabs(delTheta));

          richProfile1D( hid(rad,"ckDiffTrueVCKtheta"), "Rec-Exp CK theta Versus CK theta all : true",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec,delTheta);
          richProfile1D( hid(rad,"absckDiffTrueVCKtheta"), "| Rec-Exp CK theta | Versus CK theta all : true",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec,fabs(delTheta));

          // theta versus phi plots
          richHisto2D( hid(rad,"thetaVphiTrue"), "CK theta V phi : true CK photons",
                       0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad], nBins2D() )
            ->fill(phiRec, thetaRec);
          if ( hitPnt.y() < 0 && hitPnt.x() < 0 )
          {
            richHisto2D( hid(rad,"thetaVphiR1True"), "CK theta V phi : y<0 x<0 : true CK photons",
                         0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad], nBins2D() )
              ->fill(phiRec, thetaRec);
          }
          else if ( hitPnt.y() > 0 && hitPnt.x() < 0 )
          {
            richHisto2D( hid(rad,"thetaVphiR2True"), "CK theta V phi : y>0 x<0 : true CK photons",
                         0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad], nBins2D() )
              ->fill(phiRec, thetaRec);
          }
          else if ( hitPnt.y() < 0 && hitPnt.x() > 0 )
          {
            richHisto2D( hid(rad,"thetaVphiR3True"), "CK theta V phi : y<0 x>0 : true CK photons",
                         0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad], nBins2D() )
              ->fill(phiRec, thetaRec);
          }
          else if ( hitPnt.y() > 0 && hitPnt.x() > 0 )
          {
            richHisto2D( hid(rad,"thetaVphiR4True"), "CK theta V phi : y>0 x>0 : true CK photons",
                         0, 2*M_PI, nBins2D(), minCkTheta[rad], maxCkTheta[rad], nBins2D() )
              ->fill(phiRec, thetaRec);
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
            delThetaMC    = thetaRec     - thetaMC;
            delThetaExpMC = thetaExpTrue - thetaMC;

            richHisto1D( hid(rad,"ckThetaMCTrue"), "MC Cherenkov theta",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaMC);
            richHisto1D( hid(rad,"mcckDiffTrue"), "Rec-MC CK theta true",
                         -ckRange[rad],ckRange[rad], nBins1D() )->fill(delThetaMC);
            richHisto1D( hid(rad,mcType,"mcckDiffTrue"), "Rec-MC CK theta true",
                         -ckRange[rad],ckRange[rad], nBins1D() )->fill(delThetaMC);
            richHisto1D( hid(rad,"mcckDiffTrue"+phiR.first), "Rec-MC CK theta true : "+phiR.second,
                         -ckRange[rad],ckRange[rad], nBins1D() )->fill(delThetaMC);
            richHisto1D( hid(rad,mcType,"mcckDiffTrue"+phiR.first), "Rec-MC CK theta true : "+phiR.second,
                         -ckRange[rad],ckRange[rad], nBins1D() )->fill(delThetaMC);
            richHisto1D( hid(rad,"mcExpDiffTrue"), "Exp-MC CK theta true",
                         -ckRange[rad],ckRange[rad], nBins1D() )->fill(delThetaExpMC);
            richHisto1D( hid(rad,"mcExpDiffTrue"+phiR.first), "Exp-MC CK theta : true : "+phiR.second,
                         -ckRange[rad],ckRange[rad], nBins1D() )->fill(delThetaExpMC);

            richHisto2D( hid(rad,"mcckDiffTrueVPhi"), "Rec-MC CK theta true V true CK Phi : true",
                         0, 2*M_PI, nBins2D(), 
                         -ckRange[rad], ckRange[rad], nBins2D() )->fill(phiMC, delThetaMC);

            richProfile1D( hid(rad,"absMcCkDiffTrueVP"), "| Rec-MC CK theta | Versus Ptot : true",
                           minP, maxP, nBins1D() )->fill(pTot,fabs(delThetaMC));
            richProfile1D( hid(rad,mcType,"absMcCkDiffTrueVP"), "| Rec-MC CK theta | Versus Ptot : true",
                           minP, maxP, nBins1D() )->fill(pTot,fabs(delThetaMC));

            richProfile1D( hid(rad,"absMcCkDiffTrueVCKtheta"), "| Rec-MC CK theta | Versus CK theta all : true",
                           minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec,fabs(delThetaMC));
            richProfile1D( hid(rad,mcType,"absMcCkDiffTrueVCKtheta"), "| Rec-MC CK theta | Versus CK theta all : true",
                           minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec,fabs(delThetaMC));

            // resolution versus # ring points plots
            richProfile1D( hid(rad,"absMcCkDiffTrueVnRingPoints"),
                           "| Exp-MC CK theta | Versus # Mass Hypo Ring Points",
                           0.5, 200.5, 100 ) -> fill( nRingPointTrue, fabs(delThetaMC) );

          } // mc photon

          // make a tuple

          Tuple tuple = nTuple( hid(rad,"ckResTuple"), "CKTuple" ) ;
          StatusCode sc = StatusCode::SUCCESS;
          sc = sc && tuple->column( "RecoPtot", pTot );
          sc = sc && tuple->column( "RecoCKtheta" , thetaRec );
          sc = sc && tuple->column( "RecoCKphi" ,   phiRec );
          sc = sc && tuple->column( "McCKtheta" , thetaMC );
          sc = sc && tuple->column( "McCKphi" ,  phiMC );
          sc = sc && tuple->column( "ExpCKtheta", thetaExpTrue );
          sc = sc && tuple->column( "NumRingPoints", nRingPointTrue );
          sc = sc && tuple->write();
          if ( sc.isFailure() ) return sc;

        }
        else // fake photon
        {

          // CK angles for fake photons
          richHisto1D( hid(rad,mcType,"ckThetaFake"), "Cherenkov theta : fake",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec);
          richHisto1D( hid(rad,"ckThetaExpFake"), "Expected Cherenkov theta : fake",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaExpTrue);
          richHisto1D( hid(rad,mcType,"ckPhiFake"), "Cherenkov phi : fake", 0, 2*M_PI, nBins1D() )->fill(phiRec);
          richHisto1D( hid(rad,"ckDiffFake"), "Rec-Exp CK theta all : fake",
                       -ckRange[rad],ckRange[rad], nBins1D() )->fill(delTheta);
          richHisto1D( hid(rad,"ckDiffFake"+phiR.first), "Rec-Exp CK theta all : fake : "+phiR.second,
                       -ckRange[rad],ckRange[rad], nBins1D() )->fill(delTheta);
          richProfile1D( hid(rad,"ckDiffFakeVP"), "Rec-Exp CK theta Versus Ptot all : fake",
                         minP, maxP, nBins1D() )->fill(pTot,delTheta);

        }

      } // mc type known

    } // loop over segment photons

  } // loop over segments

  return StatusCode::SUCCESS;
}

const CherenkovAngleMonitor::PhiDesc &
CherenkovAngleMonitor::phiRegionDesc( const double phi ) const
{
  const int region = phiRegion(phi);
  Rich::Map<int,PhiDesc>::const_iterator iDesc = m_rDesc.find(region);
  if ( m_rDesc.end() == iDesc )
  {
    const double phiInc = 2.0 / m_nPhiRegions;
    const double phimin = (phiInc*region     - (phiInc/2.0));
    const double phimax = (phiInc*(region+1) - (phiInc/2.0));
    std::ostringstream desc;
    desc << "phi = " << boost::format("%6.2f PI -> %6.2f PI") % phimin % phimax;
    m_rDesc[region] = PhiDesc( boost::lexical_cast<std::string>(region), desc.str() );
    return m_rDesc[region];
  }
  return iDesc->second;
}

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CherenkovAngleMonitor )

//---------------------------------------------------------------------------
