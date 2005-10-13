
//-----------------------------------------------------------------------------
/** @file RichPhotonMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPhotonMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonMonitor.cpp,v 1.3 2005-10-13 15:45:45 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonMonitor.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPhotonMonitor>          s_factory ;
const        IAlgFactory& RichPhotonMonitorFactory = s_factory ;

// Standard constructor, initializes variables
RichPhotonMonitor::RichPhotonMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : RichRecAlgBase   ( name, pSvcLocator ),
    m_photonSig      ( 0 ),
    m_tkSignal       ( 0 ),
    m_geomEfficRay   ( 0 ),
    m_geomEfficFix   ( 0 ),
    m_richRecMCTruth ( 0 ),
    m_ckAngle        ( 0 ),
    m_richPartProp   ( 0 ),
    m_mcTkInfo       ( 0 ),
    m_refIndex       ( 0 ),
    m_geomTool       ( 0 ),
    m_forcedPhotCreator ( 0 )
{

  // Declare job options
  declareProperty( "MCTruth", m_truth = true );
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/RECON/MC/" );
  declareProperty( "HistoPath",   m_histPth   = "RICH/RECON/" );
  declareProperty( "BuildPhotons", m_buildPhotons = false );
  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks", m_maxUsedTracks = 200 );

  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
  declareProperty( "TrackMomentumCuts", m_trSelector.setMomentumCuts() );

}

// Destructor
RichPhotonMonitor::~RichPhotonMonitor() {};

// Initialisation
StatusCode RichPhotonMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "ForcedRichPhotonCreator", m_forcedPhotCreator, this );
  acquireTool( "RichPhotonSignal",        m_photonSig );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal  );
  acquireTool( "RichGeomEffFix",          m_geomEfficFix, this   );
  acquireTool( "RichGeomEffRay",          m_geomEfficRay, this   );
  acquireTool( "RichRecGeometry",         m_geomTool  );
  acquireTool( "RichCherenkovAngle",      m_ckAngle   );
  acquireTool( "RichParticleProperties",  m_richPartProp  );
  acquireTool( "RichRefractiveIndex",     m_refIndex      );
  if ( m_truth )
  {
    acquireTool( "RichRecMCTruthTool",    m_richRecMCTruth );
    acquireTool( "RichMCTrackInfoTool",   m_mcTkInfo       );
    if ( !m_richRecMCTruth || !m_mcTkInfo ) m_truth = false;
  }

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;
  if ( m_truth ) { if ( !bookMCHistograms() ) return StatusCode::FAILURE; }

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;
  m_trSelector.printTrackSelection( info() );

  // Initialise variables
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
  {
    for ( unsigned iTkType = 0; iTkType<Rich::Track::NTrTypes; ++iTkType )
    {
      m_maxTrueHitSepL[iRad][iTkType] = 0;
      m_minTrueHitSepL[iRad][iTkType] = 999;
    }
    m_maxPhotEnAtProd[iRad] = 0;
    m_minPhotEnAtProd[iRad] = 999;
  }

  return sc;
}

StatusCode RichPhotonMonitor::bookHistograms()
{
  std::string title;
  int id;
  const int nbins = 100;

  // Defines for various parameters
  RAD_HISTO_OFFSET;
  HYPO_HISTO_OFFSET;
  RADIATOR_NAMES;
  HYPOTHESIS_NAMES;
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;

  const int maxPhotons = 50000;
  const double tkHitSepMin[]  = { 0.0,     0.0,     0.0 };
  const double tkHitSepMax[]  = { 999.0,   999.0,   999.0 };
  const double minRefInd[]    = { 0.031,   0.0013,  0.00044 };
  const double maxRefInd[]    = { 0.034,   0.00155, 0.00052 };

  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {

    title = "Rec Cktheta : " +radiator[iRad];
    id = radOffset*(iRad+1) + 1;
    m_ckTheta[iRad] = histoSvc()->book(m_histPth,id,title,nbins,
                                       minCkTheta[iRad],maxCkTheta[iRad]);

    title = "Active segment fraction : " + radiator[iRad];
    id = radOffset*(iRad+1) + 6;
    m_activeSegFract[iRad] = histoSvc()->book(m_histPth,id,title,nbins,-0.01,1.01);

    title = "# Rec Photons per event : " +radiator[iRad];
    id = radOffset*(iRad+1) + 9;
    m_nPhotsPerRad[iRad] = histoSvc()->book(m_histPth,id,title,nbins,0,maxPhotons);

    title = "n - 1 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 10;
    m_photRefIndexRec[iRad] = histoSvc()->book(m_histPth,id,title,
                                               nbins,minRefInd[iRad],maxRefInd[iRad]);

    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {

      // these are initialised on demand
      m_recPhotEnAtProd[iRad][iHypo] = 0;

      title = "Exp # emitted phots : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 1 + hypoOffset*(iHypo+1);
      m_expEmitPhots[iRad][iHypo] = histoSvc()->book(m_histPth,id,title,nbins,0,100);

      title = "Exp # signal phots : " + hypothesis[iHypo] + " " + radiator[iRad];
      id = radOffset*(iRad+1) + 2 + hypoOffset*(iHypo+1);
      m_expSignalPhots[iRad][iHypo] = histoSvc()->book(m_histPth,id,title,nbins,0,100);

      title = "Geom. Eff. Ray-traced : " + hypothesis[iHypo] + " " + radiator[iRad];
      id = radOffset*(iRad+1) + 3 + hypoOffset*(iHypo+1);
      m_geomEffRay[iRad][iHypo] = histoSvc()->book(m_histPth,id,title,nbins,0.,1.);

      title = "Geom. Eff. Fixed : " + hypothesis[iHypo] + " " + radiator[iRad];
      id = radOffset*(iRad+1) + 4 + hypoOffset*(iHypo+1);
      m_geomEffFix[iRad][iHypo] = histoSvc()->book(m_histPth,id,title,nbins,0.,1.);

      title = "Geom. Eff. Fixed V Ray-traced : " + hypothesis[iHypo] + " " + radiator[iRad];
      id = radOffset*(iRad+1) + 5 + hypoOffset*(iHypo+1);
      m_geomEffCorr[iRad][iHypo] = histoSvc()->book(m_histPth,id,title,nbins,0.,1.,nbins,0.,1.);

      title = "Log10(PredPixSig) : " + hypothesis[iHypo] + " " + radiator[iRad];
      id = radOffset*(iRad+1) + 8 + hypoOffset*(iHypo+1);
      m_predPixelSignal[iRad][iHypo] = histoSvc()->book(m_histPth,id,title,nbins,-20,0);

      title = "Log10(SignalProb) : " + hypothesis[iHypo] + " " + radiator[iRad];
      id = radOffset*(iRad+1) + 16 + hypoOffset*(iHypo+1);
      m_signalProb[iRad][iHypo] = histoSvc()->book(m_histPth,id,title,nbins,-20,0);

    } // end hypo loop

    for ( unsigned iTkType = 0; iTkType<Rich::Track::NTrTypes; ++iTkType )
    {

      title = "Trk-Pix Hit sep local : " + Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 221 + iTkType;
      m_trackHitSepL[iRad][iTkType] =
        histoSvc()->book(m_histPth,id,title, nbins,tkHitSepMin[iRad],tkHitSepMax[iRad]);

    } // end track loop

  } // end rad loop

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonMonitor::bookMCHistograms()
{
  std::string title;
  int id;
  const int nbins = 100;

  // Defines for various parameters
  RAD_HISTO_OFFSET;
  HYPO_HISTO_OFFSET;
  RADIATOR_NAMES;
  HYPOTHESIS_NAMES;
  RADIATOR_GLOBAL_POSITIONS_Z;
  RADIATOR_MAX_CKTHETA;
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;
  PD_LOCAL_POSITIONS_X;
  PD_LOCAL_POSITIONS_Y;

  const int maxPhotons = 5000;
  const int maxPhotonsSignal = 75;

  //            Radiator          Aerogel  C4F10    CF4
  const double tkHitSepMin[]  = { 0.0,     0.0,     0.0     };
  const double tkHitSepMax[]  = { 500.0,   120.0,   200.0   };
  const double ckRange[]      = { 0.015,   0.01,    0.005   };
  const double pMax[]         = { 100000,  500000,  1000000 };
  const double pMin[]         = { 1000,    1000,    3000    };
  const double emissResX[]    = { 10,      10,      10      };
  const double emissResY[]    = { 10,      10,      10      };
  const double emissResZ[]    = { 10,      10,      10      };
  const double minPhotEn[]    = { 1.5,     1.5,     1.5     };
  const double maxPhotEn[]    = { 4,       7,       7.5     };
  const double minRefInd[]    = { 0.031,   0.0013,  0.00044 };
  const double maxRefInd[]    = { 0.034,   0.00155, 0.00052 };

  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {

    int iRich = ( iRad == 2 ? 1 : 0 );

    title = "True Rec Ck theta : "+radiator[iRad];
    id = radOffset*(iRad+1) + 1;
    m_ckTrueRecTheta[iRad] = histoSvc()->book(m_mcHistPth,id,title,nbins,
                                              minCkTheta[iRad],maxCkTheta[iRad]);

    title = "True MC Ck theta : "+radiator[iRad];
    id = radOffset*(iRad+1) + 403;
    m_ckTrueTheta[iRad] = histoSvc()->book(m_mcHistPth,id,title,nbins,
                                           minCkTheta[iRad],maxCkTheta[iRad]);

    title = "Rec-Exp Cktheta : true : " + radiator[iRad];
    id = radOffset*(iRad+1) + 2;
    m_ckTrueDTheta[iRad] = histoSvc()->book(m_mcHistPth,id,title,nbins,
                                            -ckRange[iRad],ckRange[iRad]);

    title = "Rec-MC Cktheta : true : " + radiator[iRad];
    id = radOffset*(iRad+1) + 402;
    m_ckTrueDTheta2[iRad] = histoSvc()->book(m_mcHistPth,id,title,nbins,
                                             -ckRange[iRad],ckRange[iRad]);

    title = "Exp-MC Cktheta : true : " + radiator[iRad];
    id = radOffset*(iRad+1) + 502;
    m_ckTrueDTheta3[iRad] = histoSvc()->book(m_mcHistPth,id,title,nbins,
                                             -ckRange[iRad],ckRange[iRad]);

    title = "Rec-Exp Cktheta : All : " + radiator[iRad];
    id = radOffset*(iRad+1) + 3;
    m_ckAllDTheta[iRad] = histoSvc()->book(m_mcHistPth,id,title,nbins,
                                           -ckRange[iRad],ckRange[iRad]);

    title = "Assumed emission point z : " + radiator[iRad];
    id = radOffset*(iRad+1) + 4;
    m_emissionZ[iRad] =
      histoSvc()->book(m_mcHistPth,id,title, nbins,zRadEntGlo[iRad],zRadExitGlo[iRad]);

    title = "True Rec Ck phi V theta : R0 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 5;
    m_ckTruePhiVThetaR0[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nbins,
                       minCkTheta[iRad],maxCkTheta[iRad],nbins,0.,2*M_PI);

    title = "True Rec Ck phi V theta : R1 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 6;
    m_ckTruePhiVThetaR1[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nbins,
                       minCkTheta[iRad],maxCkTheta[iRad],nbins,0.,2*M_PI);

    title = "True Rec Ck phi V theta : R2 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 7;
    m_ckTruePhiVThetaR2[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nbins,
                       minCkTheta[iRad],maxCkTheta[iRad],nbins,0.,2*M_PI);

    title = "True Rec Ck phi V theta : R3 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 8;
    m_ckTruePhiVThetaR3[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nbins,
                       minCkTheta[iRad],maxCkTheta[iRad],nbins,0.,2*M_PI);

    title = "# True Rec. Photons per event : " + radiator[iRad];
    id = radOffset*(iRad+1) + 9;
    m_nTruePhotsPerRad[iRad] = histoSvc()->book(m_mcHistPth,id,title,nbins,0,maxPhotons);

    title = "n - 1 : MC Photons : " + radiator[iRad];
    id = radOffset*(iRad+1) + 10;
    m_photRefIndexMC[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                              nbins,minRefInd[iRad],maxRefInd[iRad]);

    title = "SepL V reco CK theta : " + radiator[iRad];
    id = radOffset*(iRad+1) + 621;
    m_ckTvsep2L[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                         nbins,minCkTheta[iRad],maxCkTheta[iRad],
                                         nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "SepL V reco CK theta : true CK photons : " + radiator[iRad];
    id = radOffset*(iRad+1) + 622;
    m_ckTvsep2Ltrue[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                             nbins,minCkTheta[iRad],maxCkTheta[iRad],
                                             nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "SepL V MC CK theta : true CK photons : " + radiator[iRad];
    id = radOffset*(iRad+1) + 623;
    m_ckTvsep2LtrueMC[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                               nbins,minCkTheta[iRad],maxCkTheta[iRad],
                                               nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "SepL V reco CK phi : " + radiator[iRad];
    id = radOffset*(iRad+1) + 631;
    m_ckPvsep2L[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                         nbins,-M_PI,M_PI,
                                         nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "SepL V reco CK phi : true CK photons : " + radiator[iRad];
    id = radOffset*(iRad+1) + 632;
    m_ckPvsep2Ltrue[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                             nbins,-M_PI,M_PI,
                                             nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "Non-recon. true photons : CK-Theta Reco : " + radiator[iRad];
    id = radOffset*(iRad+1) + 701;
    m_notFoundCKT[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                           nbins,minCkTheta[iRad],maxCkTheta[iRad] );

    title = "Non-recon. true photons : CK-Theta MC : " + radiator[iRad];
    id = radOffset*(iRad+1) + 702;
    m_notFoundCKTMC[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                             nbins,minCkTheta[iRad],maxCkTheta[iRad] );

    title = "Non-recon. true photons : sep2 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 703;
    m_notFoundSepL[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                            nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "Non-recon. true photons : XvY : " + radiator[iRad];
    id = radOffset*(iRad+1) + 704;
    m_notFoundLocXvY[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                              nbins,xMinPDLoc[iRich],xMaxPDLoc[iRich],
                                              nbins,yMinPDLoc[iRich],yMaxPDLoc[iRich] );

    title = "Non-recon. true photons : Reco-Exp CK-Theta : " + radiator[iRad];
    id = radOffset*(iRad+1) + 705;
    m_notFoundCKres[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nbins,-ckRange[iRad],ckRange[iRad]);

    title = "Non-recon. true photons : MC-Exp CK-Theta : " + radiator[iRad];
    id = radOffset*(iRad+1) + 706;
    m_notFoundCKresMC[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nbins,-ckRange[iRad],ckRange[iRad]);

    title = "Non-recon. true photons : Sep V CK-Theta Reco : " + radiator[iRad];
    id = radOffset*(iRad+1) + 707;
    m_notFoundSepLVCKT[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                                nbins,minCkTheta[iRad],maxCkTheta[iRad],
                                                nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "Non-recon. true photons : Sep V CK-Theta MC : " + radiator[iRad];
    id = radOffset*(iRad+1) + 708;
    m_notFoundSepLVCKTMC[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                                  nbins,minCkTheta[iRad],maxCkTheta[iRad],
                                                  nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "Non-recon. true photons : Sep V CK-Phi : " + radiator[iRad];
    id = radOffset*(iRad+1) + 709;
    m_notFoundSepLVCKP[iRad] = histoSvc()->book(m_mcHistPth,id,title,
                                                nbins,-M_PI,M_PI,
                                                nbins,tkHitSepMin[iRad],tkHitSepMax[iRad] );

    title = "Non-recon. true photons : MC/reco track PD separation : " + radiator[iRad];
    id = radOffset*(iRad+1) + 710;
    m_notFoundTkSep[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,nbins,0,25 );

    for ( unsigned iTkType = 0; iTkType < Rich::Track::NTrTypes; ++iTkType ) {

      title = "Rec-Exp Cktheta : true : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 11 + iTkType;
      m_ckTrueDThetaT[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,-ckRange[iRad],ckRange[iRad]);

      title = "Rec-MC Cktheta : true : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 411 + iTkType;
      m_ckTrueDThetaT2[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,-ckRange[iRad],ckRange[iRad]);

      title = "Exp-MC Cktheta : true : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 511 + iTkType;
      m_ckTrueDThetaT3[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,-ckRange[iRad],ckRange[iRad]);

      title = "True Trk-Pix Hit sep local : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 121 + iTkType;
      m_trackHitSepLTrue[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title, nbins,tkHitSepMin[iRad],tkHitSepMax[iRad]);

      title = "True Signal Trk-Pix Hit sep local : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 231 + iTkType;
      m_trackHitSepLTrueSigOnly[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,tkHitSepMin[iRad],tkHitSepMax[iRad]);

      title = "Rec-Exp Cktheta V Exp ckTheta : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 41 + iTkType;
      m_ckresVthetaT[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,
                         minCkTheta[iRad],maxCkTheta[iRad],nbins,
                         -ckRange[iRad],ckRange[iRad]);

      title = "Rec-MC Cktheta V Rec ckTheta : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 441 + iTkType;
      m_ckresVthetaT2[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,
                         minCkTheta[iRad],maxCkTheta[iRad],nbins,
                         -ckRange[iRad],ckRange[iRad]);

      title = "Exp-MC Cktheta V MC ckTheta : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 541 + iTkType;
      m_ckresVthetaT3[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,
                         minCkTheta[iRad],maxCkTheta[iRad],nbins,
                         -ckRange[iRad],ckRange[iRad]);

      title = "Rec-Exp Cktheta V Log10(Ptot) : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 51 + iTkType;
      m_ckresVPtot[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,
                         log10(pMin[iRad]),log10(pMax[iRad]),nbins,
                         -ckRange[iRad],ckRange[iRad]);

      title = "Rec-MC Cktheta V Log10(Ptot) : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 451 + iTkType;
      m_ckresVPtot2[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,
                         log10(pMin[iRad]),log10(pMax[iRad]),nbins,
                         -ckRange[iRad],ckRange[iRad]);

      title = "Exp-MC Cktheta V Log10(Ptot) : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 551 + iTkType;
      m_ckresVPtot3[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,
                         log10(pMin[iRad]),log10(pMax[iRad]),nbins,
                         -ckRange[iRad],ckRange[iRad]);

      title = "Cktheta V Log10(Ptot) : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 61 + iTkType;
      m_ckVptot[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,
                         nbins,log10(pMin[iRad]),log10(pMax[iRad]),
                         nbins,minCkTheta[iRad],maxCkTheta[iRad]);

      title = "Cktheta V Log10(Ptot) : true : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 161 + iTkType;
      m_trueckVptot[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,
                         nbins,log10(pMin[iRad]),log10(pMax[iRad]),
                         nbins,minCkTheta[iRad],maxCkTheta[iRad]);

      title = "CkthetaRes V TrPstate errX : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 71 + iTkType;
      m_ckResVerrX[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,
                         nbins,0,10,
                         nbins,-ckRange[iRad],ckRange[iRad]);

      title = "CkthetaRes V TrPstate errY : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 81 + iTkType;
      m_ckResVerrY[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,
                         nbins,0,10,
                         nbins,-ckRange[iRad],ckRange[iRad]);

      title = "CkthetaRes V TrPstate errTX : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 91 + iTkType;
      m_ckResVerrTX[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,
                         nbins,0,0.01,
                         nbins,-ckRange[iRad],ckRange[iRad]);

      title = "CkthetaRes V TrPstate errTY : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 101 + iTkType;
      m_ckResVerrTY[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,
                         nbins,0,0.01,
                         nbins,-ckRange[iRad],ckRange[iRad]);

      title = "Rec-MC Emiss. X : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 131 + iTkType;
      m_emissResX[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,-emissResX[iRad],emissResX[iRad]);

      title = "Rec-MC Emiss. Y : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 141 + iTkType;
      m_emissResY[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,-emissResY[iRad],emissResY[iRad]);

      title = "Rec-MC Emiss. Z : " +
        Rich::text((Rich::Track::Type)iTkType) + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 151 + iTkType;
      m_emissResZ[iRad][iTkType] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,-emissResZ[iRad],emissResZ[iRad]);

    } // end track type loop

    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {

      title = "Expected (av) Cktheta : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 1 + hypoOffset*(iHypo+1);
      m_avCKTheta[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,nbins,0.,ckThetaMax[iRad]);

      title = "True # signal photons used by Reco: " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 2 + hypoOffset*(iHypo+1);
      m_trueSignalPhotsRec[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,51,-0.5,50.5);

      title = "True # signal photons used by Reco: beta>0.99 : "
        + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 3 + hypoOffset*(iHypo+1);
      m_trueSignalPhotsRecB[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,51,-0.5,50.5);

      title = "True # signal photons : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 4 + hypoOffset*(iHypo+1);
      m_trueSignalPhots[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,51,-0.5,50.5);

      title = "True # signal photons : beta>0.99 : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 5 + hypoOffset*(iHypo+1);
      m_trueSignalPhotsB[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,51,-0.5,50.5);

      title = "True-exp # signal : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 6 + hypoOffset*(iHypo+1);
      m_diffSignalPhots[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,nbins,-25.,25.);

      title = "Rec-Exp Cktheta : True : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 7 + hypoOffset*(iHypo+1);
      m_ckTrueDThetaH[iRad][iHypo] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,-ckRange[iRad],ckRange[iRad]);

      title = "Rec-Exp Cktheta : True : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 8 + hypoOffset*(iHypo+1);
      m_ckAllDThetaH[iRad][iHypo] =
        histoSvc()->book(m_mcHistPth,id,title,nbins,-ckRange[iRad],ckRange[iRad]);

      title = "True Log10(PredictPixSig) : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 9 + hypoOffset*(iHypo+1);
      m_predPixelSignalTrue[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,nbins,-20,0.);

      title = "CkRes V Log10(Ptot) : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 10 + hypoOffset*(iHypo+1);
      m_ckresVPtotHypo[iRad][iHypo] =
        histoSvc()->book(m_mcHistPth,id,title,
                         nbins,log10(pMin[iRad]),log10(pMax[iRad]),
                         nbins,-ckRange[iRad],ckRange[iRad]);

      title = "CkRes V ckTheta : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 11 + hypoOffset*(iHypo+1);
      m_ckresVthetaTHypo[iRad][iHypo] =
        histoSvc()->book(m_mcHistPth,id,title,
                         nbins,minCkTheta[iRad],maxCkTheta[iRad],
                         nbins,-ckRange[iRad],ckRange[iRad]);

      title = "Emitted photon energy : True " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 12 + hypoOffset*(iHypo+1);
      m_emitPhotEn[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,nbins,minPhotEn[iRad],maxPhotEn[iRad]);

      title = "Signal photon energy : True " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 13 + hypoOffset*(iHypo+1);
      m_sigPhotEn[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,nbins,minPhotEn[iRad],maxPhotEn[iRad]);

      title = "# Signal Photons V ckTheta : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 14 + hypoOffset*(iHypo+1);
      m_nSigVCktheta[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,
                                                     nbins,minCkTheta[iRad],maxCkTheta[iRad],
                                                     nbins,0,maxPhotonsSignal);

      title = "MC Photon En at prod. (eV) : " + hypothesis[iHypo] + " : " + radiator[iRad];
      id = radOffset*(iRad+1) + 15 + hypoOffset*(iHypo+1);
      m_mcPhotEnAtProd[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,nbins,minPhotEn[iRad],maxPhotEn[iRad]);

      title = "Log10(SignalProb) : True " + hypothesis[iHypo] + " " + radiator[iRad];
      id = radOffset*(iRad+1) + 16 + hypoOffset*(iHypo+1);
      m_signalProbTrue[iRad][iHypo] = histoSvc()->book(m_mcHistPth,id,title,nbins,-20,0);


    } // end hypo loop

  } // end rad loop

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichPhotonMonitor::execute()
{

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // If any containers are empty, form them
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }
  if ( richTracks()->size() > m_maxUsedTracks )
  {
    debug() << "Found " << richTracks()->size() << ">"
            << m_maxUsedTracks << " max usable tracks, stopping." << endreq;
    return StatusCode::SUCCESS;
  }
  if ( richPixels()->empty() )
  {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endreq;
  }
  if ( m_buildPhotons && richPhotons()->empty() )
  {
    photonCreator()->reconstructPhotons();
    debug() << "No photons found : Created "
            << richPhotons()->size() << " RichRecPhotons" << endreq;
  }

  // Tally number of photons
  unsigned int nPhots[Rich::NRadiatorTypes];
  nPhots[Rich::Aerogel]  = 0;
  nPhots[Rich::Rich1Gas] = 0;
  nPhots[Rich::Rich2Gas] = 0;
  unsigned int nPhotsT[Rich::NRadiatorTypes];
  nPhotsT[Rich::Aerogel]  = 0;
  nPhotsT[Rich::Rich1Gas] = 0;
  nPhotsT[Rich::Rich2Gas] = 0;

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    // track selection
    if ( !m_trSelector.trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // Segment momentum
    const double ptot = segment->trackSegment().bestMomentum().mag();

    // Pointer to parent MCParticle
    const MCParticle * trackMCPart =
      m_richRecMCTruth->mcParticle( segment->richRecTrack() );

    // True type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );

    // beta for true type
    const double beta = m_richPartProp->beta( segment->trackSegment().bestMomentum().mag(), mcType );

    // Track type
    const Rich::Track::Type trType = segment->richRecTrack()->trackID().trackType();

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Segment " << segment->key()
                << " Track " << segment->richRecTrack()->key() << " : "
                << rad << " : MCType " << mcType
                << " : TrTrackType " << trType << " : pTot " << ptot
                << " pthLen " << segment->trackSegment().pathLength()
                << endreq
                << " Entry : Point " << segment->trackSegment().entryPoint()
                << " Momentum " << segment->trackSegment().entryMomentum() << endreq
                << " Exit  : Point " << segment->trackSegment().exitPoint()
                << " Momentum " << segment->trackSegment().exitMomentum() << endreq;
    }

    // Loop over all particle codes
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);

      // Expected number of emitted photons
      const double emitPhots = m_tkSignal->nEmittedPhotons( segment, hypo );
      if ( emitPhots>0 ) m_expEmitPhots[rad][iHypo]->fill( emitPhots );

      // Expected number of observable signal photons
      const double sigPhots = m_tkSignal->nObservableSignalPhotons( segment, hypo );
      if ( sigPhots>0 ) m_expSignalPhots[rad][iHypo]->fill( sigPhots );

      // Geometrical Efficiency
      segment->geomEfficiency().resetData(0);
      const double geomEffFix = m_geomEfficFix->geomEfficiency(segment,hypo);
      segment->geomEfficiency().resetData(0);
      const double geomEffRay = m_geomEfficRay->geomEfficiency(segment,hypo);
      if ( geomEffFix>0 && geomEffRay>0 )
      {
        m_geomEffFix[rad][iHypo]->fill( geomEffFix );
        m_geomEffRay[rad][iHypo]->fill( geomEffRay );
        m_geomEffCorr[rad][iHypo]->fill(  geomEffRay, geomEffFix );
      }

      // Average Cherenkov Theta angle
      const double thetaExp = m_ckAngle->avgCherenkovTheta(segment,hypo);
      if ( thetaExp>0 ) m_avCKTheta[rad][iHypo]->fill( thetaExp );

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << " " << hypo << " emitted " << emitPhots
                  << " detable " << m_tkSignal->nDetectablePhotons( segment, hypo )
                  << " Signal " << m_tkSignal->nSignalPhotons( segment, hypo )
                  << " Scatter " << m_tkSignal->nScatteredPhotons( segment, hypo )
                  << " ObsSignal " << sigPhots << " geomEffRay " << geomEffRay
                  << " ckExp " << thetaExp << endreq;
      }

    } // end loop over particle codes

    // Expected Cherenkov theta angle for true particle type
    const double thetaExpTrue =
      ( mcType != Rich::Unknown ? m_ckAngle->avgCherenkovTheta( segment, mcType ) : 0 );

    // Average energy of emitted cherenkov photons for true type
    const double avgEnEmit =
      ( mcType != Rich::Unknown ? m_tkSignal->avgEmitPhotEnergy( segment, mcType ) : 0 );
    if ( mcType != Rich::Unknown ) {
      m_emitPhotEn[rad][mcType]->fill( avgEnEmit );
      m_photRefIndexRec[rad]->fill( m_refIndex->refractiveIndex( rad, avgEnEmit ) - 1 );
    }

    // Number of signal photons for true type
    const double nSigTrue =
      ( mcType != Rich::Unknown ? m_tkSignal->nSignalPhotons( segment, mcType ) : 0 );
    m_nSigVCktheta[rad][mcType]->fill( thetaExpTrue, nSigTrue );

    // loop over energy bins
    if ( mcType != Rich::Unknown ) {
      const RichPhotonSpectra<RichRecSegment::FloatType> & sigSpectra = segment->signalPhotonSpectra();
      for ( unsigned int iEnBin = 0; iEnBin < sigSpectra.energyBins(); ++iEnBin ) {
        bookRecPhotonHist(mcType,rad,sigSpectra)->fill( sigSpectra.binEnergy(iEnBin),
                                                        (sigSpectra.energyDist(mcType))[iEnBin] );
      }
    }

    // Average energy of signal cherenkov photons for true type
    const double avgEnSig =
      ( mcType != Rich::Unknown ? m_tkSignal->avgSignalPhotEnergy( segment, mcType ) : 0 );
    m_sigPhotEn[rad][mcType]->fill( avgEnSig );

    // Get photons for this segment
    const RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    int truePhotonsRec = 0;
    verbose() << " Found " << photons.size() << " photon candidates" << endreq;
    nPhots[rad] += photons.size();

    for ( RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot )
    {
      RichRecPhoton * photon = *iPhot;
      RichRecPixel  * pixel  = photon->richRecPixel();

      // Cherenkov angles
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      const double phiRec   = photon->geomPhoton().CherenkovPhi();
      const double delTheta = thetaRec-thetaExpTrue;
      m_ckTheta[rad]->fill( thetaRec );
      m_ckAllDTheta[rad]->fill( delTheta );
      if ( mcType != Rich::Unknown ) m_ckAllDThetaH[rad][mcType]->fill( delTheta );

      // Active segment fraction
      m_activeSegFract[rad]->fill(photon->geomPhoton().activeSegmentFraction());

      // segment / pixel seperation
      //const double sepG = sqrt( m_geomTool->trackPixelHitSep2Global(segment,photon->richRecPixel()) );
      const double sepL = sqrt( m_geomTool->trackPixelHitSep2( segment,photon->richRecPixel()) );
      const double sepAngle = atan2( pixel->localPosition(rad).x() - segment->pdPanelHitPointLocal().x(),
                                     pixel->localPosition(rad).y() - segment->pdPanelHitPointLocal().y() );

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  Photon " << photon->key()
                  << " angles " << thetaRec << " " << phiRec
                  << " fract " << photon->geomPhoton().activeSegmentFraction()
                  << " pixSigs " << m_photonSig->predictedPixelSignal( photon, Rich::Electron )
                  << " " << m_photonSig->predictedPixelSignal( photon, Rich::Muon )
                  << " " << m_photonSig->predictedPixelSignal( photon, Rich::Pion )
                  << " " << m_photonSig->predictedPixelSignal( photon, Rich::Kaon )
                  << " " << m_photonSig->predictedPixelSignal( photon, Rich::Proton )
                  << endreq;
        verbose() << "   EmissPtn " << photon->geomPhoton().emissionPoint() << endreq
                  << "   SphMirr  " << photon->geomPhoton().sphMirReflectionPoint() << endreq
                  << "   flatMirr " << photon->geomPhoton().flatMirReflectionPoint() << endreq
                  << "   detPtn   " << photon->geomPhoton().detectionPoint() << endreq;
      }

      // Loop over all particle codes and fill signal plots
      double trueExpPixelSig(0), trueSignalProb(0);
      for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
      {
        const Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);
        double expPixelSig = m_photonSig->predictedPixelSignal( photon, hypo );
        expPixelSig = ( expPixelSig>1E-40 ? log10(expPixelSig) : -40 );
        double signalProb = m_photonSig->signalProb( photon, hypo );
        signalProb = ( signalProb>1E-40 ? log10(signalProb) : -40 );
        if ( mcType == hypo ) {
          trueExpPixelSig = expPixelSig;
          trueSignalProb  = signalProb;
        }
        m_predPixelSignal[rad][hypo]->fill( expPixelSig );
        m_signalProb[rad][hypo]->fill( signalProb );
      }

      // correlation between CK theta and seperation distance - true photons
      //m_ckTvsep2G[rad]->fill( thetaRec, sepG );
      m_ckTvsep2L[rad]->fill( thetaRec, sepL );
      //m_ckPvsep2G[rad]->fill( sepAngle, sepG );
      m_ckPvsep2L[rad]->fill( sepAngle, sepL );

      // Is this a true cherenkov photon ?
      if ( m_truth ) {
        const MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton(photon);

        if ( photonParent ) {

          // Associated MCRichOpticalPhoton
          const MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(photon);
          const double thetaMC       = ( mcPhot ? mcPhot->cherenkovTheta() : -999 );
          const double delThetaMC    = ( mcPhot ? thetaRec-mcPhot->cherenkovTheta() : -999 );
          const double delThetaExpMC = ( mcPhot ? thetaExpTrue-mcPhot->cherenkovTheta() : -999 );
          //const double delPhiMC    = ( mcPhot ? phiRec-mcPhot->cherenkovPhi()     : -999 );

          // correlation between CK theta and seperation distance - true photons
          //m_ckTvsep2Gtrue[rad]->fill( thetaRec, sepG );
          m_ckTvsep2Ltrue[rad]->fill( thetaRec, sepL );
          //m_ckPvsep2Gtrue[rad]->fill( sepAngle, sepG );
          m_ckPvsep2Ltrue[rad]->fill( sepAngle, sepL );
          //m_ckTvsep2GtrueMC[rad]->fill( thetaMC, sepG );
          m_ckTvsep2LtrueMC[rad]->fill( thetaMC, sepL );

          if ( mcPhot )
          {
            if ( msgLevel(MSG::VERBOSE) )
            {
              verbose() << "   -> MCPhoton : " << mcPhot->key() << endreq
                        << "       emissPtn " << mcPhot->emissionPoint() << endreq
                        << "       SphMirr  " << mcPhot->sphericalMirrorReflectPoint() << endreq
                        << "       flatMirr " << mcPhot->flatMirrorReflectPoint() << endreq
                        << "       detPtn   " << mcPhot->pdIncidencePoint() << endreq;
            }
          }

          // Expected signal
          if ( mcType != Rich::Unknown )
          {
            m_predPixelSignalTrue[rad][mcType]->fill( trueExpPixelSig );
            m_signalProbTrue[rad][mcType]->fill( trueSignalProb );
          }

          // Hit point associated to photon
          const HepPoint3D & hitPnt = photon->richRecPixel()->globalPosition();

          // Cherenkov angle plots
          m_ckTrueRecTheta[rad]->fill( thetaRec );
          m_ckTrueTheta[rad]->fill( thetaMC );
          if ( hitPnt.y() < 0 && hitPnt.x() < 0 ) {
            m_ckTruePhiVThetaR0[rad]->fill( thetaRec, phiRec );
          } else if ( hitPnt.y() > 0 && hitPnt.x() < 0 ) {
            m_ckTruePhiVThetaR1[rad]->fill( thetaRec, phiRec );
          } else if ( hitPnt.y() < 0 && hitPnt.x() > 0 ) {
            m_ckTruePhiVThetaR2[rad]->fill( thetaRec, phiRec );
          } else if ( hitPnt.y() > 0 && hitPnt.x() > 0 ) {
            m_ckTruePhiVThetaR3[rad]->fill( thetaRec, phiRec );
          }

          // log of momentum
          const double logPtot = ( ptot > 0 ? log10(ptot) : 0 );

          m_ckTrueDTheta[rad]->fill( delTheta );
          if (mcPhot) {
            m_ckTrueDTheta2[rad]->fill( delThetaMC );
            m_ckTrueDTheta3[rad]->fill( delThetaExpMC );
          }
          if ( mcType != Rich::Unknown ) {
            m_ckTrueDThetaH[rad][mcType]->fill( delTheta );
            m_ckResVerrX[rad][mcType]->fill( segment->trackSegment().middleErrors().errX(), delTheta );
            m_ckResVerrY[rad][mcType]->fill( segment->trackSegment().middleErrors().errY(), delTheta );
            m_ckResVerrTX[rad][mcType]->fill( segment->trackSegment().middleErrors().errTX(), delTheta );
            m_ckResVerrTY[rad][mcType]->fill( segment->trackSegment().middleErrors().errTY(), delTheta );
            m_ckresVPtotHypo[rad][mcType]->fill( logPtot, delTheta );
            m_ckresVthetaTHypo[rad][mcType]->fill( thetaExpTrue, delTheta );
          }
          if ( trType >= 0 ) {
            m_ckTrueDThetaT[rad][trType]->fill( delTheta );
            m_ckresVthetaT[rad][trType]->fill( thetaExpTrue, delTheta );
            m_ckresVPtot[rad][trType]->fill( logPtot, delTheta );
            m_ckVptot[rad][trType]->fill( logPtot, thetaRec );
            m_trueckVptot[rad][trType]->fill( logPtot, thetaExpTrue );
            if (mcPhot) {
              m_ckTrueDThetaT2[rad][trType]->fill( delThetaMC );
              m_ckresVthetaT2[rad][trType]->fill( thetaRec, delThetaMC );
              m_ckresVPtot2[rad][trType]->fill( logPtot, delThetaMC );
              m_ckTrueDThetaT3[rad][trType]->fill( delThetaExpMC );
              m_ckresVthetaT3[rad][trType]->fill( thetaExpTrue, delThetaExpMC );
              m_ckresVPtot3[rad][trType]->fill( logPtot, delThetaExpMC );
            }
          }

          // Emission point
          const HepPoint3D & recEmis = photon->geomPhoton().emissionPoint();
          m_emissionZ[rad]->fill( recEmis.z() );
          if ( mcPhot && trType >= 0 ) {
            const HepPoint3D & mcEmis  = mcPhot->emissionPoint();
            m_emissResX[rad][trType]->fill( recEmis.x()-mcEmis.x() );
            m_emissResY[rad][trType]->fill( recEmis.y()-mcEmis.y() );
            m_emissResZ[rad][trType]->fill( recEmis.z()-mcEmis.z() );
          }

          if ( mcPhot ) {

            // Photon energy at production
            const float photEn = mcPhot->energyAtProduction()/eV;
            m_mcPhotEnAtProd[rad][mcType]->fill( photEn );
            if      ( photEn > m_maxPhotEnAtProd[rad] ) m_maxPhotEnAtProd[rad] = photEn;
            else if ( photEn < m_minPhotEnAtProd[rad] ) m_minPhotEnAtProd[rad] = photEn;

            // refractive index
            m_photRefIndexMC[rad]->fill( m_refIndex->refractiveIndex( rad, photEn ) - 1 );

          }

          // Count number of true photons
          ++truePhotonsRec;

        } // truerecphoton

      } // m_truth

    } // loop over photons for this segment
    nPhotsT[rad] += truePhotonsRec;

    // Iterate over pixels
    int truePhotons = 0;
    if ( trackMCPart ) {
      for ( RichRecPixels::const_iterator iPix = richPixels()->begin();
            iPix != richPixels()->end();
            ++iPix ) {
        RichRecPixel * pixel = *iPix;

        // skip if pixel and segment are in different Riches
        if ( segment->trackSegment().rich() != pixel->detector() ) continue;

        // Is this a correct photon
        const MCParticle * truePhotonMCP = m_richRecMCTruth->trueRecPhoton(segment,pixel);
        // Is this a true Cherenkov Photon
        const MCParticle * trueCKPhotonMCP = ( truePhotonMCP ?
                                               m_richRecMCTruth->trueCherenkovPhoton(segment,pixel) : 0 );

        // compute hit seperation : global
        const double sep = sqrt( m_geomTool->trackPixelHitSep2(segment, pixel) );

        if ( trueCKPhotonMCP )
        {
          ++truePhotons;
          // Find the reco-photon for this combination
          RichRecPhoton * recPhot = photonCreator()->checkForExistingPhoton(segment,pixel);
          if ( !recPhot )
          {
            // This is a true CK photon that was not reconstructed. Why ?
            Warning( "Failed to find true " + Rich::text(rad) + " cherenkov photon",
                     StatusCode::SUCCESS );
            m_notFoundLocXvY[rad]->fill( pixel->localPosition(rad).x(), pixel->localPosition(rad).y() );
            m_notFoundSepL[rad]->fill( sep );
            const MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(segment,pixel);
            const double thetaExp = m_ckAngle->avgCherenkovTheta(segment,mcType);
            if ( mcPhot )
            {
              m_notFoundCKTMC[rad]->fill( mcPhot->cherenkovTheta() );
              m_notFoundSepLVCKTMC[rad]->fill( mcPhot->cherenkovTheta(), sep );
              const double sepAngle = atan2( pixel->localPosition(rad).x() - segment->pdPanelHitPointLocal().x(),
                                             pixel->localPosition(rad).y() - segment->pdPanelHitPointLocal().y() );
              m_notFoundSepLVCKP[rad]->fill( sepAngle, sep );
              if ( mcType != Rich::Unknown )
              {
                m_notFoundCKresMC[rad]->fill( mcPhot->cherenkovTheta() - thetaExp );
              }
            }
            const MCRichSegment * mcSegment = m_richRecMCTruth->mcRichSegment(segment);
            if ( mcSegment )
            {
              HepPoint3D mcPDLoc;
              if ( m_mcTkInfo->panelIntersectLocal(mcSegment,mcPDLoc) )
              {
                const double sepp = mcPDLoc.distance(segment->pdPanelHitPointLocal());
                m_notFoundTkSep[rad]->fill( sepp );
              }
            }

            // Force create a photon
            recPhot = m_forcedPhotCreator->reconstructPhoton(segment,pixel);
            if ( !recPhot )
            {
              Warning( "Failed to force creation of " + Rich::text(rad) + " cherenkov photon",
                       StatusCode::SUCCESS );
            }
            else
            {
              m_notFoundCKT[rad]->fill( recPhot->geomPhoton().CherenkovTheta() );
              m_notFoundSepLVCKT[rad]->fill( recPhot->geomPhoton().CherenkovTheta(), sep );
              m_notFoundCKres[rad]->fill( recPhot->geomPhoton().CherenkovTheta() - thetaExp );
            }

          }
        }

        // compute hit seperation : local
        m_trackHitSepL[rad][trType]->fill( sep );
        if ( truePhotonMCP ) {
          m_trackHitSepLTrue[rad][trType]->fill( sep );
          if ( trueCKPhotonMCP ) {
            m_trackHitSepLTrueSigOnly[rad][trType]->fill( sep );
            if ( sep<500 ) {
              if ( sep>m_maxTrueHitSepL[rad][trType] ) m_maxTrueHitSepL[rad][trType] = sep;
              if ( sep<m_minTrueHitSepL[rad][trType] ) m_minTrueHitSepL[rad][trType] = sep;
            }
          }
        }

      } // end pixel loop
    }

    if ( m_truth ) {

      // True number of photons
      if ( mcType != Rich::Unknown // MC type known
           && trType == Rich::Track::Forward // Forward tracks only
           //&& segPathLen > npeTkLenCut[rad] // path length cut
           && m_geomEfficRay->geomEfficiency(segment,mcType) > 0.3  // Geom Eff
           ) {
        if ( truePhotons>0 ) {
          m_trueSignalPhots[rad][mcType]->fill( truePhotons );
          if ( beta>0.99 ) m_trueSignalPhotsB[rad][mcType]->fill( truePhotons );
        }
        if ( truePhotonsRec>0 ) {
          m_trueSignalPhotsRec[rad][mcType]->fill( truePhotonsRec );
          if ( beta>0.99 ) m_trueSignalPhotsRecB[rad][mcType]->fill( truePhotonsRec );
        }
      }

      // True-expected signal photons
      const double expPhots =
        ( mcType != Rich::Unknown ?
          m_tkSignal->nObservableSignalPhotons( segment, mcType ) : 0 );
      if ( expPhots > 0. )
        m_diffSignalPhots[rad][mcType]->fill( truePhotonsRec-expPhots );

    } // m_truth

  } // end loop over segments

  // Fill final plots
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
  {
    m_nPhotsPerRad[iRad]->fill( nPhots[iRad] );
    m_nTruePhotsPerRad[iRad]->fill( nPhotsT[iRad] );
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichPhotonMonitor::finalize()
{

  // final debug printout
  for ( unsigned iTkType = 0; iTkType<Rich::Track::NTrTypes; ++iTkType )
  {
    const std::string tk = Rich::text( static_cast<Rich::Track::Type>(iTkType) );
    info() << "Max/Min track-hit seps local : " << tk
           << " : Aerogel " << m_maxTrueHitSepL[0][iTkType] << "/" << m_minTrueHitSepL[0][iTkType]
           << " : C4F10 " << m_maxTrueHitSepL[1][iTkType] << "/" << m_minTrueHitSepL[1][iTkType]
           << " : CF4 " << m_maxTrueHitSepL[2][iTkType] << "/" << m_minTrueHitSepL[2][iTkType] << endreq;
  }
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
  {
    info() << "Max/Min " << static_cast<Rich::RadiatorType>(iRad) << " Phot. Energy at Prod : "
           << m_maxPhotEnAtProd[iRad] << "/" << m_minPhotEnAtProd[iRad] << endreq;
  }

  // Execute base class method
  return RichRecAlgBase::finalize();
}
