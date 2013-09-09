#ifndef BUILDMCTRACKWITHVPINFO_H 
#define BUILDMCTRACKWITHVPINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/MCTrackInfo.h"

class DeVP;
class DeSTDetector;
class DeOTStation;

/** @class BuildMCTrackWithVPInfo BuildMCTrackWithVPInfo.h
 *  Build the Reconstructable MCProperty table.
 *
 *  @author Olivier Callot
 *  @date   2004-01-08
 */
class BuildMCTrackWithVPInfo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  BuildMCTrackWithVPInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BuildMCTrackWithVPInfo( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

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

  struct increasingSensor {
     bool operator() ( LHCb::VPCluster* clust1, LHCb::VPCluster* clust2 ) const{
       return clust1->channelID().module() < clust2->channelID().module();
     }
  }  increasingSensor;

private:
  DeVP*         m_vpDet;
  DeSTDetector* m_ttDet;
  DeSTDetector* m_itDet;
  DeOTDetector* m_otDet;
  
};
#endif // BUILDMCTRACKWITHVELOPIXINFO_H
