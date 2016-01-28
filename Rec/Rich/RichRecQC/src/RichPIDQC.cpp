
//-----------------------------------------------------------------------------
/** @file RichPIDQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm :
 *    Rich::Rec::MC::PIDQC
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-13
 */
//-----------------------------------------------------------------------------
// local
#include "RichPIDQC.h"

// namespaces
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PIDQC )

// Standard constructor, initializes variables
  PIDQC::PIDQC( const std::string& name,
                ISvcLocator* pSvcLocator )
    : Rich::Rec::HistoAlgBase ( name, pSvcLocator ),
      m_pidTDS           ( "" ),
      m_requiredRads     ( Rich::NRadiatorTypes ),
      m_sF               ( "%7.3f" )
{

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
  declareProperty( "ExpertPlots", m_expertPlots = false );

  // Initialise summary information
  for ( int i = 0; i<Rich::NParticleTypes; ++i )
  {
    for ( int j = 0; j<Rich::NParticleTypes; ++j ) { m_sumTab[i][j] = 0; }
  }
  m_nEvents[0] = 0;
  m_nEvents[1] = 0;
  m_nTracks[0] = 0;
  m_nTracks[1] = 0;

}

// Destructor
PIDQC::~PIDQC() {}

// Initialisation
StatusCode PIDQC::initialize()
{
  // Initialize base class
  const StatusCode sc = Rich::Rec::HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( m_pidTDS.empty() )
  {
    return Error( "RichPID location is not set" );
  }

  acquireTool( "TrackSelector", m_trSelector, this );
  acquireTool( "RichPIDPlots", "MCAll", m_allPlotTool, this );

  // Retrieve MC tool, if needed
  if ( m_truth ) acquireTool( "RichRecMCTruthTool", m_mcTruth );

  // Plots config
  m_plotTools.clear();
  m_plotsConfig.minP  = m_trSelector->minPCut()  * Gaudi::Units::GeV;
  m_plotsConfig.maxP  = m_trSelector->maxPCut()  * Gaudi::Units::GeV;
  m_plotsConfig.minPt = m_trSelector->minPtCut() * Gaudi::Units::GeV;
  m_plotsConfig.maxPt = m_trSelector->maxPtCut() * Gaudi::Units::GeV;
  m_plotsConfig.expertPlots = m_expertPlots;

  if ( m_mcPsel )
  {
    info() << "Will filter only selected MCParticles" << endmsg;
    acquireTool( "MCParticleSelector", m_mcPselector, this );
  }

  // Warn if ignoring threshold information
  //if ( m_ignoreRecoThres ) Warning( "Ignoring reco threshold information",
  //                                  StatusCode::SUCCESS ).ignore();
  //if ( m_ignoreMCThres )   Warning( "Ignoring MC threshold information",
  //                                  StatusCode::SUCCESS ).ignore();

  // Warn if using kaon DLL cut
  //if ( m_dllKaonCut < 9999991 )
  //  Warning( "Applying kaon selection dll(kaon) > " +
  //          boost::lexical_cast<std::string>(m_dllKaonCut),
  //          StatusCode::SUCCESS ).ignore();

  // Warn if using pion DLL cut
  //if ( m_dllPionCut < 9999991 )
  //  Warning( "Applying pion selection dll(pion) > " +
  //           boost::lexical_cast<std::string>(m_dllPionCut),
  //           StatusCode::SUCCESS ).ignore();

  return sc;
}

// Main execution
StatusCode PIDQC::execute()
{

  // Load RichPID data
  if ( !loadPIDData() || m_richPIDs.empty() ) return StatusCode::SUCCESS;

  // Count tracks
  TkTally tkTally;
  Rich::Map<std::string,bool> locs;
  for ( const auto * C : m_richPIDs )
  {
    // create pointer to pid object
    const auto * fPID = dynamic_cast<const LHCb::RichPID*>(C);
    if ( !fPID ) { Warning("Dynamic Cast to RichPID* failed").ignore(); continue; }

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
          tkTally += countTracks(contLoc);
        }
        catch ( const GaudiException & exp )
        {
          return Error( exp.what(), StatusCode::SUCCESS );
        }
      }

    } // track OK
    else
    {
      Warning( "RichPID has null Track SmartRef", StatusCode::SUCCESS ).ignore();
    }

  }

  // apply track multiplicity cuts
  if ( tkTally.multiplicity < m_minMultCut ||
       tkTally.multiplicity > m_maxMultCut ) return StatusCode::SUCCESS;

  // count pids per track type
  int pidCount = 0;

  // Loop over all PID results
  for ( const auto * C : m_richPIDs )
  {
    const auto * iPID = dynamic_cast<const LHCb::RichPID*>(C);
    if ( !iPID ) { Warning("Dynamic Cast to RichPID* failed").ignore(); continue; }

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
      if ( !m_ignoreMCThres && mcpid == Rich::Unknown )
      {
        _ri_debug << "Track has no MC -> Ghost therefore below threshold :-" << endmsg;
        mcpid = Rich::BelowThreshold;
      }
      if ( mcpid != Rich::Unknown &&
           !m_ignoreMCThres &&
           !iPID->isAboveThreshold(mcpid) )
      {
        //Warning( "MC-PID "+Rich::text(mcpid)+" reassigned to BelowThreshold", StatusCode::SUCCESS, 0 );
        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << "MCPID below threshold :-" << endmsg;
          //print ( debug(), iPID, pid, mcpid );
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
        debug() << "RecoPID below threshold :-" << endmsg;
        //print ( debug(), iPID, pid, mcpid );
      }
      pid = Rich::BelowThreshold;
    }

    // some verbose printout
    if ( msgLevel(MSG::VERBOSE) ) { print ( verbose(), iPID, pid, mcpid ); }
    //print ( info(), iPID, pid, mcpid );

    // MC Truth
    if ( m_truth )
    {
      // Fill plots in PID performance tool
      if ( mcpid != Rich::Unknown &&
           pid   != Rich::Unknown )
        plotsTool(mcpid)->plots( iPID, pid, m_plotsConfig );

      // Count track and PID types
      if ( Rich::Unknown != mcpid )
      {
        ++m_trackCount[tkType].second;
        ++m_pidPerTypeCount[iPID->pidType()].second;
      }

      // Fill performance tables
      plot2D( (int)pid, (int)mcpid,
              "pidTable", "PID Performance Table", 
              -1.5, Rich::NParticleTypes+0.5, 
              -1.5, Rich::NParticleTypes+0.5, 
              Rich::NParticleTypes+1, Rich::NParticleTypes+1 );
      if ( mcpid != Rich::Unknown &&
           pid   != Rich::Unknown ) { ++m_sumTab[mcpid][pid]; }

    }

    // MC free plots
    m_allPlotTool->plots( iPID, pid, m_plotsConfig );

  } // end PID loop

  // count events and tracks
  ++m_nEvents[0];
  if ( !m_richPIDs.empty() ) ++m_nEvents[1];
  m_nTracks[0] += tkTally.nTracks;
  m_nTracks[1] += pidCount;
  plot1D( pidCount, "# PIDs per event", -0.5, 200.5, 201 );
  plot1D( (m_richPIDs.empty() ? 0 : 1), "Event Success V Failures", -0.5, 1.5, 2 );
  if ( tkTally.nTracks>0 )
  {
    plot1D(  static_cast<double>(pidCount) / static_cast<double>(tkTally.nTracks),
             "Fraction of Tracks with PIDs", 0, 1, m_bins );
  }

  _ri_debug << "Total Tracks = " << tkTally.nTracks << " : tracks PIDed = " << pidCount << endmsg;

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode PIDQC::finalize()
{
  if ( m_truth && m_finalPrintOut )
  {

    // compute efficiencies and purities
    double sumTot = 0;
    std::array<double,Rich::NParticleTypes> recTot, trueTot, trueTotExcludeX, eff, purity;
    recTot.fill(0);
    trueTot.fill(0);
    trueTotExcludeX.fill(0);
    eff.fill(0);
    purity.fill(0);
    for ( int iRec = 0; iRec<Rich::NParticleTypes; ++iRec )
    {
      for ( int iTrue = 0; iTrue<Rich::NParticleTypes; ++iTrue )
      {
        sumTot         += m_sumTab[iTrue][iRec];
        recTot[iRec]   += m_sumTab[iTrue][iRec];
        trueTot[iTrue] += m_sumTab[iTrue][iRec];
        if (iRec<Rich::NParticleTypes-1) trueTotExcludeX[iTrue] += m_sumTab[iTrue][iRec];
      }
    }
    if ( sumTot > 0.5 )
    {
      for ( int iRec = 0; iRec<Rich::NParticleTypes; ++iRec )
      {
        eff[iRec]    = ( trueTot[iRec]>0 ? 100*m_sumTab[iRec][iRec]/trueTot[iRec] : 0 );
        purity[iRec] = ( recTot[iRec]>0  ? 100*m_sumTab[iRec][iRec]/recTot[iRec]  : 0 );
      }

      // Kaon / Pion seperation
      const double truePi = ( m_ignoreRecoThres ? trueTot[Rich::Pion] : trueTotExcludeX[Rich::Pion] );
      const double trueKa = ( m_ignoreRecoThres ? trueTot[Rich::Kaon] : trueTotExcludeX[Rich::Kaon] );
      double kaonIDEff[2];
      kaonIDEff[0] = ( trueKa>0 ?
                       100*( m_sumTab[Rich::Kaon][Rich::Kaon] +
                             m_sumTab[Rich::Kaon][Rich::Proton] ) /
                       trueKa : 0 );
      kaonIDEff[1] = ( trueKa>0 ?
                       std::sqrt( kaonIDEff[0]*(100.-kaonIDEff[0])/
                                  trueKa ) : 0 );
      double kaonMisIDEff[2];
      kaonMisIDEff[0] = ( trueKa>0 ?
                          100*( m_sumTab[Rich::Kaon][Rich::Electron] +
                                m_sumTab[Rich::Kaon][Rich::Muon] +
                                m_sumTab[Rich::Kaon][Rich::Pion] ) /
                          trueKa : 0 );
      kaonMisIDEff[1] = ( trueKa>0 ?
                          std::sqrt( kaonMisIDEff[0]*(100.-kaonMisIDEff[0])/
                                     trueKa ) : 0 );
      double piIDEff[2];
      piIDEff[0] = ( truePi>0 ?
                     100*( m_sumTab[Rich::Pion][Rich::Electron] +
                           m_sumTab[Rich::Pion][Rich::Muon] +
                           m_sumTab[Rich::Pion][Rich::Pion] ) /
                     truePi : 0 );
      piIDEff[1] =  ( truePi>0 ?
                      std::sqrt( piIDEff[0]*(100.-piIDEff[0])/
                                 truePi ) : 0 );
      double piMisIDEff[2];
      piMisIDEff[0] = ( truePi>0 ?
                        100*( m_sumTab[Rich::Pion][Rich::Kaon] +
                              m_sumTab[Rich::Pion][Rich::Proton] ) /
                        truePi : 0 );
      piMisIDEff[1] = ( truePi>0 ?
                        std::sqrt( piMisIDEff[0]*(100.-piMisIDEff[0])/
                                   truePi ) : 0 );

      // Scale entries to percent of total number of entries
      for ( int iTrue = 0; iTrue<Rich::NParticleTypes; ++iTrue )
      {
        for ( int iRec = 0; iRec<Rich::NParticleTypes; ++iRec )
        {
          m_sumTab[iTrue][iRec] = 100.0*m_sumTab[iTrue][iRec]/sumTot;
        }
      }

      // compute event and track PID success rates
      double evPIDRate[2];
      double trPIDRate[2];
      evPIDRate[0] = ( m_nEvents[0]>0 ? 100.*m_nEvents[1]/m_nEvents[0] : 100 );
      evPIDRate[1] = ( m_nEvents[0]>0 ? std::sqrt(evPIDRate[0]*(100.-evPIDRate[0])/m_nEvents[0]) : 100 );
      trPIDRate[0] = ( m_nTracks[0]>0 ? 100.*m_nTracks[1]/m_nTracks[0] : 100 );
      trPIDRate[1] = ( m_nTracks[0]>0 ? std::sqrt(trPIDRate[0]*(100.-trPIDRate[0])/m_nTracks[0]) : 100 );

      info() << "-------------+---------------------------------------------------------------+------------"
             << endmsg
             << " Ptot Sel    | " << m_trSelector->minPCut() << "-" << m_trSelector->maxPCut() << " GeV/c" << endmsg
             << " TkMult Sel  | " << m_minMultCut << "-" << m_maxMultCut << " tracks/event" << endmsg;
      info() << " #Tks(+MC)   |";
      unsigned int tkCount = 0;
      for ( TkCount::const_iterator iTk = m_trackCount.begin();
            iTk != m_trackCount.end(); ++iTk, ++tkCount ) 
      {
        if ( tkCount == 3 ) { tkCount = 0; info() << endmsg << "             |"; }
        info() << " " << (*iTk).first << "=" << (*iTk).second.first
               << "(" << (*iTk).second.second << ")";
      }
      tkCount = 0;
      info() << endmsg
             << " Using PIDs  | " << m_pidTDS << endmsg
             << " #PIDs(+MC)  |";
      for ( PIDsByType::const_iterator iPC = m_pidPerTypeCount.begin();
            iPC != m_pidPerTypeCount.end(); ++iPC, ++tkCount ) 
      {
        if ( tkCount == 3 ) { tkCount = 0; info() << endmsg << "             |"; }
        info() << " " << (*iPC).first << "=" << (*iPC).second.first
               << "(" << (*iPC).second.second << ")";
      }
      info() << endmsg;
      if ( m_dllKaonCut < 9999991 )
      {
        info() << " Tagging tracks as kaons if kaon DLL > " << m_dllKaonCut << endmsg;
      }
      if ( m_dllPionCut < 9999991 )
      {
        info() << " Tagging tracks as pions if pion DLL > " << m_dllPionCut << endmsg;
      }
      info() << "-------------+---------------------------------------------------------------+------------"
             << endmsg
             << "   %total    |  Electron  Muon    Pion    Kaon   Proton  Deuteron   X  (MC)  |  %Purity"
             << endmsg
             << "-------------+---------------------------------------------------------------+------------"
             << endmsg
             << "             |                                                               |" << endmsg;
      const std::array<std::string,Rich::NParticleTypes> type
        = { " Electron    |", " Muon        |", " Pion        |",
            " Kaon        |", " Proton      |", " Deuteron    |",
            " X           |" };
      for ( int iRec = 0; iRec < Rich::NParticleTypes; ++iRec )
      {
        info() << type[iRec]
               << boost::format( " %8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f      | %8.3f" ) %
          m_sumTab[0][iRec] % m_sumTab[1][iRec] %
          m_sumTab[2][iRec] % m_sumTab[3][iRec] %
          m_sumTab[4][iRec] % m_sumTab[5][iRec] %
          m_sumTab[6][iRec] % purity[iRec] << endmsg;
      }
      info() << "   (reco)    |                                                               |" << endmsg
             << "-------------+---------------------------------------------------------------+------------"
             << endmsg;
      info() << "   %Eff.     |" << boost::format( " %8.3f%8.3f%8.3f%8.3f%8.3f%8.3f%8.3f " ) %
        eff[0] % eff[1] % eff[2] % eff[3] % eff[4] % eff[5] % eff[6]
             << "     |" << endmsg;
      info() << "-------------+---------------------------------------------------------------+------------"
             << endmsg;

      info() << " % ID eff    |  K->K,Pr   : "
             << boost::format( "%6.2f +-%6.2f   pi->e,m,pi : %6.2f +-%6.2f " ) %
        kaonIDEff[0] % kaonIDEff[1] % piIDEff[0] % piIDEff[1] << endmsg;
      info() << " % MisID eff |  K->e,m,pi : "
             << boost::format( "%6.2f +-%6.2f   pi->K,Pr   : %6.2f +-%6.2f " ) %
        kaonMisIDEff[0] % kaonMisIDEff[1] % piMisIDEff[0] % piMisIDEff[1] << endmsg;
      info() << " % ID rate   |  Events    : "
             << boost::format( "%6.2f +-%6.2f   Tracks     : %6.2f +-%6.2f " ) %
        evPIDRate[0] % evPIDRate[1] % trPIDRate[0] % trPIDRate[1] << endmsg;

      for ( const auto& RC : m_radCount )
      {
        const double effR = ( m_nTracks[0]>0 ? 100.*((double)RC.second)/m_nTracks[0]    : 100.0 );
        const double errR = ( m_nTracks[0]>0 ? std::sqrt(effR*(100.-effR)/m_nTracks[0]) : 100.0 );
        info() << "             |  -> With "
               << RC.first.radiators()
               << boost::format( "   : %6.2f +-%6.2f" ) % effR % errR << endmsg;
      }

      info() << "-------------+---------------------------------------------------------------+------------"
             << endmsg;

    }
    else
    {
      warning() << "NO ENTRIES -> PID table skipped ..." << endmsg;
    }

  } // final printout

    // finalize base class
  return Rich::Rec::HistoAlgBase::finalize();
}

StatusCode PIDQC::loadPIDData()
{
  // Load PIDs
  DataObject *pObject;
  if ( eventSvc()->retrieveObject( m_pidTDS, pObject ) )
  {
    if ( auto * pids = static_cast<KeyedContainer<LHCb::RichPID, Containers::HashMap>*>(pObject) )
    {
      m_richPIDs.erase( m_richPIDs.begin(), m_richPIDs.end() );
      pids->containedObjects( m_richPIDs );
      _ri_debug << "Located " << pids->size() << " RichPIDs at " << m_pidTDS << endmsg;
      return StatusCode::SUCCESS;
    }
  }

  // If we get here, things went wrong
  return Warning( "Failed to locate RichPIDs at " + m_pidTDS );
}

PIDQC::TkTally PIDQC::countTracks( const std::string & location )
{
  TkTally tally;
  const auto * tracks = get<LHCb::Tracks>( location );
  _ri_debug << "Found " << tracks->size() << " Tracks at " << location << endmsg;
  for ( const auto * tk : *tracks )
  {
    if ( !tk->checkFlag(LHCb::Track::Clone) ) { ++tally.multiplicity; }
    if ( selectTracks(tk) )                   { ++tally.nTracks;      }
  }
  return tally;
}

bool PIDQC::selectTracks( const LHCb::Track * track )
{
  bool OK = false;
  if ( m_trSelector->trackSelected(track) )
  {
    OK = true;
    if ( m_mcPsel )
    {
      const auto * mcP = m_mcTruth->mcParticle(track);
      OK = mcPselector()->accept(mcP);
    }
  }
  return OK;
}

void PIDQC::print( MsgStream & msg,
                   const LHCb::RichPID * iPID,
                   const Rich::ParticleIDType pid,
                   const Rich::ParticleIDType mcpid ) const
{
  msg << *iPID << endmsg;
  msg << "  (Reco/MC) PID = " << pid;
  if ( pid == Rich::BelowThreshold )
    msg << " (" << iPID->bestParticleID() << ")";
  msg << " / " << mcpid;
  if ( mcpid == Rich::BelowThreshold )
    msg << " (" << m_mcTruth->mcParticleType(iPID->track()) << ")";
  msg << endmsg;
}

const Rich::Rec::IPIDPlots * PIDQC::plotsTool( const Rich::ParticleIDType mcpid )
{
  const auto iTool = m_plotTools.find(mcpid);
  if ( iTool == m_plotTools.end() )
  {
    const auto *& tool = m_plotTools[mcpid];
    acquireTool( "RichPIDPlots", "MC"+Rich::text(mcpid), tool, this );
    return tool;
  }
  else
  {
    return iTool->second;
  }
}
