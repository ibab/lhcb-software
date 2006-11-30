
//-----------------------------------------------------------------------------
/** @file RichBaseTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichBaseTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichBaseTrackSelector.cpp,v 1.6 2006-11-30 15:38:31 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichBaseTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichBaseTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichBaseTrackSelector::RichBaseTrackSelector( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : RichRecToolBase ( type, name , parent )
{
  // interface
  declareInterface<Rich::IRichBaseTrackSelector>(this);

  declareProperty( "MinPCut",    m_minPCut     = 0.0 ); // in GeV
  declareProperty( "MinPtCut",   m_minPtCut    = 0.0 ); // in GeV
  declareProperty( "MinChi2Cut", m_minChi2Cut  = 0.0 );
  declareProperty( "MaxPCut",    m_maxPCut     = 500 ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = 500 ); // in GeV
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = 10  );

  declareProperty( "Charge",     m_chargeSel   = 0 );
  declareProperty( "AcceptClones", m_acceptClones = false );

}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichBaseTrackSelector::~RichBaseTrackSelector() {}

//=============================================================================
// initialisation
//=============================================================================
StatusCode Rich::RichBaseTrackSelector::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // print track sel
  printSel(info()) << endreq;

  return sc;
}

MsgStream & Rich::RichBaseTrackSelector::printSel( MsgStream & os ) const
{
  // get track type from name
  const int slash = name().find_last_of(".");
  const std::string tkName = ( slash>0 ? name().substr(slash+1) : "UnknownTrackType" );

  os << boost::format( " %|.10s| %|10t| : P = %|-4.2e|->%|-4.2e| GeV : Pt = %|-4.2e|->%|-4.2e| GeV : fitchi2 = %|-4.2e|->%|-4.2e|" )
    % tkName % m_minPCut % m_maxPCut % m_minPtCut % m_maxPtCut % m_minChi2Cut % m_maxChi2Cut;
  if ( m_acceptClones ) os << " clonesOK";
  if ( m_chargeSel != 0 ) os << " chargeSel=" << m_chargeSel;
  return os;
}

//=============================================================================
// finalisation
//=============================================================================
StatusCode Rich::RichBaseTrackSelector::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Test if the given Track is selected under the current criteria
bool
Rich::RichBaseTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  if (!track) { Warning("Null Track pointer"); return false; }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Trying Track " << track->key() << " " << track->type()
            << endreq;
  }

  // clones
  if ( !m_acceptClones && track->checkFlag(LHCb::Track::Clone) )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track failed clone rejection" << endreq;
    return false;
  }

  // cut p
  const double p = track->p() / Gaudi::Units::GeV;
  if ( p < m_minPCut || p > m_maxPCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> P " << p << " failed cut" << endreq;
    return false;
  }

  // cut on pt
  const double pt = track->pt() / Gaudi::Units::GeV;
  if ( pt < m_minPtCut || pt > m_maxPtCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Pt " << pt << " failed cut" << endreq;
    return false;
  }

  // chi-squared
  const double chi2 = track->chi2PerDoF();
  if ( chi2 < m_minChi2Cut || chi2 > m_maxChi2Cut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  // track charge
  if ( m_chargeSel != 0 && m_chargeSel*track->charge() < 0 )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track charge " << track->charge() << " failed cut" << endreq;
    return false;
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
  return true;
}

bool
Rich::RichBaseTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{

  if (!track) { Warning("Null RichRecTrack pointer"); return false; }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Trying RichRecTrack " << track->key() << " " << track->trackID().trackType()
            << endreq;
  }

  // clones
  if ( !m_acceptClones && !track->trackID().unique() )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track failed clone rejection" << endreq;
    return false;
  }

  // cut p
  const double p = track->vertexMomentum() / Gaudi::Units::GeV;
  //debug() << " -> RichRecTrack has p = " << p << " cuts " << m_minPCut << " " << m_maxPCut << endreq;
  if ( p < m_minPCut || p > m_maxPCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> P " << p << " failed cut" << endreq;
    return false;
  }

  // cut on pt
  const double pt = track->vertexPt() / Gaudi::Units::GeV;
  if ( pt < m_minPtCut || pt > m_maxPtCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Pt " << pt << " failed cut" << endreq;
    return false;
  }

  // chi-squared
  const double chi2 = track->chi2PerDoF();
  if ( chi2 < m_minChi2Cut || chi2 > m_maxChi2Cut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  // track charge
  if ( m_chargeSel != 0 && m_chargeSel*track->charge() < 0 )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track charge " << track->charge() << " failed cut" << endreq;
    return false;
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
  return true;
}

//=============================================================================

double Rich::RichBaseTrackSelector::minPCut()    const { return m_minPCut; }
double Rich::RichBaseTrackSelector::maxPCut()    const { return m_maxPCut; }
double Rich::RichBaseTrackSelector::minPtCut()   const { return m_minPtCut; }
double Rich::RichBaseTrackSelector::maxPtCut()   const { return m_maxPtCut; }
double Rich::RichBaseTrackSelector::minChi2Cut() const { return m_minChi2Cut; }
double Rich::RichBaseTrackSelector::maxChi2Cut() const { return m_maxChi2Cut; }
int    Rich::RichBaseTrackSelector::chargeSel()  const { return m_chargeSel; }
