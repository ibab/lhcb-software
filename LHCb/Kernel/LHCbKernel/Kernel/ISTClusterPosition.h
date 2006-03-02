// $Id: ISTClusterPosition.h,v 1.2 2006-03-02 07:40:01 mneedham Exp $
#ifndef _ISTClusterPosition_H
#define _ISTClusterPosition_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SmartRefVector.h"

#include "SiPositionInfo.h"

namespace LHCb{
class STChannelID;
class STCluster;
class STDigit;
};

/** @class ISTClusterPosition ISTClusterPosition.h
 *
 *  Interface Class for charge sharing IT strip
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ISTClusterPosition("ISTClusterPosition", 0 , 0); 

class  ISTClusterPosition: virtual public IAlgTool {

public: 

  typedef LHCb::SiPositionInfo<LHCb::STChannelID> Info;

   /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTClusterPosition; }
   
  /** calc position
  * @param cluster
  * @return Info (simple struct) 
  * strip = floored nearest channel
  * fractionStrip = interstrip position (in fraction of strip)
  * error = estimate of the error 
  */
  virtual Info estimate(const LHCb::STCluster* aCluster) const=0;

  /** calc position
  * @param vector of digits
  * @return Measurement (pair of pairs)
  * first.first = floored nearest channel
  * first.second = interstrip position (in fraction of strip)
  * second = estimate of the error 
  */
  virtual Info estimate(const SmartRefVector<LHCb::STDigit>& digits) const=0;

  /** error parameterized as cluster size
  * @param number of strips
  * @return error estimate
  */
  virtual double error(const unsigned int nStrips) const =0;

};

#endif // _ISTClusterPosition_H



