// $Id: IVeloClusterPosition.h,v 1.5 2006-04-06 13:47:47 dhcroft Exp $
#ifndef VELOALGORITHMS_IVELOCLUSTERPOSITION_H 
#define VELOALGORITHMS_IVELOCLUSTERPOSITION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "SiPositionInfo.h"

static const InterfaceID IID_IVeloClusterPos ("IVeloClusterPos", 1, 0);

/** @class IVeloClusterPos IVeloClusterPos.h VeloAlgorithms/IVeloClusterPos.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-09-30
 */

namespace LHCb
{
  class VeloCluster;
  class VeloChannelID;
}

class IVeloClusterPosition : virtual public IAlgTool{
public:

  // a new tool interface, common for Velo and ST 
  // (defined inside Kernel/SiPositonInfo.h)
  typedef LHCb::SiPositionInfo<LHCb::VeloChannelID> toolInfo;
  typedef std::pair<double, double> Pair;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_IVeloClusterPos;}
  // the main methods to retrieve the cluster position and error
  // userInfo is the input object that must be provided by user
  // this is a pair that holds information of projected angle of
  // a track (while not good enough parametrizations for the
  // angled track are available set this to 0.) and a radius
  // for clusters on the Phi sensor; the radius also could be
  // set to 0., in that case default radius will be used 
  // (depending on sensor zone)

  /// postion of the cluster given slopes
  virtual toolInfo position(const LHCb::VeloCluster* cluster,
                            Pair& userInfo) const =0;
  /// postion of the cluster
  virtual toolInfo position(const LHCb::VeloCluster* cluster) const =0;
  /// postion of the cluster at known radius
  virtual toolInfo position(const LHCb::VeloCluster* cluster,
                            double radiusOfCluster)  const =0;
  /// the method calculate the position using linear charge sharing 
  /// approximation
  virtual Pair fractionalPosMean(
                   const LHCb::VeloCluster* cluster) const =0;
  // cluster position calculation based on eta variabel - non linear
  // charge sharing approximation (particularly useful for low angle 
  // tracks
  /// fractional position using eta fit.
  virtual Pair fractionalPosEta(
          const LHCb::VeloCluster* cluster,
          double alphaOfTrack) const =0;
  /// method used to calibration plots - eta variable vs. fractional position
  virtual Pair etaFrac(
                   const LHCb::VeloCluster* cluster,
                   double fracPosTrue)const =0;

};
#endif // VELOALGORITHMS_IVELOCLUSTERPOS_H
