
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.cpp
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.cpp,v 1.41 2008-06-08 20:15:45 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

// local
#include "RichMarkovRingFinderAlg.h"

// namespaces
using namespace LHCb;
using namespace Rich::Rec;

// Declaration of the Algorithm Factories
DECLARE_ALGORITHM_FACTORY( Rich1TopPanelMarkovRingFinderAlg    );
DECLARE_ALGORITHM_FACTORY( Rich1BottomPanelMarkovRingFinderAlg );
DECLARE_ALGORITHM_FACTORY( Rich2LeftPanelMarkovRingFinderAlg   );
DECLARE_ALGORITHM_FACTORY( Rich2RightPanelMarkovRingFinderAlg  );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichMarkovRingFinderAlg::RichMarkovRingFinderAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator,
                                                  const Rich::DetectorType rich,
                                                  const Rich::Side panel,
                                                  const Rich::RadiatorType rad )
  : RichRecHistoAlgBase ( name , pSvcLocator ),
    m_ckAngle      ( NULL  ),
    m_smartIDTool  ( NULL  ),
    m_rich         ( rich  ),
    m_panel        ( panel ),
    m_rad          ( rad   ),
    m_sampler      ( NULL  )
{
  declareProperty( "RingLocation",     m_ringLocation     = RichRecRingLocation::MarkovRings+"All" );
  declareProperty( "BestRingLocation", m_bestRingLocation = RichRecRingLocation::MarkovRings+"Best" );
  declareProperty( "IsolatedRingLocation", m_isolatedRingLocation = RichRecRingLocation::MarkovRings+"Isolated" );
  declareProperty( "DumpDataToTextFile",  m_dumpText       = false );
  declareProperty( "MinAssociationProb",  m_minAssProb     = 0.05 );
  declareProperty( "MinNumHitsBestRings", m_minNumHitsBest = 4 );
  declareProperty( "MinAvProbBestRings",  m_minAvProbBest  = 0.6 );
  declareProperty( "MinNumHitsIsolatedRings", m_minNumHitsIsolated = 4 );
  declareProperty( "MinAvProbIsolatedRings",  m_minAvProbIsolated  = 0.8 );
  declareProperty( "MaxHitsInEvent",      m_maxHitsEvent = 300 );
  declareProperty( "ScaleFactor",         m_scaleFactor    = 0.030/128.0 );
}

//=============================================================================
// Destructor
//=============================================================================
RichMarkovRingFinderAlg::~RichMarkovRingFinderAlg() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichMarkovRingFinderAlg::initialize()
{
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle     );
  acquireTool( "RichSmartIDTool",    m_smartIDTool );

  // make a new sampler ( should use a factory here )
  boost::shared_ptr<Lester::NimTypeRichModel> ntrm(new Lester::NimTypeRichModel);
  m_sampler = new CrudeSampler(ntrm);

  // configure sampler
  Lester::messHandle().declare(this); // set for messages
  m_sampler->configuration.clearAllparams();
  m_sampler->configuration.setParam( "ScaleNumItsByHits", true );
  m_sampler->configuration.setParam( "TargetIterations", 1000  );
  m_sampler->configuration.setParam( "TargetHits",       250   );
  m_sampler->configuration.setParam( "AbsMaxIts",        20000 );
  m_sampler->configuration.setParam( "AbsMinIts",        400   );

  info() << "Markov Chain Configuration : " << rich() << " " << panel()
         << " " << m_sampler->configuration << endreq;

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichMarkovRingFinderAlg::finalize()
{
  // Intercept any final messages
  Lester::messHandle().declare(this);
  // cleanup
  if ( m_sampler ) { delete m_sampler; m_sampler = NULL; }
  // stop receiving messages
  Lester::messHandle().declare(NULL);
  // return
  return RichRecHistoAlgBase::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichMarkovRingFinderAlg::execute()
{
  // RICH init
  StatusCode sc = richInit();
  if ( sc.isFailure() ) return sc;

  // Dump data to text file ?
  sc = dumpToTextfile();
  if ( sc.isFailure() ) return sc;

  // Ring finder
  return runRingFinder();
}

StatusCode RichMarkovRingFinderAlg::richInit()
{
  // Set Rich event status to OK for start of Markov processing
  richStatus()->setEventOK(true);

  // Make sure RichRecPixels are available
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
  debug() << "Found " << richPixels()->size() << " RichRecPixels" << endreq;

  // Make sure RichRecTracks and RichRecSegments are available
  if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
  debug() << "Found " << richTracks()->size() << " RichRecTracks" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichMarkovRingFinderAlg::runRingFinder()
{

  // make sure ring containers are always created
  getRings( m_ringLocation         );
  getRings( m_bestRingLocation     );
  getRings( m_isolatedRingLocation );

  // do the finding in a try block to catch exceptions that leak out
  try
  {
    Lester::messHandle().declare(this); // set for messages

    // Create input data object
    GenRingF::GenericInput input;

    // add hits
    const bool OK = addDataPoints(input);
    if ( OK )
    {
      // hit selection was OK, so do the ring finding

      // run the fit
      boost::shared_ptr<GenRingF::GenericResults> outputP = m_sampler->fit(input);
      const GenRingF::GenericResults & output = *outputP;

      if ( msgLevel(MSG::DEBUG) )
        debug() << "Markov fit took " << output.numIterations << " iterations in "
                << output.timeTaken << " ms" << endreq;

      // some plots on fit stats
      plot1D( output.numIterations, "#iterations", 0, 15000 );
      plot1D( output.timeTaken,     "time(ms)", 0, 10000 );
      plot2D( output.numIterations, output.timeTaken, "time(ms) V #iterations", 0, 15000, 0, 10000 );
      plot2D( input.hits.size(), output.numIterations, "#iterations V #hits", 0, 1500, 0, 15000 );
      plot2D( input.hits.size(), output.timeTaken, "time(ms) V #hits", 0, 1500, 0, 10000 );

      // finalise the results as TES rings
      const StatusCode saveSc = saveRings(input,output);
      if ( saveSc.isFailure() ) return saveSc;

    }

    counter("Processed Events") += (int)OK;

  }
  catch ( const std::exception & excpt )
  {
    Warning( "Ring finding FAILED with exception : " + std::string(excpt.what()),
             StatusCode::SUCCESS );
  }

  return StatusCode::SUCCESS;
}

LHCb::RichRecRings * RichMarkovRingFinderAlg::getRings( const std::string & location ) const
{
  RichRecRings * rings(NULL);
  if ( exist<RichRecRings>(location) )
  {
    rings = get<RichRecRings>(location);
  }
  else
  {
    rings = new RichRecRings();
    put ( rings, location );
  }
  return rings;
}

StatusCode RichMarkovRingFinderAlg::saveRings( const GenRingF::GenericInput & input,
                                               const GenRingF::GenericResults & output ) const
{

  // load or create rings
  RichRecRings * rings     = getRings( m_ringLocation         );
  RichRecRings * bestrings = getRings( m_bestRingLocation     );
  RichRecRings * isorings  = getRings( m_isolatedRingLocation );

  debug() << "Found " << output.rings.size() << " Markov ring candidates" << endreq;

  // loop over final rings
  for ( GenRingF::GenericResults::GenericRings::const_iterator iRing = output.rings.begin();
        iRing != output.rings.end(); ++iRing )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Considering Markov Ring : " << *iRing << endreq;

    // Create a new Ring object
    RichRecRing * newRing = new RichRecRing();

    // Add pixels to this ring
    bool ringHasHits = false;
    for ( GenRingF::GenericInput::GenericHits::const_iterator iHit = input.hits.begin();
          iHit != input.hits.end(); ++iHit )
    {
      const double prob = output.inferrer->probabilityHitWasMadeByGivenCircle(iHit,iRing);
      if ( prob > m_minAssProb )
      {
        ringHasHits = true;
        RichRecPixel * pix = richPixels()->object( (*iHit).index().value() );
        if ( !pix ) return Error( "Markov hit has bad pixel pointer" );
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "  -> Adding pixel " << pix->key() << " to ring" << endreq;
        newRing->richRecPixels().push_back( RichRecPixelOnRing(pix,prob) );
      }
    }
    // are we going to keep this ring ?
    if ( newRing->richRecPixels().empty() )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> ring has no good hits -> rejecting" << endreq;
      delete newRing;
      continue;
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> ring has " << newRing->richRecPixels().size()
                  << " good hits -> keeping" << endreq;

      // insert in Gaudi container
      rings->insert( newRing );

      // set type info
      newRing->setType(RichRecRing::TracklessRing);
      newRing->setAlgorithm(RichRecRing::Markov);

      // Set detector information
      newRing->setRich  ( rich()  );
      newRing->setPanel ( panel() );

      // set ring type info
      newRing->setType      ( RichRecRing::TracklessRing );
      newRing->setAlgorithm ( RichRecRing::Markov        );

      // get ring centre, scaled back to local coords
      const double scaledX = (*iRing).x() / m_scaleFactor;
      const double scaledY = (*iRing).y() / m_scaleFactor;
      const Gaudi::XYZPoint centreLocal( scaledX, scaledY, 0 );
      newRing->setCentrePointLocal ( centreLocal );
      newRing->setCentrePointGlobal( m_smartIDTool->globalPosition( centreLocal, rich(), panel() ) );

      // ring radius
      newRing->setRadius ( (*iRing).radius() );

      // build the ring points
      buildRingPoints ( newRing );

      // match to a segment
      matchSegment ( newRing );

      // setup references to this ring in pixels
      addRingToPixels ( newRing );

      // average ring hit prob
      const double avProb = avRingHitProb(newRing);

      // If a "best" ring, clone into best container
      if ( newRing->richRecPixels().size() > m_minNumHitsBest &&
           avProb > m_minAvProbBest )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Ring selected as a 'best' ring" << endreq;
        bestrings->insert( new RichRecRing(*newRing) );
      }

      // If a "isolated" ring, clone into isolated container
      if ( newRing->richRecPixels().size() > m_minNumHitsIsolated &&
           avProb > m_minAvProbIsolated )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Ring selected as a 'isolated' ring" << endreq;
        isorings->insert( new RichRecRing(*newRing) );
      }

    } // ring OK

  } // loop over rings

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " -> Saved " << rings->size() << " rings at " << m_ringLocation << endreq;
    debug() << " -> Saved " << bestrings->size() << " rings at " << m_bestRingLocation << endreq;
    debug() << " -> Saved " << isorings->size() << " rings at " << m_isolatedRingLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

double
RichMarkovRingFinderAlg::avRingHitProb( LHCb::RichRecRing * ring ) const
{
  double prob(0);
  for ( RichRecPixelOnRing::Vector::iterator iP = ring->richRecPixels().begin();
        iP != ring->richRecPixels().end(); ++iP )
  {
    prob += (*iP).associationProb();
  }
  return ( ring->richRecPixels().empty() ? 0 : prob/(double)ring->richRecPixels().size() );
}

void RichMarkovRingFinderAlg::addRingToPixels( LHCb::RichRecRing * ring ) const
{
  verbose() << " -> Adding reference to ring " << ring->key() << " to pixels" << endreq;
  for ( RichRecPixelOnRing::Vector::iterator iP = ring->richRecPixels().begin();
        iP != ring->richRecPixels().end(); ++iP )
  {
    RichRecPixel * pix = (*iP).pixel();
    const double prob  = (*iP).associationProb();
    // CRJ : Need to remove this ...
    LHCb::RichRecPixel::RingsOnPixel* rings
      = const_cast<LHCb::RichRecPixel::RingsOnPixel*>(&pix->richRecRings());
    if ( rings )
    {
      if ( pix ) { rings->push_back( RichRecRingOnPixel(ring,prob) ); }
      else       { Exception( "Null pixel pointer in RichRecRing" ); }
    }
  }
}

bool RichMarkovRingFinderAlg::addDataPoints( GenRingF::GenericInput & input ) const
{
  bool OK = true;
  // Iterate over pixels
  const IPixelCreator::PixelRange range = pixelCreator()->range( rich(), panel() );
  if ( range.size() < m_maxHitsEvent )
  {
    input.hits.reserve( range.size() );
    for ( RichRecPixels::const_iterator iPix = range.begin(); iPix != range.end(); ++iPix )
    {
      // get X and Y
      const double X ( m_scaleFactor * (*iPix)->radCorrLocalPositions().position(rad()).x() );
      const double Y ( m_scaleFactor * (*iPix)->radCorrLocalPositions().position(rad()).y() );
      verbose() << "Adding data point at " << X << "," << Y << endreq;
      input.hits.push_back( GenRingF::GenericHit( GenRingF::GenericHitIndex((*iPix)->key()), X, Y ) );
    }
    debug() << "Selected " << input.hits.size() << " data points" << endreq;
  }
  else
  {
    std::ostringstream mess;
    mess << "# selected hits in " << Rich::text(rich()) << " " << Rich::text(rich(),panel()) 
         << " exceeded maximum of " << m_maxHitsEvent << " -> Processing aborted";
    Warning( mess.str(), StatusCode::SUCCESS );
    OK = false;
  }
  return OK;
}

void RichMarkovRingFinderAlg::buildRingPoints( RichRecRing * ring,
                                               const unsigned int nPoints ) const
{
  // NB : Much of this could be optimised and run in the initialisation
  const double incr ( 2.0 * M_PI / static_cast<double>(nPoints) );
  double angle(0);
  for ( unsigned int iP = 0; iP < nPoints; ++iP, angle += incr )
  {
    const double X( ring->centrePointLocal().x() + (sin(angle)*ring->radius())/m_scaleFactor);
    const double Y( ring->centrePointLocal().y() + (cos(angle)*ring->radius())/m_scaleFactor);
    const Gaudi::XYZPoint pLocal ( X, Y, 0*Gaudi::Units::cm );
    ring->ringPoints().push_back( RichRecPointOnRing( m_smartIDTool->globalPosition(pLocal,rich(),panel()),
                                                      pLocal,
                                                      RichSmartID(rich(),panel()),
                                                      RichRecPointOnRing::InHPDTube ) // to be improved !! Temp hack to get these rings drawn. Need to fix properly here or update Vis/SoRichRec properly
                                  );
  }
  verbose() << " -> Added " << ring->ringPoints().size() << " space points to ring" << endreq;
}

// TODO
void RichMarkovRingFinderAlg::matchSegment( LHCb::RichRecRing * /* ring */ ) const
{
  verbose() << " -> Finding segment matched to ring" << endreq;
  /*
    RichRecSegment* chosenSeg(0);
    double currentBestNormSep( boost::numeric::bounds<double>::highest() );
    for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
    iSeg != richSegments()->end(); ++iSeg )
    {
    }
  */
}

void RichMarkovRingFinderAlg::lesterMessage( const Lester::MessageLevel level,
                                             const std::string & message ) const
{
  if      ( Lester::VERBOSE == level )
  {
    verbose() << message << endreq;
  }
  else if ( Lester::DEBUG == level )
  {
    debug() << message << endreq;
  }
  else if ( Lester::INFO == level )
  {
    info() << message << endreq;
  }
  else if ( Lester::WARNING == level )
  {
    Warning( message, StatusCode::SUCCESS, 3 );
  }
  else if ( Lester::ERROR == level )
  {
    Error( message, StatusCode::FAILURE, 3 );
  }
  else if ( Lester::FATAL == level )
  {
    Exception( message );
  }
}

StatusCode RichMarkovRingFinderAlg::dumpToTextfile() const
{
  if ( m_dumpText )
  {
    // file number
    static unsigned int nFile = 0;
    ++nFile;
    // file name
    std::ostringstream filename;
    filename << Rich::text(rich()) << "-data" << nFile << ".txt";
    // open file
    info() << "Creating data text file : " << filename.str() << endreq;
    std::ofstream file(filename.str().c_str(),std::ios::app);
    // Iterate over pixels
    const IPixelCreator::PixelRange range = pixelCreator()->range( rich(), panel() );
    for ( RichRecPixels::const_iterator iPix = range.begin(); iPix != range.end(); ++iPix )
    {
      // get X and Y
      const double X ( m_scaleFactor * (*iPix)->radCorrLocalPositions().position(rad()).x() );
      const double Y ( m_scaleFactor * (*iPix)->radCorrLocalPositions().position(rad()).y() );
      file << X << " " << Y << std::endl;
    }
  }
  return StatusCode::SUCCESS;
}
