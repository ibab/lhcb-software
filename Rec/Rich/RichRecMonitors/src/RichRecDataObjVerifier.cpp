
//-----------------------------------------------------------------------------
/** @file RichRecDataObjVerifier.cpp
 *
 *  Implementation file for algorithm class : RichRecDataObjVerifier
 *
 *  CVS Log :-
 *  $Id: RichRecDataObjVerifier.cpp,v 1.2 2006-01-23 14:10:48 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecDataObjVerifier.h"

// namespace
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecDataObjVerifier>          s_factory ;
const        IAlgFactory& RichRecDataObjVerifierFactory = s_factory ;


// Standard constructor, initializes variables
RichRecDataObjVerifier::RichRecDataObjVerifier( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator ) 
{
  declareProperty( "PrintPixels",   m_bdPixels   = false );
  declareProperty( "PrintTracks",   m_bdTracks   = false );
  declareProperty( "PrintSegments", m_bdSegments = false );
  declareProperty( "PrintPhotons",  m_bdPhotons  = false );
}

// Destructor
RichRecDataObjVerifier::~RichRecDataObjVerifier() {};

//  Initialize
StatusCode RichRecDataObjVerifier::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // do stuff here...

  return sc;
}


// Main execution
StatusCode RichRecDataObjVerifier::execute()
{
  debug() << "Execute" << endreq;

  // If not rerunning at DEBUG or VERBOSE level, return
  if ( !msgLevel(MSG::DEBUG) ) return StatusCode::SUCCESS;

  // Debug printout of pixels
  if ( m_bdPixels && !richPixels()->empty() ) {
    debug() << "Located " << richPixels()->size() << " RichRecPixels" << endreq;
    for ( RichRecPixels::const_iterator pixel = richPixels()->begin();
          pixel != richPixels()->end();
          pixel++ ) {

      debug() << "RichRecPixel key= " << (*pixel)->key() << endreq;
      debug() << "  richRecPhotons(" << (*pixel)->richRecPhotons().size() << ") keys= ";
      for ( RichRecPixel::Photons::const_iterator iphot
              = (*pixel)->richRecPhotons().begin();
            iphot != (*pixel)->richRecPhotons().end();
            iphot++ ) { debug() << (*iphot)->key() << " "; }
      debug() << endreq;
      debug() << "  richRecTracks(" << (*pixel)->richRecTracks().size() << ") keys= ";
      for ( RichRecPixel::Tracks::const_iterator itk
              = (*pixel)->richRecTracks().begin();
            itk != (*pixel)->richRecTracks().end();
            itk++ ) { debug() << (*itk)->key() << " "; }
      debug() << endreq;
      std::cout << "Data members " << **pixel << std::endl;

    }
  }

  // Debug printout of tracks
  if ( m_bdTracks && !richTracks()->empty() ) {
    debug() << "Located " << richTracks()->size() << " RichRecTracks" << endreq;
    for ( RichRecTracks::const_iterator track = richTracks()->begin();
          track != richTracks()->end();
          track++ ) {

      debug() << "RichRecTrack key= " << (*track)->key() << endreq;
      debug() << "  richRecSegments(" << (*track)->richRecSegments().size() << ") keys= ";
      for ( RichRecTrack::Segments::const_iterator iseg
              = (*track)->richRecSegments().begin();
            iseg != (*track)->richRecSegments().end();
            iseg++ ) { debug() << (*iseg)->key() << " "; }
      debug() << endreq;
      /*
        debug() << "  richRecPixels(" << (*track)->richRecPixels().size() << ") keys= ";
        for ( RichRecTrack::Pixels::const_iterator ipix
        = (*track)->richRecPixels().begin();
        ipix != (*track)->richRecPixels().end();
        ipix++ ) { debug() << (*ipix)->key() << " "; }
        debug() << endreq;
      */
      debug() << "  richRecPhotons(" << (*track)->richRecPhotons().size() << ") keys= ";
      for ( RichRecTrack::Photons::const_iterator iphot
              = (*track)->richRecPhotons().begin();
            iphot != (*track)->richRecPhotons().end();
            iphot++ ) { debug() << (*iphot)->key() << " "; }
      debug() << endreq;
      std::cout << "Data members " << **track << std::endl;

    }
  }

  // Debug printout of segments
  if ( m_bdSegments && !richSegments()->empty() ) {
    debug() << "Located " << richSegments()->size() << " RichRecSegments" << endreq;
    for ( RichRecSegments::const_iterator segment = richSegments()->begin();
          segment != richSegments()->end();
          segment++ ) {

      debug() << "RichRecSegment key= " << (*segment)->key() << endreq;
      debug() << "  richRecTrack key= " << (*segment)->richRecTrack()->key() << endreq;
      debug() << "  richRecPixels(" << (*segment)->richRecPixels().size() << ") keys= ";
      for ( RichRecSegment::Pixels::const_iterator ipix
              = (*segment)->richRecPixels().begin();
            ipix != (*segment)->richRecPixels().end();
            ipix++ ) { debug() << (*ipix)->key() << " "; }
      debug() << endreq;
      debug() << "  richRecPhotons(" << (*segment)->richRecPhotons().size() << ") keys= ";
      for ( RichRecSegment::Photons::const_iterator iphot
              = (*segment)->richRecPhotons().begin();
            iphot != (*segment)->richRecPhotons().end();
            iphot++ ) { debug() << (*iphot)->key() << " "; }
      debug() << endreq;
      std::cout << "Data members " << **segment << std::endl;

    }
  }

  // Debug printout of photons
  if ( m_bdPhotons && !richPhotons()->empty() ) {
    debug() << "Located " << richPhotons()->size() << " RichRecPhotons" << endreq;
    for ( RichRecPhotons::const_iterator photon = richPhotons()->begin();
          photon != richPhotons()->end();
          photon++ ) {

      debug() << "RichRecPhoton    key= " << (*photon)->key() << endreq;
      debug() << "  richRecSegment key= " << (*photon)->richRecSegment()->key() << endreq;
      debug() << "  richRecPixel   key= " << (*photon)->richRecPixel()->key() << endreq;
      std::cout << "Data members " << **photon << std::endl;

    }
  }

  // Check internal consistency of links
  /*
    for ( RichRecTracks::const_iterator track = richTracks()->begin();
    track != richTracks()->end();
    track++ )
    {

    std::vector<RichRecPixel*> pPixels;
    pPixels.clear();
    RichRecTrack::Pixels & pixels = (*track)->richRecPixels();
    for ( RichRecTrack::Pixels::iterator iPix = pixels.begin();
    iPix != pixels.end();
    iPix++ ) {
    RichRecPixel* pPix = *iPix;
    for ( std::vector<RichRecPixel*>::iterator p = pPixels.begin();
    p != pPixels.end();
    p++ ) {
    if ( *p == pPix ) {
    err() << "Track " << (*track)->key()
    << " has multiple refernces to pixel " << pPix->key() << endreq;
    }

    }

    pPixels.push_back( pPix );
    }

    }
  */

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichRecDataObjVerifier::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
