// $Id: RichMarkovRingFinderMoni.cpp,v 1.18 2005-03-23 15:29:54 abuckley Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "RichMarkovRingFinderMoni.h"

using namespace std;
using namespace RichMarkov;

//-----------------------------------------------------------------------------
// Implementation file for class : RichMarkovRingFinderMoni
//
// 2003-12-04 : Chris Jones
// 2004-02-10 : Andy Buckley
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichMarkovRingFinderMoni>          s_factory ;
const        IAlgFactory& RichMarkovRingFinderMoniFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichMarkovRingFinderMoni::RichMarkovRingFinderMoni( const string& name, ISvcLocator* pSvcLocator)
  : RichRecAlgBase ( name, pSvcLocator )
{
  declareProperty( "HistoPath", m_histPth = "RICH/Markov" );
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/Markov/MC" );
  declareProperty( "RingHitMatchFractionCutoff", m_RingMatchFractionCutoff = 0.90 );
}


//=============================================================================
// Destructor
//=============================================================================
RichMarkovRingFinderMoni::~RichMarkovRingFinderMoni() {};


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode RichMarkovRingFinderMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth    );
  acquireTool( "RichMCTruthTool",      m_richMCTruth       );
  acquireTool( "RichMCTrackInfoTool",  m_mcTrackInfo       );
  acquireTool( "RichCherenkovAngle",   m_ckAngle           );
  acquireTool( "RichRayTracing",       m_raytrace          );
  acquireTool( "RichMCTrackInfoTool",  m_mcRichTrackInfo   );
  acquireTool( "MCEffReconstructible", m_mcReconstructible );
  acquireTool( "DebugTool",            m_debugger          );

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;

  // Initialise the MC-rec matching agreement tally
  m_numMcVsRecMatchAgreements[Rich::Rich1][false] = 0;
  m_numMcVsRecMatchAgreements[Rich::Rich1][true] = 0;
  m_numMcVsRecMatchAgreements[Rich::Rich2][false] = 0;
  m_numMcVsRecMatchAgreements[Rich::Rich2][true] = 0;

  return StatusCode::SUCCESS;
}
//=============================================================================



//=============================================================================
//  Finalize
//=============================================================================
StatusCode
RichMarkovRingFinderMoni::finalize()
{
  // Execute base class method
  StatusCode sc = RichRecAlgBase::finalize();

  // Fractional agreement between MC and rec-matching
  map<Rich::DetectorType, float> fractionAgreed;

  // Agreement in Rich1
  const double total1( m_numMcVsRecMatchAgreements[Rich::Rich1][true] + m_numMcVsRecMatchAgreements[Rich::Rich1][false] );
  if (total1 != 0) {
    fractionAgreed[Rich::Rich1] = m_numMcVsRecMatchAgreements[Rich::Rich1][true] / total1;
    always() << "Number of MC-rec matching agree/disagreements in RICH1 = "
             << m_numMcVsRecMatchAgreements[Rich::Rich1][true]   << " / "
             << m_numMcVsRecMatchAgreements[Rich::Rich1][false]  << " ("
             << 100 * fractionAgreed[Rich::Rich1] << "% agreed)" << endreq;
  } else {
    always() << "Problem with Rich1 rec/MC agreement count" << endreq;
  }

  // Agreement in Rich2
  const double total2( m_numMcVsRecMatchAgreements[Rich::Rich2][true] + m_numMcVsRecMatchAgreements[Rich::Rich2][false] );
  if (total2 != 0) {
    fractionAgreed[Rich::Rich2] = m_numMcVsRecMatchAgreements[Rich::Rich2][true] / total2;
    always() << "Number of MC-rec matching agree/disagreements in RICH2 = "
             << m_numMcVsRecMatchAgreements[Rich::Rich2][true]   << " / "
             << m_numMcVsRecMatchAgreements[Rich::Rich2][false]  << " ("
             << 100 * fractionAgreed[Rich::Rich2] << "% agreed)" << endreq;
  } else {
    always() << "Problem with Rich2 rec/MC agreement count" << endreq;
  }

  return sc;
}
//=============================================================================



//=============================================================================
// Main execution
//=============================================================================
StatusCode
RichMarkovRingFinderMoni::execute()
{
  typedef vector<const MCParticle*> MCParticleVector;
  debug() << "Execute" << endreq;

  // Locate RichRecRings from TES
  SmartDataPtr<RichRecRings> rings( eventSvc(), RichRecRingLocation::MarkovRings );
  if ( !rings ) {
    warning() << "Failed to find RichRecRings at " << RichRecRingLocation::MarkovRings << endreq;
    return StatusCode::SUCCESS;
  } else if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Successfully located " << rings->size() << " RichRecRings at "
            << RichRecRingLocation::MarkovRings << endreq;
  }

  // And the same for the MCRichSegments (problem with spillover being missed?)
  SmartDataPtr<MCRichSegments> mcRichSegments( eventSvc(), MCRichSegmentLocation::Default );
  if ( !mcRichSegments ) {
    warning() << "Failed to find MCRichSegments at " << MCRichSegmentLocation::Default << endreq;
    return StatusCode::SUCCESS;
  } else if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Successfully located " << mcRichSegments->size() << " MCRichSegments at "
            << MCRichSegmentLocation::Default << endreq;
  }

  // And the same for the MCParticles...
  SmartDataPtr<MCParticles> mcParticles( eventSvc(), MCParticleLocation::Default );
  if ( !mcParticles ) {
    warning() << "Failed to find MCParticles at " << MCParticleLocation::Default << endreq;
    return StatusCode::SUCCESS;
  } else if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Successfully located " << mcParticles->size() << " MCRichParticles at "
            << MCParticleLocation::Default << endreq;
  }

  // Just some basic histogramming of pixs and rings in the event
  map<MCParticle*, unsigned int> numRecPixelsPerMCParticle;
  {
    // Loop over event pixels
    map<Rich::DetectorType, unsigned int> numPixelsInEvent;
    for (RichRecPixels::const_iterator iPix = richPixels()->begin(); iPix != richPixels()->end(); ++iPix) {
      // Is this a RICH1 or RICH2 pixel?
      const Rich::DetectorType whichRich( (*iPix)->detector() );
      numPixelsInEvent[whichRich]++;

      // Get a number of rec pixels associated to each MCParticle
      MCParticleVector mcParts;
      m_richRecMCTruth->mcParticle( *iPix, mcParts );
      for (MCParticleVector::const_iterator mcp = mcParts.begin(); mcp != mcParts.end(); ++mcp) {
        MCParticle* tempMCP(const_cast<MCParticle*>(*mcp));
        numRecPixelsPerMCParticle[tempMCP]++;
      }
    }
    m_NumPixsPerEvent[Rich::Rich1]->fill(numPixelsInEvent[Rich::Rich1]);
    m_NumPixsPerEvent[Rich::Rich2]->fill(numPixelsInEvent[Rich::Rich2]);
  }


  // Loop over event rings
  map<Rich::DetectorType, unsigned int> numRingsInEvent;
  for ( RichRecRings::const_iterator iRing = rings->begin(); iRing != rings->end(); ++iRing ) {
    // Is this a RICH1 or RICH2 ring?
    const Rich::DetectorType whichRich( (*iRing)->rich() );
    numRingsInEvent[whichRich]++;
  }
  m_NumRingsPerEvent[Rich::Rich1]->fill(numRingsInEvent[Rich::Rich1]);
  m_NumRingsPerEvent[Rich::Rich2]->fill(numRingsInEvent[Rich::Rich2]);


  // Number of "good" Markov rings in the event, used to calculate the event's "Markov purity"
  map<Rich::DetectorType, unsigned int> numGoodRingsInEvent;
  numGoodRingsInEvent[Rich::Rich1] = 0;
  numGoodRingsInEvent[Rich::Rich2] = 0;


  // Way of recording how many unique MC particles are found by the Markov process per event
  // for calculating the event's "Markov efficiency"
  map<Rich::DetectorType, map<MCParticle*, unsigned int> > ringPixMCParts;


  // Mean photon directions (from MC data)
  map<Rich::DetectorType, map<Rich::Side, HepVector3D> > meanReversePhotonDirection;
  meanReversePhotonDirection[Rich::Rich1][Rich::top] =    HepVector3D(+0.000, -0.894, -0.434);
  meanReversePhotonDirection[Rich::Rich1][Rich::bottom] = HepVector3D(+0.000, +0.891, -0.442);
  meanReversePhotonDirection[Rich::Rich2][Rich::left] =   HepVector3D(-0.862, +0.000, -0.457);
  meanReversePhotonDirection[Rich::Rich2][Rich::right] =  HepVector3D(+0.863, +0.000, -0.457);


  // Loop over rings for more in-depth purposes
  debug() << endreq;
  for ( RichRecRings::const_iterator iRing = rings->begin(); iRing != rings->end(); ++iRing ) {

    // Is this a RICH1 or RICH2 ring?
    const Rich::DetectorType whichRich( (*iRing)->rich() );
    const Rich::Side whichSide( (*iRing)->panel() );
    const string whichRichName(Rich::text(whichRich));
    debug() << "This ring is in " << whichRich << endreq;

    // Retrieve the pixels that constitute the ring and associate to the track MCParticles
    SmartRefVector<RichRecPixel> & pixels = (*iRing)->richRecPixels();


    // Histogram some ring params: radius, number of pixels (and the correlation)
    m_MarkovRingRadius[whichRich]->fill( (*iRing)->radius() );
    m_MarkovRingNumPixs[whichRich]->fill( (*iRing)->richRecPixels().size() );
    m_MarkovRingRadiusVsNumPixs[whichRich]->fill( (*iRing)->radius(), (*iRing)->richRecPixels().size() );


    const HepPoint3D ringCentre( (*iRing)->centrePointGlobal() );


    // Loop over this ring's pixels
    map<const MCParticle*, size_t> seenMCParts;
    for ( SmartRefVector<RichRecPixel>::iterator iPix = pixels.begin(); iPix != pixels.end(); ++iPix ) {
      if ( !(*iPix) ) { Error("Null RichRecPixel in RichRecRing"); continue; }

      // Get the MC particles associated to this pixel
      MCParticleVector thispixelMCParts;
      m_richRecMCTruth->mcParticle( *iPix, thispixelMCParts );

      // Update list of seen MC particles
      for (MCParticleVector::const_iterator iMCP = thispixelMCParts.begin(); iMCP != thispixelMCParts.end(); ++iMCP) {
        if (*iMCP) { seenMCParts[*iMCP]++; }
      }


      // Get two ring points separated by ~90 degrees and the centre point to construct
      // a normal vector to the ring. Note that the angular ordering and regular separation
      // of points is assumed, but not guaranteed.
      HepVector3D& ringNormal = pdNormals[whichRich][whichSide];
      if (ringNormal == HepVector3D()) {
        const vector<HepPoint3D>& ringPoints( (*iRing)->ringPoints() );
        const size_t quarterOfNumRingPoints( static_cast<size_t>(floor(ringPoints.size()/4.0)) );
        const HepVector3D vector1( *(ringPoints.begin()) - ringCentre );
        const HepVector3D vector2( ringPoints[quarterOfNumRingPoints] - ringCentre );
        ringNormal = vector1.cross(vector2).unit();
        if (ringNormal.z() > 0) { ringNormal *= -1; } // slightly hacky way of making sure it's the "right" normal
        debug() << "Normal vector = " << ringNormal << " : mag = " << ringNormal.mag() << endreq;
      }


      // Set up the arbitrary basis vectors in the detector planes
      RichPanelBasis& pdBasisVectors = pdBases[whichRich][whichSide];
      if ( pdBasisVectors == RichPanelBasis() ) {
        pdBasisVectors.first  = ringNormal.cross( HepVector3D(0,0,1) ).unit();
        pdBasisVectors.second = pdBasisVectors.first.cross( ringNormal ).unit();
        debug() << whichRich << ">>" << whichSide << "basis vecs: "
                << pdBasisVectors.first << " and "
                << pdBasisVectors.second << endreq;
      }
    } // loop over ring pixels



    // Start being selective: processing only those rings with associated MC tracks
    if (0 == seenMCParts.size()) {
      // Very unusual!
      info() << "Found a ring with no associated MC track" << endreq;
    } else {

      // Extract useful numbers from the ring's matched MCParticle list
      pair<const MCParticle*, size_t> mostMatchedPair( *max_element(seenMCParts.begin(), seenMCParts.end()) );
      const MCParticle* mostMatchedMCP( mostMatchedPair.first );
      const size_t mostMatchedMCPNoMatches( mostMatchedPair.second );
      const size_t totalNoMatches( for_each(seenMCParts.begin(), seenMCParts.end(), CountMap<const MCParticle*>()) );
      const double bestMatchFraction( static_cast<double>(mostMatchedMCPNoMatches) / static_cast<double>(totalNoMatches) );

      // Tell the world all about it...
      debug() << "No pixels = " << pixels.size()
              << " :: Total no assoc tracks = " << totalNoMatches
              << " :: No unique tracks = " << seenMCParts.size()
              << endreq;
      debug() << "Most matched MC particle for this ring is " << mostMatchedMCP
              << " with " << mostMatchedMCPNoMatches << "/"
              << totalNoMatches << " matches" << endreq;

      // Let's invent a measure of ring "quality"
      // *** maybe a probability-weighted sum over pixs in rec mode?
      // *** const double ringquality( mostMatchedMCPNoMatches ); // normalising would be nice, but hey: can't have everything

      // Histogram the best match fraction and absolute number
      m_MarkovRingBestMCMatchNumber[whichRich]->fill(mostMatchedMCPNoMatches);
      m_MarkovRingBestMCMatchFraction[whichRich]->fill(bestMatchFraction);



      // pixMCPart is the MCParticle responsible for most rec pixels. MC type info:
      // Options are Rich::Electron, Rich::Muon, Rich::Pion, Rich::Kaon, Rich::Proton or Rich::Unknown
      // numerically 0               1           2           3           4               -1
      const MCParticle* pixMCPart( mostMatchedMCP );
      const Rich::ParticleIDType pixMCType( m_richMCTruth->mcParticleType(pixMCPart) );
      ringPixMCParts[whichRich][const_cast<MCParticle*>(pixMCPart)]++;


      // Get the MCParticle associated to the "best" reconstructed RichRecSegment for this ring
      const MCParticle* segMCPart( m_richRecMCTruth->mcParticle( (*iRing)->richRecSegment() ) );


      // Find the best MCRichSegment matching the ring
      MCRichSegment* chosenMCSeg(0);
      double currentBestSeparation( static_cast<double>( std::numeric_limits<int>::max() ) ); // VC++ problem with numeric_limits<double>::max()
      for ( MCRichSegments::const_iterator mcseg = mcRichSegments->begin(); mcseg != mcRichSegments->end(); ++mcseg ) {
        // Skip if ring and segment RICH types don't match
        if ( (*iRing)->rich() != (*mcseg)->rich() ) continue;

        // Find the PD panel point corresponding to the track projection
        HepPoint3D mcsegPoint;
        m_mcRichTrackInfo->panelIntersectLocal(*mcseg, mcsegPoint);
        
        // Do some ring-segment matching...
        const double separation( mcsegPoint.distance((*iRing)->centrePointLocal()) );
        // If the projected segment is a better match than we've seen so far ...
        if (separation < currentBestSeparation) {
          // ... then we'll select it!
          currentBestSeparation = separation;
          chosenMCSeg = *mcseg;
          verbose() << "Current best seg = " << chosenMCSeg << endreq;
        }
      }
      const MCParticle* mcsegMCPart( chosenMCSeg ? chosenMCSeg->mcParticle() : 0 );


      // Ring and event purity and efficiency
      const double ringPurity(bestMatchFraction);
      m_MarkovRingPurity[whichRich]->fill(ringPurity);

      const double ringEfficiency( mostMatchedMCPNoMatches / 
                                   static_cast<double>(numRecPixelsPerMCParticle[const_cast<MCParticle*>(pixMCPart)]) );
      m_MarkovRingEfficiency[whichRich]->fill(ringEfficiency);


      // Agreement between non-null MC parts from pix and MCseg matching
      // Used to calculate event-wise Markov purity
      if ( mcsegMCPart && pixMCPart && mcsegMCPart == pixMCPart ) { 
        numGoodRingsInEvent[whichRich]++;
      }


      // ======================================================================================
      // Make cut off on the fraction of pixels matched to the majority MCParticle
      // ======================================================================================

      // Decide whether this ring is to be considered "sufficiently uniquely matched" to the
      // best MCParticle candidate to be considered a successful Markov ring
      if (bestMatchFraction > m_RingMatchFractionCutoff) {
        assert(0 != pixMCPart);
        debug() << "Found ring with a single MC track causing >"
                << 100 * m_RingMatchFractionCutoff << "% of hits" << endreq;



        // Do the rec segment and the MC particle correspond to each other? (when neither is null)
        if ( segMCPart && pixMCPart ) {
          if ( pixMCPart != segMCPart ) {
            debug() << "Non-null geometry-matched (" << segMCPart
                    << ") and pixel-matched (" << pixMCPart
                    << ") MCParticles disagree for Markov ring " << *iRing << endreq;
          }
          ++(m_numMcVsRecMatchAgreements[whichRich][pixMCPart==segMCPart]);
          debug() << "Incremented MC vs rec agreement: "
                  << Rich::text(whichRich) <<  "::"
                  << boolalpha << (pixMCPart==segMCPart) << " => "
                  << m_numMcVsRecMatchAgreements[whichRich][pixMCPart==segMCPart] << " entries"
                  << endreq;
        }



        // Does this ring have a corresponding reconstructed track segment?
        // If not then the global algorithm will probably miss it
        RecType tempRecOrNot(rec);
        // The projected segment is the best match within the specified ring radius fraction
        // (the matching has already been done in the RichMarkovRingFinderAlg)
        if ( (*iRing)->richRecSegment() ) {
          debug() << "Found a ring with a rec track" << endreq;
        } else {
          tempRecOrNot = notrec;
          info() << "This ring has a good pixel MCParticle match ("
                 << 100*bestMatchFraction << "% of "
                 << totalNoMatches << " pix matches) but isn't reconstructed!" << endreq;
          m_debugger->printTree( pixMCPart );
          const MCParticle * mostMother(pixMCPart->mother()), * tmpMother(pixMCPart->mother());
          while( tmpMother ) { mostMother = tmpMother; tmpMother = mostMother->mother(); }
          if ( mostMother ) {
            debug() << "Complete tree:" << endreq;
            m_debugger->printTree( mostMother );
          } else { debug() << "No mothers" << endreq; }
          debug() << "Origin vertex position of pixMCPart: " << pixMCPart->originVertex()->position() << endreq;
          if (segMCPart) {
            debug() << "Origin vertex position of segMCPart: " << segMCPart->originVertex()->position() << endreq;
          }
        }
        const RecType recOrNot(tempRecOrNot); // I want this to be const, thank you very much!


        // Histogram the MC particle type
        m_RingTrackMCType[whichRich][recOrNot]->fill(pixMCType);


        // What's the true angle of incidence of this ring's photons on the detector plane?
        // Run over pixels and associated photons -> MC photons for this ring and get the true incidence angle
        HepVector3D& pdNormal = pdNormals[whichRich][whichSide];
        RichPanelBasis& pdBasis = pdBases[whichRich][whichSide];
        for (SmartRefVector<RichRecPixel>::const_iterator iPix = pixels.begin(); iPix != pixels.end(); ++iPix) {
          for (vector<RichRecPhoton*>::const_iterator iRecphoton = (*iPix)->richRecPhotons().begin();
               iRecphoton != (*iPix)->richRecPhotons().end(); ++iRecphoton) {
            const MCRichOpticalPhoton* mcphoton( m_richRecMCTruth->trueOpticalPhoton(*iRecphoton) );
            if (mcphoton) {
              const HepVector3D trueBackwardPhotonDirection( mcphoton->flatMirrorReflectPoint() - mcphoton->pdIncidencePoint() );
              // What to do with these direction vectors? Diagonalising? Just scalar to normal for now:
              const double angleToNormal( trueBackwardPhotonDirection.angle(pdNormal) );
              debug() << "MC photon PD pt = " << mcphoton->pdIncidencePoint()
                      << " and reflect pt = " << mcphoton->flatMirrorReflectPoint() << endreq;
              debug() << "MC photon angle to normal = " << angleToNormal << endreq;


              // Get shift vectors resolved in PD plane for norm-MCphoton
              const HepVector3D shiftFromNormal( trueBackwardPhotonDirection.unit() - pdNormal );
              //const HepVector3D shiftFromNormalInPdPlane( shiftFromNormal - shiftFromNormal.dot(pdNormal) * pdNormal );


              // Histogram the MC angle and shift vector
              m_MarkovRingMCPhotonNormalAngle[whichRich]->fill( angleToNormal );
              m_MarkovRingMCPhotonShiftFromNormal[whichRich][whichSide]->fill( shiftFromNormal.dot(pdBasis.first),
                                                                               shiftFromNormal.dot(pdBasis.second) );
              m_MarkovRingMCPhotonReverseDirectionX[whichRich][whichSide]->fill( trueBackwardPhotonDirection.unit().x() );
              m_MarkovRingMCPhotonReverseDirectionY[whichRich][whichSide]->fill( trueBackwardPhotonDirection.unit().y() );
              m_MarkovRingMCPhotonReverseDirectionZ[whichRich][whichSide]->fill( trueBackwardPhotonDirection.unit().z() );
              m_MarkovRingMCPhotonReverseDirection [whichRich][whichSide]->fill( trueBackwardPhotonDirection.unit().x(),
                                                                                 trueBackwardPhotonDirection.unit().y(),
                                                                                 trueBackwardPhotonDirection.unit().z() );
            } else {
              debug() << "No real MC photon" << endreq;
            }
          }
        }

        // Trace back the panel normal / mean photon direction though the Rich optics
        HepPoint3D normalEndPoint, meanEndPoint;
        HepVector3D normalEndDir, meanEndDir;
        m_raytrace->traceBackFromDetector( ringCentre, pdNormal, normalEndPoint, normalEndDir );
        m_raytrace->traceBackFromDetector( ringCentre, meanReversePhotonDirection[whichRich][whichSide], meanEndPoint, meanEndDir );
        // Adjust the directions to correspond to forward-going *tracks* rather than backward-traced photons
        normalEndDir *= -1;
        meanEndDir *= -1;
        // Tell the world
        debug() << "Normal-back-traced tracking point on sph mirror: "
                << normalEndPoint << ") in 'track' direction (" << normalEndDir << ")" << endreq;
        debug() << "Mean-back-traced tracking point on sph mirror: "
                << meanEndPoint << ") in 'track' direction (" << meanEndDir << ")" << endreq;



        // *** get the segment associated to the pixel-matched MCParticle
        // Get Cerenkov angle for MC-true mass hypothesis (from REC track, so not pure truth)
        if ((*iRing)->richRecSegment() ) {
          const double thetaExp( m_ckAngle->avgCherenkovTheta( (*iRing)->richRecSegment(), pixMCType ) );
          const double thetaMarkov( (*iRing)->radius() );
          debug() << "Expected theta = " << thetaExp
                  << " vs. Markov theta = " << thetaMarkov
                  << " (shift = " << thetaMarkov - thetaExp <<  ")"
                  << endreq;
          m_CkPull[whichRich][recOrNot]->fill(thetaMarkov - thetaExp);


          // Compare ring centre positions
          const MCRichSegment* mcSegment( m_richRecMCTruth->mcRichSegment((*iRing)->richRecSegment()) );
          if (mcSegment) {
            HepPoint3D mcsegCentrePoint;
            m_mcRichTrackInfo->panelIntersectGlobal(mcSegment, mcsegCentrePoint);
            const HepPoint3D segCentrePoint( (*iRing)->richRecSegment()->pdPanelHitPoint() );
            const HepPoint3D markovCentrePoint( (*iRing)->centrePointGlobal() );

            const HepVector3D segToMcCentreVector( segCentrePoint - mcsegCentrePoint );
            const HepVector3D markovToMcCentreVector( markovCentrePoint - mcsegCentrePoint );
            m_RecSegPdPointError[whichRich][recOrNot]->fill( segToMcCentreVector.mag() );
            m_MarkovSegPdPointError[whichRich][recOrNot]->fill( markovToMcCentreVector.mag() );


            // Comparing the back-traced photons to the associated MCRichRecSegment
            HepVector3D normalBacktracePointError( normalEndPoint - mcSegment->exitPoint() );
            HepVector3D meanBacktracePointError  ( meanEndPoint   - mcSegment->exitPoint() );
            // Take the best momentum estimate from halfway through the radiator volume
            HepVector3D normalBacktraceSlopeError( normalEndDir.unit() - mcSegment->bestMomentum(0.5).unit() ); // *** probably not quite
            HepVector3D meanBacktraceSlopeError  ( meanEndDir.unit()   - mcSegment->bestMomentum(0.5).unit() ); // *** the right sort of qty!
            double normalBacktraceAngleError( normalEndDir.angle( mcSegment->bestMomentum(0.5) ) );
            double meanBacktraceAngleError  ( meanEndDir.angle(   mcSegment->bestMomentum(0.5) ) );

            debug() << "Exit point error = " << normalBacktracePointError.mag()
                    << " and slope error = " << normalBacktraceSlopeError.mag()
                    << " and angle error = " << normalBacktraceAngleError << " (normal)" << endreq;

            // And histogram qtys
            // for exit points
            m_normalBacktracePointErrorMag[whichRich][recOrNot]->fill( normalBacktracePointError.mag() );
            m_meanBacktracePointErrorMag  [whichRich][recOrNot]->fill( meanBacktracePointError.mag() );
            m_normalBacktracePointErrorX  [whichRich][recOrNot]->fill( normalBacktracePointError.x() );
            m_meanBacktracePointErrorX    [whichRich][recOrNot]->fill( meanBacktracePointError.x() );
            m_normalBacktracePointErrorY  [whichRich][recOrNot]->fill( normalBacktracePointError.y() );
            m_meanBacktracePointErrorY    [whichRich][recOrNot]->fill( meanBacktracePointError.y() );
            m_normalBacktracePointError   [whichRich][recOrNot]->fill( normalBacktracePointError.x(), normalBacktracePointError.y() );
            m_meanBacktracePointError     [whichRich][recOrNot]->fill( meanBacktracePointError.x(),   meanBacktracePointError.y() );
            // and for slopes
            m_normalBacktraceSlopeErrorMag[whichRich][recOrNot]->fill( normalBacktraceSlopeError.mag() );
            m_meanBacktraceSlopeErrorMag  [whichRich][recOrNot]->fill( meanBacktraceSlopeError.mag() );
            m_normalBacktraceSlopeErrorX  [whichRich][recOrNot]->fill( normalBacktraceSlopeError.x() );
            m_meanBacktraceSlopeErrorX    [whichRich][recOrNot]->fill( meanBacktraceSlopeError.x() );
            m_normalBacktraceSlopeErrorY  [whichRich][recOrNot]->fill( normalBacktraceSlopeError.y() );
            m_meanBacktraceSlopeErrorY    [whichRich][recOrNot]->fill( meanBacktraceSlopeError.y() );
            m_normalBacktraceSlopeError   [whichRich][recOrNot]->fill( normalBacktraceSlopeError.x(), normalBacktraceSlopeError.y() );
            m_meanBacktraceSlopeError     [whichRich][recOrNot]->fill( meanBacktraceSlopeError.x(),   meanBacktraceSlopeError.y() );
            // and for angles
            m_normalBacktraceAngleError   [whichRich][recOrNot]->fill( normalBacktraceAngleError );
            m_meanBacktraceAngleError     [whichRich][recOrNot]->fill( meanBacktraceAngleError );
          } // if valid MC segment
        } // if segment exists



        // Origin vertices
        // Examine the particle origin vertex type and position (for particles which have 
        // enough MC pixels on this Markov ring to be considered as "real" rings)
        const MCVertex::MCVertexType originvertextype( pixMCPart->originVertex()->type() );
        string originvertextypename("Other");
        if (MCVertex::ppCollision == originvertextype) {
          originvertextypename = "Primary";
        } else if (MCVertex::Decay == originvertextype) {
          originvertextypename = "Decay";
        } else if (MCVertex::Hadronic == originvertextype ||
                   MCVertex::Pair == originvertextype ||
                   MCVertex::Brem == originvertextype ||
                   MCVertex::LHCHalo == originvertextype) {
          originvertextypename = "Interaction";
        } else if (MCVertex::Unknown == originvertextype) {
          originvertextypename = "Unknown";
        }
        debug() << "Origin vertex (MC) is of type " << originvertextypename << endreq;


        // Get the MCParticle origin vertex position
        const HepPoint3D& startPoint = pixMCPart->originVertex()->position();
        debug() << "Origin vtx position = " << startPoint/m << endreq;


        // Categorize which subdetector the origin falls within by z coord
        // Note that these numbers are NOT precise!
        MCOriginLocation originvertexlocation(Other);
        if (fabs(startPoint.z()/m) < 1.0) { originvertexlocation = InVelo; }
        else if (startPoint.z() > 0.0) {
          if (startPoint.z()/m < 1.9) { originvertexlocation = InRich1; }
          else if (startPoint.z()/m < 3.5) { originvertexlocation = InTT; }
          else if (startPoint.z()/m < 7.0) { originvertexlocation = InMagnet; }
          else if (startPoint.z()/m < 10.0) { originvertexlocation = InT123; }
          else if (startPoint.z()/m < 12.0) { originvertexlocation = InRich2; }
        }
        // Get azimuthal radius
        const double rho(sqrt( startPoint.x() * startPoint.x() + startPoint.y() * startPoint.y() ));


        // *** Need to track MC particle types from the PV and work out why
        // *** they're still leaving rings in Rich2 but aren't tracked. MCTrackInfo, MCEffReconstructible
        const IMCEffReconstructible::RecblCategory reconstructCategory( m_mcReconstructible->reconstructible(pixMCPart) );
        m_RingTrackRecCategory[whichRich][recOrNot]->fill( reconstructCategory );

        // Fill origin vertex position histos
        m_RingTrackOriginZ[whichRich][recOrNot]->fill( startPoint.z()/m );
        m_RingTrackOriginRZ[whichRich][recOrNot]->fill( startPoint.z()/m, rho/m );
        m_RingTrackOrigin[whichRich][recOrNot]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );


        // Origin position (by subdetector) -specific histos
        if (InVelo == originvertexlocation) {
          m_RingTrackOrigin1Zoom[whichRich][recOrNot]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );
          m_RingTrackOriginZ1[whichRich][recOrNot]->fill( startPoint.z()/m );
          m_RingTrackOriginXY1[whichRich][recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
          m_RingTrackOriginRZ1[whichRich][recOrNot]->fill( startPoint.z()/m, rho/m );
          m_RingTrackOriginRZ1Zoom[whichRich][recOrNot]->fill( startPoint.z()/m, rho/m );
          m_RingTrackOriginInVeloVertexType[whichRich][recOrNot]->fill( pixMCPart->originVertex()->type() );
          if (Rich::Electron == pixMCType) {
            m_RingTrackOriginInVeloElectronVertexType[whichRich][recOrNot]->fill( pixMCPart->originVertex()->type() );
          }
          m_RingTrackOriginInVeloRecCategory[whichRich][recOrNot]->fill( reconstructCategory );
        } else if (InTT == originvertexlocation) {
          m_RingTrackOriginXY2[whichRich][recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
          m_RingTrackOriginRZ2[whichRich][recOrNot]->fill( startPoint.z()/m, rho/m );
        } else if (InT123 == originvertexlocation ) {
          m_RingTrackOriginXY3[whichRich][recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
          m_RingTrackOriginRZ3[whichRich][recOrNot]->fill( startPoint.z()/m, rho/m );
        }


        // Print out properties of the MC tracks origins clustered in the VELO
        if (InVelo == originvertexlocation) { // in VELO
          map<string, double> partProps;
          partProps["momentum"] = pixMCPart->momentum().vect().mag();
          partProps["momentum_t"] = pixMCPart->pt();
          partProps["pseudorapidity"] = pixMCPart->pseudoRapidity();
          // Just me playing with algorithms
          //for_each(partProps.begin(), partProps.end(), printMap);
        }


        // -------------


        // Decay vertices
        const SmartRefVector<MCVertex> endVertices = pixMCPart->endVertices();
        for (SmartRefVector<MCVertex>::const_iterator vtx = endVertices.begin(); vtx != endVertices.end(); ++vtx) {
          const HepPoint3D & endPoint = (*vtx)->position();
          debug() << "Start vtx position = ("
                  << endPoint.x()/m << "m, "
                  << endPoint.y()/m << "m, "
                  << endPoint.z()/m << "m)" << endreq;

          // Get azimuthal distance
          double rho = sqrt( endPoint.x() * endPoint.x() + endPoint.y() * endPoint.y() );

          // Fill decay vertex position histos
          m_RingTrackDecayZ[whichRich][recOrNot]->fill( endPoint.z()/m );
          m_RingTrackDecayRZ[whichRich][recOrNot]->fill( endPoint.z()/m, rho/m );
          m_RingTrackDecay[whichRich][recOrNot]->fill( endPoint.z()/m, endPoint.x()/m, endPoint.y()/m );

          if (  MCVertex::Decay == (*vtx)->type() ) {
            m_RingTrackEndDecayZ[whichRich][recOrNot]->fill( endPoint.z()/m );
          } else {
            m_RingTrackEndNotDecayZ[whichRich][recOrNot]->fill( endPoint.z()/m );
          }

        }

      } // ring passes best MCParticle match criterion?
    }

  } // end ring loop

  vector<Rich::DetectorType> riches;
  riches.push_back(Rich::Rich1);
  riches.push_back(Rich::Rich2);

  for (vector<Rich::DetectorType>::const_iterator rich = riches.begin(); rich != riches.end(); ++rich) {
    // eventMarkovPurity = number of true (?) Markov rings / total number of Markov rings in that RICH
    const double eventPurity(numGoodRingsInEvent[*rich] / static_cast<double>(numRingsInEvent[*rich]));
    m_MarkovEventPurity[*rich]->fill(eventPurity);
    
    // eventMarkovEfficiency = size of Markov MCParticle set / size of "reconstructible" MCParticle set
    /// Note problem in that not all recbl MCParticles will leave tracks in either RICH...
    unsigned int numRecblMCParts(0);
    for (MCParticles::const_iterator mcParticle = mcParticles->begin(); mcParticle != mcParticles->end(); ++mcParticle) {
      if ( m_mcReconstructible->reconstructible(*mcParticle) != IMCEffReconstructible::NotRec ) { numRecblMCParts++; }
    }
    const double eventEfficiency(ringPixMCParts[*rich].size() / static_cast<double>(numRecblMCParts));
    m_MarkovEventEfficiency[*rich]->fill(eventEfficiency);
  }

  return StatusCode::SUCCESS;
}



StatusCode
RichMarkovRingFinderMoni::bookHistograms() {

  string title, id;
  const int nbins1D(100), nbins2D(50), nbins3D(10);
  double rmax(0);
  const Rich::DetectorType richTypes[2] = {Rich::Rich1, Rich::Rich2};
  const Rich::Side         richSides[2] = {Rich::left, Rich::right};
  const RecType            recTypes[2]  = {rec, notrec};


  for (int richNo = 0; richNo < 2; ++richNo) {
    Rich::DetectorType richType( richTypes[richNo] );
    const string mainindex( Rich::text(richType) );
    const string mainsubtitle( Rich::text(richType) ); //
    const string mainhistopath( m_histPth + "/" + Rich::text(richType) );

    // Histos where rec / nonrec doesn't matter (or make sense)
    // Event params
    id = "NumRingsPerEvent";
    title = "Number of Markov rings per event in " + mainsubtitle;
    m_NumRingsPerEvent[richType] = histoSvc()->book(mainhistopath, id, title, 101, -0.5, 100.5);
    id = "NumPixsPerEvent";
    title = "Number of pixels per event in " + mainsubtitle;
    m_NumPixsPerEvent[richType] =  histoSvc()->book(mainhistopath, id, title, 51, -10, 2010);

    // Ring params
    id = "MarkovRingRadius";
    title = "Ring radius (radians) in " + mainsubtitle;
    m_MarkovRingRadius[richType] = histoSvc()->book(mainhistopath, id, title, 50, 0, 0.05); // radians
    id = "MarkovRingNumPixs";
    title = "Number of pixs per ring in " + mainsubtitle;
    m_MarkovRingNumPixs[richType] = histoSvc()->book(mainhistopath, id, title, 201, -0.5, 200.5);
    id = "MarkovRingRadiusVsNumPixs";
    title = "Markov ring radius (radians) vs num pixs in "  + mainsubtitle;
    m_MarkovRingRadiusVsNumPixs[richType] = histoSvc()->book(mainhistopath, id, title, 30, 0, 0.05, 51, -0.5, 50.5);

    // MC-rec matching
    id = "MarkovRingBestMCMatchNumber";
    title = "Num MC matches for the best MCParticle in " + mainsubtitle;
    m_MarkovRingBestMCMatchNumber[richType] = histoSvc()->book(mainhistopath, id, title, 31, -0.5, 30.5);
    id = "MarkovRingBestMCMatchFraction";
    title = "Frac MC matches for the best MCParticle in " + mainsubtitle;
    m_MarkovRingBestMCMatchFraction[richType] = histoSvc()->book(mainhistopath, id, title, 50, 0, 1);

    // Purities and efficiencies
    id = "MarkovRingPurity";
    title = "Markov ring pixel set purity in " + mainsubtitle;
    m_MarkovRingPurity[richType] = histoSvc()->book(mainhistopath, id, title, 50, 0, 1);
    id = "MarkovRingEfficiency";
    title = "Markov ring pixel set efficiency in " + mainsubtitle;
    m_MarkovRingEfficiency[richType] = histoSvc()->book(mainhistopath, id, title, 50, 0, 1);
    id = "MarkovEventPurity";
    title = "Markov ring set purity in " + mainsubtitle;
    m_MarkovEventPurity[richType] = histoSvc()->book(mainhistopath, id, title, 50, 0, 1);
    id = "MarkovEventEfficiency";
    title = "Markov ring set efficiency in " + mainsubtitle;
    m_MarkovEventEfficiency[richType] = histoSvc()->book(mainhistopath, id, title, 50, 0, 1);

    // MC photon angles and in-plane shifts from the PD normal
    id = "MarkovRingMCPhotonNormalAngle";
    title = "Angles of MC photons to the PD plane normal in " + mainsubtitle;
    m_MarkovRingMCPhotonNormalAngle[richType] = histoSvc()->book(mainhistopath, id, title, 50, 0, 0.25);



    // Histos indexed also by PD panel (MCPhoton angles and directions)
    for (size_t richSideNo = 0; richSideNo < 2; ++richSideNo) {
      const Rich::Side richSide( richSides[richSideNo] );
      const string richSideText( Rich::text(richSide) + " panel" );
      const string subtitle( mainsubtitle + ", " + richSideText );
      const string histopath( mainhistopath );
      const string panelID( boost::lexical_cast<string>(richSideNo) );

      // Shift of photon vector from the normal in PD plane coords
      id = "MarkovRingMCPhotonShiftFromNormal" + panelID;
      title = "In-PD-plane shift of MC photon direction from normal in "  + subtitle;
      m_MarkovRingMCPhotonShiftFromNormal[richType][richSide] = histoSvc()->book(histopath, id, title, 50, -0.2, 0.2, 50, -0.2, 0.2);

      // Reverse photon vector direction
      id = "MarkovRingMCPhotonReverseDirection" + panelID;
      title = "Back-traced MC photon direction in "  + subtitle;
      m_MarkovRingMCPhotonReverseDirection[richType][richSide] = histoSvc()->book(histopath, id, title, 50, -1, 1, 50, -1, 1, 50, -1, 1);
      id = "MarkovRingMCPhotonReverseDirectionX" + panelID;
      title = "Back-traced MC photon direction in "  + subtitle + "(x-component)";
      m_MarkovRingMCPhotonReverseDirectionX[richType][richSide] = histoSvc()->book(histopath, id, title, 50, -1, 1);
      id = "MarkovRingMCPhotonReverseDirectionY" + panelID;
      title = "Back-traced MC photon direction in "  + subtitle + "(y-component)";
      m_MarkovRingMCPhotonReverseDirectionY[richType][richSide] = histoSvc()->book(histopath, id, title, 50, -1, 1);
      id = "MarkovRingMCPhotonReverseDirectionZ" + panelID;
      title = "Back-traced MC photon direction in "  + subtitle + "(z-component)";
      m_MarkovRingMCPhotonReverseDirectionZ[richType][richSide] = histoSvc()->book(histopath, id, title, 50, -1, 1);
    }



    // Histos where rec/nonrec matters: for debugging tracking problems
    for (int recNo = 0; recNo < 2; ++recNo) {
      const RecType recType( recTypes[recNo] );
      const string recOrNot[2] = {"rec", "notRec"};
      const string histopath( mainhistopath + "/" + recOrNot[recNo] );
      const string trackingTitle[2] = {"tracking-matched MCMC rings", "MCMC rings not found by tracking"};
      const string subtitle( mainsubtitle + " " + trackingTitle[recNo] );

      // MC type IDs
      title = "MC particle type of tracks giving " + subtitle;
      id = "RingTrackMCType";
      m_RingTrackMCType[richType][recType] = histoSvc()->book(histopath, id, title, 6, -1.5, 4.5);

      // Cerenkov angle and center-point pulls dbns
      title = "Cerenkov angle pull dbn of " + subtitle;
      id = "CerenkovAnglePull";
      m_CkPull[richType][recType] = histoSvc()->book(histopath, id, title, 50, -0.01, 0.01);
      title = "Ring centre error of rec seg for " + subtitle;
      id = "RingCentreErrorRecSeg";
      m_RecSegPdPointError[richType][recType] = histoSvc()->book(histopath, id, title, 50, -5*cm, 5*cm);
      title = "Ring centre error of Markov ring for " + subtitle;
      id = "RingCentreErrorMarkov";
      m_MarkovSegPdPointError[richType][recType] = histoSvc()->book(histopath, id, title, 50, -5*cm, 5*cm);

      // *** also look at the 2D correlation of pull magnitude and ring "quality"


      // Track reconstruction category
      title = "Reconstruction type for tracks of " + subtitle + " with origins in the VELO";
      id = "RingTrackOriginInVeloRecCategory";
      m_RingTrackOriginInVeloRecCategory[richType][recType] = histoSvc()->book(histopath, id, title, 5, -0.5, 4.5);
      title = "Reconstruction type for tracks of " + subtitle;
      id = "RingTrackRecCategory";
      m_RingTrackRecCategory[richType][recType] = histoSvc()->book(histopath, id, title, 5, -0.5, 4.5);


      title = "Normal backtrace exit point error for " + subtitle;
      id = "BacktraceNormalPointErrorMag";
      m_normalBacktracePointErrorMag[richType][recType] = histoSvc()->book(histopath, id, title, 50, 0, 1*m);
      title = "Mean backtrace exit point error for " + subtitle;
      id = "BacktraceMeanPointErrorMag";
      m_meanBacktracePointErrorMag[richType][recType] = histoSvc()->book(histopath, id, title, 50, 0, 1*m);
      title = "Normal backtrace exit point error (x) for " + subtitle;
      id = "BacktraceNormalPointErrorX";
      m_normalBacktracePointErrorX[richType][recType] = histoSvc()->book(histopath, id, title, 50, -1*m, 1*m);
      title = "Mean backtrace exit point error (x) for " + subtitle;
      id = "BacktraceMeanPointErrorX";
      m_meanBacktracePointErrorX[richType][recType] = histoSvc()->book(histopath, id, title, 50, -1*m, 1*m);
      title = "Normal backtrace exit point error (y) for " + subtitle;
      id = "BacktraceNormalPointErrorY";
      m_normalBacktracePointErrorY[richType][recType] = histoSvc()->book(histopath, id, title, 50, -1*m, 1*m);
      title = "Mean backtrace exit point error (y) for " + subtitle;
      id = "BacktraceMeanPointErrorY";
      m_meanBacktracePointErrorY[richType][recType] = histoSvc()->book(histopath, id, title, 50, -1*m, 1*m);
      title = "Normal backtrace exit point error for " + subtitle;
      id = "BacktraceNormalPointError";
      m_normalBacktracePointError[richType][recType] = histoSvc()->book(histopath, id, title, 50, -1*m, 1*m, 50, -1*m, 1*m);
      title = "Mean backtrace exit point error for " + subtitle;
      id = "BacktraceMeanPointError";
      m_meanBacktracePointError[richType][recType] = histoSvc()->book(histopath, id, title, 50, -1*m, 1*m, 50, -1*m, 1*m);

      // and for slopes
      title = "Normal backtrace exit slope error mag for " + subtitle;
      id = "BacktraceNormalSlopeErrorMag";
      m_normalBacktraceSlopeErrorMag[richType][recType] = histoSvc()->book(histopath, id, title, 50, 0, 0.02);
      title = "Mean backtrace exit slope error mag for " + subtitle;
      id = "BacktraceMeanSlopeErrorMag";
      m_meanBacktraceSlopeErrorMag[richType][recType] = histoSvc()->book(histopath, id, title, 50, 0, 0.02);
      title = "Normal backtrace exit slope error (x) for " + subtitle;
      id = "BacktraceNormalSlopeErrorX";
      m_normalBacktraceSlopeErrorX[richType][recType] = histoSvc()->book(histopath, id, title, 50, -0.02, 0.02);
      title = "Mean backtrace exit slope error (x) for " + subtitle;
      id = "BacktraceMeanSlopeErrorX";
      m_meanBacktraceSlopeErrorX[richType][recType] = histoSvc()->book(histopath, id, title, 50, -0.02, 0.02);
      title = "Normal backtrace exit slope error (y) for " + subtitle;
      id = "BacktraceNormalSlopeErrorY";
      m_normalBacktraceSlopeErrorY[richType][recType] = histoSvc()->book(histopath, id, title, 50, -0.02, 0.02);
      title = "Mean backtrace exit slope error (y) for " + subtitle;
      id = "BacktraceMeanSlopeErrorY";
      m_meanBacktraceSlopeErrorY[richType][recType] = histoSvc()->book(histopath, id, title, 50, -0.02, 0.02);
      title = "Normal backtrace exit slope error for " + subtitle;
      id = "BacktraceNormalSlopeError";
      m_normalBacktraceSlopeError[richType][recType] = histoSvc()->book(histopath, id, title, 50, -1, 1, 50, -0.02, 0.02);
      title = "Mean backtrace exit slope error for " + subtitle;
      id = "BacktraceMeanSlopeError";
      m_meanBacktraceSlopeError[richType][recType] = histoSvc()->book(histopath, id, title, 50, -1, 1, 50, -0.02, 0.02);

      // and for angles
      title = "Normal backtrace exit angle error for " + subtitle;
      id = "BacktraceNormalAngleError";
      m_normalBacktraceAngleError[richType][recType] = histoSvc()->book(histopath, id, title, 50, 0, 0.02);
      title = "Mean backtrace exit angle error for " + subtitle;
      id = "BacktraceMeanAngleError";
      m_meanBacktraceAngleError[richType][recType] = histoSvc()->book(histopath, id, title, 50, 0, 0.02);


      // Origin vertex types
      title = "Origin vertex type for origin vertices of " + subtitle + " in the VELO";
      id = "RingTrackOriginInVeloVtxType";
      m_RingTrackOriginInVeloVertexType[richType][recType] = histoSvc()->book(histopath, id, title, 15, -0.5, 14.5);
      title = "Origin vertex type for origin vertices of " + subtitle + " in the VELO (electrons)";
      id = "RingTrackOriginInVeloElectronVtxType";
      m_RingTrackOriginInVeloElectronVertexType[richType][recType] = histoSvc()->book(histopath, id, title, 15, -0.5, 14.5);


      // Vertex positions
      // 1D plots
      title = "Z origins of " + subtitle;
      id = "RingTrackOriginZ";
      m_RingTrackOriginZ[richType][recType] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);
      title = "Z origins of " + subtitle;
      id = "RingTrackOriginZ1";
      m_RingTrackOriginZ1[richType][recType] = histoSvc()->book(histopath, id, title, nbins1D, -0.5, 0.5);
      title = "Z end points of " + subtitle;
      id = "RingTrackDecayZ";
      m_RingTrackDecayZ[richType][recType] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);
      title = "Z end (decay) points of " + subtitle;
      id = "RingTrackEndDecayZ";
      m_RingTrackEndDecayZ[richType][recType] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);
      title = "Z end (not decay) points of " + subtitle;
      id = "RingTrackNotEndDecayZ";
      m_RingTrackEndNotDecayZ[richType][recType] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);
      title = "Z decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVeloZ";
      m_RingTrackDecaysWithVeloOriginZ[richType][recType] = histoSvc()->book(histopath, id, title, nbins1D, -1, 20);


      // ----------------

      // 2D plots
      title = "X-Y origins of VELO " + subtitle;
      id = "RingTrackOriginXY1";
      rmax = 0.01;
      m_RingTrackOriginXY1[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);
      title = "X-Y origins of TT " + subtitle;
      id = "RingTrackOriginXY2";
      rmax = 0.35;
      m_RingTrackOriginXY2[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);
      title = "X-Y origins of T1-T3 " + subtitle;
      id = "RingTrackOriginXY3";
      rmax = 1.5;
      m_RingTrackOriginXY3[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);
      title = "rho-Z origins of " + subtitle;
      id = "RingTrackOriginRZ";
      rmax = 1.5;
      m_RingTrackOriginRZ[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, -1, 12, nbins2D, 0, rmax);
      title = "rho-Z decay points of " + subtitle;
      id = "RingTrackDecayRZ";
      rmax = 1.8;
      m_RingTrackDecayRZ[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, -1, 12, nbins2D, 0, rmax);
      title = "rho-Z decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVeloRZ";
      rmax = 3.0;
      m_RingTrackDecaysWithVeloOriginRZ[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, -1, 20, nbins2D, 0, rmax);
      title = "rho-Z origins of VELO " + subtitle;
      id = "RingTrackOriginRZ1";
      rmax = 0.02;
      m_RingTrackOriginRZ1[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, -0.5, 1, nbins2D, 0, rmax);
      title = "rho-Z origins of zoomed VELO " + subtitle;
      id = "RingTrackOriginRZ1Zoom";
      rmax = 0.01;
      m_RingTrackOriginRZ1Zoom[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, -0.2, 0.2, nbins2D, 0, rmax);
      title = "rho-Z origins of TT " + subtitle;
      id = "RingTrackOriginRZ2";
      rmax = 0.35;
      m_RingTrackOriginRZ2[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, 1.9, 3.5, nbins2D, 0, rmax);
      title = "rho-Z origins of T1-T3 " + subtitle;
      id = "RingTrackOriginRZ3";
      rmax = 1.5;
      m_RingTrackOriginRZ3[richType][recType] = histoSvc()->book(histopath, id, title, nbins2D, 7, 10, nbins2D, 0, rmax);


      // 3D plots
      title = "Track origins of " + subtitle;
      id = "RingTrackOrigin";
      rmax = 1.8;
      m_RingTrackOrigin[richType][recType] = histoSvc()->book(histopath, id, title, nbins3D, -1, 12, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);
      title = "Track origins of zoomed VELO " + subtitle;
      id = "RingTrackOrigin1Zoom";
      rmax = 0.01;
      m_RingTrackOrigin1Zoom[richType][recType] = histoSvc()->book(histopath, id, title, nbins3D, -0.2, 0.2, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);
      title = "Track decay points of " + subtitle;
      id = "RingTrackDecay";
      rmax = 1.8;
      m_RingTrackDecay[richType][recType] = histoSvc()->book(histopath, id, title, nbins3D, -1, 12, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);
      title = "Track decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVelo";
      rmax = 3.0;
      m_RingTrackDecaysWithVeloOrigin[richType][recType] = histoSvc()->book(histopath, id, title, nbins3D, -1, 20, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

    }
  }

  return StatusCode::SUCCESS;
}
