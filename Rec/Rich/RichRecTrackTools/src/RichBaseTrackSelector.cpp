
//-----------------------------------------------------------------------------
/** @file RichBaseTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::BaseTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichBaseTrackSelector.cpp,v 1.2 2009-04-14 14:43:12 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichBaseTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BaseTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BaseTrackSelector::BaseTrackSelector( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichRecToolBase ( type, name , parent )
{
  // interface
  declareInterface<IBaseTrackSelector>(this);

  declareProperty( "MinPCut",    m_minPCut     = 0.0 ); // in GeV
  declareProperty( "MinPtCut",   m_minPtCut    = 0.0 ); // in GeV
  declareProperty( "MinChi2Cut", m_minChi2Cut  = 0.0 );
  declareProperty( "MaxPCut",    m_maxPCut     = 500 ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = 500 ); // in GeV
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = 10  );

  declareProperty( "Charge",     m_chargeSel   = 0 );
  declareProperty( "AcceptClones", m_acceptClones = false );

  declareProperty( "CloneCut",    m_cloneCut   = -1e10 );

}

//=============================================================================
// Destructor
//=============================================================================
BaseTrackSelector::~BaseTrackSelector() {}

//=============================================================================
// initialisation
//=============================================================================
StatusCode BaseTrackSelector::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // print track sel
  printSel(info()) << endreq;

  return sc;
}

MsgStream & BaseTrackSelector::printSel( MsgStream & os ) const
{
  // get track type from name
  const int slash = name().find_last_of(".");
  const std::string tkName = ( slash>0 ? name().substr(slash+1) : "UnknownTrackType" );

  os << boost::format( " %|.12s| %|12t| : P = %|-4.2e|->%|-4.2e| GeV : Pt = %|-4.2e|->%|-4.2e| GeV : fitchi2 = %|-4.2e|->%|-4.2e| : cloneCut = %|-4.2e|" )
    % tkName % m_minPCut % m_maxPCut % m_minPtCut % m_maxPtCut % m_minChi2Cut % m_maxChi2Cut % m_cloneCut;
  if ( m_acceptClones ) os << " clonesOK";
  if ( m_chargeSel != 0 ) os << " chargeSel=" << m_chargeSel;
  return os;
}

//=============================================================================
// finalisation
//=============================================================================
StatusCode BaseTrackSelector::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Test if the given Track is selected under the current criteria
bool
BaseTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  if (!track) { Warning("Null Track pointer").ignore(); return false; }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying Track " << track->key() << " " << track->type()
            << endreq;
  }

  // clones
  if ( !m_acceptClones && ( track->checkFlag(LHCb::Track::Clone) || 
                            track->info(LHCb::Track::CloneDist,9e99)<m_cloneCut ) )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track failed clone rejection" << endreq;
    return false;
  }

  // cut p
  const double p = track->p() / Gaudi::Units::GeV;
  if ( p < m_minPCut || p > m_maxPCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> P " << p << " failed cut" << endreq;
    return false;
  }

  // cut on pt
  const double pt = track->pt() / Gaudi::Units::GeV;
  if ( pt < m_minPtCut || pt > m_maxPtCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Pt " << pt << " failed cut" << endreq;
    return false;
  }

  // chi-squared
  const double chi2 = track->chi2PerDoF();
  if ( chi2 < m_minChi2Cut || chi2 > m_maxChi2Cut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  // track charge
  if ( m_chargeSel != 0 && m_chargeSel*track->charge() < 0 )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track charge " << track->charge() << " failed cut" << endreq;
    return false;
  }

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Track selected" << endreq;
  return true;
}

bool
BaseTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{

  if (!track) { Warning("Null RichRecTrack pointer").ignore(); return false; }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying RichRecTrack " << track->key() << " " << track->trackID().trackType()
            << endreq;
  }

  // clones
  if ( !m_acceptClones && ( !track->trackID().unique() || 
                            track->cloneDist()<m_cloneCut ) )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track failed clone rejection" << endreq;
    return false;
  }

  // cut p
  const double p = track->vertexMomentum() / Gaudi::Units::GeV;
  //verbose() << " -> RichRecTrack has p = " << p << " cuts " << m_minPCut << " " << m_maxPCut << endreq;
  if ( p < m_minPCut || p > m_maxPCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> P " << p << " failed cut" << endreq;
    return false;
  }

  // cut on pt
  const double pt = track->vertexPt() / Gaudi::Units::GeV;
  if ( pt < m_minPtCut || pt > m_maxPtCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Pt " << pt << " failed cut" << endreq;
    return false;
  }

  // chi-squared
  const double chi2 = track->chi2PerDoF();
  if ( chi2 < m_minChi2Cut || chi2 > m_maxChi2Cut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  // track charge
  if ( m_chargeSel != 0 && m_chargeSel*track->charge() < 0 )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track charge " << track->charge() << " failed cut" << endreq;
    return false;
  }

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Track selected" << endreq;
  return true;
}

//=============================================================================

double BaseTrackSelector::minPCut()    const { return m_minPCut; }
double BaseTrackSelector::maxPCut()    const { return m_maxPCut; }
double BaseTrackSelector::minPtCut()   const { return m_minPtCut; }
double BaseTrackSelector::maxPtCut()   const { return m_maxPtCut; }
double BaseTrackSelector::minChi2Cut() const { return m_minChi2Cut; }
double BaseTrackSelector::maxChi2Cut() const { return m_maxChi2Cut; }
int    BaseTrackSelector::chargeSel()  const { return m_chargeSel; }
double BaseTrackSelector::cloneCut()   const { return m_cloneCut; }
