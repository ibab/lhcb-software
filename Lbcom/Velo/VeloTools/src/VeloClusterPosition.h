// $Id: VeloClusterPosition.h,v 1.6 2007-06-01 10:36:18 szumlat Exp $
#ifndef VELOCLUSTERPOSITION_H 
#define VELOCLUSTERPOSITION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IVeloClusterPosition.h"            // Interface
#include "Kernel/VeloChannelID.h"
#include "Kernel/SiPositionInfo.h"

/** @class VeloClusterPosition VeloClusterPosition.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-09-30
 */

class DeVelo;

class VeloClusterPosition : public GaudiTool, virtual public IVeloClusterPosition {
public:

  // throughout the code LA stands for Linear Approximation
  // typedefs for object returned by tool
  typedef IVeloClusterPosition::toolInfo toolInfo;
  typedef IVeloClusterPosition::Pair Pair;
  // structure to keep resolution parametrisations
  typedef std::pair<double, std::pair<double, double> > ResPair;
  typedef std::vector<ResPair> ResVector;

  virtual StatusCode initialize();    ///< Tool initialization
  virtual StatusCode finalize();      ///< Tool finalization
  /// Standard constructor
  VeloClusterPosition( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  virtual ~VeloClusterPosition( ); ///< Destructor
  //
  virtual toolInfo position(const LHCb::VeloCluster* cluster) const;
  virtual toolInfo position(const LHCb::VeloCluster* cluster,
                            const Gaudi::XYZPoint& aPoint,
                            const Pair& aDirection) const;
  virtual Pair fracPosLA(const LHCb::VeloCluster* cluster) const;
  double angleOfTrack(const Pair localSlopes,
                      Gaudi::XYZVector& parallel2Track) const;
  double projectedAngle() const;
  
protected:

  toolInfo weightedLAPos(const LHCb::VeloCluster* cluster) const;
  double errorEstimate(const LHCb::VeloCluster* cluster,
                       const LHCb::VeloChannelID& intDistanceID,
                       const double& fractionalPos) const;
  double resolution(const double& pitch,
                            const int& sensType) const;
  StatusCode createResParaTable();
  double errorOnR(const double projAngle, const double pitch) const;
  double errorOnPhi(const double projAngle, const double pitch) const;
  void setOutsideFlag() const;
  bool outsideFlag() const;
  
private:

  DeVelo* m_veloDet;
  std::vector<double> m_defaultRSensorResolution;
  std::vector<double> m_defaultPhiSensorResolution;
  std::vector<double> m_rAngleLimits;
  std::vector<double> m_phiAngleLimits;
  std::vector<double> m_rConstPara;
  std::vector<double> m_rSlopePara;
  std::vector<double> m_phiConstPara;
  std::vector<double> m_phiSlopePara;
  ResVector m_rParaTable;
  ResVector m_phiParaTable;
  mutable double m_projectedAngle;
  mutable bool m_isOutsideSensor;
    
};
#endif // VELOCLUSTERPOS_H
