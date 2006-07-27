
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.cpp
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.cpp,v 1.20 2006-07-27 20:15:25 jonrob Exp $
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
    m_rad          ( rad   )
{
  declareProperty( "RingLocation", m_ringLocation = RichRecRingLocation::MarkovRings );
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

  info() << "Will find rings in " << rich() << " " << panel()
         << " for " << rad() << endreq;

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichMarkovRingFinderAlg::finalize()
{

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

    // Make a sampler ( should use factory here )
    CrudeSampler c;

    // run the fit
    const GenRingF::GenericResults output = c.fit(input);

    // finalise the results
    const StatusCode saveSc = saveRings(output);
    if ( saveSc.isFailure() ) return saveSc;

  }
  catch ( const GenRingF::GenericRingFinder::CouldNotFit & excpt )
  {
    Warning( "Ring Finding Failed with message : " + std::string(excpt.what()),
             StatusCode::SUCCESS );
  }

  return StatusCode::SUCCESS;
}

StatusCode RichMarkovRingFinderAlg::saveRings( const GenRingF::GenericResults & output ) const
{

  // load or create rings
  RichRecRings * rings;
  if ( exist<RichRecRings>(m_ringLocation) )
  {
    rings = get<RichRecRings>(m_ringLocation);
    debug() << "Loaded  RichRecRing container at " << m_ringLocation << endreq;
  }
  else
  {
    rings = new RichRecRings();
    put ( rings, m_ringLocation );
    debug() << "Created RichRecRing container at " << m_ringLocation << endreq;
  }

  // get the final results
  const Lester::RichParams currentPoint(output);
  debug() << "Final answer was : " << currentPoint << endreq;

  // loop over final rings
  for ( GenRingF::GenericResults::GenericRings::const_iterator iRing = output.rings.begin();
        iRing != output.rings.end();
        ++iRing )
  {
    // Create a new Ring object
    RichRecRing * newRing = new RichRecRing();
    // insert in Gaudi container
    rings->insert( newRing );

    // Set detector information
    newRing->setRich  ( rich()  );
    newRing->setPanel ( panel() );

    // ring centre point
    const Gaudi::XYZPoint centreLocal( (*iRing).x(), (*iRing).y(), 0 );
    newRing->setCentrePointLocal ( centreLocal );
    newRing->setCentrePointGlobal( m_smartIDTool->globalPosition( centreLocal, rich(), panel() ) );

    // ring radius
    newRing->setRadius ( (*iRing).radius() );

    // scale factor
    const double scale = Lester::Constants::realDataInputScaleFactor;

    // build the ring points
    buildRingPoints ( newRing, scale );

    // Add pixels to this ring
    //for ( HitsOnCircle::const_iterator hIt = hitsOnCircle.begin(); hIt != hitsOnCircle.end(); ++hIt ) {
    //  newRing->addToRichRecPixels( (*hIt)->richPixel() );
    // }

    // match to a segment
    matchSegment ( newRing );

  }

  return StatusCode::SUCCESS;
}

StatusCode RichMarkovRingFinderAlg::addDataPoints( GenRingF::GenericInput & input ) const
{

  // scale factor
  const double scale = Lester::Constants::realDataInputScaleFactor;

  // Iterate over pixels
  RichRecPixels::const_iterator iPix   ( pixelCreator()->begin ( rich(), panel() ) );
  RichRecPixels::const_iterator endPix ( pixelCreator()->end   ( rich(), panel() ) );
  for ( ; iPix != endPix; ++iPix )
  {
    // get X and Y
    const double X ( scale * (*iPix)->localPosition(rad()).x() );
    const double Y ( scale * (*iPix)->localPosition(rad()).y() );

    verbose() << "Adding data point at " << X << "," << Y << endreq;
    input.hits.push_back( GenRingF::GenericHit( (*iPix)->key(), X, Y ) );
  }

  debug() << "Selected " << input.hits.size() << " data points" << endreq;

  return StatusCode::SUCCESS;
}

void RichMarkovRingFinderAlg::buildRingPoints( RichRecRing * ring,
                                               const double scale,
                                               const unsigned int nPoints ) const
{
  // NB : Much of this could be optimised and run in the initialisation
  const double incr ( twopi / static_cast<double>(nPoints) );
  double angle(0);
  for ( unsigned int iP = 0; iP < nPoints; ++iP, angle += incr ) 
  {
    double distortedX( ring->centrePointLocal().x() + (sin(angle)*ring->radius())/scale);
    double distortedY( ring->centrePointLocal().y() + (cos(angle)*ring->radius())/scale);
    // CRJ : Need to correct for distortions here
    //if (m_useDistortedRings && Rich::Rich2 == m_rich) {
    //  distortedX /= 1 + 2.5/131.0;
    //  distortedY /= 1 - 2.5/131.0;
    // }
    const Gaudi::XYZPoint pLocal ( distortedX, distortedY, 0 );
    ring->ringPoints().push_back( m_smartIDTool->globalPosition(pLocal,rich(),panel()) );
  }
}

void RichMarkovRingFinderAlg::matchSegment( LHCb::RichRecRing * ring ) const
{
  RichRecSegment* chosenSeg(0);
  double currentBestNormSep( boost::numeric::bounds<double>::highest() );
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin(); 
        iSeg != richSegments()->end(); ++iSeg ) 
  {
  }
}

//=============================================================================
