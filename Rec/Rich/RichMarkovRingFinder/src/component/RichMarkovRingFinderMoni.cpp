// $Id: RichMarkovRingFinderMoni.cpp,v 1.8 2004-09-23 16:52:53 abuckley Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
using namespace std;
#include "RichMarkovRingFinderMoni.h"


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

  acquireTool( "RichRecMCTruthTool",  m_richRecMCTruth );
  acquireTool( "RichMCTruthTool",     m_richMCTruth    );
  acquireTool( "RichMCTrackInfoTool", m_mcTrackInfo    );

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
};


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

  // loop over rings
  info() << endreq;
  for ( RichRecRings::const_iterator iRing = rings->begin(); iRing != rings->end(); ++iRing ) {

    // Is this a RICH1 or RICH2 ring?
    const Rich::DetectorType whichRich( (*iRing)->rich() );
    const string whichRichName(Rich::text(whichRich));
    info() << "This ring is in " << whichRich << endreq;

    // Retrieve the pixels that constitute the ring and associate to the track MCParticles
    SmartRefVector<RichRecPixel> & pixels = (*iRing)->richRecPixels();
    map<const MCParticle*, size_t> seenMCParts;
    for ( SmartRefVector<RichRecPixel>::iterator iPix = pixels.begin(); iPix != pixels.end(); ++iPix ) {
      if ( !(*iPix) ) { Error("Null RichRecPixel in RichRecRing"); continue; }

      // Get the MC particles associated to this pixel
      MCParticleVector thispixelMCParts;
      m_richRecMCTruth->mcParticle( *iPix, thispixelMCParts );

      // Update list of seen MC particles
      for (MCParticleVector::const_iterator iMCP = thispixelMCParts.begin(); iMCP != thispixelMCParts.end(); ++iMCP) {
        seenMCParts[*iMCP]++;
      }
    } // loop over pixels


    if (0 == seenMCParts.size()) {
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
      info() << "Most matched MC particle for this ring is " << mostMatchedMCP
             << " with " << mostMatchedMCPNoMatches << "/"
             << totalNoMatches << " matches" << endreq;


      // Decide whether this ring is to be considered "sufficiently uniquely matched" to the
      // best MCParticle candidate to be considered a successful Markov ring
      if (bestMatchFraction > m_RingMatchFractionCutoff) {
        info() << "Found ring with a single MC track causing >"
               << 100 * m_RingMatchFractionCutoff << "% of hits" << endreq;
        // *** Plot match fraction: I suspect this has a sudden peak at 100% from
        // looking at debug output


        // MC type info
        // Options are Rich::Electron, Rich::Muon, Rich::Pion, Rich::Kaon, Rich::Proton or Rich::Unknown
        // numerically 0               1           2           3           4               -1
        const MCParticle* mcpart = mostMatchedMCP;
        const Rich::ParticleIDType mcType = m_richMCTruth->mcParticleType(mcpart);
        info() << "MC type: " << mcType << endreq;


        // Does this ring have a corresponding reconstructed track segment?
        // If not then the global algorithm will probably miss it
        string tempRecOrNot("rec");
        // The projected segment is the best match within the specified ring radius fraction
        // (the matching has already been done in the RichMarkovRingFinderAlg)
        if ( (*iRing)->richRecSegment() ) {
          info() << "Found a ring with a fairly unique MC track and a matching rec track" << endreq;
        } else {
          info() << "This ring has a good MCParticle match but isn't reconstructed!" << endreq;
          tempRecOrNot = "notRec";
        }
        const string recOrNot(tempRecOrNot); // I want this to be const, thank you very much!
        // *** Do the rec segment and the MC particle correspond to each other?


        // Histogram the MC particle type
        //cout << whichRichName + recOrNot << endl;
        m_RingTrackMCType[whichRichName + recOrNot]->fill(mcType);


        //SmartDataPtr<MCParticles> mcEvt(eventSvc(), MCParticleLocation::Default);
        //sc = toolSvc()->retrieveTool( "DebugTool", m_pDebugTool, this );
        //m_pDebugTool->printEventAsTree(mcEvt);


        // Origin vertices
        if (mcpart) {
          // Examine the particle origin vertex type and position
          const MCVertex::MCVertexType originvertextype( mcpart->originVertex()->type() );
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
          info() << "Origin vertex (MC) is " << originvertextypename << endreq;


          // Get the particle origin vertex position
          const HepPoint3D& startPoint = mcpart->originVertex()->position();
          info() << "Origin vtx position = ("
                 << startPoint.x()/m << ", "
                 << startPoint.y()/m << ", "
                 << startPoint.z()/m << ")m" << endreq;


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

          //cout << "test" << endl;

          // Fill origin vertex position histos
          m_RingTrackOriginZ[whichRichName + recOrNot]->fill( startPoint.z()/m );
          m_RingTrackOriginRZ[whichRichName + recOrNot]->fill( startPoint.z()/m, rho/m );
          m_RingTrackOrigin[whichRichName + recOrNot]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );
          // Subdetector-specific histos
          if (InVelo == originvertexlocation) {
            m_RingTrackOrigin1Zoom[whichRichName + recOrNot]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );
            m_RingTrackOriginZ1[whichRichName + recOrNot]->fill( startPoint.z()/m );
            m_RingTrackOriginXY1[whichRichName + recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
            m_RingTrackOriginRZ1[whichRichName + recOrNot]->fill( startPoint.z()/m, rho/m );
            m_RingTrackOriginRZ1Zoom[whichRichName + recOrNot]->fill( startPoint.z()/m, rho/m );
            m_RingTrackOriginInVeloVertexType[whichRichName + recOrNot]->fill( mcpart->originVertex()->type() );
            m_RingTrackOriginInVeloElectronVertexType[whichRichName + recOrNot]->fill( mcpart->originVertex()->type() );
          } else if (InTT == originvertexlocation) {
            m_RingTrackOriginXY2[whichRichName + recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
            m_RingTrackOriginRZ2[whichRichName + recOrNot]->fill( startPoint.z()/m, rho/m );
          } else if (InT123 == originvertexlocation ) {
            m_RingTrackOriginXY3[whichRichName + recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
            m_RingTrackOriginRZ3[whichRichName + recOrNot]->fill( startPoint.z()/m, rho/m );
          }


          // Print out properties of the MC tracks origins clustered in the VELO
          if (InVelo == originvertexlocation) { // in VELO
            map<string, double> partProps;
            partProps["momentum"] = mcpart->momentum().vect().mag();
            partProps["momentum_t"] = mcpart->pt();
            partProps["pseudorapidity"] = mcpart->pseudoRapidity();
            // Just me playing with algorithms
            //for_each(partProps.begin(), partProps.end(), printMap);
          }

        }


        // -------------


        // Decay vertices
        if (mcpart) {
          const SmartRefVector<MCVertex> endVertices = mcpart->endVertices();
          for (SmartRefVector<MCVertex>::const_iterator vtx = endVertices.begin(); vtx != endVertices.end(); ++vtx) {
            const HepPoint3D & endPoint = (*vtx)->position();
            debug() << "Start vtx position = ("
                    << endPoint.x()/m << "m, "
                    << endPoint.y()/m << "m, "
                    << endPoint.z()/m << "m)" << endreq;

            // Get azimuthal distance
            double rho = sqrt( endPoint.x() * endPoint.x() + endPoint.y() * endPoint.y() );

            // Fill decay vertex position histos
            m_RingTrackDecayZ[whichRichName + recOrNot]->fill( endPoint.z()/m );
            m_RingTrackDecayRZ[whichRichName + recOrNot]->fill( endPoint.z()/m, rho/m );
            m_RingTrackDecay[whichRichName + recOrNot]->fill( endPoint.z()/m, endPoint.x()/m, endPoint.y()/m );

            if (  MCVertex::Decay == (*vtx)->type() ) {
              m_RingTrackEndDecayZ[whichRichName + recOrNot]->fill( endPoint.z()/m );
            } else {
              m_RingTrackEndNotDecayZ[whichRichName + recOrNot]->fill( endPoint.z()/m );
            }

            /*
              if (fabs(startPoint.z()/m) < 1) { // in VELO
              m_RingTrackOrigin1Zoom[recOrNot]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );
              m_RingTrackOriginZ1[recOrNot]->fill( startPoint.z()/m );
              m_RingTrackOriginXY1[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
              m_RingTrackOriginRZ1[recOrNot]->fill( startPoint.z()/m, rho/m );
              m_RingTrackOriginRZ1Zoom[recOrNot]->fill( startPoint.z()/m, rho/m );
              } else if (startPoint.z()/m > 1.9  && startPoint.z()/m < 3.5 ) { // in TT
              m_RingTrackOriginXY2[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
              m_RingTrackOriginRZ2[recOrNot]->fill( startPoint.z()/m, rho/m );
              } else if (startPoint.z()/m > 7  && startPoint.z()/m < 10 ) { // in T1-T3
              m_RingTrackOriginXY3[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
              m_RingTrackOriginRZ3[recOrNot]->fill( startPoint.z()/m, rho/m );
              }
            */

          }
        }

      } else {
        debug() << "(Lots) more than one associated track for this ring" << endreq;
      }
    }

  } // end ring loop


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode
RichMarkovRingFinderMoni::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}

//=============================================================================


StatusCode
RichMarkovRingFinderMoni::bookHistograms() {

  string title, id;
  const int nbins1D(100), nbins2D(50), nbins3D(10);
  double rmax(0);
  const string recOrNot[2] = {"rec", "notRec"};
  const string richType[2] = {"Rich1", "Rich2"};
  const string trackingTitle[2] = {"tracking-matched MCMC rings", "MCMC rings not found by tracking"};

  for (int richNo = 0; richNo < 2; ++richNo) {
    for (int recNo = 0; recNo < 2; ++recNo) {

      const string index( richType[richNo] + recOrNot[recNo] );
      const string subtitle( richType[richNo] + " " + trackingTitle[recNo] );
      const string histopath( m_histPth + "/" + richType[richNo] );

      // MC type IDs
      title = "MC type IDs of " + subtitle;
      id = "RingTrackMCType-" + index;
      m_RingTrackMCType[index] = histoSvc()->book(histopath, id, title, 6, -1.5, 4.5);

      // Origin vertex types
      title = "Origin vertex type for origin vertices of " + subtitle + " in the VELO";
      id = "RingTrackOriginInVeloVtxType-" + index;
      m_RingTrackOriginInVeloVertexType[index] = histoSvc()->book(histopath, id, title, 15, -0.5, 14.5);

      title = "Origin vertex type for origin vertices of " + subtitle + " in the VELO (electrons)";
      id = "RingTrackOriginInVeloElectronVtxType-" + index;
      m_RingTrackOriginInVeloElectronVertexType[index] = histoSvc()->book(histopath, id, title, 15, -0.5, 14.5);


      // Vertex positions
      // 1D plots
      title = "Z origins of " + subtitle;
      id = "RingTrackOriginZ-" + index;
      m_RingTrackOriginZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);

      title = "Z origins of " + subtitle;
      id = "RingTrackOriginZ1-" + index;
      m_RingTrackOriginZ1[index] = histoSvc()->book(histopath, id, title, nbins1D, -0.5, 0.5);

      title = "Z end points of " + subtitle;
      id = "RingTrackDecayZ-" + index;
      m_RingTrackDecayZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);

      title = "Z end (decay) points of " + subtitle;
      id = "RingTrackEndDecayZ-" + index;
      m_RingTrackEndDecayZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);

      title = "Z end (not decay) points of " + subtitle;
      id = "RingTrackNotEndDecayZ-" + index;
      m_RingTrackEndNotDecayZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);

      title = "Z decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVeloZ-" + index;
      m_RingTrackDecaysWithVeloOriginZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 20);


      // ----------------

      // 2D plots
      title = "X-Y origins of VELO " + subtitle;
      id = "RingTrackOriginXY1-" + index;
      rmax = 0.01;
      m_RingTrackOriginXY1[index] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);

      title = "X-Y origins of TT " + subtitle;
      id = "RingTrackOriginXY2-" + index;
      rmax = 0.35;
      m_RingTrackOriginXY2[index] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);

      title = "X-Y origins of T1-T3 " + subtitle;
      id = "RingTrackOriginXY3-" + index;
      rmax = 1.5;
      m_RingTrackOriginXY3[index] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);

      title = "rho-Z origins of " + subtitle;
      id = "RingTrackOriginRZ-" + index;
      rmax = 1.5;
      m_RingTrackOriginRZ[index] = histoSvc()->book(histopath, id, title, nbins2D, -1, 12, nbins2D, 0, rmax);

      title = "rho-Z decay points of " + subtitle;
      id = "RingTrackDecayRZ-" + index;
      rmax = 1.8;
      m_RingTrackDecayRZ[index] = histoSvc()->book(histopath, id, title, nbins2D, -1, 12, nbins2D, 0, rmax);

      title = "rho-Z decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVeloRZ-" + index;
      rmax = 3.0;
      m_RingTrackDecaysWithVeloOriginRZ[index] = histoSvc()->book(histopath, id, title, nbins2D, -1, 20, nbins2D, 0, rmax);

      title = "rho-Z origins of VELO " + subtitle;
      id = "RingTrackOriginRZ1-" + index;
      rmax = 0.02;
      m_RingTrackOriginRZ1[index] = histoSvc()->book(histopath, id, title, nbins2D, -0.5, 1, nbins2D, 0, rmax);

      title = "rho-Z origins of zoomed VELO " + subtitle;
      id = "RingTrackOriginRZ1Zoom-" + index;
      rmax = 0.01;
      m_RingTrackOriginRZ1Zoom[index] = histoSvc()->book(histopath, id, title, nbins2D, -0.2, 0.2, nbins2D, 0, rmax);

      title = "rho-Z origins of TT " + subtitle;
      id = "RingTrackOriginRZ2-" + index;
      rmax = 0.35;
      m_RingTrackOriginRZ2[index] = histoSvc()->book(histopath, id, title, nbins2D, 1.9, 3.5, nbins2D, 0, rmax);

      title = "rho-Z origins of T1-T3 " + subtitle;
      id = "RingTrackOriginRZ3-" + index;
      rmax = 1.5;
      m_RingTrackOriginRZ3[index] = histoSvc()->book(histopath, id, title, nbins2D, 7, 10, nbins2D, 0, rmax);


      // 3D plots
      title = "Track origins of " + subtitle;
      id = "RingTrackOrigin-" + index;
      rmax = 1.8;
      m_RingTrackOrigin[index] = histoSvc()->book(histopath, id, title, nbins3D, -1, 12, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

      title = "Track origins of zoomed VELO " + subtitle;
      id = "RingTrackOrigin1Zoom-" + index;
      rmax = 0.01;
      m_RingTrackOrigin1Zoom[index] = histoSvc()->book(histopath, id, title, nbins3D, -0.2, 0.2, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

      title = "Track decay points of " + subtitle;
      id = "RingTrackDecay-" + index;
      rmax = 1.8;
      m_RingTrackDecay[index] = histoSvc()->book(histopath, id, title, nbins3D, -1, 12, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

      title = "Track decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVelo-" + index;
      rmax = 3.0;
      m_RingTrackDecaysWithVeloOrigin[index] = histoSvc()->book(histopath, id, title, nbins3D, -1, 20, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

    }
  }

  return StatusCode::SUCCESS;
}
