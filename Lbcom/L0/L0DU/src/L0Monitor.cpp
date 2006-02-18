// $Id: L0Monitor.cpp,v 1.7 2006-02-18 11:29:37 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "Event/L0DUReport.h"
// local
#include "L0Monitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0Monitor
//
// 2006-02-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Monitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Monitor::L0Monitor( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
L0Monitor::~L0Monitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Monitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_totEvt    = 0.;
  m_goodL0    = 0.;
  m_puVeto    = 0.;
  m_trigElec  = 0.;
  m_trigPhot  = 0.;
  m_trigHadr  = 0.;
  m_trigPi0L  = 0.;
  m_trigPi0G  = 0.;
  m_trigPi0   = 0.;
  m_trigMuon  = 0.;
  m_trigSumMu = 0.;

  m_exclElec  = 0.;
  m_exclPhot  = 0.;
  m_exclHadr  = 0.;
  m_exclPi0L  = 0.;
  m_exclPi0G  = 0.;
  m_exclPi0   = 0.;
  m_exclMuon  = 0.;
  m_exclSumMu = 0.;

  m_minPeak2Veto = 3.;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Monitor::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::L0DUReport* decision = get<LHCb::L0DUReport>( LHCb::L0DUReportLocation::Default );
  
  debug() << "L0 decision : " << decision->decision()
          << " fired:";
  for ( int bit=0; 32 > bit; ++bit  ) {
    if ( decision->channelDecision( bit ) ) {
      debug() << bit << "=" << decision->channelName( bit ) << ",";
    } 
  }
  debug() << endreq;

  m_totEvt  += 1.;

  int pattern = 0;
  if (  decision->channelDecision( 0 ) ) {
    m_goodL0 += 1.;
    for ( int bit=1; 32 > bit; ++bit  ) {
      if ( decision->channelDecision( bit ) ) {
        std::string myName =  decision->channelName( bit );
        std::string key = myName.substr( 2, 4);
        
        debug() << "understood key = " << key << endreq;
        if ( "elec" == key )  pattern |= 1;
        if ( "phot" == key )  pattern |= 2;
        if ( "hadr" == key )  pattern |= 4;
        if ( "loca" == key )  pattern |= 8;
        if ( "glob" == key )  pattern |= 16;
        if ( "muon" == key )  pattern |= 32;
        if ( "dimu" == key )  pattern |= 64;
      }
    }
    
    if ( pattern & 1  ) m_trigElec  += 1.;
    if ( pattern & 2  ) m_trigPhot  += 1.;
    if ( pattern & 4  ) m_trigHadr  += 1.;
    if ( pattern & 8  ) m_trigPi0L  += 1.;
    if ( pattern & 16 ) m_trigPi0G  += 1.;
    if ( pattern & 24 ) m_trigPi0   += 1.;
    if ( pattern & 32 ) m_trigMuon  += 1.;
    if ( pattern & 64 ) m_trigSumMu += 1.;

    if ( pattern == 1  ) m_exclElec  += 1.;
    if ( pattern == 2  ) m_exclPhot  += 1.;
    if ( pattern == 4  ) m_exclHadr  += 1.;
    if ( pattern == 8  ) m_exclPi0L  += 1.;
    if ( pattern == 16 ) m_exclPi0G  += 1.;
    if ( (pattern == 8 ) || 
         (pattern == 16) || 
         (pattern == 24)   ) m_exclPi0   += 1.;
    if ( pattern == 32 ) m_exclMuon  += 1.;
    if ( pattern == 64 ) m_exclSumMu += 1.;
    
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Monitor::finalize() {

  debug() << "==> Finalize" << endmsg;
  info() << "== L0 Performance on " << m_totEvt << " events." << endreq;
  double fact;
  if ( 1. > m_totEvt ) {
    fact = 0.;
  } else {
    fact = 100. / m_totEvt;
  }
  
  info() << "PileUp Veto  : " << format( " %8.0f  %6.2f %%", m_puVeto, fact * m_puVeto ) << endreq;
  info() << "Accepted L0  : " << format( " %8.0f  %6.2f %%", m_goodL0, fact * m_goodL0 ) << endreq;

  info() << "  Electron   : " << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                                         m_trigElec, fact*m_trigElec, m_exclElec, fact*m_exclElec )
         << endreq;
  info() << "    Photon   : " << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                                         m_trigPhot, fact*m_trigPhot, m_exclPhot, fact*m_exclPhot )
         << endreq;
  info() << "    Hadron   : " << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                                         m_trigHadr, fact*m_trigHadr, m_exclHadr, fact*m_exclHadr )
         << endreq;
  info() << " Pi0 Local   : " << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                                         m_trigPi0L, fact*m_trigPi0L, m_exclPi0L, fact*m_exclPi0L )
         << endreq;
  info() << "Pi0 Global   : " << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                                         m_trigPi0G, fact*m_trigPi0G, m_exclPi0G, fact*m_exclPi0G )
         << endreq;
  info() << "   any Pi0   : " << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                                         m_trigPi0, fact*m_trigPi0, m_exclPi0, fact*m_exclPi0 )
         << endreq;
  info() << "      Muon   : " << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                                         m_trigMuon, fact*m_trigMuon, m_exclMuon, fact*m_exclMuon )
         << endreq;
  info() << "  Sum Muon   : " << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                                         m_trigSumMu, fact*m_trigSumMu, m_exclSumMu, fact*m_exclSumMu )
         << endreq;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
