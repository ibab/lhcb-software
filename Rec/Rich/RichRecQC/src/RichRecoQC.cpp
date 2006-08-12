
//---------------------------------------------------------------------------------
/** @file RichRecoQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : RichRecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.cpp,v 1.27 2006-08-12 10:53:31 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

// local
#include "RichRecoQC.h"

// namespaces
using namespace LHCb;

//---------------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecoQC>          s_factory ;
const        IAlgFactory& RichRecoQCFactory = s_factory ;

// Standard constructor, initializes variables
RichRecoQC::RichRecoQC( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richPartProp      ( 0 ),
    m_ckAngle           ( 0 ),
    m_richRecMCTruth    ( 0 ),
    m_truePhotCount     ( Rich::NRadiatorTypes, 0 ),
    m_nSegs             ( Rich::NRadiatorTypes, 0 ),
    m_chThetaRecHistoLimitMax (  Rich::NRadiatorTypes, 999 ),
    m_chThetaRecHistoLimitMin (  Rich::NRadiatorTypes, 0 )
{
  // Declare job options
  // min beta
  declareProperty( "MinBeta",     m_minBeta   = 0.999 );
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
  declareProperty( "TrackMomentumCuts", m_trSelector.setMomentumCuts() );
  // use of MC info
  declareProperty( "UseMCInfo",   m_useMCInfo = true );

  // Ch Theta Rec histogram limits: low, high -> aerogel, C4F10, Cf4
  m_chThetaRecHistoLimitMin[Rich::Aerogel]  = 0.1;
  m_chThetaRecHistoLimitMax[Rich::Aerogel]  = 0.3;
  m_chThetaRecHistoLimitMin[Rich::Rich1Gas] = 0.03;
  m_chThetaRecHistoLimitMax[Rich::Rich1Gas] = 0.08;
  m_chThetaRecHistoLimitMin[Rich::Rich2Gas] = 0.01;
  m_chThetaRecHistoLimitMax[Rich::Rich2Gas] = 0.05;
  declareProperty( "ChThetaRecHistoLimitMax", m_chThetaRecHistoLimitMax );
  declareProperty( "ChThetaRecHistoLimitMin", m_chThetaRecHistoLimitMin );
}

// Destructor
RichRecoQC::~RichRecoQC() {};

// Initialisation
StatusCode RichRecoQC::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( m_useMCInfo )
  {
    // acquire tools
    acquireTool( "RichParticleProperties", m_richPartProp );
    acquireTool( "RichCherenkovAngle",      m_ckAngle     );
    acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
    acquireTool( "RichCherenkovResolution", m_ckRes       );
  }
  
  // Configure track selector
  if ( !m_trSelector.configureTrackTypes(msg()) )
    return Error( "Problem configuring track selection" );
  m_trSelector.printTrackSelection( info() );

  return sc;
}

// Main execution
StatusCode RichRecoQC::execute()
{
  debug() << "Execute" << endreq;

  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Rich Histo ID
  const RichHistoID hid;

  // Histo ranges               Aero   Rich1Gas  Rich2Gas
  const double ckResRange[] = { 0.01, 0.005,  0.0025 };

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );
  
  // make sure RichrecPixels are ready
  if ( pixelCreator()->newPixels().isFailure() )
    return Error( "Problem creating RichRecPixels" );
  
  // make sure photons are available
  if ( photonCreator()->reconstructPhotons().isFailure() )
    return Error( "Problem creating RichRecPhotons" );

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    // track selection
    if ( !m_trSelector.trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    double thetaExpTrue(0.0), resExpTrue(0.0);
    if ( m_useMCInfo )
    {
      // True particle type
      const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
      if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly..

      // segment momentum
      const double pTot = sqrt(segment->trackSegment().bestMomentum().Mag2());

      // beta for ion hypo
      //const double beta = m_richPartProp->beta( pTot, mcType );
      const double beta = m_richPartProp->beta( pTot, Rich::Pion );
      if ( beta < m_minBeta ) continue; // skip non-saturated tracks

      // Expected Cherenkov theta angle for true particle type
      thetaExpTrue = m_ckAngle->avgCherenkovTheta( segment, mcType );

      // Cherenkov angle resolution for true type
      resExpTrue = m_ckRes->ckThetaResolution( segment, mcType );
    }
    
    // loop over photons for this segment
    unsigned int truePhotons(0);
    double avRecTrueTheta(0);
    for ( RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      RichRecPhoton * photon = *iPhot;

      // reconstructed theta
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      const double phiRec = photon->geomPhoton().CherenkovPhi();
      plot1D( thetaRec, hid(rad,"thetaRec"), "Reconstructed Ch Theta", m_chThetaRecHistoLimitMin[rad],
              m_chThetaRecHistoLimitMax[rad], 50 );
      plot1D( phiRec, hid(rad,"phiRec"), "Reconstructed Ch Phi", 0.0, 2*Gaudi::Units::pi, 50 );

      // skip the rest if no MC
      if ( !m_useMCInfo ) continue;

      // Is this a true photon ?
      const MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton( photon );
      if ( photonParent )
      {
        ++truePhotons;
        avRecTrueTheta += thetaRec;
        // resolution plot
        plot1D( thetaRec-thetaExpTrue,
                hid(rad,"ckRes"), "Rec-Exp Cktheta : beta=1", -ckResRange[rad], ckResRange[rad], 50 );
        if ( resExpTrue>0 )
        {
          // pull plot
          const double ckPull = (thetaRec-thetaExpTrue)/resExpTrue;
          plot1D( ckPull, hid(rad,"ckPull"), "(Rec-Exp)/Res Cktheta : beta=1", -4, 4, 50 );
        }

      }

    } // photon loop

    // number of true photons
    if ( truePhotons > 0 )
    {
      plot1D( truePhotons, hid(rad,"nCKphots"), "True # p.e.s : beta=1", -0.5, 50, 51 );
      profile1D( avRecTrueTheta/(double)truePhotons, 
                 truePhotons, hid(rad,"nCKphotsVcktheta"), "True # p.e.s Versus CK theta : beta=1", 
                 m_chThetaRecHistoLimitMin[rad], m_chThetaRecHistoLimitMax[rad] );
      m_truePhotCount[rad] += truePhotons;
      ++m_nSegs[rad];
    }

  } // end loop over segments

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichRecoQC::finalize()
{

  // test fitting

  // Rich Histo ID
  //const RichHistoID hid;

  // Fit params
  // CK res
  /*
  std::vector< std::vector<double> > ckResParams(Rich::NRadiatorTypes);
  ckResParams[Rich::Aerogel]  = boost::assign::list_of(0.0)(0.002)(100.0);
  ckResParams[Rich::Rich1Gas] = boost::assign::list_of(0.0)(0.0015)(100.0);
  ckResParams[Rich::Rich2Gas] = boost::assign::list_of(0.0)(0.001)(100.0);
  std::vector< std::pair<double,double> > ckResRange(Rich::NRadiatorTypes);
  ckResRange[Rich::Aerogel]  = std::pair<double,double>( -0.004, 0.004 );
  ckResRange[Rich::Rich1Gas] = std::pair<double,double>( -0.003, 0.003 );
  ckResRange[Rich::Rich2Gas] = std::pair<double,double>( -0.002, 0.002 );
  // CK pull
  std::vector< std::vector<double> > ckPullParams(Rich::NRadiatorTypes);
  ckPullParams[Rich::Aerogel]  = boost::assign::list_of(0.0)(1.0)(100.0);
  ckPullParams[Rich::Rich1Gas] = boost::assign::list_of(0.0)(1.0)(100.0);
  ckPullParams[Rich::Rich2Gas] = boost::assign::list_of(0.0)(1.0)(100.0);
  std::vector< std::pair<double,double> > ckPullRange(Rich::NRadiatorTypes);
  ckPullRange[Rich::Aerogel]  = std::pair<double,double>( -2.0, 2.0 );
  ckPullRange[Rich::Rich1Gas] = std::pair<double,double>( -2.0, 2.0 );
  ckPullRange[Rich::Rich2Gas] = std::pair<double,double>( -2.0, 2.0 );
  */

  // min number of entries for fitting
  //const int minEnts = 10;

  // statistical tool
  const RichStatDivFunctor occ("%10.2f +-%7.2f");

  info() << "=============================================================================="
         << endreq;

  // track selection
  info() << " Track Selection : " << m_trSelector.selectedTracksAsString()
         << " : beta > " << m_minBeta
         << endreq;

  // loop over radiators
  //for ( Rich::Radiators::const_iterator rad = Rich::radiators().begin();
  //      rad != Rich::radiators().end(); ++rad )
  //{
  for ( int irad = 0; irad < Rich::NRadiatorTypes; ++irad )
  {
    const Rich::RadiatorType rad = (Rich::RadiatorType)irad;

    // rad name
    std::string radName = Rich::text(rad);
    radName.resize(8,' ');
    // photon count
    if ( m_truePhotCount[rad]>0 )
    {
      info() << " " << radName << " Av. # CK photons = "
             << occ(m_truePhotCount[rad],m_nSegs[rad]) << " photons/segment" << endreq;

      /*
      // CK resolution
      AIDA::IHistogram1D * hRes = histo1D( HistoID(hid(rad,"ckRes")) );
      if ( hRes && hRes->entries() > minEnts )
      {
        AIDA::IFitResult * fit = fitHisto( hRes, ckResRange[rad], ckResParams[rad] );
        if ( fit && fit->isValid() )
        {
          info() << " " << radName << "CK res  : fit status=" << fit->fitStatus()
                 << " quality=" << fit->quality() << " ndf=" << fit->ndf() << endreq
                 << "                 : " << fit->fittedParameterNames()[0] << " = "
                 << 100*fit->fittedParameters()[0] << " +- " << 100*fit->errors()[0] << " mrad" << endreq;
          info() << "                 : " << fit->fittedParameterNames()[1] << " = "
                 << 100*fit->fittedParameters()[1] << " +- " << 100*fit->errors()[1] << " mrad" << endreq;
        }
        else
        {
          info() << " " << radName << "CK res fit FAILED" << endreq;
        }
      }
      // CK pull
      AIDA::IHistogram1D * hPull = histo1D( HistoID(hid(rad,"ckPull")) );
      if ( hPull && hPull->entries() > minEnts )
      {
        AIDA::IFitResult * fit = fitHisto( hPull, ckPullRange[rad], ckPullParams[rad] );
        if ( fit && fit->isValid() )
        {
          info() << " " << radName << "CK pull : fit status=" << fit->fitStatus()
                 << " quality=" << fit->quality() << " ndf=" << fit->ndf() << endreq
                 << "                 : " << fit->fittedParameterNames()[0] << " = "
                 << fit->fittedParameters()[0] << " +- " << fit->errors()[0] << endreq;
          info() << "                 : " << fit->fittedParameterNames()[1] << " = "
                 << fit->fittedParameters()[1] << " +- " << fit->errors()[1] << endreq;
        }
        else
        {
          info() << " " << radName << "CK pull fit FAILED" << endreq;
        }
      }

      */

    }

  }


  info() << "=============================================================================="
         << endreq;

  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}

/*
AIDA::IFitResult * RichRecoQC::fitHisto( AIDA::IHistogram1D * histo,
                                         const std::pair<double,double> & range,
                                         const std::vector<double> & params )
{
  if ( !histo ) return NULL;

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Fitting " << histo->title() << endreq;
  }

  // Creating the function which is going to be fitted with the histogram data
  pi_aida::Function gaussFun("G");

  // set function initial parameters
  gaussFun.setParameter("mean" , params[0]);
  gaussFun.setParameter("sigma", params[1]);
  gaussFun.setParameter("amp"  , params[2]);

  // Creating the fitter
  pi_aida::Fitter fitter("Chi2","lcg_minuit");
  //pi_aida::Fitter fitter("Chi2","minuit");

  // create fit data
  pi_aida::FitData fitData;
  fitData.create1DConnection(*histo);

  // set fit range
  AIDA::IRangeSet & rangeset = fitData.range(0);
  rangeset.include( range.first, range.second );

  // Performing the fit and return
  return fitter.fit( fitData, gaussFun );
}
*/
