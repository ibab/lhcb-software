
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.cpp
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.cpp,v 1.68 2008-06-20 10:31:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

// local
#include "RichMarkovRingFinderAlg.h"

// namespaces
using namespace Rich::Rec::MarkovRingFinder;

// Declaration of the Algorithm Factories
DECLARE_ALGORITHM_FACTORY( Rich1TopPanel    );
DECLARE_ALGORITHM_FACTORY( Rich1BottomPanel );
DECLARE_ALGORITHM_FACTORY( Rich2LeftPanel   );
DECLARE_ALGORITHM_FACTORY( Rich2RightPanel  );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
template < class SAMPLER >
AlgBase<SAMPLER>::AlgBase( const std::string& name,
                           ISvcLocator* pSvcLocator,
                           const Rich::DetectorType rich,
                           const Rich::Side         panel,
                           const Rich::RadiatorType rad )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_smartIDTool       ( NULL  ),
    m_rich              ( rich  ),
    m_panel             ( panel ),
    m_rad               ( rad   ),
    m_sampler           ( NULL  )
{
  // Set RICH specific parameters
  if ( Rich::Rich2 == rich )
  {
    m_scaleFactor      = 0.0283/128.0;
    m_minAssProb       = 0.05;
    m_maxHitsEvent     = 300;
    m_maxHitsHPD       = 30;
    m_maxPixelSep      = 260;
    //m_TargetIterations = 1000;
    //m_TargetHits       = 250;
    //m_AbsMaxIts        = 20000;
    //m_AbsMinIts        = 400;
    m_TargetIterations = 2000;
    m_TargetHits       = 500;
    m_AbsMaxIts        = 20000;
    m_AbsMinIts        = 800;
  }
  else
  {
    m_scaleFactor      = 0.047/64.0; // CRJ : TO BE CHECKED
    m_minAssProb       = 0.05;
    m_maxHitsEvent     = 300;
    m_maxHitsHPD       = 30;
    m_maxPixelSep      = 150; // CRJ : TO BE CHECKED
    m_TargetIterations = 1000;
    m_TargetHits       = 250;
    m_AbsMaxIts        = 20000;
    m_AbsMinIts        = 400;
  }
  // JOs
  declareProperty( "RingLocation",
                   m_ringLocation = LHCb::RichRecRingLocation::MarkovRings+"All" );
  declareProperty( "DumpDataToTextFile",   m_dumpText        = false );
  declareProperty( "EnableFileCache",      m_enableFileCache = false );
  declareProperty( "MinAssociationProb",   m_minAssProb        );
  declareProperty( "MaxHitsInEvent",       m_maxHitsEvent      );
  declareProperty( "MaxHitsInHPD",         m_maxHitsHPD        );
  declareProperty( "ScaleFactor",          m_scaleFactor       );
  declareProperty( "MaxPixelDistFromRing", m_maxPixelSep       );
  declareProperty( "TargetIterations",     m_TargetIterations  );
  declareProperty( "TargetHits",           m_TargetHits        );
  declareProperty( "AbsMaxIts",            m_AbsMaxIts         );
  declareProperty( "AbsMinIts",            m_AbsMinIts         );
  // Disable histograms by default
  setProduceHistos( false );
  // data maps
  //declareProperty( "ApproxCoPointMap",     
  //m_coPointMap = DefaultDataMaps::defaultCoPointMap(rad);
  //);
}

//=============================================================================
// Destructor
//=============================================================================
template < class SAMPLER >
AlgBase<SAMPLER>::~AlgBase() { }

//=============================================================================
// Initialization
//=============================================================================
template < class SAMPLER >
StatusCode AlgBase<SAMPLER>::initialize()
{
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool",    m_smartIDTool );

  // Intercept initialisation messages from MC code
  Lester::messHandle().declare(this);

  // Only allow one instance of each sampler type
  // Note : with this method we should NOT delete the sampler when finishing
  //m_sampler = Lester::getInstance<SAMPLER>();
  // Each instance of this algorithm has its own sampler. Must delete when finished
  m_sampler = new SAMPLER();

  // configure sampler
  m_sampler->configuration.clearAllparams();
  m_sampler->configuration.setParam( "ScaleNumItsByHits", true );
  m_sampler->configuration.setParam( "TargetIterations", m_TargetIterations );
  m_sampler->configuration.setParam( "TargetHits",       m_TargetHits       );
  m_sampler->configuration.setParam( "AbsMaxIts",        m_AbsMaxIts        );
  m_sampler->configuration.setParam( "AbsMinIts",        m_AbsMinIts        );
  m_sampler->configuration.setParam( "EnableFileCache",  m_enableFileCache  );

  // initialise sampler
  m_sampler->initialise();

  info() << "Markov Chain Configuration : " << rich() << " " << panel()
         << " " << m_sampler->configuration << endreq;

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
template < class SAMPLER >
StatusCode AlgBase<SAMPLER>::finalize()
{
  // Intercept finalisation messages from MC code
  Lester::messHandle().declare(this);
  // clean up
  delete m_sampler;
  m_sampler = NULL;
  // No longer want messages
  Lester::messHandle().declare(NULL);
  // return
  return RichRecHistoAlgBase::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
template < class SAMPLER >
StatusCode AlgBase<SAMPLER>::execute()
{
  // make sure ring containers are always created
  getRings( m_ringLocation );

  // RICH init
  StatusCode sc = richInit();
  if ( sc.isFailure() ) return sc;

  // Dump data to text file ?
  sc = dumpToTextfile();
  if ( sc.isFailure() ) return sc;

  // Ring finder
  return runRingFinder();
}

template < class SAMPLER >
StatusCode AlgBase<SAMPLER>::richInit()
{
  // Set Rich event status to OK for start of Markov processing
  richStatus()->setEventOK(true);

  // Make sure RichRecPixels are available
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
  debug() << "Found " << richPixels()->size() << " RichRecPixels" << endreq;

  return StatusCode::SUCCESS;
}

template < class SAMPLER >
StatusCode AlgBase<SAMPLER>::runRingFinder()
{
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
      if ( produceHistos() )
      {
        const int maxTime = 2000;
        const int maxIts  = 2000;
        plot1D( output.numIterations, "Number of iterations",
                -0.5, maxIts+0.5, maxIts+1 );
        plot1D( output.timeTaken,     "Processin time(ms)", 0, maxTime );
        plot2D( output.numIterations, output.timeTaken, "time(ms) V #iterations | 2D",
                -0.5, maxIts+0.5,
                0, maxTime,
                maxIts+1, 100 );
        profile1D( output.numIterations, output.timeTaken, "time(ms) V #iterations | Profile",
                   -0.5, maxIts+0.5, maxIts+1 );
        plot2D( input.hits.size(), output.numIterations, "#iterations V #hits | 2D",
                -0.5, m_maxHitsEvent+0.5,
                -0.5, maxIts+0.5,
                m_maxHitsEvent+1, maxIts+1 );
        profile1D( input.hits.size(), output.numIterations, "#iterations V #hits | Profile",
                   -0.5, m_maxHitsEvent+0.5,
                   m_maxHitsEvent+1 );
        plot2D( input.hits.size(), output.timeTaken, "time(ms) V #hits | 2D",
                -0.5, m_maxHitsEvent+0.5,
                0, maxTime,
                m_maxHitsEvent+1, 100 );
        profile1D( input.hits.size(), output.timeTaken, "time(ms) V #hits | 2D",
                   -0.5, m_maxHitsEvent+0.5,
                   m_maxHitsEvent+1 );
      }

      // finalise the results as TES rings
      const StatusCode saveSc = saveRings(input,output);
      if ( saveSc.isFailure() ) return saveSc;

    }

    counter("Processed Events") += (int)OK;

  }
  catch ( const std::exception & excpt )
  {
    Warning( "Ring finding FAILED : " + std::string(excpt.what()),
             StatusCode::SUCCESS );
  }

  return StatusCode::SUCCESS;
}

template < class SAMPLER >
StatusCode AlgBase<SAMPLER>::saveRings( const GenRingF::GenericInput & input,
                                        const GenRingF::GenericResults & output ) const
{

  // load or create rings
  LHCb::RichRecRings * rings = getRings( m_ringLocation );

  debug() << "Found " << output.rings.size() << " Markov ring candidates" << endreq;

  // loop over final rings
  for ( GenRingF::GenericResults::GenericRings::const_iterator iRing = output.rings.begin();
        iRing != output.rings.end(); ++iRing )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Considering Markov Ring : " << *iRing << endreq;

    // reference to generic ring
    const GenRingF::GenericRing & gen_ring = *iRing;

    // Create a new Ring object
    LHCb::RichRecRing * newRing = new LHCb::RichRecRing();

    // Add pixels to this ring
    bool ringHasHits = false;
    for ( GenRingF::GenericInput::GenericHits::const_iterator iHit = input.hits.begin();
          iHit != input.hits.end(); ++iHit )
    {
      const double prob = output.inferrer->probabilityHitWasMadeByGivenCircle(iHit,iRing);
      if ( prob > m_minAssProb )
      {
        const double RingCentreLocalx = gen_ring.x() / m_scaleFactor;
        const double RingCentreLocaly = gen_ring.y() / m_scaleFactor;

        LHCb::RichRecPixel * pix = richPixels()->object( (*iHit).index().value() );
        if ( !pix ) return Error( "Markov hit has bad pixel pointer" );
        //const Gaudi::XYZPoint & PixelPtnLocal = pix->radCorrLocalPositions().position(rad());
        const Gaudi::XYZPoint & PixelPtnLocal = pix->localPosition();

        const double Xsegringsep  = std::pow( fabs(RingCentreLocalx-PixelPtnLocal.x()), 2 );
        const double Ysegringsep  = std::pow( fabs(RingCentreLocaly-PixelPtnLocal.y()), 2 );
        const double pixelSep     = std::sqrt( Xsegringsep+Ysegringsep );

        if (pixelSep > m_maxPixelSep) continue; // next pixel
        ringHasHits = true;

        // finally, selected so add to ring
        newRing->richRecPixels().push_back( LHCb::RichRecPixelOnRing(pix,prob) );
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "  -> Adding pixel " << pix->key() << " to ring" << endreq;

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

      // Set detector information
      newRing->setRich     ( rich()  );
      newRing->setPanel    ( panel() );
      newRing->setRadiator ( rad()   );

      // set ring type info
      newRing->setType      ( LHCb::RichRecRing::TracklessRing );
      newRing->setAlgorithm ( LHCb::RichRecRing::Markov        );

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

template < class SAMPLER >
void AlgBase<SAMPLER>::addRingToPixels( LHCb::RichRecRing * ring ) const
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

template < class SAMPLER >
bool AlgBase<SAMPLER>::addDataPoints( GenRingF::GenericInput & input ) const
{
  bool OK = false;
  // Iterate over pixels
  const IPixelCreator::PixelRange range = pixelCreator()->range( rich(), panel() );
  if ( range.size() > m_maxHitsEvent )
  {
    std::ostringstream mess;
    mess << "# selected hits in " << Rich::text(rich()) << " " << Rich::text(rich(),panel())
         << " exceeded maximum of " << m_maxHitsEvent << " -> Processing aborted";
    Warning( mess.str(), StatusCode::SUCCESS, 3 );
  }
  else if ( range.size() < 3 )
  {
    Warning( "Too few hits (<3) to find any rings", StatusCode::SUCCESS, 3 );
  }
  else
  {
    OK = true;
    input.hits.reserve( range.size() );
    for ( LHCb::RichRecPixels::const_iterator iPix = range.begin(); iPix != range.end(); ++iPix )
    {
      // Is this HPD selected ?
      const unsigned int hitsInHPD = pixelCreator()->range( (*iPix)->hpd() ).size();
      if ( hitsInHPD < m_maxHitsHPD )
      {
        // get X and Y
        //const double X ( m_scaleFactor * (*iPix)->radCorrLocalPositions().position(rad()).x() );
        //const double Y ( m_scaleFactor * (*iPix)->radCorrLocalPositions().position(rad()).y() );
        const double X ( m_scaleFactor * (*iPix)->localPosition().x() );
        const double Y ( m_scaleFactor * (*iPix)->localPosition().y() );
        input.hits.push_back( GenRingF::GenericHit( GenRingF::GenericHitIndex((*iPix)->key()), X, Y ) );
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
      debug() << "Selected " << input.hits.size() << " data points for "
              << Rich::text(rich()) << " " << Rich::text(rich(),panel())
              << endreq;
  }

  return OK;
}

template < class SAMPLER >
void AlgBase<SAMPLER>::buildRingPoints( LHCb::RichRecRing * ring,
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
      const double X( ring->centrePointLocal().x() + (std::sin(angle)*ring->radius())/m_scaleFactor);
      const double Y( ring->centrePointLocal().y() + (std::cos(angle)*ring->radius())/m_scaleFactor);
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

// Intrecepts messages from the 'Lester' code and sends them to gaudi
template < class SAMPLER >
void AlgBase<SAMPLER>::lesterMessage( const Lester::MessageLevel level,
                                      const std::string & message ) const
{
  if      ( Lester::VERBOSE == level )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << message << endreq;
  }
  else if ( Lester::DEBUG == level )
  {
    if (msgLevel(MSG::DEBUG)) debug() << message << endreq;
  }
  else if ( Lester::INFO == level )
  {
    if (msgLevel(MSG::INFO)) info() << message << endreq;
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

template < class SAMPLER >
StatusCode AlgBase<SAMPLER>::dumpToTextfile() const
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
    for ( LHCb::RichRecPixels::const_iterator iPix = range.begin(); iPix != range.end(); ++iPix )
    {
      // get X and Y
      //const double X ( m_scaleFactor * (*iPix)->radCorrLocalPositions().position(rad()).x() );
      //const double Y ( m_scaleFactor * (*iPix)->radCorrLocalPositions().position(rad()).y() );
      const double X ( m_scaleFactor * (*iPix)->localPosition().x() );
      const double Y ( m_scaleFactor * (*iPix)->localPosition().y() );
      file << X << " " << Y << std::endl;
    }
  }
  return StatusCode::SUCCESS;
}
