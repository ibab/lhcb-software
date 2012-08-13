#ifndef IPRFORWARDTOOL_H 
#define IPRFORWARDTOOL_H 1

// Include files
// from STL
#include <string>
#include "GaudiAlg/GaudiTool.h"
#include "Event/Track.h"
#include "PrKernel/IPrDebugTool.h"
#include "PrForwardTrack.h"

static const InterfaceID IID_IPrForwardTool ( "IPrForwardTool", 1, 0 );

/** @class IPrForwardTool IPrForwardTool.h
 *  Interface for the various implementations of the Forward tool
 *
 *  @author Olivier Callot
 *  @date   2012-07-13
 */
class IPrForwardTool : public GaudiTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrForwardTool; }

  IPrForwardTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent) : GaudiTool( type, name, parent ) {};


  virtual void extendTrack( LHCb::Track* velo, LHCb::Tracks* result ) = 0;

  void setDebugParams( IPrDebugTool* tool, int key, int veloKey ) {
    m_debugTool = tool;
    m_wantedKey = key;
    m_veloKey   = veloKey;
  }

  bool matchKey( const PrHit* hit ) {
    if ( m_debugTool ) return m_debugTool->matchKey( hit->id(), m_wantedKey );
    return false;
  };

  void printHit ( const PrHit* hit, std::string title = "" ) {
    info() << "  " << title
           << format( "Plane%3d zone%2d z0 %8.2f x0 %8.2f  coord %8.2f size%2d charge%3d used%2d ",
                      hit->planeCode(), hit->zone(), hit->z(), hit->x(), hit->coord(),
                      hit->size(), hit->charge(), hit->isUsed() );
    if ( m_debugTool ) m_debugTool->printKey( info(), hit->id() );
    if ( matchKey( hit ) ) info() << " ***";
    info() << endmsg;
  }

  //=========================================================================
  //  Print the content of a track
  //=========================================================================
  void printTrack( const PrForwardTrack& track ) {
    if ( 0 < track.nDoF() ) {
      float dx = track.x( 8500.) - track.xFromVelo( 8500. );
      
      info() << format( "   Track nDoF %3d   chi2/nDoF %7.3f  dXCoord %7.3f dy%7.2f quality %6.3f cy%8.3f dx%8.2f Chi2YMag %7.3f",
                        track.nDoF(), track.chi2PerDoF(), track.dXCoord(), track.meanDy(), track.quality(),
                        1.e6 * track.cy(), dx, track.chi2AtMagnet() ) 
             << endmsg;
    } else {
      info() << "   Track" << endmsg;
    }

    for ( PrHits::const_iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
      info() << format( "dist %7.3f dy %7.2f chi2 %7.2f ", track.distance( *itH ), track.deltaY( *itH ), track.chi2( *itH ) );
      printHit( *itH );
    }
  }
  //=========================================================================
  //  Print the hits for a given track
  //=========================================================================
  void printHitsForTrack( const PrHits& hits, const PrForwardTrack& track ) {
    info() << "=== Hits to Track ===" << endmsg;
    for ( PrHits::const_iterator itH = hits.begin(); hits.end() != itH; ++itH ) {
      info() << format( "dist %7.3f dy %7.2f chi2 %7.2f ", track.distance( *itH ), track.deltaY( *itH ), track.chi2( *itH ) );
      printHit( *itH );
    }
  }
protected:
  IPrDebugTool*   m_debugTool;
  int             m_wantedKey;
  int             m_veloKey;
private:
};
#endif // IPRFORWARDTOOL_H
