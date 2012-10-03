#ifndef BUILDMCTRACKINFOUPGRADE_H 
#define BUILDMCTRACKINFOUPGRADE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/MCTrackInfo.h"

class DeVelo;
class DeVP;
class DeSTDetector;
class DeOTDetector;
class DeFTDetector;

/** @class BuildMCTrackInfoUpgrade BuildMCTrackInfoUpgrade.h
 *  Build the Reconstructable MCProperty table.
 *
 *  @author Olivier Callot
 *  @date   2012-04-02 : Updated version for upgrade: IT, OT and FT optional, Pixel/normal Velo
 */
class BuildMCTrackInfoUpgrade : public GaudiAlgorithm {
public: 
  /// Standard constructor
  BuildMCTrackInfoUpgrade( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BuildMCTrackInfoUpgrade( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  void updateBit( int& result, int sta, bool isX ) {
    if ( 0 == sta ) {
      result |= MCTrackInfo::maskTT1;
    } else if ( 1 == sta ) {
      result |= MCTrackInfo::maskTT2;
    } else if ( 2 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskT1X;
      else       result |= MCTrackInfo::maskT1S;
    } else if ( 3 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskT2X;
      else       result |= MCTrackInfo::maskT2S;
    } else if ( 4 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskT3X;
      else       result |= MCTrackInfo::maskT3S;
    }
  }
  

  void updateAccBit( int& result, int sta, bool isX ) {
    if ( 0 == sta ) {
      result |= MCTrackInfo::maskAccTT1;
    } else if ( 1 == sta ) {
      result |= MCTrackInfo::maskAccTT2;
    } else if ( 2 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskAccT1X;
      else       result |= MCTrackInfo::maskAccT1S;
    } else if ( 3 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskAccT2X;
      else       result |= MCTrackInfo::maskAccT2S;
    } else if ( 4 == sta ) {
      if ( isX ) result |= MCTrackInfo::maskAccT3X;
      else       result |= MCTrackInfo::maskAccT3S;
    }
  }
  
  void computeAcceptance ( std::vector<int>& station ) ;

  ///< method for sorting the VP clusters. Should be in the VPCluster class!
  struct increasingSensor {
     bool operator() ( LHCb::VPCluster* clust1, LHCb::VPCluster* clust2 ) const{
       return clust1->channelID().sensor() < clust2->channelID().sensor();
     }
  };

private:
  bool m_withVelo;
  bool m_withVL;
  bool m_withVP;
  bool m_withIT;
  bool m_withOT;
  bool m_withFT;

  DeVelo*       m_velo;
  DeVL*         m_vlDet;
  DeVP*         m_vpDet;

  DeSTDetector* m_ttDet;
  DeSTDetector* m_itDet;

  // OT Detector information
  DeOTDetector* m_otDet;
  DeFTDetector* m_ftDet;
};
#endif // BUILDMCTRACKINFOUPGRADE_H
