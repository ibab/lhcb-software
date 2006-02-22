// $Id: IVeloClusterPosition.h,v 1.2 2006-02-22 11:27:03 szumlat Exp $
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
  // stripPair keeps the closest channel assigned to strip that is
  // the closest one to the calculated cluster centre and fractional
  // distance to the strip
  typedef std::pair<LHCb::VeloChannelID, double> stripPair;
  // object that is returned from the tool, consists of strip
  // info and error estimate of the cluster position in the
  // units of strip distance (local pitch)
  typedef std::pair<stripPair, double> toolPair;
  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_IVeloClusterPos;}
  // the main method to retrieve the cluster position and error
  // userInfo is the input object that must be provided by user
  // this is a pair that holds information of projected angle of
  // a track (while not good enough parametrizations for the
  // angled track are available set this to 0.) and a radius
  // for clusters on the Phi sensor; the radius also could be
  // set to 0., in that case default radius will be used 
  // (depending on sensor zone)
  virtual toolPair position(const LHCb::VeloCluster* cluster,
                            Pair& userInfo)=0;
  // the method calculate the position using linear charge sharing
  // approximation
  virtual Pair fractionalPosMean(
                   const LHCb::VeloCluster* cluster)=0;
  // cluster position calculation based on eta variabel - non linear
  // charge sharing approximation (particularly useful for low angle 
  // tracks)
  virtual Pair fractionalPosEta(
          const LHCb::VeloCluster* cluster,
          double alphaOfTrack)=0;
  // method used to calibration plots - eta variable vs. fractional
  // position
  virtual Pair etaFrac(
                   const LHCb::VeloCluster* cluster,
                   double fracPosTrue)=0;
  // helper method
  virtual std::string sensType()=0;
  virtual void setSensType(std::string type)=0;
  
protected:

  // algorithm to calculate the centre position of the cluster
  // based on linear approximation
  virtual toolPair weightedMeanPos(
                   const LHCb::VeloCluster* cluster,
                   Pair& userInfo)=0;
  // algorithm to calculate cluster centre position using eta
  // variable 
  virtual toolPair etaFitPos(
                   const LHCb::VeloCluster* cluster,
                   Pair& userInfo)=0;
  // determination of the VeloChannelID and error (the same code for
  // both linear and eta variable approches)
  virtual void posAndError(
               const LHCb::VeloCluster* cluster,
               const LHCb::VeloChannelID& intDistanceID,
               const double& fractionalPos,
               Pair& userInfo, Pair& toolInfo)=0;
  // error estimate based on resolution parametrization from silicon
  // simulation
  virtual double resolution(const double& pitch,
                            const Pair& resInfo)=0;
  // helper methods
  virtual void createResParTable()=0;
  virtual void createEtaParTable()=0;

private:

};
#endif // VELOALGORITHMS_IVELOCLUSTERPOS_H
