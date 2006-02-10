// $Id: IVeloClusterPos.h,v 1.1 2006-02-10 12:48:09 cattanem Exp $
#ifndef VELOALGORITHMS_IVELOCLUSTERPOS_H 
#define VELOALGORITHMS_IVELOCLUSTERPOS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IVeloClusterPos ("IVeloClusterPos", 1, 0);

/** @class IVeloClusterPos IVeloClusterPos.h VeloAlgorithms/IVeloClusterPos.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-09-30
 */

namespace LHCb
{
  class InternalVeloCluster;
  class VeloChannelID;
}

class IVeloClusterPos : virtual public IAlgTool{
public:

  typedef std::vector<double> toolVec;
  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_IVeloClusterPos;}
  //
  virtual LHCb::VeloChannelID position(
                        const LHCb::InternalVeloCluster* cluster,
                        toolVec& userInfo, toolVec& toolInfo)=0;
  typedef std::pair<double, double> fracPair;
  virtual fracPair fractionalPosMean(
                   const LHCb::InternalVeloCluster* cluster)=0;
  virtual fracPair fractionalPosEta(
          const LHCb::InternalVeloCluster* cluster,
          const double& fracTrue, fracPair& fracEta)=0;
  
protected:
  
  virtual LHCb::VeloChannelID weightedMeanPos(
                const LHCb::InternalVeloCluster* cluster,
                toolVec& userInfo, toolVec& toolInfo)=0;
  virtual LHCb::VeloChannelID etaFitPos(
                const LHCb::InternalVeloCluster* cluster,
                toolVec& userInfo, toolVec& toolInfo)=0;
  virtual void posAndError(
               const LHCb::InternalVeloCluster* cluster,
               const LHCb::VeloChannelID& intDistanceID,
               const double& fractionalPos,
               toolVec& userInfo, toolVec& toolInfo)=0;
  virtual double resolution(const double& pitch)=0;

private:

};
#endif // VELOALGORITHMS_IVELOCLUSTERPOS_H
