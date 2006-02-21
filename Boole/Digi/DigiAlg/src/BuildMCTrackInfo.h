// $Id: BuildMCTrackInfo.h,v 1.1 2006-02-21 17:17:18 ocallot Exp $
#ifndef BUILDMCTRACKINFO_H 
#define BUILDMCTRACKINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "VeloDet/DeVelo.h"
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTStation.h"
#include "Event/MCTrackInfoBits.h"

/** @class BuildMCTrackInfo BuildMCTrackInfo.h
 *  Build the Reconstructable MCProperty table.
 *
 *  @author Olivier Callot
 *  @date   2004-01-08
 */
class BuildMCTrackInfo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  BuildMCTrackInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BuildMCTrackInfo( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void updateBit( int& result, int sta, bool isX ) {
    if ( 0 == sta ) {
      result |= MCTrackInfoBits::maskTT1;
    } else if ( 1 == sta ) {
      result |= MCTrackInfoBits::maskTT2;
    } else if ( 2 == sta ) {
      if ( isX ) result |= MCTrackInfoBits::maskT1X;
      else       result |= MCTrackInfoBits::maskT1S;
    } else if ( 3 == sta ) {
      if ( isX ) result |= MCTrackInfoBits::maskT2X;
      else       result |= MCTrackInfoBits::maskT2S;
    } else if ( 4 == sta ) {
      if ( isX ) result |= MCTrackInfoBits::maskT3X;
      else       result |= MCTrackInfoBits::maskT3S;
    }
  }
  

  void updateAccBit( int& result, int sta, bool isX ) {
    if ( 0 == sta ) {
      result |= MCTrackInfoBits::maskAccTT1;
    } else if ( 1 == sta ) {
      result |= MCTrackInfoBits::maskAccTT2;
    } else if ( 2 == sta ) {
      if ( isX ) result |= MCTrackInfoBits::maskAccT1X;
      else       result |= MCTrackInfoBits::maskAccT1S;
    } else if ( 3 == sta ) {
      if ( isX ) result |= MCTrackInfoBits::maskAccT2X;
      else       result |= MCTrackInfoBits::maskAccT2S;
    } else if ( 4 == sta ) {
      if ( isX ) result |= MCTrackInfoBits::maskAccT3X;
      else       result |= MCTrackInfoBits::maskAccT3S;
    }
  }
  
  void computeAcceptance ( std::vector<int>& station ) ;

private:
  DeVelo*       m_velo;

  DeSTDetector* m_ttDet;
  DeSTDetector* m_itDet;

  // OT Detector information
  DeOTStation* m_OT1Station;        ///< Pointer to OT Station 1
  DeOTStation* m_OT2Station;        ///< Pointer to OT Station 2
  DeOTStation* m_OT3Station;        ///< Pointer to OT Station 3
};
#endif // BUILDMCTRACKINFO_H
