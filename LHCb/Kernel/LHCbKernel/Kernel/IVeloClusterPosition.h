// $Id: IVeloClusterPosition.h,v 1.1 2006-02-21 17:38:15 szumlat Exp $
#ifndef VELOALGORITHMS_IVELOCLUSTERPOSITION_H 
#define VELOALGORITHMS_IVELOCLUSTERPOSITION_H 1

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
  class VeloCluster;
  class VeloChannelID;
}

class IVeloClusterPosition : virtual public IAlgTool{
public:

  typedef std::pair<double, double> Pair;
  typedef std::pair<LHCb::VeloChannelID, double> stripPair;
  typedef std::pair<stripPair, double> toolPair;
  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_IVeloClusterPos;}
  //
  virtual toolPair position(const LHCb::VeloCluster* cluster,
                            Pair& userInfo)=0;
  virtual Pair fractionalPosMean(
                   const LHCb::VeloCluster* cluster)=0;
  virtual Pair fractionalPosEta(
          const LHCb::VeloCluster* cluster,
          double alphaOfTrack)=0;
  virtual Pair etaFrac(
                   const LHCb::VeloCluster* cluster,
                   double fracPosTrue)=0;
  virtual std::string sensType()=0;
  virtual void setSensType(std::string type)=0;
  
protected:
  
  virtual toolPair weightedMeanPos(
                   const LHCb::VeloCluster* cluster,
                   Pair& userInfo)=0;
  virtual toolPair etaFitPos(
                   const LHCb::VeloCluster* cluster,
                   Pair& userInfo)=0;
  virtual void posAndError(
               const LHCb::VeloCluster* cluster,
               const LHCb::VeloChannelID& intDistanceID,
               const double& fractionalPos,
               Pair& userInfo, Pair& toolInfo)=0;
  virtual double resolution(const double& pitch,
                            const Pair& resInfo)=0;
  virtual void createResParTable()=0;
  virtual void createEtaParTable()=0;

private:

};
#endif // VELOALGORITHMS_IVELOCLUSTERPOS_H
