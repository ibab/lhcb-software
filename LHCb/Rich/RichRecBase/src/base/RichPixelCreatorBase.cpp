
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorBase.cpp
 *
 *  Implementation file for tool base class : RichPixelCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorBase.cpp,v 1.2 2005-05-26 16:45:51 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/04/2005
 */
//---------------------------------------------------------------------------------

// local
#include "RichRecBase/RichPixelCreatorBase.h"

//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichPixelCreatorBase::RichPixelCreatorBase( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_allDone       ( false ),
    m_hpdTool       ( 0 ),
    m_pixels        ( 0 ),
    m_bookKeep      ( false ),
    m_hpdCheck      ( false ),
    m_usedDets      ( Rich::NRiches, true ),
    m_begins        ( boost::extents[Rich::NRiches][Rich::NHPDPanelsPerRICH] ),
    m_ends          ( boost::extents[Rich::NRiches][Rich::NHPDPanelsPerRICH] ),
    m_Nevts         ( 0 ),
    m_hasBeenCalled ( false )
{

  // Define the interface
  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "DoBookKeeping",      m_bookKeep  );
  declareProperty( "UseDetectors",       m_usedDets  );
  declareProperty( "CheckHPDsAreActive", m_hpdCheck  );

}

StatusCode RichPixelCreatorBase::initialize()
{
  // base class initilize
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  if ( m_hpdCheck )
  {
    acquireTool( "RichHPDInfoTool", m_hpdTool );
    Warning( "Will check each pixel for HPD status. Takes additional CPU.",
             StatusCode::SUCCESS );
  }

  // Check which detectors to use
  if ( !m_usedDets[Rich::Rich1] )
    Warning( "Pixels for RICH1 are disabled", StatusCode::SUCCESS );
  if ( !m_usedDets[Rich::Rich2] )
    Warning( "Pixels for RICH2 are disabled", StatusCode::SUCCESS );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  // Intialise counts
  m_hitCount[Rich::Rich1] = 0;
  m_hitCount[Rich::Rich2] = 0;

  return sc;
}

StatusCode RichPixelCreatorBase::finalize()
{
  // print stats
  printStats();

  // base class finalize
  return RichRecToolBase::finalize();
}

void RichPixelCreatorBase::printStats() const
{
  if ( m_Nevts > 0
       && !( m_hitCount[Rich::Rich1] == 0 &&
             m_hitCount[Rich::Rich2] == 0 ) )
  {
    RichStatDivFunctor occ("%8.2f +-%7.2f");
    info() << "================================================" << endreq
           << "  Pixel summary for " << m_Nevts << " events :-" << endreq
           << "     Rich1  "
           << occ(m_hitCount[Rich::Rich1],m_Nevts) << "  pixels/event" << endreq
           << "     Rich2  "
           << occ(m_hitCount[Rich::Rich2],m_Nevts) << "  pixels/event" << endreq
         << "================================================" << endreq;
  }
  else
  {
    Warning( "No pixels produced", StatusCode::SUCCESS );
  }

}

void RichPixelCreatorBase::fillIterators() const
{

  // Reset to defaults
  resetIterators();

  // if there are some pixels, compute iterators
  if ( !richPixels()->empty() )
  {

    RichRecPixels::iterator iPix = richPixels()->begin();
    Rich::DetectorType rich      = (*iPix)->smartID().rich();
    Rich::Side        panel      = (*iPix)->smartID().panel();
    Rich::DetectorType lastrich  = rich;
    Rich::Side        lastpanel  = panel;
    ++iPix; // skip first pixel

    // loop over remaining pixels
    for ( ; iPix != richPixels()->end(); ++iPix )
    {
      if ( panel != (*iPix)->smartID().panel() )
      {
        panel = (*iPix)->smartID().panel();
        m_begins[(*iPix)->smartID().rich()][panel] = iPix;
        m_ends[lastrich][lastpanel]                = iPix;
        lastpanel = panel;
      }
      if ( rich != (*iPix)->smartID().rich() )
      {
        rich                = (*iPix)->smartID().rich();
        m_richBegin[rich]   = iPix;
        m_richEnd[lastrich] = iPix;
        lastrich            = rich;
      }
    }

    // Set final iterators
    m_richEnd[rich]     = iPix;
    m_ends[rich][panel] = iPix;

  }

}

RichRecPixels * RichPixelCreatorBase::richPixels() const
{

  // If first time for this event, either create or load the RichRecPixels
  if ( !m_pixels )
  {

    if ( !exist<RichRecPixels>(pixelLocation()) )
    {

      // Reinitialise the Pixel Container
      m_pixels = new RichRecPixels();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_pixels, pixelLocation() );

    }
    else
    {

      // get pixels from TES
      m_pixels = get<RichRecPixels>( pixelLocation() );
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Found " << m_pixels->size() << " pre-existing RichRecPixels in TES at "
                << pixelLocation() << endreq;
      }

      if ( bookKeep() )
      {
        // Remake local pixel reference map
        for ( RichRecPixels::const_iterator iPixel = m_pixels->begin();
              iPixel != m_pixels->end(); ++iPixel )
        {
          m_pixelExists [ (*iPixel)->smartID() ] = *iPixel;
          m_pixelDone   [ (*iPixel)->smartID() ] = true;
        }
      }

      // find iterators
      fillIterators();

    }
  }

  return m_pixels;
}

RichRecPixels::iterator
RichPixelCreatorBase::begin( const Rich::DetectorType rich ) const
{
  return m_richBegin[rich];
}

RichRecPixels::iterator
RichPixelCreatorBase::end( const Rich::DetectorType rich ) const
{
  return m_richEnd[rich];
}

RichRecPixels::iterator
RichPixelCreatorBase::begin( const Rich::DetectorType rich,
                             const Rich::Side         panel ) const
{
  return m_begins[rich][panel];
}

RichRecPixels::iterator
RichPixelCreatorBase::end( const Rich::DetectorType rich,
                           const Rich::Side         panel ) const
{
  return m_ends[rich][panel];
}

void RichPixelCreatorBase::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() )
  {
    this->InitNewEvent();
  }
  // Debug printout at the end of each event
  else if ( IncidentType::EndEvent == incident.type() )
  {
    this->FinishEvent();
  }
}

void RichPixelCreatorBase::InitNewEvent()
{
  m_hasBeenCalled = false;
  m_allDone = false;
  m_pixels  = 0;
  if ( m_bookKeep )
  {
    m_pixelExists.clear();
    m_pixelDone.clear();
  }
}

void RichPixelCreatorBase::FinishEvent()
{
  if ( m_hasBeenCalled ) ++m_Nevts;
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << richPixels()->size() << " RichRecPixels at "
            << pixelLocation() << endreq;
  }
}
