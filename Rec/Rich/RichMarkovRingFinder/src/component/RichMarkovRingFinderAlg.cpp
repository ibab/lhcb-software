// $Id: RichMarkovRingFinderAlg.cpp,v 1.12 2004-10-22 19:15:28 abuckley Exp $
// Include files

// local
#include "RichMarkovRingFinderAlg.h"
#include "finder/Inferrer.h"

using namespace RichMarkov;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : RichMarkovRingFinderAlg
//
// 2003-12-02 : Chris Lester, Chris Jones, Andy Buckley
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factories
static const  AlgFactory< Rich1UMarkovRingFinderAlg >        s_factoryR1U;
const        IAlgFactory& Rich1UMarkovRingFinderAlgFactory = s_factoryR1U;
static const  AlgFactory< Rich1LMarkovRingFinderAlg >        s_factoryR1L;
const        IAlgFactory& Rich1LMarkovRingFinderAlgFactory = s_factoryR1L;
static const  AlgFactory< Rich2RMarkovRingFinderAlg >        s_factoryR2R;
const        IAlgFactory& Rich2RMarkovRingFinderAlgFactory = s_factoryR2R;
static const  AlgFactory< Rich2LMarkovRingFinderAlg >        s_factoryR2L;
const        IAlgFactory& Rich2LMarkovRingFinderAlgFactory = s_factoryR2L;



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
template <class MyFinder>
RichMarkovRingFinderAlg<MyFinder>::RichMarkovRingFinderAlg( const std::string& name,
                                                            ISvcLocator* pSvcLocator,
                                                            const Rich::DetectorType rich,
                                                            const Rich::Side panel )
  : RichRecAlgBase ( name, pSvcLocator ),
    m_ckAngle      ( 0     ),
    m_smartIDTool  ( 0     ),
    m_useRichSeed  ( true  ),
    m_rich         ( rich  ),
    m_panel        ( panel ),
    m_finder       ( 0     )
{
  declareProperty( "RingLocation", m_ringLocation = RichRecRingLocation::MarkovRings );
  declareProperty( "MatchedRingLocation", m_matchedRingLocation = "Rec/Rich/Markov/MatchedRings" );
  declareProperty( "PixelsLocation", m_pixelLocation = "Rec/Rich/Markov/Pixels" );
  declareProperty( "MatchedPixelsLocation", m_matchedPixelLocation = "Rec/Rich/Markov/MatchedPixels" );
  declareProperty( "InitialiseUsingRich", m_useRichSeed = true );
  declareProperty( "HitOnCircleProbCutoff", m_CutoffHitOnCircleProbability = 0.1 );
  declareProperty( "SegPositionInRingCutoff", m_CutoffSegPositionInRing = 0.5 );
  declareProperty( "HitIsBgProbabilityLowCut", m_HitIsBgProbabilityLowCut = 0.5 );
}



//=============================================================================
// Destructor
//=============================================================================
template <class MyFinder>
RichMarkovRingFinderAlg<MyFinder>::~RichMarkovRingFinderAlg() {}



//=============================================================================
// Initialisation
//=============================================================================
template <class MyFinder>
StatusCode RichMarkovRingFinderAlg<MyFinder>::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle     );
  acquireTool( "RichSmartIDTool",    m_smartIDTool );

  // Put Markov code in try block
  try {

    // Make a run initialisation object
    typedef std::pair<Rich::DetectorType,int> InitType;
    typename MyFinder::RunInitialisationObject & rio =
      MyFinder::theRunInitialisationObjectFor(InitType(m_rich, m_panel));

    const unsigned int myFavNumOfIts = 2000;
    rio.setStoppingCondition( typename MyFinder::RunForFiniteNumberOfSteps(myFavNumOfIts) );

    // Make a new finder for this algorithm
    m_finder = new MyFinder( rio );

  }
  catch (FinderExternalException& fee) {
    return Error( "Caught FinderExternalException of type " +
                  (std::string)typeid(fee).name() + " : '" + fee.message() + "'" );
  }
  catch (FinderException& fee) {
    return Error( "Caught exception of type " + (std::string)typeid(fee).name() );
  }
  catch (...) {
    return Error( "Caught unknown exception from MarkovRingFinder" );
  }

  // Configuration debug messages
  debug() << " Configured for " << m_rich << " panel " << m_panel << endreq;
  if ( m_useRichSeed ) {
    debug() << " Initialisation from track data" << endreq;
  } else {
    debug() << " Initialisation from random rings" << endreq;
  }

  // Some warning messages about things to do...
  warning() << "Remove private use of CLHEP random number engine." << endreq
            << "Remove assertions and cout/cerr statements" << endreq;

  return StatusCode::SUCCESS;
}



//=============================================================================
// Main execution
//=============================================================================
template <class MyFinder>
StatusCode RichMarkovRingFinderAlg<MyFinder>::execute() {

  debug() << "Execute" << endreq;

  // Set Rich event status to OK for start of Markov processing
  richStatus()->setEventOK(true);

  // First, check if track processing was aborted.
  // NB : Need to review what to do, since in principle can proceed without tracks
  // question is, is this useful ?
  ProcStatus * procStat = get<ProcStatus>( ProcStatusLocation::Default );
  if ( procStat->aborted() ) {
    info() << "Processing aborted -> RICH Markov Ring Finder aborted" << endreq;
    procStat->addAlgorithmStatus( "RichMarkovRingFinder", Rich::Rec::ProcStatAbort );
    richStatus()->setEventOK( false );
    return StatusCode::SUCCESS;
  }

  // Make sure RichRecPixels are available
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
  debug() << "Found " << richPixels()->size() << " RichRecPixels" << endreq;

  // Make sure RichRecTracks and RichRecSegments are available
  if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
  debug() << "Found " << richTracks()->size() << " RichRecTracks" << endreq;

  // Finally, all is OK so process event
  try {
    return processEvent();
  }
  catch (FinderExternalException& fee) {
    return Error( "Caught FinderExternalException of type " +
                  (std::string)typeid(fee).name() + " : '" + fee.message() + "'" );
  }
  catch (FinderException& fee) {
    return Error( "Caught exception of type " + (std::string)typeid(fee).name() );
  }
  catch (...) {
    return Error( "Caught unknown exception from MarkovRingFinder" );
  }

};



template <class MyFinder>
const StatusCode RichMarkovRingFinderAlg<MyFinder>::processEvent() {

  // Make event initialisation object
  typename MyFinder::EventInitialisationObject eio;

  // Add data points
  if ( !addDataPoints(eio) ) return StatusCode::FAILURE;

  // If required, add circle suggestions
  if ( m_useRichSeed ) { if ( !addCircleSuggestions(eio) ) return StatusCode::FAILURE; }

  // Initialise the finder for this event
  m_finder->initialiseEvent(eio);

  // Do the finding
  m_finder->doFit();

  // Get the final results
  const typename MyFinder::RichParamsT bestRPSoFar = m_finder->bestFit();
  debug() << "Found " << bestRPSoFar.getCircles().size() << " ring candidates" << endreq;


  // See if the ring containers already exist
  SmartDataPtr<RichRecRings> tesRings( eventSvc(), m_ringLocation );
  RichRecRings * rings(0);
  if ( !tesRings ) {
    // Make a new container of ring objects and register with Gaudi
    rings = new RichRecRings();
    put ( rings, m_ringLocation );
  } else {
    // ring container exists, so just add to it
    rings = tesRings;
  }
  SmartDataPtr<RichRecRings> tesMatchedRings( eventSvc(), m_matchedRingLocation );
  RichRecRings * matchedRings(0);
  if ( !tesMatchedRings ) {
    // Make a new container of ring objects and register with Gaudi
    matchedRings = new RichRecRings();
    put ( matchedRings, m_matchedRingLocation );
  } else {
    // ring container exists, so just add to it
    matchedRings = tesMatchedRings;
  }

  SmartDataPtr<RichRecPixels> tesPixels( eventSvc(), m_pixelLocation );
  RichRecPixels * pixels(0);
  if ( !tesPixels ) {
    // Make a new container of ring objects and register with Gaudi
    pixels = new RichRecPixels();
    put ( pixels, m_pixelLocation );
  } else {
    // ring container exists, so just add to it
    pixels = tesPixels;
  }
  SmartDataPtr<RichRecPixels> tesMatchedPixels( eventSvc(), m_matchedPixelLocation );
  RichRecPixels * matchedPixels(0);
  if ( !tesMatchedPixels ) {
    // Make a new container of ring objects and register with Gaudi
    matchedPixels = new RichRecPixels();
    put ( matchedPixels, m_matchedPixelLocation );
  } else {
    // ring container exists, so just add to it
    matchedPixels = tesMatchedPixels;
  }


  // Create final rings
  const typename MyFinder::DataT & myData = eio.data();
  typedef typename MyFinder::InferrerT MyInferrer;
  const MyInferrer inf(myData, bestRPSoFar);
  const double scale = m_finder->getmode().realXYDataInputScaleFactor();

  for ( typename MyFinder::RichParamsT::Circs::const_iterator iCircle = bestRPSoFar.getCircles().begin();
        iCircle != bestRPSoFar.getCircles().end(); ++iCircle ) {

    typedef vector<const Hit*> HitsOnCircle;
    HitsOnCircle hitsOnCircle;

    for ( typename MyFinder::DataT::Hits::const_iterator hIt = eio.data().hits.begin();
          hIt != eio.data().hits.end(); ++hIt ) {
      // Semi-arbitrary cut on prob that hit is associated to a ring: needs
      // Markov-assigned probability to be greater than this for the hit to
      // be associated with that RichRecRing

      // Not good on its own since the probability is shared between rings
      // if ( prob > m_CutoffHitOnCircleProbability )

      const double prob( inf.probabilityHitWasMadeByGivenCircle(hIt, iCircle) );
      const double bgprob( inf.probabilityHitWasMadeBySomethingOtherThanACircle(hIt) );

      if ( bgprob < m_HitIsBgProbabilityLowCut ) {
        // Loop over all the circles to see if any match this hit better than the current ring
        bool thisIsTheBestCircle(true);
        for ( typename MyFinder::RichParamsT::Circs::const_iterator iCircle2 = bestRPSoFar.getCircles().begin();
              iCircle2 != bestRPSoFar.getCircles().end(); ++iCircle2 ) {
          if ( iCircle != iCircle2 &&
               prob < inf.probabilityHitWasMadeByGivenCircle(hIt, iCircle2) ) 
            { thisIsTheBestCircle = false; }
        }
        if (thisIsTheBestCircle) {
          debug() << "Found a best circle for hit " << &(*hIt) << ": " << &(*iCircle) << endreq;
          hitsOnCircle.push_back(&(*hIt));
        }
        
        // or with a likelihood for this specific ring (ignoring others)
        //const double likelihoodOfHitFromThisCircle( m_finder->priorProbabilityOfHitDueToCircle( *hIt, *iCircle ) );
        //const double m_CutoffHitOnCircleLikelihood(0.5); // move this to a class variable
        //debug() << "Likelihood of hit being from this circle: " << likelihoodOfHitFromThisCircle << endreq;
        //if ( likelihoodOfHitFromThisCircle > m_CutoffHitOnCircleLikelihood )
        //
      }

    } // loop over hits


    if (!hitsOnCircle.empty()) {
      debug() << "Creating RichRecRing with " << hitsOnCircle.size() << " pixels" << endreq;

      // New ring for this result
      RichRecRing * newRing = new RichRecRing();
      // insert in container
      rings->insert( newRing );
      
      
      // set center point and radius (same units as cherenkov angle)
      const HepPoint3D centreLocal ( (*iCircle).centre().x()/scale,
                                     (*iCircle).centre().y()/scale,
                                     0 );
      newRing->setCentrePointLocal  ( centreLocal  );
      newRing->setCentrePointGlobal ( m_smartIDTool->globalPosition( centreLocal, rich(), panel() ) );

      newRing->setRadius ( (*iCircle).radius() );
      const double ringRadiusOnPDPlane( newRing->radius() / scale );

      // Set detector information
      newRing->setRich  ( rich()  );
      newRing->setPanel ( panel() );

      // Add pixels to this ring
      for ( HitsOnCircle::const_iterator hIt = hitsOnCircle.begin();
            hIt != hitsOnCircle.end(); ++hIt ) {
        newRing->addToRichRecPixels( (*hIt)->richPixel() );
      }

      // build the ring points
      buildRingPoints ( newRing, scale );

      // Add the Markov pixels to the TES
      for (SmartRefVector<RichRecPixel>::const_iterator pix = newRing->richRecPixels().begin();
           pix != newRing->richRecPixels().end(); ++pix) {
        if ( !pixels->object( (*pix)->key() ) )
          pixels->insert( new RichRecPixel( **pix ), (*pix)->key() );
      }

      // Identify which rings have no associated track within them
      RichRecSegment* chosenSeg(0);
      double currentBestNormSeparation( static_cast<double>( std::numeric_limits<int>::max() ) );
      for ( RichRecSegments::const_iterator iSeg = richSegments()->begin(); iSeg != richSegments()->end(); ++iSeg ) {
        if (*iSeg) {
          // Find the PD panel point corresponding to the track projection
          const HepPoint3D & segPoint = (*iSeg)->pdPanelHitPointLocal();
          
          // Skip if ring and segment RICH types don't match
          if ( rich() != (*iSeg)->trackSegment().rich() ) continue;
          
          // Do some ring-segment matching...
          const double normSeparation( segPoint.distance(centreLocal) / ringRadiusOnPDPlane );
          // If the projected segment lies within the specified fraction of the ring radius ...
          if (normSeparation < m_CutoffSegPositionInRing) {
            // ... and it's a better match than we've seen so far ...
            if (normSeparation < currentBestNormSeparation) {
              // ... then we'll select it!
              chosenSeg = *iSeg;
              currentBestNormSeparation = normSeparation;
            }
          }
        }
      }

      // set segment
      newRing->setRichRecSegment( chosenSeg );

      // Add matched rings and pixels to TES containers
      if ( newRing->richRecSegment() ) {
        matchedRings->insert( new RichRecRing( *newRing ), newRing->key() );
        for (SmartRefVector<RichRecPixel>::const_iterator matchpix = newRing->richRecPixels().begin();
             matchpix != newRing->richRecPixels().end(); ++matchpix) {
          if ( !matchedPixels->object( (*matchpix)->key() ) )
            matchedPixels->insert( new RichRecPixel( **matchpix ), (*matchpix)->key() );
        }
      }

    }

  } // loop over circles

  return StatusCode::SUCCESS;
}



template <class MyFinder>
void RichMarkovRingFinderAlg<MyFinder>::buildRingPoints( RichRecRing * ring,
                                                         const double scale,
                                                         const unsigned int nPoints ) const
{
  // NB : Much of this could be optimised and run in the initialisation
  const double incr = M_2PI / static_cast<double>(nPoints);
  double angle = 0;
  for ( unsigned int iP = 0; iP < nPoints; ++iP, angle += incr ) {
    const HepPoint3D pLocal ( ring->centrePointLocal().x() + (sin(angle)*ring->radius())/scale,
                              ring->centrePointLocal().y() + (cos(angle)*ring->radius())/scale,
                              0 );
    ring->ringPoints().push_back( m_smartIDTool->globalPosition(pLocal,rich(),panel()) );
  } 
}



template <class MyFinder>
template <class AnInitialisationObject>
const StatusCode
RichMarkovRingFinderAlg<MyFinder>::addCircleSuggestions( AnInitialisationObject & eio ) const
{

  // Iterate over segments and form vector of first guess rings
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
    if ( inCorrectArea(*iSeg) ) {
      const double radius = m_ckAngle->avgCherenkovTheta(*iSeg);
      if ( radius > 1e-10 ) {
        const HepVector3D & hitPnt = (*iSeg)->pdPanelHitPointLocal();
        const Hep2Vector centre( m_finder->getmode().realXYDataInputScaleFactor() * hitPnt.x(),
                                 m_finder->getmode().realXYDataInputScaleFactor() * hitPnt.y() );
        debug() << "Starting circle (" << centre.x() << "," << centre.y()
                << ") r=" << radius << endreq;
        eio.addCircleSuggestion( typename MyFinder::CircleParamsT(centre, radius) );
      }
    }
  } // loop over segments

  debug() << "Created " << eio.data().secretCircles().size()
          << " starting rings from RichRecSegments" << endreq;

  return StatusCode::SUCCESS;
}



template <class MyFinder>
template <class AnInitialisationObject>
const StatusCode
RichMarkovRingFinderAlg<MyFinder>::addDataPoints( AnInitialisationObject & eio ) const
{

  // Iterate over pixels
  const double scale = m_finder->getmode().realXYDataInputScaleFactor();
  for ( RichRecPixels::const_iterator iPix = richPixels()->begin();
        iPix != richPixels()->end(); ++iPix ) {
    if ( inCorrectArea(*iPix) ) {
      debug() << "Adding data point at " << scale * (*iPix)->localPosition().x()
              << "," << scale * (*iPix)->localPosition().y() << endreq;
      eio.addHit( Hit( scale * (*iPix)->localPosition().x(),
                       scale * (*iPix)->localPosition().y(), *iPix ) );
    };
  };

  debug() << "Selected " << eio.data().getHits().size() << " data points" << endreq;

  return StatusCode::SUCCESS;
}



template <class MyFinder>
const bool
RichMarkovRingFinderAlg<MyFinder>::inCorrectArea( const RichRecSegment * segment ) const
{
  const Rich::DetectorType rich = segment->trackSegment().rich();
  if ( m_rich != rich ) return false;
  const HepVector3D & pnt = segment->pdPanelHitPoint();
  return ( rich == Rich::Rich2 ?
           ( m_panel == Rich::left  ? pnt.x() > 0 : pnt.x() < 0 ) :
           ( m_panel == Rich::top   ? pnt.y() > 0 : pnt.y() < 0 ) );
}



template <class MyFinder>
const bool
RichMarkovRingFinderAlg<MyFinder>::inCorrectArea( const RichRecPixel * pixel ) const
{
  return ( m_rich  == static_cast<Rich::DetectorType>( pixel->smartID().rich()) &&
           m_panel == pixel->smartID().panel() );
}



//=============================================================================
//  Finalize
//=============================================================================
template <class MyFinder>
StatusCode RichMarkovRingFinderAlg<MyFinder>::finalize()
{
  // remove ring finder
  if ( m_finder ) { delete m_finder; m_finder = NULL; }

  // Execute base class method
  return RichRecAlgBase::finalize();
}

//=============================================================================
