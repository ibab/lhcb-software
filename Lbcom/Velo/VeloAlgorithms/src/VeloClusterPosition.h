// $Id: VeloClusterPosition.h,v 1.3 2006-03-01 13:52:08 szumlat Exp $
#ifndef VELOCLUSTERPOS_H 
#define VELOCLUSTERPOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IVeloClusterPosition.h"            // Interface
#include "Kernel/VeloChannelID.h"
#include "VeloDet/DeVelo.h"
#include "Kernel/SiPositionInfo.h"

/** @class VeloClusterPos VeloClusterPos.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-09-30
 */

class DeVelo;
class DeVeloRType;
class DeVeloPhiType;

class VeloClusterPosition : public GaudiTool, virtual public IVeloClusterPosition {
public:

  // typedefs for object returned by tool
  typedef LHCb::SiPositionInfo<LHCb::VeloChannelID> toolInfo;
  typedef std::pair<double, double> Pair;
  //    typedef std::pair<LHCb::VeloChannelID, double> stripPair;
  //    typedef std::pair<stripPair, double> toolPair;
  // create structure to keep resolution parametrisations needed to
  // calculate error of the R or Phi cluster position; the structure
  // is a vector of pairs, first component of each pair holds value
  // of the angle for which the resolution parametrisation was determ.
  // second component holds pair with parameters descrobing linear fit
  // to resolution vs. pitch
  typedef std::pair<double, std::pair<double, double> > ResPair;
  typedef std::vector<ResPair> ResTable;
  // create structure to keep eta fits to calculate fractional
  // distance needed to determine cluster resolution on the sensors
  // the structure is a vector of pairs, first component also holds
  // the angle value, second holds vector with eta fit parameters
  // (polynomial of 3rd order)
  typedef std::vector<double> EtaPar;
  typedef std::pair<double, EtaPar> EtaPair;
  typedef std::vector<EtaPair> EtaTable;

  /// Standard constructor
  VeloClusterPosition( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~VeloClusterPosition( ); ///< Destructor

  virtual toolInfo position(const LHCb::VeloCluster* cluster);
  virtual toolInfo position(const LHCb::VeloCluster* cluster,
                            double radiusOfCluster);
  virtual toolInfo position(const LHCb::VeloCluster* cluster,
                            Pair& userInfo);
  virtual Pair fractionalPosMean(
                   const LHCb::VeloCluster* cluster);
  virtual Pair fractionalPosEta(
                   const LHCb::VeloCluster* cluster,
                   double alphaOfTrack);
  virtual Pair etaFrac(
                   const LHCb::VeloCluster* cluster,
                   double fracPosTrue=0.);
  
protected:

  virtual toolInfo weightedMeanPos(
                   const LHCb::VeloCluster* cluster,
                   Pair& userInfo);
  virtual toolInfo etaFitPos(
                   const LHCb::VeloCluster* cluster,
                   Pair& userInfo);
  virtual void posAndError(
               const LHCb::VeloCluster* cluster,
               const LHCb::VeloChannelID& intDistanceID,
               const double& fractionalPos,
               Pair& userInfo, Pair& toolInfo);
  virtual double resolution(const double& pitch,
                            const Pair& resInfo);
  virtual void createResParTable();
  virtual void createEtaParTable();
  
private:

  DeVelo* m_veloDet;
  bool m_printInfo;
  bool m_useWeightedMean;
  bool m_useEtaFit;
  std::vector<double> m_paraTypes;
  std::vector<double> m_defaultResOneStrip;
  std::vector<double> m_defaultResTwoStrip;
  std::vector<double> m_defaultEtaPar;
  // parametrisations for error calculation
  std::vector<double> m_resParameters;
  std::vector<double> m_etaParameters;
  ResTable m_resTable;
  EtaTable m_etaTable;
  
};
#endif // VELOCLUSTERPOS_H
