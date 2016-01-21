
//-----------------------------------------------------------------------------
/** @file RichENNRingFinderAlg.cpp
 *
 *  Header file for algorithm : RichENNRingFinderAlg
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
DECLARE_ALGORITHM_FACTORY( Rich1AerogelTopPanel    )
DECLARE_ALGORITHM_FACTORY( Rich1AerogelBottomPanel )
DECLARE_ALGORITHM_FACTORY( Rich1GasTopPanel        )
DECLARE_ALGORITHM_FACTORY( Rich1GasBottomPanel     )
DECLARE_ALGORITHM_FACTORY( Rich2GasLeftPanel       )
DECLARE_ALGORITHM_FACTORY( Rich2GasRightPanel      )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
template < class FINDER >
AlgBase<FINDER>::AlgBase( const std::string& name,
                          ISvcLocator* pSvcLocator,
                          const Rich::DetectorType rich,
                          const Rich::Side         panel,
                          const Rich::RadiatorType rad )
  : Rich::Rec::HistoAlgBase ( name, pSvcLocator ),
    m_rich                  ( rich  ),
    m_panel                 ( panel ),
    m_rad                   ( rad   )
{
  // Set RICH specific parameters
  if ( Rich::Rich2Gas == rad )
  {
    //m_scaleFactor      = 0.0283/128.0;
    m_scaleFactor      = 0.03/130.0;
    m_minAssProb       = 0.05;
    m_maxHitsEvent     = 300;
    m_maxHitsHPD       = 30;
    m_maxPixelSep      = 260;
    m_hitSigma         = 5.0;
    m_minHitsPerRing   = 8;
    m_minRingRadius    = 85.0;
    m_maxRingRadius    = 150.0;
    m_maxRingChi2      = 150.0;
    m_minRingPurity    = 0.7999;
    m_rejectionFactor  = 0.5;
  }
  else if ( Rich::Rich1Gas == rad )
  {
    //m_scaleFactor      = 0.047/64.0;
    m_scaleFactor      = 0.052/75.0;
    m_minAssProb       = 0.05;
    m_maxHitsEvent     = 300;
    m_maxHitsHPD       = 30;
    m_maxPixelSep      = 150;
    m_hitSigma         = 5.0;
    m_minHitsPerRing   = 8;
    m_minRingRadius    = 40.0;
    m_maxRingRadius    = 85.0;
    m_maxRingChi2      = 150.0;
    m_minRingPurity    = 0.7999;
    m_rejectionFactor  = 0.5;
  }
  else // Aerogel (Very Experimental .....)
  {
    m_scaleFactor      = 0.24/342.0;
    m_minAssProb       = 0.05;
    m_maxHitsEvent     = 300;
    m_maxHitsHPD       = 30;
    m_maxPixelSep      = 150;
    m_hitSigma         = 5.0;
    m_minHitsPerRing   = 5;
    m_minRingRadius    = 200.0;
    m_maxRingRadius    = 400.0;
    m_maxRingChi2      = 150.0;
    m_minRingPurity    = 0.7999;
    m_rejectionFactor  = 0.5;
  }
  // JOs
  declareProperty( "RingLocation",
                   m_ringLocation = LHCb::RichRecRingLocation::ENNRings+"All" );
  declareProperty( "MinAssociationProb",   m_minAssProb      );
  declareProperty( "MaxHitsInEvent",       m_maxHitsEvent    );
  declareProperty( "MaxHitsInHPD",         m_maxHitsHPD      );
  declareProperty( "ScaleFactor",          m_scaleFactor     );
  declareProperty( "MaxPixelDistFromRing", m_maxPixelSep     );
  declareProperty( "MaxRingChiSquared",    m_maxRingChi2     );
  declareProperty( "MinRingPurity",        m_minRingPurity   );
  declareProperty( "NoiseRejectionFactor", m_rejectionFactor );
  declareProperty( "BuildRingPoints",      m_buildRingPoints = true );
  declareProperty( "RefitRings",           m_refitRings      = true );
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
  const StatusCode sc = Rich::Rec::HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );

  // Each instance of this algorithm has its own finder. Must delete when finished
  typename FINDER::Config config ( m_hitSigma,
                                   m_minHitsPerRing,
                                   m_minRingRadius,
                                   m_maxRingRadius,
                                   m_rejectionFactor,
                                   msgLevel(MSG::VERBOSE) );
  m_finder = new FINDER(config);

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
  m_finder = nullptr;
  // return
  return Rich::Rec::HistoAlgBase::finalize();
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
  const StatusCode sc = richInit();
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
  _ri_debug << "Found in total " << richPixels()->size() << " RichRecPixels" << endmsg;

  return StatusCode::SUCCESS;
}

template < class FINDER >
StatusCode AlgBase<FINDER>::runRingFinder()
{
  StatusCode sc = StatusCode::SUCCESS;

  // clear the ring finder for a new event
  m_finder->clear();

  // add hits
  const bool OK = addDataPoints();
  if ( OK )
  {
    // hit selection was OK, so do the ring finding

    // run the fit
    m_finder->FindRings();

    // some plots on fit stats
    if ( produceHistos() )
    {
      // for later
    }

    // finalise the results as TES rings
    sc = saveRings();
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
  const unsigned int nRingsBefore = rings->size();

  _ri_debug << "Found " << m_finder->rings().size() << " ENN ring candidates" << endmsg;

  // loop over final rings
  for ( const auto & ring : m_finder->rings() )
  {
    _ri_verbo << "Considering ENN Ring : " << ring << endmsg;

    // Select rings
    if ( ring.chi2 > m_maxRingChi2 )
    {
      _ri_verbo << " -> Chi^2 too large -> reject " << endmsg;
      continue;
    }
    if ( ring.purity() <= m_minRingPurity )
    {
      _ri_verbo << " -> Purity too small -> reject " << endmsg;
      continue;
    }

    // Create a new Ring object
    auto * newRing = new LHCb::RichRecRing();

    // Add pixels to this ring
    _ri_verbo << "  Ring has " << ring.Hits.size() << " associated Hits" << endmsg;
    for ( const auto * hit : ring.Hits )
    {
      _ri_verbo << "  -> Associated hit " << *hit << endmsg;
      const double prob = m_finder->hitProbability(ring,*hit);
      if ( prob > m_minAssProb )
      {
        auto * pix = richPixels()->object( hit->key );
        newRing->richRecPixels().emplace_back( LHCb::RichRecPixelOnRing(pix,prob) );
      }
    }

    // are we going to keep this ring ?
    if ( newRing->richRecPixels().empty() )
    {
      _ri_verbo << " -> ring has no good hits -> rejecting" << endmsg;
      delete newRing;
      continue;
    }
    else
    {
      _ri_verbo << " -> ring has " << newRing->richRecPixels().size()
                << " good hits -> keeping" << endmsg;

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
      const Gaudi::XYZPoint centreLocal( ring.x, ring.y, 0 );
      newRing->setCentrePointLocal ( centreLocal );
      newRing->setCentrePointGlobal( m_smartIDTool->globalPosition( centreLocal, rich(), panel() ) );

      // get the radius on the detector plane
      auto radius = ring.radius();

      // refit the radius ?
      if ( m_refitRings ) refit( newRing, radius );

      // ring radius
      newRing->setRadius ( (LHCb::RichRecRing::FloatType) (radius*m_scaleFactor) );

      // build the ring points
      if ( m_buildRingPoints ) buildRingPoints ( newRing );

      // setup references to this ring in pixels
      addRingToPixels ( newRing );

    } // ring OK

  } // loop over rings

  _ri_debug << " -> Saved " << rings->size() - nRingsBefore
            << " rings at " << m_ringLocation << endmsg;

  // count # found rings per event
  counter("# "+Rich::text(rich())+" Found Rings") += (rings->size() - nRingsBefore);

  return StatusCode::SUCCESS;
}

template < class FINDER >
void AlgBase<FINDER>::addRingToPixels( LHCb::RichRecRing * ring ) const
{
  _ri_verbo << " -> Adding reference to ring " << ring->key() << " to pixels" << endmsg;
  for ( const auto & P : ring->richRecPixels() )
  {
    auto * pix = P.pixel();
    if ( pix )
    {
      LHCb::RichRecPixel::RingsOnPixel* rings
        = const_cast<LHCb::RichRecPixel::RingsOnPixel*>(&(pix->richRecRings()));
      if ( rings )
      {
        const double prob = P.associationProb();
        rings->emplace_back( LHCb::RichRecRingOnPixel(ring,prob) );
      }
    }
    else
    {
      Exception( "Null pixel pointer in RichRecRing" ); 
    }
  }
}

template < class FINDER >
bool AlgBase<FINDER>::addDataPoints( ) const
{
  bool OK = false;

  // Iterate over pixels
  const auto range = pixelCreator()->range( rich(), panel() );

  _ri_debug << "Found " << range.size() << " hits for " << rich()
            << " " << Rich::text(rich(),panel()) << endmsg;
  
  if ( range.size() < 3 )
  {
    _ri_debug <<  "Too few hits (<3) to find any rings" << endmsg;
  }
  else
  {

    m_finder->hits().reserve( range.size() );
    for ( const auto * pix : range )
    {
      // Is this HPD selected ?
      const unsigned int hitsInHPD = pixelCreator()->range( pix->hpd() ).size();
      if ( hitsInHPD < m_maxHitsHPD )
      {
        // get X and Y
        const double X ( pix->radCorrLocalPositions().position(rad()).x() );
        const double Y ( pix->radCorrLocalPositions().position(rad()).y() );
        m_finder->hits().emplace_back( typename FINDER::Hit( pix->key(), X, Y ) );
        _ri_verbo << " -> Adding data point ( " << X << " , " << Y << " )" << endmsg;
      }
      else
      {
        if ( msgLevel(MSG::DEBUG) )
        {
          std::ostringstream mess;
          mess << "Skipping hits in " << Rich::DAQ::HPDIdentifier(pix->hpd()).number();
          ++counter(mess.str());
        }
      }
    } // loop over pixels

    if ( m_finder->hits().size() > m_maxHitsEvent )
    {
      m_finder->hits().clear();
      if ( msgLevel(MSG::DEBUG) )
      {
        std::ostringstream mess;
        mess << "# hits in " << Rich::text(rich()) << " " << Rich::text(rich(),panel())
             << " exceeded maximum of " << m_maxHitsEvent << " -> Processing aborted";
        debug() <<  mess.str() << endmsg;
      }
    }
    else
    {
      OK = true;
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Selected " << m_finder->hits().size() << " data points for "
                << Rich::text(rich()) << " " << Rich::text(rich(),panel())
                << endmsg;
    }
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
    const double incr ( 2.0 * M_PI / (double)(nPoints) );
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
  _ri_verbo << " -> Added " << ring->ringPoints().size() << " space points to ring" << endmsg;
}

template < class FINDER >
void AlgBase<FINDER>::refit( LHCb::RichRecRing * ring,
                             double & radius ) const
{
  _ri_verbo << "Radius refitting before = " << radius << " OK = ";

  // make a fitter
  FastRingFitter fitter(*ring);

  // run the fit
  fitter.fit();

  // fit was OK so update radius
  const bool refitOK = ( fitter.result().Status == 0 );
  if ( refitOK ) radius = fitter.result().Radius;

  _ri_verbo << refitOK << " after = " << radius << endmsg;
}
