// $Id: RichMarkovRingFinderMoni.cpp,v 1.7 2004-08-20 09:59:24 abuckley Exp $
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
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/MARKOV/MC/" );
  declareProperty( "HistoPath", m_histPth = "RICH/MARKOV/" );
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
StatusCode RichMarkovRingFinderMoni::execute()
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


    // ring centre point
    //const HepPoint3D & ringPoint = (*iRing)->centrePointLocal();
    //const double ringRadius = (*iRing)->radius();


    // Retrieve the pixels that constitute the ring and associate to the track MCParticles
    SmartRefVector<RichRecPixel> & pixels = (*iRing)->richRecPixels();
    MCParticleVector seenMCParts;
    unsigned int totalNoTracksPerRing(0);
    for ( SmartRefVector<RichRecPixel>::iterator iPix = pixels.begin(); iPix != pixels.end(); ++iPix ) {

      if ( !(*iPix) ) {
        Error("Null RichRecPixel in RichRecRing"); continue;
      }

      MCParticleVector thispixelMCParts;
      m_richRecMCTruth->mcParticle( *iPix, thispixelMCParts );
      
      // Add MC particle to list of seen particles if it isn't already there 
      for (MCParticleVector::const_iterator iMCP = thispixelMCParts.begin(); iMCP != thispixelMCParts.end(); ++iMCP) {
        bool alreadySeenThisMCP(false);
        for (MCParticleVector::const_iterator iMCPseen = seenMCParts.begin(); iMCPseen != seenMCParts.end(); ++iMCPseen) {
          if (*iMCPseen == *iMCP) {
            alreadySeenThisMCP = true;
            continue;
          }
        }
        if (!alreadySeenThisMCP) {
          seenMCParts.push_back(*iMCP);
        }
      }
      totalNoTracksPerRing += thispixelMCParts.size();
    } // loop over pixels


    debug() << "No pixels = " << pixels.size()
            << " :: Total no assoc tracks = " << totalNoTracksPerRing
            << " :: No unique tracks = " << seenMCParts.size()
            << endreq;


    if (0 == seenMCParts.size()) {
      info() << "Found a ring with no associated MC track" << endreq;
    } else if (1 == seenMCParts.size()) {
      info() << "Found a ring with only one associated MC track" << endreq;
      // *** Does it have any associated rec tracks? If not, where does it come from?
      // *** Slacken the unique association requirement to ~90% uniqueness?
      // *** "Pragmatic" cut on event busyness?

      const MCParticle * mcpart = seenMCParts.front();
      // CRJ : Add MC type info
      // Options are Rich::Electron, Rich::Muon, Rich::Pion, Rich::Kaon, Rich::Proton or Rich::Unknown
      // numerically 0               1           2           3           4               -1
      const Rich::ParticleIDType mcType = m_richMCTruth->mcParticleType(mcpart);
      info() << "MC type: " << mcType << endreq;

      // loop over reco segments
      bool ringMatchesNoRecSegs(true);
      for ( RichRecSegments::const_iterator iSeg = richSegments()->begin(); iSeg != richSegments()->end(); ++iSeg ) {

        // NB. contains spillover
        if (*iSeg) {
          // Skip if ring and segment RICH types don't match
          if ( (*iRing)->rich() != (*iSeg)->trackSegment().rich() ) continue;
          
          if ( (*iRing)->richRecSegment() ) {
            // If the projected segment lies within the ring radius (the matching
            // has already been done in the RichMarkovRingFinderAlg)
            ringMatchesNoRecSegs = false;
            info() << "Found a ring uniquely associated to 1 MC track which also has "
                   << "a reconstructed track within the ring radius" << endreq;
          }
        }

      } // end reco segments


      // Determine which histogram set is written into (they are string->histo maps)
      string recOrNot("rec");
      if (ringMatchesNoRecSegs) {
        recOrNot = "notRec";
        debug() << "MCMC ring matches an MC track but no rec tracks" << endreq;
      }


      // Histogram the MC particle type
      m_RingTrackMCType[recOrNot]->fill(mcType);


      //SmartDataPtr<MCParticles> mcEvt(eventSvc(), MCParticleLocation::Default);
      //sc = toolSvc()->retrieveTool( "DebugTool", m_pDebugTool, this );
      //m_pDebugTool->printEventAsTree(mcEvt);


      {
        const HepPoint3D& startPoint = mcpart->originVertex()->position();
        debug() << "Start vtx position = ("
            << startPoint.x()/m << "m, "
            << startPoint.y()/m << "m, "
            << startPoint.z()/m << "m)" << endreq;
        
        // Get azimuthal distance
        double rho = sqrt( startPoint.x() * startPoint.x() + startPoint.y() * startPoint.y() );
        
        // Fill origin vertex position histos
        m_RingTrackOriginZ[recOrNot]->fill( startPoint.z()/m );
        m_RingTrackOriginRZ[recOrNot]->fill( startPoint.z()/m, rho/m );
        m_RingTrackOrigin[recOrNot]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );

        if (fabs(startPoint.z()/m) < 1) { // in VELO
          m_RingTrackOrigin1Zoom[recOrNot]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );
          m_RingTrackOriginZ1[recOrNot]->fill( startPoint.z()/m );
          m_RingTrackOriginXY1[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
          m_RingTrackOriginRZ1[recOrNot]->fill( startPoint.z()/m, rho/m );
          m_RingTrackOriginRZ1Zoom[recOrNot]->fill( startPoint.z()/m, rho/m );

          m_RingTrackOriginInVeloVertexType[recOrNot]->fill( mcpart->originVertex()->type() );
          m_RingTrackOriginInVeloElectronVertexType[recOrNot]->fill( mcpart->originVertex()->type() );

        } else if (startPoint.z()/m > 1.9  && startPoint.z()/m < 3.5 ) { // in TT
          m_RingTrackOriginXY2[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
          m_RingTrackOriginRZ2[recOrNot]->fill( startPoint.z()/m, rho/m );
        } else if (startPoint.z()/m > 7  && startPoint.z()/m < 10 ) { // in T1-T3
          m_RingTrackOriginXY3[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
          m_RingTrackOriginRZ3[recOrNot]->fill( startPoint.z()/m, rho/m );
        }

        // Print out properties of the MC tracks origins clustered in the VELO
        if (fabs(startPoint.z()/m) < 1) { // in VELO
          map<string, double> partProps;
          partProps["momentum"] = mcpart->momentum().vect().mag();
          partProps["momentum_t"] = mcpart->pt();
          partProps["pseudorapidity"] = mcpart->pseudoRapidity();

          // Just me playing with algorithms
          //for_each(partProps.begin(), partProps.end(), printMap);
        }
      }

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
          m_RingTrackDecayZ[recOrNot]->fill( endPoint.z()/m );
          m_RingTrackDecayRZ[recOrNot]->fill( endPoint.z()/m, rho/m );
          m_RingTrackDecay[recOrNot]->fill( endPoint.z()/m, endPoint.x()/m, endPoint.y()/m );

          if ( 	MCVertex::Decay == (*vtx)->type() ) {
            m_RingTrackEndDecayZ[recOrNot]->fill( endPoint.z()/m );
          } else {
            m_RingTrackEndNotDecayZ[recOrNot]->fill( endPoint.z()/m );
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
      debug() << "More than one associated track" << endreq;
    }

  } // end ring loop


  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichMarkovRingFinderMoni::finalize() 
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}

//=============================================================================


StatusCode RichMarkovRingFinderMoni::bookHistograms() {

  string title, id;
  int nbins1D(100), nbins2D(50), nbins3D(10);
  double rmax(0);
  string recOrNot[2] = {"rec", "notRec"};
  string trackingTitle[2] = {"tracking-matched MCMC rings", "MCMC rings not found by tracking"};


  for (int i = 0; i < 2; ++i) {

    string index = recOrNot[i];
    string subtitle = trackingTitle[i];

    // 1D plots
    title = "Z origins of " + subtitle;
    id = "RingTrackOriginZ-" + index;
    m_RingTrackOriginZ[index] = histoSvc()->book(m_histPth, id, title, nbins1D, -1, 15);

    title = "Z origins of " + subtitle;
    id = "RingTrackOriginZ1-" + index;
    m_RingTrackOriginZ1[index] = histoSvc()->book(m_histPth, id, title, nbins1D, -0.5, 0.5);
   
    title = "Z end points of " + subtitle;
    id = "RingTrackDecayZ-" + index;
    m_RingTrackDecayZ[index] = histoSvc()->book(m_histPth, id, title, nbins1D, -1, 15);

    title = "Z end (decay) points of " + subtitle;
    id = "RingTrackEndDecayZ-" + index;
    m_RingTrackEndDecayZ[index] = histoSvc()->book(m_histPth, id, title, nbins1D, -1, 15);

    title = "Z end (not decay) points of " + subtitle;
    id = "RingTrackNotEndDecayZ-" + index;
    m_RingTrackEndNotDecayZ[index] = histoSvc()->book(m_histPth, id, title, nbins1D, -1, 15);

    title = "Z decay points of " + subtitle + " with VELO origins";
    id = "RingTrackDecaysFromVeloZ-" + index;
    m_RingTrackDecaysWithVeloOriginZ[index] = histoSvc()->book(m_histPth, id, title, nbins1D, -1, 20);

    // MC type IDs
    title = "MC type IDs of " + subtitle;
    id = "RingTrackMCType-" + index;
    m_RingTrackMCType[index] = histoSvc()->book(m_histPth, id, title, 6, -1.5, 4.5);

    // Origin vertex types
    title = "Origin vertex type for origin vertices of " + subtitle + " in the VELO";
    id = "RingTrackOriginInVeloVtxType-" + index;
    m_RingTrackOriginInVeloVertexType[index] = histoSvc()->book(m_histPth, id, title, 15, -0.5, 14.5);

    title = "Origin vertex type for origin vertices of " + subtitle + " in the VELO (electrons)";
    id = "RingTrackOriginInVeloElectronVtxType-" + index;
    m_RingTrackOriginInVeloElectronVertexType[index] = histoSvc()->book(m_histPth, id, title, 15, -0.5, 14.5);


    // ----------------
   
    // 2D plots
    title = "X-Y origins of VELO " + subtitle;
    id = "RingTrackOriginXY1-" + index;
    rmax = 0.01;
    m_RingTrackOriginXY1[index] = histoSvc()->book(m_histPth, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);
   
    title = "X-Y origins of TT " + subtitle;
    id = "RingTrackOriginXY2-" + index;
    rmax = 0.35;
    m_RingTrackOriginXY2[index] = histoSvc()->book(m_histPth, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);
   
    title = "X-Y origins of T1-T3 " + subtitle;
    id = "RingTrackOriginXY3-" + index;
    rmax = 1.5;
    m_RingTrackOriginXY3[index] = histoSvc()->book(m_histPth, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);
   
    title = "rho-Z origins of " + subtitle;
    id = "RingTrackOriginRZ-" + index;
    rmax = 1.5;
    m_RingTrackOriginRZ[index] = histoSvc()->book(m_histPth, id, title, nbins2D, -1, 12, nbins2D, 0, rmax);
   
    title = "rho-Z decay points of " + subtitle;
    id = "RingTrackDecayRZ-" + index;
    rmax = 1.8;
    m_RingTrackDecayRZ[index] = histoSvc()->book(m_histPth, id, title, nbins2D, -1, 12, nbins2D, 0, rmax);
   
    title = "rho-Z decay points of " + subtitle + " with VELO origins";
    id = "RingTrackDecaysFromVeloRZ-" + index;
    rmax = 3.0;
    m_RingTrackDecaysWithVeloOriginRZ[index] = histoSvc()->book(m_histPth, id, title, nbins2D, -1, 20, nbins2D, 0, rmax);

    title = "rho-Z origins of VELO " + subtitle;
    id = "RingTrackOriginRZ1-" + index;
    rmax = 0.02;
    m_RingTrackOriginRZ1[index] = histoSvc()->book(m_histPth, id, title, nbins2D, -0.5, 1, nbins2D, 0, rmax);
   
    title = "rho-Z origins of zoomed VELO " + subtitle;
    id = "RingTrackOriginRZ1Zoom-" + index;
    rmax = 0.01;
    m_RingTrackOriginRZ1Zoom[index] = histoSvc()->book(m_histPth, id, title, nbins2D, -0.2, 0.2, nbins2D, 0, rmax);
   
    title = "rho-Z origins of TT " + subtitle;
    id = "RingTrackOriginRZ2-" + index;
    rmax = 0.35;
    m_RingTrackOriginRZ2[index] = histoSvc()->book(m_histPth, id, title, nbins2D, 1.9, 3.5, nbins2D, 0, rmax);
   
    title = "rho-Z origins of T1-T3 " + subtitle;
    id = "RingTrackOriginRZ3-" + index;
    rmax = 1.5;
    m_RingTrackOriginRZ3[index] = histoSvc()->book(m_histPth, id, title, nbins2D, 7, 10, nbins2D, 0, rmax);

   
    // 3D plots
    title = "Track origins of " + subtitle;
    id = "RingTrackOrigin-" + index;
    rmax = 1.8;
    m_RingTrackOrigin[index] = histoSvc()->book(m_histPth, id, title, nbins3D, -1, 12, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);
   
    title = "Track origins of zoomed VELO " + subtitle;
    id = "RingTrackOrigin1Zoom-" + index;
    rmax = 0.01;
    m_RingTrackOrigin1Zoom[index] = histoSvc()->book(m_histPth, id, title, nbins3D, -0.2, 0.2, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);
   
    title = "Track decay points of " + subtitle;
    id = "RingTrackDecay-" + index;
    rmax = 1.8;
    m_RingTrackDecay[index] = histoSvc()->book(m_histPth, id, title, nbins3D, -1, 12, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

    title = "Track decay points of " + subtitle + " with VELO origins";
    id = "RingTrackDecaysFromVelo-" + index;
    rmax = 3.0;
    m_RingTrackDecaysWithVeloOrigin[index] = histoSvc()->book(m_histPth, id, title, nbins3D, -1, 20, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);
   
  }

  return StatusCode::SUCCESS;
}
