
//-----------------------------------------------------------------------------
/** @file RichENNRingFinderAlg.cpp
 *
 *  Header file for algorithm : RichENNRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichENNRingFinderAlg.cpp,v 1.1.1.1 2009-05-21 17:34:14 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

// local
#include "RichENNRingFinderAlg.h"

// namespaces
using namespace Rich::Rec::ENNRingFinder;

// Declaration of the Algorithm Factories
DECLARE_ALGORITHM_FACTORY( Rich1TopPanel    );
DECLARE_ALGORITHM_FACTORY( Rich1BottomPanel );
DECLARE_ALGORITHM_FACTORY( Rich2LeftPanel   );
DECLARE_ALGORITHM_FACTORY( Rich2RightPanel  );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
template < class FINDER >
AlgBase<FINDER>::AlgBase( const std::string& name,
                          ISvcLocator* pSvcLocator,
                          const Rich::DetectorType rich,
                          const Rich::Side         panel,
                          const Rich::RadiatorType rad )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_smartIDTool       ( NULL  ),
    m_rich              ( rich  ),
    m_panel             ( panel ),
    m_rad               ( rad   ),
    m_finder            ( NULL  )
{
  // Set RICH specific parameters
  if ( Rich::Rich2 == rich )
  {
    m_scaleFactor      = 0.0283/128.0;
    m_minAssProb       = 0.05;
    m_maxHitsEvent     = 300;
    m_maxHitsHPD       = 30;
    m_maxPixelSep      = 260;
    m_hitSigma         = 5.0;
    m_minHitsPerRing   = 8;
    m_minRingRadius    = 150.0;
    m_maxRingRadius    = 75.0;
  }
  else // RICH1
  {
    m_scaleFactor      = 0.047/64.0; // CRJ : TO BE CHECKED
    m_minAssProb       = 0.05;
    m_maxHitsEvent     = 300;
    m_maxHitsHPD       = 30;
    m_maxPixelSep      = 150; // CRJ : TO BE CHECKED
    m_hitSigma         = 5.0;
    m_minHitsPerRing   = 8;
    m_minRingRadius    = 75.0;
    m_maxRingRadius    = 45.0;
  }
  // JOs
  declareProperty( "RingLocation",
                   m_ringLocation = LHCb::RichRecRingLocation::ENNRings+"All" );
  declareProperty( "MinAssociationProb",   m_minAssProb        );
  declareProperty( "MaxHitsInEvent",       m_maxHitsEvent      );
  declareProperty( "MaxHitsInHPD",         m_maxHitsHPD        );
  declareProperty( "ScaleFactor",          m_scaleFactor       );
  declareProperty( "MaxPixelDistFromRing", m_maxPixelSep       );
  // Disable histograms by default
  setProduceHistos( false );
}

//=============================================================================
// Destructor
//=============================================================================
template < class FINDER >
AlgBase<FINDER>::~AlgBase() { }

//=============================================================================
// Initialization
//=============================================================================
template < class FINDER >
StatusCode AlgBase<FINDER>::initialize()
{
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );

  // Each instance of this algorithm has its own finder. Must delete when finished
  m_finder = new FINDER( msgLevel(MSG::DEBUG) );

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
template < class FINDER >
StatusCode AlgBase<FINDER>::finalize()
{
  // clean up
  delete m_finder;
  m_finder = NULL;
  // return
  return RichRecHistoAlgBase::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
template < class FINDER >
StatusCode AlgBase<FINDER>::execute()
{
  // make sure ring containers are always created
  getRings( m_ringLocation );

  // RICH init
  StatusCode sc = richInit();
  if ( sc.isFailure() ) return sc;

  // Ring finder
  return runRingFinder();
}

template < class FINDER >
StatusCode AlgBase<FINDER>::richInit()
{
  // Set Rich event status to OK for start of ENN processing
  richStatus()->setEventOK(true);

  // Make sure RichRecPixels are available
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
  debug() << "Found " << richPixels()->size() << " RichRecPixels" << endreq;

  return StatusCode::SUCCESS;
}

template < class FINDER >
StatusCode AlgBase<FINDER>::runRingFinder()
{
  StatusCode sc = StatusCode::SUCCESS;

  // clear the ring finder
  m_finder->clear();

  // add hits
  const bool OK = addDataPoints();
  if ( OK )
  {
    // hit selection was OK, so do the ring finding

    // run the fit
    m_finder->FindRings(m_hitSigma,m_minHitsPerRing,m_minRingRadius,m_maxRingRadius);

    // some plots on fit stats
    if ( produceHistos() )
    {
      // for later
    }

    // finalise the results as TES rings
    sc = saveRings();
    if ( sc.isFailure() ) return sc;

  }

  // count processed (or not) events
  counter("Processed Events") += (int)OK;

  return sc;
}

template < class FINDER >
StatusCode AlgBase<FINDER>::saveRings() const
{

  // load or create rings
  LHCb::RichRecRings * rings = getRings( m_ringLocation );

  debug() << "Found " << m_finder->rings().size() << " ENN ring candidates" << endreq;

  // loop over final rings
  for ( ENNRingFinder::Finder::Ring::Vector::const_iterator iRing = m_finder->rings().begin();
        iRing != m_finder->rings().end(); ++iRing )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Considering ENN Ring : " << *iRing << endreq;

    // Create a new Ring object
    LHCb::RichRecRing * newRing = new LHCb::RichRecRing();

    // Add pixels to this ring
    for ( ENNRingFinder::Finder::Hit::PtnVector::const_iterator iHit = (*iRing).Hits.begin();
          iHit != (*iRing).Hits.end(); ++iHit )
    {
      LHCb::RichRecPixel * pix = richPixels()->object( (*iHit)->key );
      newRing->richRecPixels().push_back( LHCb::RichRecPixelOnRing(pix,1.0) );
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "  -> Adding hit " << *iHit << " to ring" << endreq;
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

      // Set detector information
      newRing->setRich     ( rich()  );
      newRing->setPanel    ( panel() );
      newRing->setRadiator ( rad()   );

      // set ring type info
      newRing->setType      ( LHCb::RichRecRing::TracklessRing );
      newRing->setAlgorithm ( LHCb::RichRecRing::ENN           );

      // get ring centre, scaled back to local coords
      const double scaledX = (*iRing).x;
      const double scaledY = (*iRing).y;
      const Gaudi::XYZPoint centreLocal( scaledX, scaledY, 0 );
      newRing->setCentrePointLocal ( centreLocal );
      newRing->setCentrePointGlobal( m_smartIDTool->globalPosition( centreLocal, rich(), panel() ) );

      // ring radius
      newRing->setRadius ( (*iRing).radius() * m_scaleFactor );

      // build the ring points
      buildRingPoints ( newRing );

      // setup references to this ring in pixels
      addRingToPixels ( newRing );

    } // ring OK

  } // loop over rings

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " -> Saved " << rings->size() << " rings at " << m_ringLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

template < class FINDER >
void AlgBase<FINDER>::addRingToPixels( LHCb::RichRecRing * ring ) const
{
  verbose() << " -> Adding reference to ring " << ring->key() << " to pixels" << endreq;
  for ( LHCb::RichRecPixelOnRing::Vector::iterator iP = ring->richRecPixels().begin();
        iP != ring->richRecPixels().end(); ++iP )
  {
    LHCb::RichRecPixel * pix = (*iP).pixel();
    const double prob        = (*iP).associationProb();
    LHCb::RichRecPixel::RingsOnPixel* rings
      = const_cast<LHCb::RichRecPixel::RingsOnPixel*>(&pix->richRecRings());
    if ( rings )
    {
      if ( pix ) { rings->push_back( LHCb::RichRecRingOnPixel(ring,prob) ); }
      else       { Exception( "Null pixel pointer in RichRecRing" ); }
    }
  }
}

template < class FINDER >
bool AlgBase<FINDER>::addDataPoints( ) const
{
  bool OK = false;
  // Iterate over pixels
  const IPixelCreator::PixelRange range = pixelCreator()->range( rich(), panel() );
  if ( range.size() > m_maxHitsEvent )
  {
    std::ostringstream mess;
    mess << "# selected hits in " << Rich::text(rich()) << " " << Rich::text(rich(),panel())
         << " exceeded maximum of " << m_maxHitsEvent << " -> Processing aborted";
    debug() <<  mess.str() << endmsg;
  }
  else if ( range.size() < 3 )
  {
    debug() <<  "Too few hits (<3) to find any rings" << endmsg;
  }
  else
  {
    OK = true;
    m_finder->hits().reserve( range.size() );
    for ( LHCb::RichRecPixels::const_iterator iPix = range.begin(); iPix != range.end(); ++iPix )
    {
      // Is this HPD selected ?
      const unsigned int hitsInHPD = pixelCreator()->range( (*iPix)->hpd() ).size();
      if ( hitsInHPD < m_maxHitsHPD )
      {
        // get X and Y
        const double X ( (*iPix)->radCorrLocalPositions().position(rad()).x() );
        const double Y ( (*iPix)->radCorrLocalPositions().position(rad()).y() );
        m_finder->hits().push_back( typename FINDER::Hit( (*iPix)->key(), X, Y ) );
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Adding data point at " << X << "," << Y << endreq;
      }
      else
      {
        std::ostringstream mess;
        mess << "Skipping hits in " << (*iPix)->hpd();
        ++counter(mess.str());
      }
    }
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Selected " << m_finder->hits().size() << " data points for "
              << Rich::text(rich()) << " " << Rich::text(rich(),panel())
              << endreq;
  }

  return OK;
}

template < class FINDER >
void AlgBase<FINDER>::buildRingPoints( LHCb::RichRecRing * ring,
                                       const unsigned int nPoints ) const
{
  if ( nPoints>0 )
  {
    // NB : Much of this could be optimised and run in the initialisation
    const double incr ( 2.0 * M_PI / static_cast<double>(nPoints) );
    double angle(0);
    ring->ringPoints().reserve(nPoints);
    for ( unsigned int iP = 0; iP < nPoints; ++iP, angle += incr )
    {
      const double X( ring->centrePointLocal().x() + (std::sin(angle)*ring->radius())/m_scaleFactor );
      const double Y( ring->centrePointLocal().y() + (std::cos(angle)*ring->radius())/m_scaleFactor );
      const Gaudi::XYZPoint pLocal ( X, Y, 0*Gaudi::Units::cm );
      ring->ringPoints().push_back( LHCb::RichRecPointOnRing( m_smartIDTool->globalPosition(pLocal,
                                                                                            rich(),
                                                                                            panel()),
                                                              pLocal,
                                                              LHCb::RichSmartID(rich(),panel()),
                                                              LHCb::RichRecPointOnRing::InHPDTube )
                                    );
    }
  }
  if (msgLevel(MSG::VERBOSE))
    verbose() << " -> Added " << ring->ringPoints().size() << " space points to ring" << endreq;
}

