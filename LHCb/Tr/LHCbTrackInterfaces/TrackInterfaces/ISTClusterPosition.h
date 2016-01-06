// $Id: $
#ifndef TrackInterfaces_ISTClusterPosition_H
#define TrackInterfaces_ISTClusterPosition_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SmartRefVector.h"

#include "Event/StateVector.h"

#include "Kernel/SiPositionInfo.h"

namespace LHCb{
class STChannelID;
class STCluster;
class STDigit;
}

/** @class ISTClusterPosition TrackInterfaces/ISTClusterPosition.h
 *
 *  Interface Class for charge sharing IT strip
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */


class  ISTClusterPosition: public extend_interfaces<IAlgTool> {

public:
  // Declaration of the interface ID ( interface id, major version, minor version)
  DeclareInterfaceID( ISTClusterPosition, 1 , 0);

  typedef LHCb::SiPositionInfo<LHCb::STChannelID> Info;

  /** calc position
  * @param aCluster cluster
  * @return Info (simple struct)
  * <br> strip = floored nearest channel
  * <br> fractionStrip = interstrip position (in fraction of strip)
  * <br> error = estimate of the error
  */
  virtual Info estimate(const LHCb::STCluster* aCluster) const=0;


  /** calc position
  * @param aCluster cluster
  * @param refVector Reference vector from the track
  * @return Info (simple struct)
  * <br> strip = floored nearest channel
  * <br> fractionStrip = interstrip position (in fraction of strip)
  * <br> error = estimate of the error
  */
  virtual Info estimate(const LHCb::STCluster* aCluster,
                        const LHCb::StateVector& refVector) const=0;

  /** calc position
  * @param digits vector of digits
  * @return Measurement (pair of pairs)
  * <br> first.first = floored nearest channel
  * <br> first.second = interstrip position (in fraction of strip)
  * <br> second = estimate of the error
  */
  virtual Info estimate(const SmartRefVector<LHCb::STDigit>& digits) const=0;

  /** error parameterized as cluster size
  * @param nStrips number of strips
  * @return error estimate
  */
  virtual double error(const unsigned int nStrips) const =0;

  /** error parameterized as cluster size
  * @param nStrips number of strips
  * @param refVector Reference vector from the track
  * @return error estimate
  */
  virtual double error(const unsigned int nStrips,
                       const LHCb::StateVector& refVector) const =0;

};

#endif // TrackInterfaces_ISTClusterPosition_H
