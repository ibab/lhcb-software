// $Id: RichPixelStrippingAlg.cpp,v 1.3 2004-07-12 14:52:31 jonrob Exp $

// local
#include "RichPixelStrippingAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPixelStrippingAlg
//
// 2004-06-20 : Andy Buckley Andrew.Buckley@cern.ch
// 2004-06-20 : Chris Jones  Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPixelStrippingAlg>          s_factory ;
const        IAlgFactory& RichPixelStrippingAlgFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichPixelStrippingAlg::RichPixelStrippingAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : RichRecAlgBase ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
RichPixelStrippingAlg::~RichPixelStrippingAlg() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode RichPixelStrippingAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichPixelStrippingAlg::execute()
{
  debug() << "Execute" << endreq;

  // Locate RichRecRings from TES
  SmartDataPtr<RichRecRings> rings( eventSvc(), RichRecRingLocation::MarkovRings );
  if ( !rings ) {
    warning() << "Failed to find RichRecRings at " << RichRecRingLocation::MarkovRings << endreq;
    return StatusCode::SUCCESS;
  } else {

    if ( msgLevel(MSG::DEBUG) ) {
      debug() << "Successfully located " << rings->size() << " RichRecRings at "
              << RichRecRingLocation::MarkovRings << endreq;
    }

    // Create and initialise decision map for stripping out Markov-unassociated pixels
    std::map<RichRecPixel*,bool> decisionMap;
    for ( RichRecPixels::const_iterator iPix = richPixels()->begin(); iPix != richPixels()->end(); ++iPix ) {
      // Set true by default if in RICH1, otherwise false
      decisionMap[*iPix] = ( Rich::Rich1 == (*iPix)->smartID().rich() );
    }


    // Loop over all Markov rings
    for ( RichRecRings::const_iterator iRing = rings->begin(); iRing != rings->end(); ++iRing ) {

      // Set "Markov-seen pixels" to be selected by stripping
      SmartRefVector<RichRecPixel> & pixels = (*iRing)->richRecPixels();
      for ( SmartRefVector<RichRecPixel>::iterator iPix = pixels.begin(); iPix != pixels.end(); ++iPix ) {
        if ( (*iRing)->richRecSegment() )
          decisionMap[*iPix] = true;
      }
    }

    // Loop over all Markov rings
    for ( RichRecRings::iterator iRing = rings->begin(); iRing != rings->end(); ++iRing ) {

      // Clean up SmartRefs in the ring itself
      SmartRefVector<RichRecPixel> & pixels = (*iRing)->richRecPixels();
      std::vector<RichRecPixel*> pixsToRemove;
      for ( SmartRefVector<RichRecPixel>::iterator iPix = pixels.begin(); iPix != pixels.end(); ++iPix ) {
        if ( !decisionMap[*iPix] ) pixsToRemove.push_back(*iPix);
      }
      for ( std::vector<RichRecPixel*>::iterator iPix = pixsToRemove.begin(); iPix != pixsToRemove.end(); ++iPix) {
        (*iRing)->removeFromRichRecPixels(*iPix);
      }
    }

    // Actually remove stripped pixels as indicated by decisionMap
    for ( std::map<RichRecPixel*, bool>::const_iterator iP = decisionMap.begin(); iP != decisionMap.end(); ++iP) {
      if ( ! (*iP).second  ) richPixels()->remove( (*iP).first );
    }


  }


  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichPixelStrippingAlg::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}

//=============================================================================
