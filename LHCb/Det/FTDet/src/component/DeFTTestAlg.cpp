// $Id: $

// Gaudi
#include "GaudiKernel/AlgFactory.h" 

// FTDet
#include "FTDet/DeFTDetector.h"

// Local
#include "DeFTTestAlg.h"

// LHCb Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

/** @file DeFTTestAlg.cpp
 *
 *  Implementation of class : DeFTTestAlg
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DeFTTestAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTTestAlg::DeFTTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm ( name, pSvcLocator ),
  m_DeFT(0),
  m_MCHitsLocation("")
{
  declareProperty( "MCHitsLocation", m_MCHitsLocation = "/Event/MC/FT/Hits" );
}

//=============================================================================
// Destructor
//=============================================================================
DeFTTestAlg::~DeFTTestAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTTestAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  /// Retrieve and initialize DeFT
  m_DeFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  if ( m_DeFT != 0 ) { debug() << "Successfully retrieved DeFT" << endmsg; }
  else { error() << "Error getting DeFT" << endmsg; }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DeFTTestAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if ( m_DeFT != 0 ) {

    /// Manually defined points
    /*
    int cntrMax = 20;
    for (int cntr = 0; cntr < cntrMax; cntr++){
      double x, y, z;
      x = 1000.;
      y = 1000.;
      z = 7720. + cntr;
      const Gaudi::XYZPoint globalPoint(x, y, z);
      ...
    }
    */

    /// Points of the real MCHits
    const LHCb::MCHits* hitsCont = get<LHCb::MCHits>(m_MCHitsLocation);
    /// Iterate over the first few hits and test the calculateHits method
    LHCb::MCHits::const_iterator aHit;
    for ( aHit=hitsCont->begin(); aHit != hitsCont->end(); ++aHit ) {

      Gaudi::XYZPoint pMid = (*aHit)->midPoint();

      // Make DeFT checks
      debug() << "\n\n\n**************************\nMC Hit " << (*aHit)->index() << "\n"
              << "**************************" << endmsg;

      std::string lVolName;

      debug() << "Test of det. element geometry() methods, using "
              << "the midPoint() of the MC hit: " << pMid << endmsg;

      /// check isInside FT
      bool isInsideFT = m_DeFT->isInside( pMid );
      debug() << "Global Point " << pMid << "; isInside =  " << isInsideFT << endmsg;

      /// test findStation method
      const DeFTStation* pStation = m_DeFT->findStation(pMid);
      lVolName = (pStation ? pStation->geometry()->lvolumeName() : "");
      debug() << "Found Station: " << lVolName << endmsg;

      /// test findBiLayer method
      const DeFTBiLayer* pBiLayer = m_DeFT->findBiLayer(pMid);
      lVolName = (pBiLayer ? pBiLayer->geometry()->lvolumeName() : "");
      debug() << "Found BiLayer: " << lVolName << endmsg;

      /// test findLayer method
      const DeFTLayer* pLayer = m_DeFT->findLayer(pMid);
      lVolName = (pLayer ? pLayer->geometry()->lvolumeName() : "");
      debug() << "Found Layer  : " << lVolName << endmsg;

      if ( pLayer != 0 ) {
        std::vector< std::pair<LHCb::FTChannelID, double> > vectChanAndFrac;
        debug() << "Test of function calculateHits:\n" << endmsg;
        pLayer->calculateHits((*aHit)->entry(), (*aHit)->exit(), vectChanAndFrac);
        debug() << "Size of the vector of FT pairs after the call of"
                << "function calculateHits: " << vectChanAndFrac.size() << endmsg;
      }

    }// end loop over hits
  }// end if( m_deFT != 0 )
  
  else {
    error() << "m_DeFT is not valid" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DeFTTestAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

