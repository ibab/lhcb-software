//-------------------------------------------------------------------------------
/** @file RichRecoQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::RecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.cpp,v 1.35 2007-11-26 17:33:38 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//-------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecoQC.h"

// namespaces
using namespace Rich::Rec::MC;

//-------------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RecoQC );

// Standard constructor, initializes variables
RecoQC::RecoQC( const std::string& name,
                ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richPartProp      ( NULL ),
    m_ckAngle           ( NULL ),
    m_ckRes             ( NULL ),
    m_richRecMCTruth    ( NULL ),
    m_trSelector        ( NULL ),
    m_minBeta           ( Rich::NRadiatorTypes, 0.999 ),
    m_truePhotCount     ( Rich::NRadiatorTypes, 0 ),
    m_nSegs             ( Rich::NRadiatorTypes, 0 ),
    m_chThetaRecHistoLimitMax ( Rich::NRadiatorTypes, 0 ),
    m_chThetaRecHistoLimitMin ( Rich::NRadiatorTypes, 0 )
{
  // Declare job options

  // min beta
  declareProperty( "MinBeta", m_minBeta );

  // Ch Theta Rec histogram limits: low, high -> aerogel, C4F10, Cf4
  m_chThetaRecHistoLimitMin[Rich::Aerogel]  = 0.1;
  m_chThetaRecHistoLimitMin[Rich::Rich1Gas] = 0.03;
  m_chThetaRecHistoLimitMin[Rich::Rich2Gas] = 0.01;
  declareProperty( "ChThetaRecHistoLimitMax", m_chThetaRecHistoLimitMax );
  m_chThetaRecHistoLimitMax[Rich::Aerogel]  = 0.3;
  m_chThetaRecHistoLimitMax[Rich::Rich1Gas] = 0.08;
  m_chThetaRecHistoLimitMax[Rich::Rich2Gas] = 0.05;
  declareProperty( "ChThetaRecHistoLimitMin", m_chThetaRecHistoLimitMin );
  declareProperty( "CKResHistoRange", 
                   m_ckResRange = boost::assign::list_of(0.01)(0.005)(0.0025) );

  declareProperty( "NumberBins", m_nBins = 100 );
}

// Destructor
RecoQC::~RecoQC() {};

// Initialisation
StatusCode RecoQC::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get track selector
  acquireTool( "TrackSelector", m_trSelector, this );

  return sc;
}

// Main execution
StatusCode RecoQC::execute()
{
  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Rich Histo ID
  const RichHistoID hid;

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );

  // make sure RichrecPixels are ready
  if ( pixelCreator()->newPixels().isFailure() )
    return Error( "Problem creating RichRecPixels" );

  // make sure photons are available
  if ( photonCreator()->reconstructPhotons().isFailure() )
    return Error( "Problem creating RichRecPhotons" );

  // Is MC available
  const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
  const bool mcRICHOK  = richRecMCTool()->pixelMCHistoryAvailable();

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // segment momentum
    const double pTot = sqrt(segment->trackSegment().bestMomentum().Mag2());

    double thetaExpTrue(0.0), resExpTrue(0.0);
    Rich::ParticleIDType mcType = Rich::Pion; // If MC not available, assume pion
    if ( mcTrackOK )
    {
      // True particle type
      const Rich::ParticleIDType mcType = richRecMCTool()->mcParticleType( segment );
      if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly..

      // Expected Cherenkov theta angle for true particle type
      thetaExpTrue = ckAngleTool()->avgCherenkovTheta( segment, mcType );

      // Cherenkov angle resolution for true type
      resExpTrue = ckResTool()->ckThetaResolution( segment, mcType );
    }

    // beta
    const double beta = partPropTool()->beta( pTot, mcType );
    // selection cuts
    if ( beta < m_minBeta[rad] ) continue; // skip non-saturated tracks

    // loop over photons for this segment
    unsigned int truePhotons(0);
    double avRecTrueTheta(0);
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      LHCb::RichRecPhoton * photon = *iPhot;

      // reconstructed theta
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      // reconstructed phi
      const double phiRec   = photon->geomPhoton().CherenkovPhi();

      plot1D( thetaRec, hid(rad,"thetaRec"), "Reconstructed Ch Theta : All photons",
              m_chThetaRecHistoLimitMin[rad], m_chThetaRecHistoLimitMax[rad], m_nBins );
      plot1D( phiRec, hid(rad,"phiRec"), "Reconstructed Ch Phi : All photons", 0.0, 2*Gaudi::Units::pi, m_nBins );
      plot1D( thetaRec-thetaExpTrue,
              hid(rad,"ckResAll"), "Rec-Exp Cktheta : beta=1 : All photons",
              -m_ckResRange[rad], m_ckResRange[rad], m_nBins );

      if ( mcTrackOK && mcRICHOK )
      {
        // Is this a true photon ?
        const LHCb::MCParticle * photonParent = richRecMCTool()->trueCherenkovPhoton( photon );
        if ( photonParent )
        {
          ++truePhotons;
          avRecTrueTheta += thetaRec;
          // resolution plot
          plot1D( thetaRec-thetaExpTrue,
                  hid(rad,"ckResTrue"), "Rec-Exp Cktheta : beta=1 : MC true photons", 
                  -m_ckResRange[rad], m_ckResRange[rad], m_nBins );
          if ( resExpTrue>0 )
          {
            // pull plot
            const double ckPull = (thetaRec-thetaExpTrue)/resExpTrue;
            plot1D( ckPull, hid(rad,"ckPull"), "(Rec-Exp)/Res Cktheta : beta=1", -4, 4, m_nBins );
          }
        }
        else // fake photon
        {
          plot1D( thetaRec-thetaExpTrue,
                  hid(rad,"ckResFake"), "Rec-Exp Cktheta : beta=1 : MC fake photons", 
                  -m_ckResRange[rad], m_ckResRange[rad], m_nBins );
        }
      } // MC is available

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
StatusCode RecoQC::finalize()
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
  const StatDivFunctor occ("%8.2f +-%5.2f");

  info() << "=============================================================================="
         << endreq;

  // track selection
  info() << " Track Selection : " << m_trSelector->selectedTracks() << endreq;
  info() << "                 : beta > " << m_minBeta << endreq;

  // loop over radiators
  for ( int irad = 0; irad < Rich::NRadiatorTypes; ++irad )
  {
    const Rich::RadiatorType rad = (Rich::RadiatorType)irad;

    // rad name
    std::string radName = Rich::text(rad);
    radName.resize(15,' ');
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
  AIDA::IFitResult * RecoQC::fitHisto( AIDA::IHistogram1D * histo,
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
