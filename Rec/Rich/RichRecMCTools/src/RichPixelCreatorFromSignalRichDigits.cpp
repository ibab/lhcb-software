// $Id: RichPixelCreatorFromSignalRichDigits.cpp,v 1.2 2004-06-29 19:45:38 jonesc Exp $

// local
#include "RichPixelCreatorFromSignalRichDigits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPixelCreatorFromSignalRichDigits
//
// 15/09/2003 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromSignalRichDigits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromSignalRichDigitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromSignalRichDigits::
RichPixelCreatorFromSignalRichDigits( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichRecToolBase       ( type, name, parent ),
    m_pixels              ( 0 ),
    m_mcTool              ( 0 ),
    m_pixMaker            ( 0 ),
    m_subPixelCreatorName ( "RichPixelCreatorReco" ),
    m_allDone             ( false )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "DelegatedPixelCreator", m_subPixelCreatorName );

}

StatusCode RichPixelCreatorFromSignalRichDigits::initialize()
{

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool",     m_mcTool   );
  acquireTool( m_subPixelCreatorName, m_pixMaker );

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc->addListener( this, IncidentType::EndEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichPixelCreatorFromSignalRichDigits::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromSignalRichDigits::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() )
  {
    debug() << "Created " << richPixels()->size() << " RichRecPixels" << endreq;
  }
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromSignalRichDigits::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // Test if this is a background hit
  if ( m_mcTool->isBackground( m_mcTool->mcRichDigit(digit) ) ) return NULL;

  // Finally, delegate work to pixel creator
  return m_pixMaker->newPixel( obj );
}

StatusCode RichPixelCreatorFromSignalRichDigits::newPixels() const
{
  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Obtain smart data pointer to RichDigits
  const RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );

  // Loop over RichDigits and create working pixels
  richPixels()->reserve( digits->size() );
  for ( RichDigits::const_iterator digit = digits->begin();
        digit != digits->end(); ++digit ) { newPixel( *digit ); }

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Located " << digits->size() << " RichDigits at "
            << m_recoDigitsLocation << endreq
            << "Created " << richPixels()->size() << " RichRecPixels at "
            << m_richRecPixelLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

RichRecPixels * RichPixelCreatorFromSignalRichDigits::richPixels() const
{
  if ( !m_pixels ) {
    SmartDataPtr<RichRecPixels> tdsPixels( evtSvc(),
                                           m_richRecPixelLocation );
    if ( !tdsPixels ) {

      // Reinitialise the Pixel Container
      m_pixels = new RichRecPixels();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_pixels, m_richRecPixelLocation );

    } else {

      debug() << "Found " << tdsPixels->size() << " pre-existing RichRecPixels in TES at "
              << m_richRecPixelLocation << endreq;

      // Set smartref to TES pixel container
      m_pixels = tdsPixels;

    }
  }
  return m_pixels;
}
