
//-----------------------------------------------------------------------------
/** @file RichTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichTrackSelector.cpp,v 1.1 2006-08-13 17:15:34 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichTrackSelector::RichTrackSelector( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : RichRecToolBase ( type, name , parent )
{
  // interface
  declareInterface<Rich::IRichTrackSelector>(this);

  // Default track types to select
  m_trNames =
    boost::assign::list_of("Forward")("Match")("Seed")("VeloTT")("KsTrack");
  declareProperty( "TrackAlgs", m_trNames );

  declareProperty( "MinPCut",    m_minPCut     = 0.0 ); // in GeV
  declareProperty( "MinPtCut",   m_minPtCut    = 0.0 ); // in GeV
  declareProperty( "MinChi2Cut", m_minChi2Cut  = 0.0 );
  declareProperty( "MaxPCut",    m_maxPCut     = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = boost::numeric::bounds<double>::highest() );

  m_jobOpts = boost::assign::list_of("MinPCut")("MaxPCut")("MinPtCut")("MaxPtCut");

}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichTrackSelector::~RichTrackSelector() {}

//=============================================================================
// initialisation
//=============================================================================
StatusCode Rich::RichTrackSelector::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // define tools to use for each track type
  // should try and find a better way to do this
  m_tkToolNames[Rich::Track::Forward]     = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::Match]       = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::VeloTT]      = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::Seed]        = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::KsTrack]     = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::Velo]        = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::Trigger]     = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::MCRichTrack] = "Rich::RichBaseTrackSelector";

  info() << "Track Selection : " << m_trNames << endreq;
  for ( TrackNames::const_iterator iName = m_trNames.begin();
        iName != m_trNames.end(); ++iName )
  {
    if      ( *iName == "Forward"     ) { setUpTrack( Rich::Track::Forward ); }
    else if ( *iName == "Match"       ) { setUpTrack( Rich::Track::Match   ); }
    else if ( *iName == "KsTrack"     ) { setUpTrack( Rich::Track::KsTrack ); }
    else if ( *iName == "VeloTT"      ) { setUpTrack( Rich::Track::VeloTT  ); }
    else if ( *iName == "Seed"        ) { setUpTrack( Rich::Track::Seed    ); }
    else if ( *iName == "Velo"        ) { setUpTrack( Rich::Track::Velo    ); }
    else if ( *iName == "Trigger"     ) { setUpTrack( Rich::Track::Trigger ); }
    else if ( *iName == "MCRichTrack" ) { setUpTrack( Rich::Track::MCRichTrack ); }
    else
    {
      throw GaudiException( "Unknown track selection criteria '"+(*iName)+"'",
                            "*RichTrackSelector*", StatusCode::FAILURE );
      return false;
    }
  }

  return sc;
}

void Rich::RichTrackSelector::setUpTrack( const Rich::Track::Type type )
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Acquiring RichTrackSelector '" << m_tkToolNames[type]
            << "' for type " << Rich::text(type) << endreq;
  }
  if ( "" == m_tkToolNames[type] )
  {
    Exception( "Selection tool for track algorithm '"+Rich::text(type)+"' undefined" );
  }

  // Try and give options
  for ( std::vector<std::string>::const_iterator iP = m_jobOpts.begin();
        iP != m_jobOpts.end(); ++iP )
  {
    const Property * prop = NULL;
    try { prop = &this->getProperty(*iP); } catch (...) { prop = NULL; }
    if ( prop )
    {
      debug() << "Found global " << *iP << " option" << endreq;
      const std::string fullname = name()+"."+Rich::text(type);
      const std::vector<const Property*> * properties = joSvc()->getProperties(fullname);
      bool found = false;
      if ( properties )
      {
        for ( std::vector<const Property*>::const_iterator itProp = properties->begin();
              itProp != properties->end(); ++itProp )
        {
          const StringProperty * sp = dynamic_cast<const StringProperty*>(*itProp);
          if ( sp )
          {
            if ( *iP == (*itProp)->name() ) { found = true; break; }
          }
        }
      }
      if ( !found )
      {
        debug() << " Adding option " << *iP << " to " << fullname << endmsg;
        joSvc()->addPropertyToCatalogue( fullname, prop );
      }
    }
  }

  // finally, load the tool
  m_tkTools[type] =
    tool<Rich::IRichBaseTrackSelector>( m_tkToolNames[type], Rich::text(type), this );
}

//=============================================================================
// finalisation
//=============================================================================
StatusCode Rich::RichTrackSelector::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

const std::vector<std::string> & Rich::RichTrackSelector::selectedTracks() const
{
  return m_trNames;
}

// Test if the given Track is selected
bool Rich::RichTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  // track type
  const Rich::Track::Type type = Rich::Track::type(track);
  // is this type selected ?
  TrackTools::const_iterator iT = m_tkTools.find(type);
  if ( iT == m_tkTools.end() ) return false;

  // otherwise, delegate to base tool
  return iT->second->trackSelected(track);
}

// Test it the given RichRecTrack is selected
bool Rich::RichTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{
  // track type
  const Rich::Track::Type type = track->trackID().trackType();
  // is this type selected ?
  TrackTools::const_iterator iT = m_tkTools.find(type);
  if ( iT == m_tkTools.end() ) return false;

  // otherwise, delegate to base tool
  return iT->second->trackSelected(track);
}


//=============================================================================

double Rich::RichTrackSelector::minPCut()    const { return m_minPCut; }
double Rich::RichTrackSelector::maxPCut()    const { return m_maxPCut; }
double Rich::RichTrackSelector::minPtCut()   const { return m_minPtCut; }
double Rich::RichTrackSelector::maxPtCut()   const { return m_maxPtCut; }
double Rich::RichTrackSelector::minChi2Cut() const { return m_minChi2Cut; }
double Rich::RichTrackSelector::maxChi2Cut() const { return m_maxChi2Cut; }

double Rich::RichTrackSelector::minPCut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? -999 : iT->second->minPCut() );
}

double Rich::RichTrackSelector::maxPCut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? -999 : iT->second->maxPCut() );
}

double Rich::RichTrackSelector::minPtCut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? -999 : iT->second->minPtCut() );
}

double Rich::RichTrackSelector::maxPtCut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? -999 : iT->second->maxPtCut() );
}

double Rich::RichTrackSelector::minChi2Cut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? -999 : iT->second->minChi2Cut() );
}

double Rich::RichTrackSelector::maxChi2Cut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? -999 : iT->second->maxChi2Cut() );
}
