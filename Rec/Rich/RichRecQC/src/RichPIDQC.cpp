// $Id: RichPIDQC.cpp,v 1.10 2003-07-08 06:19:36 cattanem Exp $
// Include files

// local
#include "RichPIDQC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPIDQC
//
// 2002-06-13 : Chris Jones   (Christopher.Rob.Jones@cern.ch)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPIDQC>          s_factory ;
const        IAlgFactory& RichPIDQCFactory = s_factory ;

// Standard constructor, initializes variables
RichPIDQC::RichPIDQC( const std::string& name,
                      ISvcLocator* pSvcLocator )
  : Algorithm ( name, pSvcLocator ) {

  // Declare job options
  declareProperty( "TrackAsctName", m_tkMCTruthName = "TrackToMCP" );
  declareProperty( "TrackAsctType", m_tkMCTruthType =
                   "AssociatorWeighted<TrStoredTrack,MCParticle,double>" );
  declareProperty( "InputPIDs", m_pidTDS = RichPIDLocation::Default );
  declareProperty( "InputTracks", m_trackTDS = TrStoredTrackLocation::Default );
  declareProperty( "MCHistoPath", m_mcHstPth = "RICH/PIDQC/MC/" );
  declareProperty( "HistoPath", m_hstPth = "RICH/PIDQC/" );
  declareProperty( "MinPCut", m_pMinCut = 2.0 );
  declareProperty( "MaxPCut", m_pMaxCut = 100.0 );
  declareProperty( "MCTruth", m_truth = true );
  m_trNames.push_back( "unique" );
  m_trNames.push_back( "seed" );
  m_trNames.push_back( "match" );
  m_trNames.push_back( "forward" );
  m_trNames.push_back( "upstream" );
  m_trNames.push_back( "veloTT" );
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
  declareProperty( "MinimumTrackMultiplicity", m_minMultCut = 1 );
  declareProperty( "MaximumTrackMultiplicity", m_maxMultCut = 999999 );
  declareProperty( "HistoBins", m_bins = 50 );
  declareProperty( "FinalPrintout", m_finalPrintOut = true );
  declareProperty( "ExtraHistos", m_extraHistos = false );

}

// Destructor
RichPIDQC::~RichPIDQC() {};

// Initialisation
StatusCode RichPIDQC::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Check momentum cuts make sense
  if ( m_pMinCut >= m_pMaxCut ) {
    msg << MSG::ERROR
        << "Invalid Min/Max momentum cuts "
        << m_pMinCut << "/" << m_pMaxCut << endreq;
    return StatusCode::FAILURE;
  }

  if ( m_truth ) {

    // Retrieve tracking association tool
    if ( !toolSvc()->retrieveTool( m_tkMCTruthType, m_tkMCTruthName,
                                   m_trackToMCP ) ) {
      msg << MSG::DEBUG << " Tracking MCTruth not available" << endreq;
      m_truth = false;
    } else {
      msg << MSG::DEBUG << " Successfully retrieved " << m_tkMCTruthType
          << " as " << m_tkMCTruthName << endreq;
    }

    // Retrieve particle property service
    if ( !service( "ParticlePropertySvc", m_ppSvc) ) {
      msg << MSG::WARNING << "Unable to retrieve ParticlePropertySvc" << endreq;
      return StatusCode::FAILURE;
    }

    // Setup the PDG code mappings
    m_localID[ 0 ] = 0;
    m_localID[ abs(m_ppSvc->find("e+")->jetsetID()) ]  = 0;
    m_localID[ abs(m_ppSvc->find("mu+")->jetsetID()) ] = 1;
    m_localID[ abs(m_ppSvc->find("pi+")->jetsetID()) ] = 2;
    m_localID[ abs(m_ppSvc->find("K+")->jetsetID()) ]  = 3;
    m_localID[ abs(m_ppSvc->find("p+")->jetsetID()) ]  = 4;

    m_ppSvc->release();
  }

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;
  if ( m_truth && !bookMCHistograms() ) return StatusCode::FAILURE;

  // Initialise summary information
  for ( int i = 0; i<6; i++ ) {
    for ( int j = 0; j<6; j++ ) { m_sumTab[i][j] = 0; }
  }
  m_nEvents[0] = 0;
  m_nEvents[1] = 0;
  m_nTracks[0] = 0;
  m_nTracks[1] = 0;

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;

  msg << MSG::DEBUG
      << " Track types selected   = " << m_trSelector.selectedTrackTypes() << endreq
      << " RichPIDs location      = " << m_pidTDS << endreq
      << " Histogram location     = " << m_hstPth << endreq;
  if ( m_truth ) {
    msg << " MC Histogram location  = " << m_mcHstPth << endreq;
  }

  return StatusCode::SUCCESS;
};

StatusCode RichPIDQC::bookHistograms() {

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

    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; iHypo++ ) {
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

StatusCode RichPIDQC::bookMCHistograms() {

  std::string title;
  HYPOTHESIS_NAMES;

  title = "PID Performance Table";
  m_perfTable = histoSvc()->book( m_mcHstPth, 1, title,
                                  6, 0.5, 6.5, 6, 0.5, 6.5 );

  for ( int iHypo = 0; iHypo < Rich::NParticleTypes; iHypo++ ) {
    title = "True " + hypothesis[iHypo] + " Ptot : All";
    m_trueSpec[iHypo] = histoSvc()->book( m_mcHstPth, 10*(1+iHypo)+1, title,
                                          m_bins, m_pMinCut, m_pMaxCut );
    title = "True " + hypothesis[iHypo] + " Ptot : ID";
    m_trueIDSpec[iHypo] = histoSvc()->book( m_mcHstPth, 10*(1+iHypo)+2, title,
                                            m_bins, m_pMinCut, m_pMaxCut );
    title = "True " + hypothesis[iHypo] + " Ptot : misID";
    m_trueMisIDSpec[iHypo] = histoSvc()->book( m_mcHstPth, 10*(1+iHypo)+3, title,
                                               m_bins, m_pMinCut, m_pMaxCut );
    title = "Fake " + hypothesis[iHypo] + " Ptot : ID";
    m_fakeIDSpec[iHypo] = histoSvc()->book( m_mcHstPth, 10*(1+iHypo)+4, title,
                                            m_bins, m_pMinCut, m_pMaxCut );
  }

  title = "True pion Ptot : ID (light)";
  m_lightIdSpec = histoSvc()->book( m_mcHstPth, 61, title,
                                    m_bins, m_pMinCut, m_pMaxCut );
  title = "True pion Ptot : misID (light)";
  m_lightMisIdSpec = histoSvc()->book( m_mcHstPth, 62, title,
                                       m_bins, m_pMinCut, m_pMaxCut );
  title = "True kaon Ptot : ID (heavy)";
  m_heavyIdSpec = histoSvc()->book( m_mcHstPth, 63, title,
                                    m_bins, m_pMinCut, m_pMaxCut );
  title = "True kaon Ptot : misID (heavy)";
  m_heavyMisIdSpec = histoSvc()->book( m_mcHstPth, 64, title,
                                       m_bins, m_pMinCut, m_pMaxCut );

  if ( m_extraHistos ) {

    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; iHypo++ ) {
      title =  hypothesis[iHypo] + " true ID : delta LogLikelihood";
      m_deltaLLTrue[iHypo] = histoSvc()->book( m_mcHstPth, 70 + iHypo+1, title,
                                               m_bins, 0.0, 150.0 );
      title =  hypothesis[iHypo] + " fake ID : delta LogLikelihood";
      m_deltaLLFake[iHypo] = histoSvc()->book( m_mcHstPth, 80 + iHypo+1, title,
                                               m_bins, 0.0, 150.0 );
    } // end hypo loop

    title = "dLL(pi-el) True El";
    m_dLLPiElTrueEl = histoSvc()->book( m_mcHstPth, 91, title, m_bins, -100, 100 );
    title = "dLL(pi-el) Fake El";
    m_dLLPiElFakeEl = histoSvc()->book( m_mcHstPth, 92, title, m_bins, -100, 100 );
    title = "dLL(pi-mu) True Mu";
    m_dLLPiMuTrueMu = histoSvc()->book( m_mcHstPth, 101, title, m_bins, -100, 100 );
    title = "dLL(pi-mu) Fake Mu";
    m_dLLPiMuFakeMu = histoSvc()->book( m_mcHstPth, 102, title, m_bins, -100, 100 );
    title = "dLL(pi-K) True K/Pr";
    m_dLLPiKaTrueKaPr = histoSvc()->book( m_mcHstPth, 111, title, m_bins, -100, 100 );
    title = "dLL(pi-K) Fake K/Pr";
    m_dLLPiKaFakeKaPr = histoSvc()->book( m_mcHstPth, 112, title, m_bins, -100, 100 );
    title = "dLL(K-pr) True K";
    m_dLLKaPrTrueKa = histoSvc()->book( m_mcHstPth, 121, title, m_bins, -100, 100 );
    title = "dLL(pi-K) True Pr";
    m_dLLKaPrTruePr = histoSvc()->book( m_mcHstPth, 122, title, m_bins, -100, 100 );

  } // extra histos if

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichPIDQC::execute() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Load data
  if ( !loadPIDData()   ) return StatusCode::SUCCESS;
  if ( !loadTrackData() ) return StatusCode::SUCCESS;

  // Initial loop over tracks
  int multiplicity(0), totalSelTracks(0);
  for ( TrStoredTracks::const_iterator iTrk = m_tracks->begin();
        iTrk != m_tracks->end(); ++iTrk ) {
    if ( (*iTrk)->unique() ) ++multiplicity;
    if ( !m_trSelector.trackSelected( *iTrk ) ) continue;
    TrStateP* pState = getTrStateP( *iTrk );
    double tkPtot = ( pState ? pState->p()/GeV : 0 );
    if ( tkPtot > m_pMaxCut || tkPtot < m_pMinCut ) continue;
    ++totalSelTracks;
  }

  // apply track multiplicity cuts
  if ( multiplicity < m_pMinCut ||
       multiplicity > m_maxMultCut ) return StatusCode::SUCCESS;

  // count pids per track type
  int pidCount = 0;

  // Loop over all PID results
  if ( !m_richPIDs.empty() ) {
    for ( std::vector<ContainedObject*>::const_iterator iC = m_richPIDs.begin();
          iC != m_richPIDs.end(); ++iC ) {
      RichPID * iPID = dynamic_cast<RichPID*>(*iC);

      // Get TrStoredTrack SmartRef and perform track selection
      TrStoredTrack * trTrack = iPID->recTrack();
      if ( !trTrack ) {
        msg << MSG::WARNING
            << "Null TrStoredTrack reference for PID " << iPID->key() << endreq;
        continue;
      }

      // Track selection 
      if ( !m_trSelector.trackSelected( trTrack ) ) continue;

      // Track momentum in GeV/C
      TrStateP* pState = getTrStateP( trTrack );
      double tkPtot = ( pState ? pState->p()/GeV : 0 );
      if ( tkPtot > m_pMaxCut || tkPtot < m_pMinCut ) continue;

      // Count PIDS
      ++pidCount;

      // Get best PID
      Rich::ParticleIDType pid = iPID->bestParticleID();
      if ( !iPID->isAboveThreshold(pid) ) { pid = Rich::BelowThreshold; }
      msg << MSG::DEBUG << "PID " << iPID->key() << " ("
          << iPID->pidType() << "), Track " << trTrack->key()
          << " (" << trTrack->history()<< ") " << tkPtot << " GeV/c, "
          << "Dlls " << iPID->particleLLValues() << "PID '" << pid << "'";

      // Fill histos
      m_ids->fill( pid+1, 1. );
      if ( m_extraHistos ) {
        for ( int iHypo = 0; iHypo < Rich::NParticleTypes; iHypo++ ) {
          m_pRaw[iHypo]->fill( iPID->particleRawProb((Rich::ParticleIDType)iHypo), 1 );
          m_pNorm[iHypo]->fill( iPID->particleNormProb((Rich::ParticleIDType)iHypo), 1 );
          m_deltaLL[iHypo]->fill( iPID->particleDeltaLL((Rich::ParticleIDType)iHypo), 1 );
        }
      }

      // MC Truth
      if ( m_truth && (0 != trTrack) ) {

        MCParticle* mcPart = m_trackToMCP->associatedFrom( trTrack );
        Rich::ParticleIDType mcpid = Rich::Unknown;
        if ( mcPart ) {
          mcpid = (Rich::ParticleIDType)m_localID[abs(mcPart->particleID().pid())];
          if ( !iPID->isAboveThreshold( mcpid ) ) mcpid = Rich::BelowThreshold;
        }
        msg << ", MCID '" << mcpid << "'" << endreq;

        // Fill performance tables
        m_perfTable->fill( mcpid+1, pid+1, 1. );
        if ( mcpid>=0 && pid>=0 ) { m_sumTab[mcpid][pid] += 1; }

        if ( m_extraHistos ) {

          // MC delta LLs
          for ( int iHypo = 0; iHypo < Rich::NParticleTypes; iHypo++ ) {
            if ( iHypo == mcpid ) {
              m_deltaLLTrue[iHypo]->fill( iPID->particleDeltaLL((Rich::ParticleIDType)iHypo), 1 );
            } else {
              m_deltaLLFake[iHypo]->fill( iPID->particleDeltaLL((Rich::ParticleIDType)iHypo), 1 );
            }
          }

          double dllPiEl =  ( iPID->particleDeltaLL(Rich::Pion) -
                              iPID->particleDeltaLL(Rich::Electron) );
          if ( Rich::Electron == mcpid ) {
            m_dLLPiElTrueEl->fill( dllPiEl, 1 );
          } else {
            m_dLLPiElFakeEl->fill( dllPiEl, 1 );
          }
          double dllPiMu = ( iPID->particleDeltaLL(Rich::Pion) -
                             iPID->particleDeltaLL(Rich::Muon) );
          if ( Rich::Muon == mcpid ) {
            m_dLLPiMuTrueMu->fill( dllPiMu, 1 );
          } else {
            m_dLLPiMuFakeMu->fill( dllPiMu, 1 );
          }
          double dllpik = ( iPID->particleDeltaLL(Rich::Pion) -
                            iPID->particleDeltaLL(Rich::Kaon) );
          if ( Rich::Kaon == mcpid || Rich::Proton == mcpid ) {
            m_dLLPiKaTrueKaPr->fill( dllpik, 1 );
          } else {
            m_dLLPiKaFakeKaPr->fill( dllpik, 1 );
          }
          double dllkpr = ( iPID->particleDeltaLL(Rich::Kaon) -
                            iPID->particleDeltaLL(Rich::Proton) );
          if ( Rich::Kaon == mcpid ) {
            m_dLLKaPrTrueKa->fill( dllkpr, 1 );
          } else if ( Rich::Proton == mcpid ) {
            m_dLLKaPrTruePr->fill( dllkpr, 1 );
          }
        } // extra histos

        // Pion and Kaon ID momentum spectra
        if ( iPID->isAboveThreshold(mcpid) &&
             mcpid>=0 && pid>=0 && mcpid != Rich::BelowThreshold ) {

          m_trueSpec[mcpid]->fill( tkPtot, 1 ); // True Ptot spectrum
          if ( mcpid == pid ) {
            m_trueIDSpec[mcpid]->fill( tkPtot, 1 ); // True Ptot spectrum : correct ID
          } else {
            m_trueMisIDSpec[mcpid]->fill( tkPtot, 1 ); // True Ptot spectrum : mis ID
            if ( pid != Rich::BelowThreshold ) {
              m_fakeIDSpec[pid]->fill( tkPtot, 1 ); // Fake Ptot spectrum : mis ID
            }
          }

          // light ID for pions
          if ( mcpid == Rich::Pion ) {
            if ( pid == Rich::Pion || pid == Rich::Electron || pid == Rich::Muon ) {
              m_lightIdSpec->fill( tkPtot, 1. );
            } else {
              m_lightMisIdSpec->fill( tkPtot, 1. );
            }
          }

          // heavy ID for kaons
          if ( mcpid == Rich::Kaon ) {
            if ( pid == Rich::Kaon || pid == Rich::Proton ) {
              m_heavyIdSpec->fill( tkPtot, 1. );
            } else {
              m_heavyMisIdSpec->fill( tkPtot, 1. );
            }
          }

        } // threshold if

      } else {
        msg << endreq;
      }

    } // end PID loop
  } // end empty if

  // count events and tracks
  ++m_nEvents[0];
  if ( !m_richPIDs.empty() ) ++m_nEvents[1];
  m_nTracks[0] += totalSelTracks;
  m_nTracks[1] += pidCount;
  m_Nids->fill( pidCount, 1 );
  m_eventRate->fill( (m_richPIDs.empty() ? 0 : 1), 1 );
  if ( totalSelTracks>0 ) {
    m_pidRate->fill( (double)pidCount/(double)totalSelTracks, 1 );
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichPIDQC::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  if ( m_truth && m_finalPrintOut ) {

    // index variables
    int iRec, iTrue;

    // Set printout level for following messages
    msg << MSG::INFO;

    // compute efficiencies and purities
    double sumTot = 0;
    double recTot[]          = {0,0,0,0,0,0};
    double trueTot[]         = {0,0,0,0,0,0};
    double trueTotExcludeX[] = {0,0,0,0,0,0};
    double eff[]             = {0,0,0,0,0,0};
    double purity[]          = {0,0,0,0,0,0};
    for ( iRec = 0; iRec<6; iRec++ ) {
      for ( iTrue = 0; iTrue<6; iTrue++ ) {
        sumTot += m_sumTab[iTrue][iRec];;
        recTot[iRec] += m_sumTab[iTrue][iRec];
        trueTot[iTrue] += m_sumTab[iTrue][iRec];
        if (iRec<5) trueTotExcludeX[iTrue] += m_sumTab[iTrue][iRec];
      }
    }
    if ( sumTot < 1 ) sumTot = 1;
    for ( iRec = 0; iRec<6; iRec++ ) {
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
    for ( iTrue = 0; iTrue<6; iTrue++ ) {
      for ( iRec = 0; iRec<6; iRec++ ) {
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

    msg << "-----------+-----------------------------------------------+-----------"
        << endreq << "Tracks     | " << m_pMinCut << "-" << m_pMaxCut << " GeV/c : " 
        << m_trNames << endreq
        << "-----------+-----------------------------------------------+-----------"
        << endreq
        << "  %total   | Electron Muon   Pion   Kaon  Proton   X  (MC) |  %Purity"
        << endreq
        << "-----------+-----------------------------------------------+-----------"
        << endreq
        << "           |                                               |" << endreq;
    std::string type[6] = { " Electron  |", " Muon      |", " Pion      |",
                            " Kaon      |", " Proton    |", " X         |" };
    for ( iRec = 0; iRec < 6; iRec++ ) {
      msg << type[iRec] << format( "%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f     |%7.2f",
                                   m_sumTab[0][iRec], m_sumTab[1][iRec],
                                   m_sumTab[2][iRec], m_sumTab[3][iRec],
                                   m_sumTab[4][iRec], m_sumTab[5][iRec],
                                   purity[iRec] ) << endreq;
    }
    msg << "  (reco)   |                                               |" << endreq
        << "-----------+-----------------------------------------------+-----------"
        << endreq
        << " %Eff.     |" << format( "%7.2f%7.2f%7.2f%7.2f%7.2f%7.2f",
                                     eff[0],eff[1],eff[2],eff[3],eff[4],eff[5] )
        << "     |" << endreq
        << "-----------+-----------------------------------------------+-----------" << endreq;
    msg << format( "  %ID      |    Ka: %6.2f+-%6.2f    Pi: %6.2f+-%6.2f ",
                   kaonIDEff[0], kaonIDEff[1], piIDEff[0], piIDEff[1] ) << endreq;
    msg << format( "  %MisID   |    Ka: %6.2f+-%6.2f    Pi: %6.2f+-%6.2f ",
                   kaonMisIDEff[0], kaonMisIDEff[1], piMisIDEff[0], piMisIDEff[1] ) << endreq;
    msg << "PID rate   |    events " << evPIDRate[0] << " +- " << evPIDRate[1]
        << "%,  tracks " << trPIDRate[0] << " +- " << trPIDRate[1] << "%" << endreq
        << "-----------+-----------------------------------------------+-----------"
        << endreq;

  } // final printout

  // release tools
  if ( m_trackToMCP ) { toolSvc()->releaseTool( m_trackToMCP ); m_trackToMCP=0; }

  return sc;
}

StatusCode RichPIDQC::loadPIDData() {

  // Load PIDs
  DataObject *pObject;
  if ( eventSvc()->retrieveObject( m_pidTDS, pObject ) ) {
    if ( KeyedContainer<RichPID, Containers::HashMap> * pids =
         static_cast<KeyedContainer<RichPID, Containers::HashMap>*> (pObject) ) {
      m_richPIDs.erase(m_richPIDs.begin(), m_richPIDs.end());
      pids->containedObjects( m_richPIDs );
      return StatusCode::SUCCESS;
    }
  }

  // If we get here, things went wrong
  MsgStream msg( msgSvc(), name() );
  msg << MSG::WARNING << "Failed to located RichPIDs at " << m_pidTDS << endreq;
  return StatusCode::FAILURE;
}

StatusCode RichPIDQC::loadTrackData() {

  // Locate tracks
  SmartDataPtr<TrStoredTracks> tracks( eventSvc(), m_trackTDS );
  if ( !tracks ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Cannot locate TrStoredTracks at "
        << m_trackTDS << endreq;
    return StatusCode::FAILURE;
  }
  m_tracks = tracks;

  return StatusCode::SUCCESS;
}

TrStateP * RichPIDQC::getTrStateP( const TrStoredTrack * track,
                                   const double zPos ) {

  TrStateP* pState = 0;
  if ( track ) {
    SmartRef<TrState> trState = track->closestState( zPos );
    pState = dynamic_cast<TrStateP*>( (TrState*)trState );
  }

  return pState;
}
