
//-----------------------------------------------------------------------------
/** @file RichPIDQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::PIDQC
 *
 *  CVS Log :-
 *  $Id: RichPIDQC.cpp,v 1.62 2007-07-09 17:11:49 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/AlgFactory.h"

// local
#include "RichPIDQC.h"

// namespaces
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PIDQC );

// Standard constructor, initializes variables
PIDQC::PIDQC( const std::string& name,
              ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase    ( name, pSvcLocator ),
    m_requiredRads ( Rich::NRadiatorTypes )
{

  // Declare job options
  declareProperty( "InputPIDs",   m_pidTDS   = LHCb::RichPIDLocation::Default );
  declareProperty( "MCHistoPath", m_mcHstPth = "RICH/PIDQC/MC/" );
  declareProperty( "HistoPath",   m_hstPth   = "RICH/PIDQC/" );
  declareProperty( "MCTruth",     m_truth    = true );
  declareProperty( "MinimumTrackMultiplicity", m_minMultCut = 0 );
  declareProperty( "MaximumTrackMultiplicity", m_maxMultCut = 999999 );
  declareProperty( "HistoBins",     m_bins = 50 );
  declareProperty( "FinalPrintout", m_finalPrintOut = true );
  declareProperty( "ExtraHistos",   m_extraHistos = false );
  declareProperty( "IgnoreRecoThresholds", m_ignoreRecoThres = false );
  declareProperty( "IgnoreMCThresholds", m_ignoreMCThres     = false );
  declareProperty( "KaonDLLCut", m_dllKaonCut = 9999999 );
  declareProperty( "PionDLLCut", m_dllPionCut = 9999999 );
  m_requiredRads[Rich::Aerogel]  = false;
  m_requiredRads[Rich::Rich1Gas] = false;
  m_requiredRads[Rich::Rich2Gas] = false;
  declareProperty( "RequiredRads", m_requiredRads );

}

// Destructor
PIDQC::~PIDQC() {};

// Initialisation
StatusCode PIDQC::initialize()
{
  // Initialize base class
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  acquireTool( "TrackSelector", m_trSelector, this );

  // Retrieve MC tool, if needed
  if ( m_truth ) acquireTool( "RichRecMCTruthTool", m_mcTruth );

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;
  if ( m_truth && !bookMCHistograms() ) return StatusCode::FAILURE;

  // Initialise summary information
  for ( int i = 0; i<6; ++i ) { for ( int j = 0; j<6; ++j ) { m_sumTab[i][j] = 0; } }
  m_nEvents[0] = 0;
  m_nEvents[1] = 0;
  m_nTracks[0] = 0;
  m_nTracks[1] = 0;

  // Warn if extra histos are enabled
  if ( m_extraHistos ) Warning( "Extra histograms are enabled", StatusCode::SUCCESS );

  // Warn if ignoring threshold information
  if ( m_ignoreRecoThres ) Warning( "Ignoring reco threshold information", StatusCode::SUCCESS );
  if ( m_ignoreMCThres )   Warning( "Ignoring MC threshold information", StatusCode::SUCCESS );

  // Warn if using kaon DLL cut
  if ( m_dllKaonCut < 9999991 ) Warning( "Applying kaon selection dll(kaon) < " +
                                         boost::lexical_cast<std::string>(m_dllKaonCut),
                                         StatusCode::SUCCESS );

  // Warn if using pion DLL cut
  if ( m_dllPionCut < 9999991 ) Warning( "Applying pion selection dll(pion) < " +
                                         boost::lexical_cast<std::string>(m_dllPionCut),
                                         StatusCode::SUCCESS );

  return sc;
}

StatusCode PIDQC::bookHistograms()
{
  std::string title;
  HYPOTHESIS_NAMES;

  title = "# PIDs per hypothesis";
  m_ids = histoSvc()->book( m_hstPth, 1, title, 7, -0.5, 6.5 );
  title = "# PIDs per event";
  m_Nids = histoSvc()->book( m_hstPth, 2, title, 201, -0.5, 200.5 );
  title = "Event Success/Failures";
  m_eventRate = histoSvc()->book( m_hstPth, 3, title, 2, -0.5, 1.5 );
  title = "Fraction of Tracks with PIDs";
  m_pidRate = histoSvc()->book( m_hstPth, 4, title, m_bins, 0, 1 );

  if ( m_extraHistos ) {

    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      title =  hypothesis[iHypo] + " ID : raw probability";
      m_pRaw[iHypo] = histoSvc()->book( m_hstPth, 10 + iHypo+1, title, m_bins, 0, 1 );
      title =  hypothesis[iHypo] + " ID : normalised probability";
      m_pNorm[iHypo] =  histoSvc()->book( m_hstPth, 20 + iHypo+1, title, m_bins, 0, 1 );
      title =  hypothesis[iHypo] + " ID : delta LogLikelihood";
      m_deltaLL[iHypo] = histoSvc()->book( m_hstPth, 30 + iHypo+1, title, m_bins, 0, 150 );
    } // hypo loop

  } // extra histo if

  return StatusCode::SUCCESS;
}

StatusCode PIDQC::bookMCHistograms()
{
  std::string title;
  int id;
  HYPOTHESIS_NAMES;

  title = "PID Performance Table";
  m_perfTable = histoSvc()->book( m_mcHstPth, 1, title,
                                  6, 0.5, 6.5, 6, 0.5, 6.5 );

  {for ( int iTrue = 0; iTrue < Rich::NParticleTypes+1; ++iTrue ) {
    for ( int iID = 0; iID < Rich::NParticleTypes+1; ++iID ) {
      title = "Ptot : MC=" + hypothesis[iTrue] + " ID=" + hypothesis[iID];
      id = 10*(1+iTrue) + (1+iID) + 100;
      m_ptotSpec[iTrue][iID] = histoSvc()->book( m_mcHstPth, id, title,
                                                 m_bins, m_trSelector->minPCut(), m_trSelector->maxPCut() );
    }
  }}

  if ( m_extraHistos ) {

    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      title = hypothesis[iHypo] + " true ID : delta LogLikelihood";
      m_deltaLLTrue[iHypo] = histoSvc()->book( m_mcHstPth, 70 + iHypo+1, title,
                                               m_bins, 0.0, 150.0 );
      title = hypothesis[iHypo] + " fake ID : delta LogLikelihood";
      m_deltaLLFake[iHypo] = histoSvc()->book( m_mcHstPth, 80 + iHypo+1, title,
                                               m_bins, 0.0, 150.0 );
    } // end hypo loop

    for ( int iID = 0; iID < Rich::NParticleTypes; ++iID ) {
      for ( int iSec = 0; iSec < Rich::NParticleTypes; ++iSec ) {
        if ( iSec != iID ) {

          title = "Dll("+hypothesis[iID]+"-"+hypothesis[iSec]+") : true " + hypothesis[iID];
          id = 10*(1+iSec) + (1+iID) + 200;
          m_dLLTrue[iID][iSec] = histoSvc()->book( m_mcHstPth, id, title, m_bins, -100, 100 );

          title = "Dll("+hypothesis[iID]+"-"+hypothesis[iSec]+") : false " + hypothesis[iID];
          id = 10*(1+iSec) + (1+iID) + 300;
          m_dLLFalse[iID][iSec] = histoSvc()->book( m_mcHstPth, id, title, m_bins, -100, 100 );

          title = "#sigma("+hypothesis[iID]+"-"+hypothesis[iSec]+") V P : true " + hypothesis[iID];
          id = 10*(1+iSec) + (1+iID) + 400;
          m_nsigvpTrue[iID][iSec] = histoSvc()->book( m_mcHstPth, id, title,
                                                      m_bins, m_trSelector->minPCut(), m_trSelector->maxPCut(),
                                                      m_bins, -30, 30 );

          title = "#sigma("+hypothesis[iID]+"-"+hypothesis[iSec]+") V P : false " + hypothesis[iID];
          id = 10*(1+iSec) + (1+iID) + 500;
          m_nsigvpFalse[iID][iSec] = histoSvc()->book( m_mcHstPth, id, title,
                                                       m_bins, m_trSelector->minPCut(), m_trSelector->maxPCut(),
                                                       m_bins, -30, 30 );

        }
      }
    }

  } // extra histos if

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode PIDQC::execute()
{
  debug() << "Execute" << endreq;

  // Load RichPID data
  if ( !loadPIDData() || m_richPIDs.empty() ) return StatusCode::SUCCESS;

  // Count tracks
  m_totalSelTracks = 0;
  m_multiplicity = 0;
  Rich::Map<std::string,bool> locs;
  for ( std::vector<ContainedObject*>::const_iterator iC = m_richPIDs.begin();
        iC != m_richPIDs.end(); ++iC )
  {
    // create pointer to pid object
    LHCb::RichPID * fPID = dynamic_cast<LHCb::RichPID*>(*iC);

    // check track is OK
    if ( fPID->track() )
    {

      // get location of associated track container
      const std::string contLoc = objectLocation( fPID->track()->parent() );

      // if new, count all tracks in this container
      if ( locs.end() == locs.find(contLoc) )
      {
        locs[contLoc] = true;
        try
        {
          countTracks(contLoc);
        }
        catch ( const GaudiException & exp )
        {
          error() << exp << endreq;
          return Warning( "Failed to load tracking objects at "+contLoc,
                          StatusCode::SUCCESS );
        }
      }

    } // track OK
    else
    {
      Warning( "RichPID has null Track SmartRef" );
    }

  }

  // apply track multiplicity cuts
  if ( m_multiplicity < m_minMultCut ||
       m_multiplicity > m_maxMultCut ) return StatusCode::SUCCESS;

  // count pids per track type
  int pidCount = 0;

  // Loop over all PID results
  if ( !m_richPIDs.empty() ) {
    for ( std::vector<ContainedObject*>::const_iterator iC = m_richPIDs.begin();
          iC != m_richPIDs.end(); ++iC ) {
      LHCb::RichPID * iPID = dynamic_cast<LHCb::RichPID*>(*iC);

      // Track for this PID
      const LHCb::Track * track = iPID->track();

      // check rads that must have been used
      if ( (m_requiredRads[Rich::Aerogel]  && !iPID->usedAerogel())  ||
           (m_requiredRads[Rich::Rich1Gas] && !iPID->usedRich1Gas()) ||
           (m_requiredRads[Rich::Rich2Gas] && !iPID->usedRich2Gas()) ) continue;

      // must have aero or c4f10 segment
      //if ( !iPID->usedAerogel() && !iPID->usedRich1Gas() ) continue;

      // Track selection
      if ( !m_trSelector->trackSelected( track ) ) continue;

      // Track momentum in GeV/C
      const LHCb::State* state = &(track)->firstState();
      const double tkPtot = ( state ? state->p()/Gaudi::Units::GeV : 0 );

      // Track type
      const Rich::Rec::Track::Type tkType = Rich::Rec::Track::type(track);

      // Count PIDs and tracks
      ++m_trackCount[tkType].first;
      ++pidCount;
      ++m_pidPerTypeCount[iPID->pidType()].first;
      ++m_radCount[ Radiators(iPID->usedAerogel(),iPID->usedRich1Gas(),iPID->usedRich2Gas()) ];

      // Get best PID
      Rich::ParticleIDType pid = iPID->bestParticleID();

      // Apply DLL based selection for kaons
      if ( m_dllKaonCut < 999999 )
      {
        if      ( iPID->particleDeltaLL(Rich::Kaon)-iPID->particleDeltaLL(Rich::Pion) > m_dllKaonCut )
        {
          //Warning( "DLL cut reassigned "+Rich::text(pid)+" to "+Rich::text(Rich::Kaon), StatusCode::SUCCESS, 1 );
          pid = Rich::Kaon;
        }
        else
        {
          pid = Rich::Pion;
        }
      }
      if ( m_dllPionCut < 999999 )
      {
        if ( iPID->particleDeltaLL(Rich::Pion)-iPID->particleDeltaLL(Rich::Kaon) > m_dllPionCut )
        {
          //Warning( "DLL cut reassigned "+Rich::text(pid)+" to "+Rich::text(Rich::Pion), StatusCode::SUCCESS, 1 );
          pid = Rich::Pion;
        }
      }

      // MC Truth
      Rich::ParticleIDType mcpid = Rich::Unknown;
      if ( m_truth )
      {
        // Get true track type from MC
        mcpid = m_mcTruth->mcParticleType(track);
        if ( mcpid != Rich::Unknown &&
             !m_ignoreMCThres &&
             !iPID->isAboveThreshold(mcpid) )
        {
          //Warning( "MC-PID "+Rich::text(mcpid)+" reassigned to BelowThreshold", StatusCode::SUCCESS, 0 );
          mcpid = Rich::BelowThreshold;
        }
      }

      // Check for threshold
      if ( !m_ignoreRecoThres && !iPID->isAboveThreshold(pid) )
      {
        //Warning( "Rec-PID "+Rich::text(pid)+" reassigned to BelowThreshold (MC-PID="+Rich::text(mcpid)+")",
        //        StatusCode::SUCCESS, 0 );
        pid = Rich::BelowThreshold;
      }

      // some verbose printout
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "RichPID " << iPID->key() << " ("
                  << iPID->pidType() << "), '"
                  << tkType << "' track, Ptot " << tkPtot << " GeV/c," << endreq
                  << "  Active rads =";
        if ( iPID->usedAerogel()  ) { verbose() << " " << Rich::Aerogel;  }
        if ( iPID->usedRich1Gas() ) { verbose() << " " << Rich::Rich1Gas; }
        if ( iPID->usedRich2Gas() ) { verbose() << " " << Rich::Rich2Gas; }
        verbose() << endreq
                  << "  Threshold   = ";
        {for ( int ipid = 0; ipid < Rich::NParticleTypes; ++ipid ) {
          const Rich::ParticleIDType pid = static_cast<Rich::ParticleIDType>(ipid);
          const std::string T = iPID->isAboveThreshold(pid) ? "T" : "F";
          verbose() << T << " ";
        }}
        verbose() << endreq
                  << "  Dlls        = " << iPID->particleLLValues() << endreq
                  << "  Prob(r/n)   = ";
        {for ( int ipid = 0; ipid < Rich::NParticleTypes; ++ipid ) {
          const Rich::ParticleIDType pid = static_cast<Rich::ParticleIDType>(ipid);
          verbose() << iPID->particleRawProb(pid) << "/" << iPID->particleNormProb(pid) << " ";
        }}
        verbose() << endreq << "  RecoPID     = " << pid << endreq;
      }

      // Fill histos for deltaLLs and probabilities
      m_ids->fill( pid+1 );

      // Extra histograms
      if ( m_extraHistos )
      {
        for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
        {
          m_pRaw[iHypo]->fill    ( iPID->particleRawProb((Rich::ParticleIDType)iHypo) );
          m_pNorm[iHypo]->fill   ( iPID->particleNormProb((Rich::ParticleIDType)iHypo) );
          m_deltaLL[iHypo]->fill ( iPID->particleDeltaLL((Rich::ParticleIDType)iHypo) );
        }
      }

      // MC Truth
      if ( m_truth )
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "  MCID        = " << mcpid << endreq;

        // Count track and PID types
        if ( Rich::Unknown != mcpid ) {
          ++m_trackCount[tkType].second;
          ++m_pidPerTypeCount[iPID->pidType()].second;
        }

        // Fill performance tables
        if ( mcpid != Rich::Unknown &&
             pid   != Rich::Unknown )
        { m_perfTable->fill( mcpid+1, pid+1 ); ++m_sumTab[mcpid][pid]; }

        // Momentum spectra histograms...
        if ( mcpid != Rich::Unknown &&
             pid   != Rich::Unknown ) { (m_ptotSpec[mcpid][pid])->fill(tkPtot); }

        // Extra histograms
        if ( m_extraHistos )
        {

          // Delta LL values with respect to the best ID
          for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
            if ( iHypo == mcpid ) {
              m_deltaLLTrue[iHypo]->fill(iPID->particleDeltaLL((Rich::ParticleIDType)iHypo));
            } else {
              m_deltaLLFake[iHypo]->fill(iPID->particleDeltaLL((Rich::ParticleIDType)iHypo));
            }
          }

          // Delta LL between particle types
          for ( int iID = 0; iID < Rich::NParticleTypes; ++iID ) {
            for ( int iSec = 0; iSec < Rich::NParticleTypes; ++iSec ) {
              if ( iSec != iID ) {
                const double Dll =  ( iPID->particleDeltaLL((Rich::ParticleIDType)iID) -
                                      iPID->particleDeltaLL((Rich::ParticleIDType)iSec) );
                const double nsig = ( iPID->nSigmaSeparation((Rich::ParticleIDType)iID,
                                                             (Rich::ParticleIDType)iSec) );
                if ( mcpid == iID ) {
                  (m_dLLTrue[iID][iSec])->fill( Dll );
                  (m_nsigvpTrue[iID][iSec])->fill( tkPtot, nsig );
                } else {
                  (m_dLLFalse[iID][iSec])->fill( Dll );
                  (m_nsigvpFalse[iID][iSec])->fill( tkPtot, nsig );
                }
              }
            }
          }

        } // extra histos

      }

    } // end PID loop
  } // end empty if

    // count events and tracks
  ++m_nEvents[0];
  if ( !m_richPIDs.empty() ) ++m_nEvents[1];
  m_nTracks[0] += m_totalSelTracks;
  m_nTracks[1] += pidCount;
  m_Nids->fill( pidCount );
  m_eventRate->fill( (m_richPIDs.empty() ? 0 : 1) );
  if ( m_totalSelTracks>0 )
  {
    m_pidRate->fill( static_cast<double>(pidCount) / static_cast<double>(m_totalSelTracks) );
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Total Tracks = " << m_totalSelTracks << " : tracks PIDed = " << pidCount << endreq;
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode PIDQC::finalize()
{
  if ( m_truth && m_finalPrintOut )
  {

    // index variables
    int iRec, iTrue;

    // compute efficiencies and purities
    double sumTot = 0;
    double recTot[]          = {0,0,0,0,0,0};
    double trueTot[]         = {0,0,0,0,0,0};
    double trueTotExcludeX[] = {0,0,0,0,0,0};
    double eff[]             = {0,0,0,0,0,0};
    double purity[]          = {0,0,0,0,0,0};
    for ( iRec = 0; iRec<6; ++iRec ) {
      for ( iTrue = 0; iTrue<6; ++iTrue ) {
        sumTot += m_sumTab[iTrue][iRec];;
        recTot[iRec] += m_sumTab[iTrue][iRec];
        trueTot[iTrue] += m_sumTab[iTrue][iRec];
        if (iRec<5) trueTotExcludeX[iTrue] += m_sumTab[iTrue][iRec];
      }
    }
    if ( sumTot < 1 ) sumTot = 1;
    for ( iRec = 0; iRec<6; ++iRec ) {
      eff[iRec] = ( trueTot[iRec]>0 ? 100*m_sumTab[iRec][iRec]/trueTot[iRec] : 0 );
      purity[iRec] = ( recTot[iRec]>0 ? 100*m_sumTab[iRec][iRec]/recTot[iRec] : 0 );
    }

    // Kaon Pion seperation
    double kaonIDEff[2];
    kaonIDEff[0] = ( trueTotExcludeX[Rich::Kaon]>0 ?
                     100*( m_sumTab[Rich::Kaon][Rich::Kaon] +
                           m_sumTab[Rich::Kaon][Rich::Proton] ) /
                     trueTotExcludeX[Rich::Kaon] : 0 );
    kaonIDEff[1] = ( trueTotExcludeX[Rich::Kaon]>0 ?
                     sqrt( kaonIDEff[0]*(100.-kaonIDEff[0])/
                           trueTotExcludeX[Rich::Kaon] ) : 0 );
    double kaonMisIDEff[2];
    kaonMisIDEff[0] = ( trueTotExcludeX[Rich::Kaon]>0 ?
                        100*( m_sumTab[Rich::Kaon][Rich::Electron] +
                              m_sumTab[Rich::Kaon][Rich::Muon] +
                              m_sumTab[Rich::Kaon][Rich::Pion] ) /
                        trueTotExcludeX[Rich::Kaon] : 0 );
    kaonMisIDEff[1] = ( trueTotExcludeX[Rich::Kaon]>0 ?
                        sqrt( kaonMisIDEff[0]*(100.-kaonMisIDEff[0])/
                              trueTotExcludeX[Rich::Kaon] ) : 0 );
    double piIDEff[2];
    piIDEff[0] = ( trueTotExcludeX[Rich::Pion]>0 ?
                   100*( m_sumTab[Rich::Pion][Rich::Electron] +
                         m_sumTab[Rich::Pion][Rich::Muon] +
                         m_sumTab[Rich::Pion][Rich::Pion] ) /
                   trueTotExcludeX[Rich::Pion] : 0 );
    piIDEff[1] =  ( trueTotExcludeX[Rich::Pion]>0 ?
                    sqrt( piIDEff[0]*(100.-piIDEff[0])/
                          trueTotExcludeX[Rich::Pion] ) : 0 );
    double piMisIDEff[2];
    piMisIDEff[0] = ( trueTotExcludeX[Rich::Pion]>0 ?
                      100*( m_sumTab[Rich::Pion][Rich::Kaon] +
                            m_sumTab[Rich::Pion][Rich::Proton] ) /
                      trueTotExcludeX[Rich::Pion] : 0 );
    piMisIDEff[1] = ( trueTotExcludeX[Rich::Pion]>0 ?
                      sqrt( piMisIDEff[0]*(100.-piMisIDEff[0])/
                            trueTotExcludeX[Rich::Pion] ) : 0 );

    // Scale entries to percent of total number of entries
    for ( iTrue = 0; iTrue<6; ++iTrue )
    {
      for ( iRec = 0; iRec<6; ++iRec )
      {
        m_sumTab[iTrue][iRec] = 100.0*m_sumTab[iTrue][iRec]/sumTot;
      }
    }

    // compute event and track PID success rates
    double evPIDRate[2];
    double trPIDRate[2];
    evPIDRate[0] = ( m_nEvents[0]>0 ? 100.*m_nEvents[1]/m_nEvents[0] : 100 );
    evPIDRate[1] = ( m_nEvents[0]>0 ? sqrt(evPIDRate[0]*(100.-evPIDRate[0])/m_nEvents[0]) : 100 );
    trPIDRate[0] = ( m_nTracks[0]>0 ? 100.*m_nTracks[1]/m_nTracks[0] : 100 );
    trPIDRate[1] = ( m_nTracks[0]>0 ? sqrt(trPIDRate[0]*(100.-trPIDRate[0])/m_nTracks[0]) : 100 );

    info() << "-------------+-------------------------------------------------+------------"
           << endreq
           << " Ptot Sel    | " << m_trSelector->minPCut() << "-" << m_trSelector->maxPCut() << " GeV/c" << endreq
           << " TkMult Sel  | " << m_minMultCut << "-" << m_maxMultCut << " tracks/event" << endreq;
    info() << " #Tks(+MC)   |";
    unsigned int tkCount = 0;
    for ( TkCount::const_iterator iTk = m_trackCount.begin();
          iTk != m_trackCount.end(); ++iTk, ++tkCount ) {
      if ( tkCount == 3 ) { tkCount = 0; info() << endreq << "             |"; }
      info() << " " << (*iTk).first << "=" << (*iTk).second.first
             << "(" << (*iTk).second.second << ")";
    }
    tkCount = 0;
    info() << endreq
           << " Using PIDs  | " << m_pidTDS << endreq
           << " #PIDs(+MC)  |";
    for ( PIDsByType::const_iterator iPC = m_pidPerTypeCount.begin();
          iPC != m_pidPerTypeCount.end(); ++iPC, ++tkCount ) {
      if ( tkCount == 3 ) { tkCount = 0; info() << endreq << "             |"; }
      info() << " " << (*iPC).first << "=" << (*iPC).second.first
             << "(" << (*iPC).second.second << ")";
    }
    info() << endreq;
    if ( m_dllKaonCut < 9999991 ) {
      info() << " Tagging tracks as kaons if kaon DLL > " << m_dllKaonCut << endreq;
    }
    if ( m_dllPionCut < 9999991 ) {
      info() << " Tagging tracks as pions if pion DLL > " << m_dllPionCut << endreq;
    }
    info() << "-------------+-------------------------------------------------+------------"
           << endreq
           << "   %total    |  Electron Muon   Pion   Kaon  Proton   X  (MC)  |  %Purity"
           << endreq
           << "-------------+-------------------------------------------------+------------"
           << endreq
           << "             |                                                 |" << endreq;
    const std::string type[6] = { " Electron    |", " Muon        |", " Pion        |",
                                  " Kaon        |", " Proton      |", " X           |" };
    for ( iRec = 0; iRec < 6; ++iRec )
    {
      info() << type[iRec]
             << boost::format( " %7.2f%7.2f%7.2f%7.2f%7.2f%7.2f      | %7.2f" ) %
        m_sumTab[0][iRec] % m_sumTab[1][iRec] %
        m_sumTab[2][iRec] % m_sumTab[3][iRec] %
        m_sumTab[4][iRec] % m_sumTab[5][iRec] %
        purity[iRec] << endreq;
    }
    info() << "   (reco)    |                                                 |" << endreq
           << "-------------+-------------------------------------------------+------------"
           << endreq;
    info() << "   %Eff.     |" << boost::format( " %7.2f%7.2f%7.2f%7.2f%7.2f%7.2f " ) %
      eff[0] % eff[1] % eff[2] % eff[3] % eff[4] % eff[5]
           << "     |" << endreq;
    info() << "-------------+-------------------------------------------------+------------"
           << endreq;

    info() << " % ID eff    |  K->K,Pr   : "
           << boost::format( "%6.2f +-%6.2f   pi->e,m,pi : %6.2f +-%6.2f " ) %
      kaonIDEff[0] % kaonIDEff[1] % piIDEff[0] % piIDEff[1] << endreq;
    info() << " % MisID eff |  K->e,m,pi : "
           << boost::format( "%6.2f +-%6.2f   pi->K,Pr   : %6.2f +-%6.2f " ) %
      kaonMisIDEff[0] % kaonMisIDEff[1] % piMisIDEff[0] % piMisIDEff[1] << endreq;
    info() << " % ID rate   |  Events    : "
           << boost::format( "%6.2f +-%6.2f   Tracks     : %6.2f +-%6.2f " ) %
      evPIDRate[0] % evPIDRate[1] % trPIDRate[0] % trPIDRate[1] << endreq;
    
    for ( RadCount::const_iterator iR = m_radCount.begin(); iR != m_radCount.end(); ++iR )
    {
      const double eff = ( m_nTracks[0]>0 ? 100.*((double)iR->second)/m_nTracks[0] : 100 );
      const double err = ( m_nTracks[0]>0 ? sqrt(eff*(100.-eff)/m_nTracks[0]) : 100 );
      info() << "             |  -> With "
             << (*iR).first.radiators() << boost::format( "   : %6.2f +-%6.2f" ) % eff % err << endreq;
    }

    info() << "-------------+-------------------------------------------------+------------"
           << endreq;

  } // final printout

    // finalize base class
  return Rich::Rec::AlgBase::finalize();
}

StatusCode PIDQC::loadPIDData()
{
  // Load PIDs
  DataObject *pObject;
  if ( eventSvc()->retrieveObject( m_pidTDS, pObject ) ) {
    if ( KeyedContainer<LHCb::RichPID, Containers::HashMap> * pids =
         static_cast<KeyedContainer<LHCb::RichPID, Containers::HashMap>*>(pObject) )
    {
      m_richPIDs.erase( m_richPIDs.begin(), m_richPIDs.end() );
      pids->containedObjects( m_richPIDs );
      debug() << "Located " << pids->size() << " RichPIDs at " << m_pidTDS << endreq;
      return StatusCode::SUCCESS;
    }
  }

  // If we get here, things went wrong
  return Warning( "Failed to locate RichPIDs at " + m_pidTDS );
}

void PIDQC::countTracks( const std::string & location )
{
  LHCb::Tracks * tracks = get<LHCb::Tracks>( location );
  debug() << "Found " << tracks->size() << " Tracks at " << location << endreq;
  for ( LHCb::Tracks::const_iterator iTrk = tracks->begin();
        iTrk != tracks->end(); ++iTrk )
  {
    if ( !(*iTrk)->checkFlag(LHCb::Track::Clone ) ) { ++m_multiplicity;   }
    if ( m_trSelector->trackSelected( *iTrk ) ) { ++m_totalSelTracks; }
  }
}
