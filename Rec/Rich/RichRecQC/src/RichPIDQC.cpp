
/** @file RichPIDQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : RichPIDQC
 *
 *  CVS Log :-
 *  $Id: RichPIDQC.cpp,v 1.26 2004-10-13 09:39:01 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.25  2004/08/20 16:08:30  jonrob
 *  Add tally for each type of PID result to final results table
 *
 *  Revision 1.24  2004/08/19 14:14:09  jonrob
 *  Tidy up monitoring algorithms and add new options to create
 *  histograms for different PID efficiency and purity settings
 *
 *  Revision 1.23  2004/07/27 13:56:30  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichPIDQC.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPIDQC>          s_factory ;
const        IAlgFactory& RichPIDQCFactory = s_factory ;

// Standard constructor, initializes variables
RichPIDQC::RichPIDQC( const std::string& name,
                      ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator )
{

  // Declare job options
  declareProperty( "InputPIDs",   m_pidTDS = RichPIDLocation::Default );
  declareProperty( "InputTracks", m_trackTDS = TrStoredTrackLocation::Default );
  declareProperty( "MCHistoPath", m_mcHstPth = "RICH/PIDQC/MC/" );
  declareProperty( "HistoPath",   m_hstPth = "RICH/PIDQC/" );
  declareProperty( "MinPCut",     m_pMinCut = 2.0 );
  declareProperty( "MaxPCut",     m_pMaxCut = 100.0 );
  declareProperty( "MCTruth",     m_truth = true );
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
  declareProperty( "MinimumTrackMultiplicity", m_minMultCut = 1 );
  declareProperty( "MaximumTrackMultiplicity", m_maxMultCut = 999999 );
  declareProperty( "HistoBins",     m_bins = 50 );
  declareProperty( "FinalPrintout", m_finalPrintOut = true );
  declareProperty( "ExtraHistos",   m_extraHistos = true );

  declareProperty( "TightNSigmaCutPion", m_tightNsigCutPion = 0 );
  declareProperty( "LooseNSigmaCutPion", m_looseNsigCutPion = -99999999 );
  declareProperty( "TightNSigmaCutKaon", m_tightNsigCutKaon = 0 );
  declareProperty( "LooseNSigmaCutKaon", m_looseNsigCutKaon = -99999999 );

}

// Destructor
RichPIDQC::~RichPIDQC() {};

// Initialisation
StatusCode RichPIDQC::initialize()
{
  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Check momentum cuts make sense
  if ( m_pMinCut >= m_pMaxCut ) {
    err() << "Invalid Min/Max momentum cuts "
          << m_pMinCut << "/" << m_pMaxCut << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve MC tool, if needed
  if ( m_truth ) acquireTool( "RichMCTruthTool", m_mcTruth );

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;
  if ( m_truth && !bookMCHistograms() ) return StatusCode::FAILURE;

  // Initialise summary information
  for ( int i = 0; i<6; ++i ) { for ( int j = 0; j<6; ++j ) { m_sumTab[i][j] = 0; } }
  m_nEvents[0] = 0;
  m_nEvents[1] = 0;
  m_nTracks[0] = 0;
  m_nTracks[1] = 0;
  for ( unsigned iTk = 0; iTk < Rich::Track::NTrTypes; ++iTk ) {
    m_trackCount[0][iTk] = 0;
    m_trackCount[1][iTk] = 0;
  }

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;

  // Warn if extra histos are enabled (To be renoved for public release)
  if ( m_extraHistos ) Warning( "Extra histograms are enabled", StatusCode::SUCCESS );

  // debug printout of configuration
  debug() << " Track types selected   = " << m_trSelector.selectedTrackTypes()
          << endreq;

  return StatusCode::SUCCESS;
};

StatusCode RichPIDQC::bookHistograms()
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

StatusCode RichPIDQC::bookMCHistograms()
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
                                                 m_bins, m_pMinCut, m_pMaxCut );
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
                                                      m_bins, m_pMinCut, m_pMaxCut,
                                                      m_bins, -30, 30 );

          title = "#sigma("+hypothesis[iID]+"-"+hypothesis[iSec]+") V P : false " + hypothesis[iID];
          id = 10*(1+iSec) + (1+iID) + 500;
          m_nsigvpFalse[iID][iSec] = histoSvc()->book( m_mcHstPth, id, title,
                                                       m_bins, m_pMinCut, m_pMaxCut,
                                                       m_bins, -30, 30 );

        }
      }
    }

  } // extra histos if

  return StatusCode::SUCCESS;
}

void RichPIDQC::countTrStoredTracks()
{
  m_multiplicity = 0;
  m_totalSelTracks = 0;
  TrStoredTracks * tracks = get<TrStoredTracks>( m_trackTDS );
  debug() << "Found " << tracks->size() << " TrStoredTracks at " << m_trackTDS << endreq;
  for ( TrStoredTracks::const_iterator iTrk = tracks->begin();
        iTrk != tracks->end(); ++iTrk ) {
    if ( (*iTrk)->unique() ) ++m_multiplicity;
    if ( !m_trSelector.trackSelected( *iTrk ) ) continue;
    const TrStateP* pState = getTrStateP( *iTrk );
    const double tkPtot = ( pState ? pState->p()/GeV : 0 );
    if ( tkPtot > m_pMaxCut || tkPtot < m_pMinCut ) continue;
    ++m_totalSelTracks;
  }
}

void RichPIDQC::countTrgTracks()
{
  m_multiplicity = 0;
  m_totalSelTracks = 0;
  TrgTracks * tracks = get<TrgTracks>( TrgTrackLocation::Long );
  debug() << "Found " << tracks->size() << " TrgTracks at " << TrgTrackLocation::Long << endreq;
  m_multiplicity = tracks->size();
  for ( TrgTracks::const_iterator iTrk = tracks->begin();
        iTrk != tracks->end(); ++iTrk ) {
    if ( !m_trSelector.trackSelected( *iTrk ) ) continue;
    const double tkPtot = fabs((*iTrk)->firstState().momentum());
    if ( tkPtot > m_pMaxCut || tkPtot < m_pMinCut ) continue;
    ++m_totalSelTracks;
  }
}

// Main execution
StatusCode RichPIDQC::execute()
{
  debug() << "Execute" << endreq;

  // Load RichPID data
  if ( !loadPIDData() || m_richPIDs.empty() ) return StatusCode::SUCCESS;

  // get first PID to test track type
  RichPID * fPID = dynamic_cast<RichPID*>(*m_richPIDs.begin());

  if ( fPID->isTrStoredTrackPID() ) {
    countTrStoredTracks();
  } else if ( fPID->isTrgTrackPID() ) {
    countTrgTracks();
  } else {
    return Error( "Unknown Track Type in RichPIDs" );
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
      RichPID * iPID = dynamic_cast<RichPID*>(*iC);

      // Track selection
      if ( !pidIsSelected( iPID ) ) continue;

      // Track momentum in GeV/C
      const double tkPtot = momentum( iPID );
      if ( tkPtot > m_pMaxCut || tkPtot < m_pMinCut ) continue;

      // Track type
      Rich::Track::Type tkType = trackType(iPID);

      // Count PIDs and tracks
      ++m_trackCount[0][tkType];
      ++pidCount;
      ++m_pidPerTypeCount[iPID->pidType()];

      // Get best PID
      Rich::ParticleIDType pid = iPID->bestParticleID();

      // Apply loose nsigma cut.
      if      ( iPID->nSigmaSeparation(pid,Rich::Kaon) < m_looseNsigCutKaon ) { pid = Rich::Kaon; }
      else if ( iPID->nSigmaSeparation(pid,Rich::Pion) < m_looseNsigCutPion ) { pid = Rich::Pion; }

      // apply tight cut
      if      ( Rich::Pion == pid &&
                iPID->nSigmaSeparation(Rich::Pion,Rich::Kaon) < m_tightNsigCutPion ) { pid = Rich::BelowThreshold; }
      else if ( Rich::Kaon == pid &&
                iPID->nSigmaSeparation(Rich::Kaon,Rich::Pion) < m_tightNsigCutKaon ) { pid = Rich::BelowThreshold; }

      // Check for threshold
      if ( !iPID->isAboveThreshold(pid) ) { pid = Rich::BelowThreshold; }

      // some debug printout
      if ( msgLevel(MSG::DEBUG) ) {
        debug() << "RichPID " << iPID->key() << " ("
                << iPID->pidType() << "), Track type "
                << tkType << tkPtot << " GeV/c,"
                << " Rads " << iPID->usedAerogel() << " " << iPID->usedC4F10() << " " << iPID->usedCF4()
                << endreq
                << "  Dlls      = " << iPID->particleLLValues() << endreq
                << "  Prob(r/n) = ";
        for ( int ipid = 0; ipid < Rich::NParticleTypes; ++ipid ) {
          const Rich::ParticleIDType pid = static_cast<Rich::ParticleIDType>(ipid);
          debug() << iPID->particleRawProb(pid) << "/" << iPID->particleNormProb(pid) << " ";
        }
        debug() << endreq << "  RecoPID   = " << pid;
      }

      // Fill histos for deltaLLs and probabilities
      m_ids->fill( pid+1 );

      // Extra histograms
      if ( m_extraHistos ) {
        for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
          m_pRaw[iHypo]->fill( iPID->particleRawProb((Rich::ParticleIDType)iHypo) );
          m_pNorm[iHypo]->fill( iPID->particleNormProb((Rich::ParticleIDType)iHypo) );
          m_deltaLL[iHypo]->fill( iPID->particleDeltaLL((Rich::ParticleIDType)iHypo) );
        }
      }

      // MC Truth
      if ( m_truth ) {

        // Get true track type from MC
        Rich::ParticleIDType mcpid = trueMCType( iPID );
        if ( mcpid != Rich::Unknown &&
             !iPID->isAboveThreshold(mcpid) ) mcpid = Rich::BelowThreshold;
        if ( msgLevel(MSG::DEBUG) ) debug() << ", MCID = " << mcpid << endreq;

        // Count track types
        if ( Rich::Unknown != mcpid ) ++m_trackCount[1][tkType];

        // Fill performance tables
        m_perfTable->fill( mcpid+1, pid+1 );
        if ( mcpid>=0 && pid>=0 ) { m_sumTab[mcpid][pid] += 1; }

        // Momentum spectra
        if ( mcpid != Rich::Unknown &&
             pid   != Rich::Unknown ) { (m_ptotSpec[mcpid][pid])->fill(tkPtot); }

        // Extra histograms
        if ( m_extraHistos ) {

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

      } else {
        debug() << endreq;
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
  if ( m_totalSelTracks>0 ) {
    m_pidRate->fill( static_cast<double>(pidCount) / static_cast<double>(m_totalSelTracks) );
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichPIDQC::finalize()
{
  if ( m_truth && m_finalPrintOut ) {

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
    for ( iTrue = 0; iTrue<6; ++iTrue ) {
      for ( iRec = 0; iRec<6; ++iRec ) {
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

    info() << "-----------+-----------------------------------------------+-----------"
           << endreq << "  Tk Sel   | " << m_pMinCut << "-" << m_pMaxCut << " GeV/c" << endreq;
    info() << " #Tks(+MC) |";
    unsigned tkCount = 0;
    for ( unsigned iTk = 0; iTk < Rich::Track::NTrTypes; ++iTk ) {
      if ( tkCount == 4 ) { tkCount = 0; info() << endreq << "           |"; }
      if ( m_trSelector.typeSelected((Rich::Track::Type)iTk) ) {
        info() << " " << (Rich::Track::Type)iTk << " " << m_trackCount[0][iTk]
               << "(" << m_trackCount[1][iTk] << ")";
        ++tkCount;
      }
    }
    info() << endreq << " #RichPIDs |";
    for ( PIDsByType::const_iterator iPC = m_pidPerTypeCount.begin();
          iPC != m_pidPerTypeCount.end(); ++iPC ) {
      info() << " " << (*iPC).first << "=" << (*iPC).second;
    }
    info() << endreq
           << "-----------+-----------------------------------------------+-----------"
           << endreq
           << "  %total   | Electron Muon   Pion   Kaon  Proton   X  (MC) |  %Purity"
           << endreq
           << "-----------+-----------------------------------------------+-----------"
           << endreq
           << "           |                                               |" << endreq;
    std::string type[6] = { " Electron  |", " Muon      |", " Pion      |",
                            " Kaon      |", " Proton    |", " X         |" };
    for ( iRec = 0; iRec < 6; ++iRec ) {
      info() << type[iRec] << format( "%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f     |%7.2f",
                                      m_sumTab[0][iRec], m_sumTab[1][iRec],
                                      m_sumTab[2][iRec], m_sumTab[3][iRec],
                                      m_sumTab[4][iRec], m_sumTab[5][iRec],
                                      purity[iRec] ) << endreq;
    }
    info() << "  (reco)   |                                               |" << endreq
           << "-----------+-----------------------------------------------+-----------"
           << endreq
           << " %Eff.     |" << format( "%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f",
                                        eff[0],eff[1],eff[2],eff[3],eff[4],eff[5] )
           << "     |" << endreq
           << "-----------+-----------------------------------------------+-----------" << endreq;
    info() << format( "  %ID      |    Ka: %6.2f+-%6.2f    Pi: %6.2f+-%6.2f ",
                      kaonIDEff[0], kaonIDEff[1], piIDEff[0], piIDEff[1] ) << endreq;
    info() << format( "  %MisID   |    Ka: %6.2f+-%6.2f    Pi: %6.2f+-%6.2f ",
                      kaonMisIDEff[0], kaonMisIDEff[1], piMisIDEff[0], piMisIDEff[1] ) << endreq;
    info() << " PID rate  |    events " << evPIDRate[0] << " +- " << evPIDRate[1]
           << "%,  tracks " << trPIDRate[0] << " +- " << trPIDRate[1] << "%" << endreq
           << "-----------+-----------------------------------------------+-----------"
           << endreq;

  } // final printout

  // finalize base class
  return RichAlgBase::finalize();
}

StatusCode RichPIDQC::loadPIDData()
{
  // Load PIDs
  DataObject *pObject;
  if ( eventSvc()->retrieveObject( m_pidTDS, pObject ) ) {
    if ( KeyedContainer<RichPID, Containers::HashMap> * pids =
         static_cast<KeyedContainer<RichPID, Containers::HashMap>*>(pObject) ) {
      m_richPIDs.erase( m_richPIDs.begin(), m_richPIDs.end() );
      pids->containedObjects( m_richPIDs );
      debug() << "Located " << pids->size() << " RichPIDs at " << m_pidTDS << endreq;
      return StatusCode::SUCCESS;
    }
  }

  // If we get here, things went wrong
  return Warning( "Failed to located RichPIDs at " + m_pidTDS );
}
