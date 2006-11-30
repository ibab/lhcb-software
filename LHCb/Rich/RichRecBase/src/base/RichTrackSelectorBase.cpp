
//-----------------------------------------------------------------------------
/** @file RichTrackSelectorBase.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichTrackSelectorBase
 *
 *  CVS Log :-
 *  $Id: RichTrackSelectorBase.cpp,v 1.3 2006-11-30 15:26:45 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichRecBase/RichTrackSelectorBase.h"

//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichTrackSelectorBase::RichTrackSelectorBase( const std::string& type,
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

  declareProperty( "MinPCut",       m_minPCut     = 0.0 ); // in GeV
  declareProperty( "MinPtCut",      m_minPtCut    = 0.0 ); // in GeV
  declareProperty( "MinChi2Cut",    m_minChi2Cut  = 0.0 );
  declareProperty( "MaxPCut",       m_maxPCut     = 200 ); // in GeV
  declareProperty( "MaxPtCut",      m_maxPtCut    = 200 ); // in GeV
  declareProperty( "MaxFitChi2Cut", m_maxChi2Cut  = 10  );
  declareProperty( "Charge",        m_chargeSel   = 0 );

  m_jobOpts =
    boost::assign::list_of("MinPCut")("MaxPCut")("MinPtCut")("MaxPtCut")("MinChi2Cut")("MaxChi2Cut")("Charge");

}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichTrackSelectorBase::~RichTrackSelectorBase() {}

//=============================================================================
// initialisation
//=============================================================================
StatusCode Rich::RichTrackSelectorBase::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Set up tracks
  sc = setUpTracks();

  return sc;
}

StatusCode Rich::RichTrackSelectorBase::setUpTracks()
{
  StatusCode sc = StatusCode::SUCCESS;

  // define tools to use for each track type
  // should try and find a better way to do this
  m_tkToolNames[Rich::Track::Forward]     = "Rich::RichForwardTrackSelector";
  m_tkToolNames[Rich::Track::Match]       = "Rich::RichMatchTrackSelector";
  m_tkToolNames[Rich::Track::VeloTT]      = "Rich::RichVeloTTTrackSelector";
  m_tkToolNames[Rich::Track::Seed]        = "Rich::RichSeedTrackSelector";
  m_tkToolNames[Rich::Track::KsTrack]     = "Rich::RichKsTrackSelector";
  m_tkToolNames[Rich::Track::Velo]        = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::Trigger]     = "Rich::RichBaseTrackSelector";
  m_tkToolNames[Rich::Track::MCRichTrack] = "Rich::RichBaseTrackSelector";

  info() << "Track Selection : " << m_trNames << endreq;
  for ( TrackNames::const_iterator iName = m_trNames.begin();
        iName != m_trNames.end(); ++iName )
  {
    if      ( *iName == "Forward"     ) { sc = setUpTrack( Rich::Track::Forward ); }
    else if ( *iName == "Match"       ) { sc = setUpTrack( Rich::Track::Match   ); }
    else if ( *iName == "KsTrack"     ) { sc = setUpTrack( Rich::Track::KsTrack ); }
    else if ( *iName == "VeloTT"      ) { sc = setUpTrack( Rich::Track::VeloTT  ); }
    else if ( *iName == "Seed"        ) { sc = setUpTrack( Rich::Track::Seed    ); }
    else if ( *iName == "Velo"        ) { sc = setUpTrack( Rich::Track::Velo    ); }
    else if ( *iName == "Trigger"     ) { sc = setUpTrack( Rich::Track::Trigger ); }
    else if ( *iName == "MCRichTrack" ) { sc = setUpTrack( Rich::Track::MCRichTrack ); }
    else
    {
      sc = Warning( "Unknown track algorithm type '"+(*iName)+"'" );
    }
    // check track was setup ok
    if ( sc.isFailure() ) return sc;
  }

  // clear map to save a little space
  m_tkToolNames.clear();

  return sc;
}

StatusCode Rich::RichTrackSelectorBase::setUpTrack( const Rich::Track::Type type )
{
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Acquiring RichTrackSelector '" << m_tkToolNames[type]
              << "' for type " << Rich::text(type) << endreq;

  if ( "" == m_tkToolNames[type] )
  {
    return Error( "Selection tool for track algorithm '"+Rich::text(type)+"' undefined" );
  }

  // Try and give options (CRJ : Should see if a nicer way that avoids this is possible)
  const std::vector<const Property*> * myprops = joSvc()->getProperties(name());
  if ( myprops )
  {
    for ( std::vector<const Property*>::const_iterator itP = myprops->begin();
          itP != myprops->end(); ++itP )
    {
      for ( std::vector<std::string>::const_iterator iP = m_jobOpts.begin();
            iP != m_jobOpts.end(); ++iP )
      {
        if ( *iP == (*itP)->name() )
        {
          //---------------------------------------------------------------------------------
          if (msgLevel(MSG::VERBOSE))
            verbose() << "Found global " << *iP << " option" << endreq;
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
                if ( *iP == sp->name() ) { found = true; break; }
              }
            }
          }
          if ( !found )
          {
            //if ( msgLevel(MSG::VERBOSE) )
            if (msgLevel(MSG::VERBOSE))
              verbose() << " Adding option " << *iP << " to " << fullname << endmsg;
            joSvc()->addPropertyToCatalogue( fullname, *(*itP) );
          }
          //---------------------------------------------------------------------------------
        }
      }
    }
  }

  // finally, load the tool
  m_tkTools[type] =
    tool<Rich::IRichBaseTrackSelector>( m_tkToolNames[type], Rich::text(type), this );

  return StatusCode::SUCCESS;
}

//=============================================================================
// finalisation
//=============================================================================
StatusCode Rich::RichTrackSelectorBase::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

const std::vector<std::string> & Rich::RichTrackSelectorBase::selectedTracks() const
{
  return m_trNames;
}

// Test if the given Track is selected
bool Rich::RichTrackSelectorBase::trackSelected( const LHCb::Track * track ) const
{
  // is this type selected ?
  TrackTools::const_iterator iT = m_tkTools.find( Rich::Track::type(track) );
  return ( iT != m_tkTools.end() ? iT->second->trackSelected(track) : false );
}

// Test it the given RichRecTrack is selected
bool Rich::RichTrackSelectorBase::trackSelected( const LHCb::RichRecTrack * track ) const
{
  // is this type selected ?
  TrackTools::const_iterator iT = m_tkTools.find( track->trackID().trackType() );
  return ( iT != m_tkTools.end() ? iT->second->trackSelected(track) : false );
}

//=============================================================================

double Rich::RichTrackSelectorBase::minPCut()    const { return m_minPCut; }
double Rich::RichTrackSelectorBase::maxPCut()    const { return m_maxPCut; }
double Rich::RichTrackSelectorBase::minPtCut()   const { return m_minPtCut; }
double Rich::RichTrackSelectorBase::maxPtCut()   const { return m_maxPtCut; }
double Rich::RichTrackSelectorBase::minChi2Cut() const { return m_minChi2Cut; }
double Rich::RichTrackSelectorBase::maxChi2Cut() const { return m_maxChi2Cut; }
int    Rich::RichTrackSelectorBase::chargeSel()  const { return m_chargeSel; }

double Rich::RichTrackSelectorBase::minPCut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? minPCut() : max(minPCut(),iT->second->minPCut()) );
}

double Rich::RichTrackSelectorBase::maxPCut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? maxPCut() : min(maxPCut(),iT->second->maxPCut()) );
}

double Rich::RichTrackSelectorBase::minPtCut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? minPtCut() : max(minPtCut(),iT->second->minPtCut()) );
}

double Rich::RichTrackSelectorBase::maxPtCut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? maxPtCut() : min(maxPtCut(),iT->second->maxPtCut()) );
}

double Rich::RichTrackSelectorBase::minChi2Cut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? minChi2Cut() : max(minChi2Cut(),iT->second->minChi2Cut()) );
}

double Rich::RichTrackSelectorBase::maxChi2Cut( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? maxChi2Cut() : min(maxChi2Cut(),iT->second->maxChi2Cut()) );
}

int Rich::RichTrackSelectorBase::chargeSel( const Rich::Track::Type type ) const
{
  TrackTools::const_iterator iT = m_tkTools.find(type);
  return ( iT == m_tkTools.end() ? chargeSel() : iT->second->chargeSel() );
}
