
//-----------------------------------------------------------------------------
/** @file RichRecDataObjVerifier.cpp
 *
 *  Implementation file for algorithm class : RichRecDataObjVerifier
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecDataObjVerifier.h"

// namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( DataObjVerifier )

// Standard constructor, initializes variables
DataObjVerifier::DataObjVerifier( const std::string& name,
                                  ISvcLocator* pSvcLocator )
  : HistoAlgBase ( name, pSvcLocator )
{
  declareProperty( "PrintPixels",   m_bdPixels   = false );
  declareProperty( "PrintTracks",   m_bdTracks   = false );
  declareProperty( "PrintSegments", m_bdSegments = false );
  declareProperty( "PrintPhotons",  m_bdPhotons  = false );
}

// Destructor
DataObjVerifier::~DataObjVerifier() {}

// Main execution
StatusCode DataObjVerifier::execute()
{
  debug() << "Execute" << endmsg;

  using namespace LHCb;

  const Rich::HistoID hid;

  // If not rerunning at DEBUG or VERBOSE level, return
  //if ( !msgLevel(MSG::DEBUG) ) return StatusCode::SUCCESS;

  // Debug printout of pixels
  if ( m_bdPixels && !richPixels()->empty() )
  {
    debug() << "Located " << richPixels()->size() << " RichRecPixels" << endmsg;
    for ( RichRecPixels::const_iterator pixel = richPixels()->begin();
          pixel != richPixels()->end();
          pixel++ )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "RichRecPixel key= " << (*pixel)->key() << endmsg;
        debug() << "  richRecPhotons(" << (*pixel)->richRecPhotons().size() << ") keys= ";
        for ( RichRecPixel::Photons::const_iterator iphot
                = (*pixel)->richRecPhotons().begin();
              iphot != (*pixel)->richRecPhotons().end();
              iphot++ ) { debug() << (*iphot)->key() << " "; }
        debug() << endmsg;
        debug() << "  richRecTracks(" << (*pixel)->richRecTracks().size() << ") keys= ";
        for ( RichRecPixel::Tracks::const_iterator itk
                = (*pixel)->richRecTracks().begin();
              itk != (*pixel)->richRecTracks().end();
              itk++ ) { debug() << (*itk)->key() << " "; }
        debug() << endmsg;
        std::cout << "Data members " << **pixel << std::endl;
      }
      // plot reference sizes
      plot1D( (*pixel)->richRecTracks().size(), hid((*pixel)->detector(),"pixTracks"),
              "Tracks per pixel", -0.5, 100.5, 101 );
      plot1D( (*pixel)->richRecPhotons().size(), hid((*pixel)->detector(),"pixPhots"),
              "Photons per pixel", -0.5, 100.5, 101 );
      plot1D( (*pixel)->richRecRings().size(), hid((*pixel)->detector(),"pixRings"),
              "Rings per pixel", -0.5, 100.5, 101 );
    }
  }

  // Debug printout of tracks
  if ( m_bdTracks && !richTracks()->empty() )
  {
    debug() << "Located " << richTracks()->size() << " RichRecTracks" << endmsg;
    for ( RichRecTracks::const_iterator track = richTracks()->begin();
          track != richTracks()->end();
          track++ )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "RichRecTrack key= " << (*track)->key() << endmsg;
        debug() << "  richRecSegments(" << (*track)->richRecSegments().size() << ") keys= ";
        for ( RichRecTrack::Segments::const_iterator iseg
                = (*track)->richRecSegments().begin();
              iseg != (*track)->richRecSegments().end();
              iseg++ ) { debug() << (*iseg)->key() << " "; }
        debug() << endmsg;
        /*
          debug() << "  richRecPixels(" << (*track)->richRecPixels().size() << ") keys= ";
          for ( RichRecTrack::Pixels::const_iterator ipix
          = (*track)->richRecPixels().begin();
          ipix != (*track)->richRecPixels().end();
          ipix++ ) { debug() << (*ipix)->key() << " "; }
          debug() << endmsg;
        */
        debug() << "  richRecPhotons(" << (*track)->richRecPhotons().size() << ") keys= ";
        for ( RichRecTrack::Photons::const_iterator iphot
                = (*track)->richRecPhotons().begin();
              iphot != (*track)->richRecPhotons().end();
              iphot++ ) { debug() << (*iphot)->key() << " "; }
        debug() << endmsg;
        std::cout << "Data members " << **track << std::endl;
      }
      // plot reference sizes
      plot1D( (*track)->richRecPhotons().size(), "trackPhots",
              "Photons per track", -0.5, 100.5, 101 );
      plot1D( (*track)->richRecPixels().size(), "trackPixels",
              "Pixels per track", -0.5, 100.5, 101 );
    }
  }

  // Debug printout of segments
  if ( m_bdSegments && !richSegments()->empty() )
  {
    debug() << "Located " << richSegments()->size() << " RichRecSegments" << endmsg;
    for ( RichRecSegments::const_iterator segment = richSegments()->begin();
          segment != richSegments()->end();
          segment++ )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "RichRecSegment key= " << (*segment)->key() << endmsg;
        debug() << "  richRecTrack key= " << (*segment)->richRecTrack()->key() << endmsg;
        debug() << "  richRecPixels(" << (*segment)->richRecPixels().size() << ") keys= ";
        for ( RichRecSegment::Pixels::const_iterator ipix
                = (*segment)->richRecPixels().begin();
              ipix != (*segment)->richRecPixels().end();
              ipix++ ) { debug() << (*ipix)->key() << " "; }
        debug() << endmsg;
        debug() << "  richRecPhotons(" << (*segment)->richRecPhotons().size() << ") keys= ";
        for ( RichRecSegment::Photons::const_iterator iphot
                = (*segment)->richRecPhotons().begin();
              iphot != (*segment)->richRecPhotons().end();
              iphot++ ) { debug() << (*iphot)->key() << " "; }
        debug() << endmsg;
        std::cout << "Data members " << **segment << std::endl;
      }
      // plot reference sizes
      plot1D( (*segment)->richRecPhotons().size(), hid((*segment)->trackSegment().radiator(),"segPhots"),
              "Photons per segment", -0.5, 100.5, 101 );
      plot1D( (*segment)->richRecPixels().size(), hid((*segment)->trackSegment().radiator(),"segPixels"),
              "Pixels per segment", -0.5, 100.5, 101 );

    }
  }

  // Debug printout of photons
  if ( m_bdPhotons && !richPhotons()->empty() )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Located " << richPhotons()->size() << " RichRecPhotons" << endmsg;
      for ( RichRecPhotons::const_iterator photon = richPhotons()->begin();
            photon != richPhotons()->end();
            photon++ )
      {
        debug() << "RichRecPhoton    key= " << (*photon)->key() << endmsg;
        debug() << "  richRecSegment key= " << (*photon)->richRecSegment()->key() << endmsg;
        debug() << "  richRecPixel   key= " << (*photon)->richRecPixel()->key() << endmsg;
        std::cout << "Data members " << **photon << std::endl;
      }
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
    << " has multiple refernces to pixel " << pPix->key() << endmsg;
    }

    }

    pPixels.push_back( pPix );
    }

    }
  */

  return StatusCode::SUCCESS;
}
