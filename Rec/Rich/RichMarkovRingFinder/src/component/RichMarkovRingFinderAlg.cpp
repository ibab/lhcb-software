
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.cpp
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.cpp,v 1.26 2006-08-03 23:26:56 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

// local
#include "RichMarkovRingFinderAlg.h"

// namespaces
using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Rich2LeftPanelMarkovRingFinderAlg  );
DECLARE_ALGORITHM_FACTORY( Rich2RightPanelMarkovRingFinderAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichMarkovRingFinderAlg::RichMarkovRingFinderAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator,
                                                  const Rich::DetectorType rich,
                                                  const Rich::Side panel,
                                                  const Rich::RadiatorType rad )
  : RichRecAlgBase ( name , pSvcLocator ),
    m_ckAngle      ( NULL  ),
    m_smartIDTool  ( NULL  ),
    m_rich         ( rich  ),
    m_panel        ( panel ),
    m_rad          ( rad   ),
    m_sampler      ( NULL  )
{
  declareProperty( "RingLocation",     m_ringLocation     = RichRecRingLocation::MarkovRings );
  declareProperty( "BestRingLocation", m_bestRingLocation = RichRecRingLocation::MarkovRings+"Best" );
  declareProperty( "DumpDataToTextFile",  m_dumpText       = false );
  declareProperty( "MinAssociationProb",  m_minAssProb     = 0.05 );
  declareProperty( "MinNumHitsBestRings", m_minNumHitsBest = 4 );
  declareProperty( "MinAvProbBestRings",  m_minAvProbBest  = 0.75 );
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
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle     );
  acquireTool( "RichSmartIDTool",    m_smartIDTool );

  // make a new sampler ( should use a factory here )
  m_sampler = new CrudeSampler();

  info() << "Will find rings in " << rich() << " " << panel()
         << " for " << rad() << endreq;

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichMarkovRingFinderAlg::finalize()
{
  // cleanup
  if ( m_sampler ) { delete m_sampler; m_sampler = NULL; }
  // return
  return RichRecAlgBase::finalize();
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
  sc = runRingFinder();
  if ( sc.isFailure() ) return sc;

  return StatusCode::SUCCESS;
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

  // do the finding in a try block to catch exceptions that leak out
  try
  {

    // Create input data object
    GenRingF::GenericInput input;

    // add hits
    const StatusCode dataAddSc = addDataPoints(input);
    if ( dataAddSc.isFailure() ) return dataAddSc;

    // run the fit
    boost::shared_ptr<GenRingF::GenericResults> outputP = m_sampler->fit(input);
    const GenRingF::GenericResults & output = *outputP;

    // finalise the results
    const StatusCode saveSc = saveRings(input,output);
    if ( saveSc.isFailure() ) return saveSc;

  }
  catch ( const GenRingF::GenericRingFinder::CouldNotFit & excpt )
  {
    Warning( "Ring Finding Failed with message : " + std::string(excpt.what()),
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
    debug() << "Loaded  RichRecRing container at " << location << endreq;
  }
  else
  {
    rings = new RichRecRings();
    put ( rings, location );
    debug() << "Created RichRecRing container at " << location << endreq;
  }
  return rings;
}

StatusCode RichMarkovRingFinderAlg::saveRings( const GenRingF::GenericInput & input,
                                               const GenRingF::GenericResults & output ) const
{

  // load or create rings
  RichRecRings * rings     = getRings( m_ringLocation     );
  RichRecRings * bestrings = getRings( m_bestRingLocation );

  // loop over final rings
  for ( GenRingF::GenericResults::GenericRings::const_iterator iRing = output.rings.begin();
        iRing != output.rings.end();
        ++iRing )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Considering Markov Ring : " << *iRing << endreq;
    }

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
        verbose() << "  -> Adding pixel " << pix->key() << " to ring" << endreq;
        newRing->richRecPixels().push_back( RichRecPixelOnRing(pix,prob) );
      }
    }
    // are we going to keep this ring ?
    if ( newRing->richRecPixels().empty() )
    {
      debug() << " -> ring has no good hits -> rejecting" << endreq;
      delete newRing;
      continue;
    }
    else
    {
      debug() << " -> ring has " << newRing->richRecPixels().size() << " good hits -> keeping" << endreq;

      // insert in Gaudi container
      rings->insert( newRing );

      // Set detector information
      newRing->setRich  ( rich()  );
      newRing->setPanel ( panel() );

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
      StatusCode sc = addRingToPixels ( newRing );
      if ( sc.isFailure() ) return sc;

      // If a "best" ring, clone into best container
      if ( newRing->richRecPixels().size() > m_minNumHitsBest &&
           avRingHitProb(newRing) > m_minAvProbBest )
      {
        debug() << " -> Ring selected as a 'best' ring" << endreq;
        RichRecRing * bestRing = new RichRecRing(*newRing);
        bestrings->insert( bestRing );
      }

    } // ring OK

  } // loop over rings

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

StatusCode RichMarkovRingFinderAlg::addRingToPixels( LHCb::RichRecRing * ring ) const
{
  debug() << " -> Adding reference to ring " << ring->key() << " to pixels" << endreq;
  for ( RichRecPixelOnRing::Vector::iterator iP = ring->richRecPixels().begin();
        iP != ring->richRecPixels().end(); ++iP )
  {
    RichRecPixel * pix = (*iP).pixel();
    const double prob  = (*iP).associationProb();
    verbose() << "  -> Found pixel " << pix->key() << " Found prob  " << prob << endreq;
    if ( pix )
    {
      pix->richRecRings().push_back( RichRecRingOnPixel(ring,prob) );
    }
    else
    {
      return Error( "Null pixel pointer in RichRecRing" );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode RichMarkovRingFinderAlg::addDataPoints( GenRingF::GenericInput & input ) const
{
  // Iterate over pixels
  RichRecPixels::const_iterator iPix   ( pixelCreator()->begin ( rich(), panel() ) );
  RichRecPixels::const_iterator endPix ( pixelCreator()->end   ( rich(), panel() ) );
  for ( ; iPix != endPix; ++iPix )
  {
    // get X and Y
    const double X ( m_scaleFactor * (*iPix)->localPosition(rad()).x() );
    const double Y ( m_scaleFactor * (*iPix)->localPosition(rad()).y() );
    verbose() << "Adding data point at " << X << "," << Y << endreq;
    input.hits.push_back( GenRingF::GenericHit( GenRingF::GenericHitIndex((*iPix)->key()), X, Y ) );
  }

  debug() << "Selected " << input.hits.size() << " data points" << endreq;

  return StatusCode::SUCCESS;
}

void RichMarkovRingFinderAlg::buildRingPoints( RichRecRing * ring,
                                               const unsigned int nPoints ) const
{
  debug() << " -> Adding space points to ring" << endreq;
  // NB : Much of this could be optimised and run in the initialisation
  const double incr ( twopi / static_cast<double>(nPoints) );
  double angle(0);
  for ( unsigned int iP = 0; iP < nPoints; ++iP, angle += incr )
  {
    const double X( ring->centrePointLocal().x() + (sin(angle)*ring->radius())/m_scaleFactor);
    const double Y( ring->centrePointLocal().y() + (cos(angle)*ring->radius())/m_scaleFactor);
    const Gaudi::XYZPoint pLocal ( X, Y, 0 );
    ring->ringPoints().push_back( m_smartIDTool->globalPosition(pLocal,rich(),panel()) );
  }
}

// TODO
void RichMarkovRingFinderAlg::matchSegment( LHCb::RichRecRing * /* ring */ ) const
{
  debug() << " -> Finding segment matched to ring" << endreq;
  /*
    RichRecSegment* chosenSeg(0);
    double currentBestNormSep( boost::numeric::bounds<double>::highest() );
    for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
    iSeg != richSegments()->end(); ++iSeg )
    {
    }
  */
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
    filename << rich() << "-data" << nFile << ".txt";
    // open file
    info() << "Creating data text file : " << filename.str() << endreq;
    std::ofstream file(filename.str().c_str(),std::ios::app);
    // Iterate over pixels
    RichRecPixels::const_iterator iPix   ( pixelCreator()->begin ( rich(), panel() ) );
    RichRecPixels::const_iterator endPix ( pixelCreator()->end   ( rich(), panel() ) );
    for ( ; iPix != endPix; ++iPix )
    {
      // get X and Y
      const double X ( m_scaleFactor * (*iPix)->localPosition(rad()).x() );
      const double Y ( m_scaleFactor * (*iPix)->localPosition(rad()).y() );
      file << X << " " << Y << std::endl;
    }
  }
  return StatusCode::SUCCESS;
}
