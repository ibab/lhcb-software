
//-----------------------------------------------------------------------------
/** @file RichPIDQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::PIDQC
 *
 *  CVS Log :-
 *  $Id: RichPIDQC.cpp,v 1.70 2008-05-06 15:35:33 jonrob Exp $
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
  : Rich::Rec::HistoAlgBase ( name, pSvcLocator ),
    m_pidTDS           ( LHCb::RichPIDLocation::Default ),
    m_trSelector       ( NULL ),
    m_mcTruth          ( NULL ),
    m_mcPselector      ( NULL ),
    m_requiredRads     ( Rich::NRadiatorTypes ),
    m_sF               ( "%7.3f" ),
    m_recoplots        ( NULL ),
    m_mcplots          ( NULL )
{

  if      ( context() == "Offline" )
  {
    m_pidTDS = LHCb::RichPIDLocation::Offline;
  }
  else if ( context() == "HLT" || context() == "Hlt" )
  {
    m_pidTDS = LHCb::RichPIDLocation::HLT;
  }

  // Declare job options
  declareProperty( "InputPIDs",   m_pidTDS );
  declareProperty( "MCTruth",     m_truth    = true );
  declareProperty( "MinimumTrackMultiplicity", m_minMultCut = 0 );
  declareProperty( "MaximumTrackMultiplicity", m_maxMultCut = 999999 );
  declareProperty( "HistoBins",     m_bins = 50 );
  declareProperty( "FinalPrintout", m_finalPrintOut = true );
  declareProperty( "IgnoreRecoThresholds", m_ignoreRecoThres = false );
  declareProperty( "IgnoreMCThresholds", m_ignoreMCThres     = false );
  declareProperty( "KaonDLLCut", m_dllKaonCut = 9999999 );
  declareProperty( "PionDLLCut", m_dllPionCut = 9999999 );
  m_requiredRads[Rich::Aerogel]  = false;
  m_requiredRads[Rich::Rich1Gas] = false;
  m_requiredRads[Rich::Rich2Gas] = false;
  declareProperty( "RequiredRads", m_requiredRads );
  declareProperty( "ApplyMCPSel", m_mcPsel = false );

}

// Destructor
PIDQC::~PIDQC() {}

// Initialisation
StatusCode PIDQC::initialize()
{
  // Initialize base class
  const StatusCode sc = Rich::Rec::HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  acquireTool( "TrackSelector", m_trSelector, this );

  // Retrieve MC tool, if needed
  if ( m_truth ) acquireTool( "RichRecMCTruthTool", m_mcTruth );

  // Retrieve the PID plotting tool
  acquireTool( "RichPIDPlots", "Reco", m_recoplots, this );
  acquireTool( "RichPIDPlots", "MC",   m_mcplots,   this );

  // Plots config
  m_plotsConfig.minP  = m_trSelector->minPCut()  * Gaudi::Units::GeV;
  m_plotsConfig.maxP  = m_trSelector->maxPCut()  * Gaudi::Units::GeV;
  m_plotsConfig.minPt = m_trSelector->minPtCut() * Gaudi::Units::GeV;
  m_plotsConfig.maxPt = m_trSelector->maxPtCut() * Gaudi::Units::GeV;

  // Initialise summary information
  for ( int i = 0; i<6; ++i ) { for ( int j = 0; j<6; ++j ) { m_sumTab[i][j] = 0; } }
  m_nEvents[0] = 0;
  m_nEvents[1] = 0;
  m_nTracks[0] = 0;
  m_nTracks[1] = 0;

  if ( m_mcPsel )
  {
    info() << "Will filter only selected MCParticles" << endreq;
    acquireTool( "MCParticleSelector", m_mcPselector, this );
  }

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
          return Error( exp.what(), StatusCode::SUCCESS );
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

      // Track selection
      if ( !selectTracks(track) ) continue;

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
          if ( msgLevel(MSG::DEBUG) )
          {
            debug() << "MCPID below threshold :-" << endreq;
            print ( debug(), iPID, pid, mcpid );
          }
          mcpid = Rich::BelowThreshold;
        }
      }

      // Check for threshold
      if ( !m_ignoreRecoThres && !iPID->isAboveThreshold(pid) )
      {
        //Warning( "Rec-PID "+Rich::text(pid)+" reassigned to BelowThreshold (MC-PID="+Rich::text(mcpid)+")",
        //        StatusCode::SUCCESS, 0 );
        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << "RecoPID below threshold :-" << endreq;
          print ( debug(), iPID, pid, mcpid );
        }
        pid = Rich::BelowThreshold;
      }

      // some verbose printout
      if ( msgLevel(MSG::VERBOSE) ) { print ( verbose(), iPID, pid, mcpid ); }

      // MC Truth
      if ( m_truth )
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "  MCID        = " << mcpid << endreq;

        // Fill plots in PID performance tool
        m_recoplots->plots( iPID, pid, m_plotsConfig );
        m_mcplots->plots( iPID, mcpid, m_plotsConfig );

        // Fill performance table
        plot2D( (int)pid, (int)mcpid, "pidTable", "PID Performance Table", -1.5, 6.5, -1.5, 6.5, 7, 7 );

        // Count track and PID types
        if ( Rich::Unknown != mcpid ) 
        {
          ++m_trackCount[tkType].second;
          ++m_pidPerTypeCount[iPID->pidType()].second;
        }

        // Fill performance tables
        if ( mcpid != Rich::Unknown &&
             pid   != Rich::Unknown ) { ++m_sumTab[mcpid][pid]; }

      }

    } // end PID loop
  } // end empty if

    // count events and tracks
  ++m_nEvents[0];
  if ( !m_richPIDs.empty() ) ++m_nEvents[1];
  m_nTracks[0] += m_totalSelTracks;
  m_nTracks[1] += pidCount;
  plot1D( pidCount, "# PIDs per event", -0.5, 200.5, 201 );
  plot1D( (m_richPIDs.empty() ? 0 : 1), "Event Success/Failures", -0.5, 1.5, 2 );
  if ( m_totalSelTracks>0 )
  {
    plot1D(  static_cast<double>(pidCount) / static_cast<double>(m_totalSelTracks),
             "Fraction of Tracks with PIDs", 0, 1, m_bins );
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
    for ( iRec = 0; iRec<6; ++iRec )
    {
      for ( iTrue = 0; iTrue<6; ++iTrue )
      {
        sumTot += m_sumTab[iTrue][iRec];;
        recTot[iRec] += m_sumTab[iTrue][iRec];
        trueTot[iTrue] += m_sumTab[iTrue][iRec];
        if (iRec<5) trueTotExcludeX[iTrue] += m_sumTab[iTrue][iRec];
      }
    }
    if ( sumTot > 0.5 )
    {
      for ( iRec = 0; iRec<6; ++iRec )
      {
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

    }
    else
    {
      warning() << "NO ENTRIES -> PID table skipped ..." << endreq;
    }

  } // final printout

    // finalize base class
  return Rich::Rec::HistoAlgBase::finalize();
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
    if ( !(*iTrk)->checkFlag(LHCb::Track::Clone ) ) { ++m_multiplicity; }
    if ( selectTracks(*iTrk) )
    {
      ++m_totalSelTracks;
    }
  }
}

bool PIDQC::selectTracks( const LHCb::Track * track )
{
  bool OK = false;
  if ( m_trSelector->trackSelected(track) )
  {
    OK = true;
    if ( m_mcPsel )
    {
      const LHCb::MCParticle * mcP = m_mcTruth->mcParticle(track);
      OK = mcPselector()->accept(mcP);
    }
  }
  return OK;
}

void PIDQC::print( MsgStream & msg,
                   LHCb::RichPID * iPID,
                   const Rich::ParticleIDType pid,
                   const Rich::ParticleIDType mcpid  ) const
{
  // Track type
  const Rich::Rec::Track::Type tkType = Rich::Rec::Track::type(iPID->track());

  // Track momentum in GeV/C
  const double tkPtot = trackP( iPID );

  msg << "RichPID " << iPID->key() << " ("
      << iPID->pidType() << "), '"
      << tkType << "' track, Ptot " << boost::format(m_sF)%tkPtot << " GeV/c," << endreq
      << "  Active rads =";
  if ( iPID->usedAerogel()  ) { msg << " " << Rich::Aerogel;  }
  if ( iPID->usedRich1Gas() ) { msg << " " << Rich::Rich1Gas; }
  if ( iPID->usedRich2Gas() ) { msg << " " << Rich::Rich2Gas; }
  msg << endreq
      << "  Threshold   = ";
  {for ( int ipid = 0; ipid < Rich::NParticleTypes; ++ipid ) {
    const Rich::ParticleIDType pid = static_cast<Rich::ParticleIDType>(ipid);
    const std::string T = iPID->isAboveThreshold(pid) ? "T" : "F";
    msg << T << " ";
  }}
  msg << endreq
      << "  Dlls        = "
      << boost::format(m_sF)%(iPID->particleDeltaLL(Rich::Electron)) << " "
      << boost::format(m_sF)%(iPID->particleDeltaLL(Rich::Muon)) << " "
      << boost::format(m_sF)%(iPID->particleDeltaLL(Rich::Pion)) << " "
      << boost::format(m_sF)%(iPID->particleDeltaLL(Rich::Kaon)) << " "
      << boost::format(m_sF)%(iPID->particleDeltaLL(Rich::Proton))
      << endreq
      << "  Prob(r/n)   = ";
  {for ( int ipid = 0; ipid < Rich::NParticleTypes; ++ipid ) {
    const Rich::ParticleIDType pid = static_cast<Rich::ParticleIDType>(ipid);
    msg << boost::format(m_sF)%(iPID->particleRawProb(pid))
        << "/" << boost::format(m_sF)%(iPID->particleNormProb(pid)) << " ";
  }}
  msg << endreq
      << "  RecoPID     = " << pid << endreq;
  msg << "  MCID        = " << mcpid << endreq;
}
