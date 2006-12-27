// $Id: VeloClusterPosition.h,v 1.2 2006-12-27 17:47:24 szumlat Exp $
#ifndef VELOCLUSTERPOS_H 
#define VELOCLUSTERPOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IVeloClusterPosition.h"            // Interface
#include "Kernel/VeloChannelID.h"
#include "Kernel/SiPositionInfo.h"

/** @class VeloClusterPos VeloClusterPos.h
 *  
 *   typedefs for object returned by tool
 *   typedef LHCb::SiPositionInfo<LHCb::VeloChannelID> toolInfo;
 *   typedef std::pair<double, double> Pair;
 *   create structure to keep resolution parametrisations needed to
 *   calculate error of the R or Phi cluster position; the structure
 *   is a vector of pairs, first component of each pair holds value
 *   of the angle for which the resolution parametrisation was determ.
 *   second component holds pair with parameters descrobing linear fit
 *   to resolution vs. pitch
 *   typedef std::pair<double, std::pair<double, double> > ResPair;
 *   typedef std::vector<ResPair> ResTable;
 *   create structure to keep eta fits to calculate fractional
 *   distance needed to determine cluster resolution on the sensors
 *   the structure is a vector of pairs, first component also holds
 *   the angle value, second holds vector with eta fit parameters
 *   (polynomial of 3rd order)
 *
 *   The tool is meant to return for a given cluster a toolInfo object.
 *   Each such an object is actually instantiation of the templated structure
 *   SiPositionInfo (LHCbKernel) with VeloChannelID as template parameter:
 *   ==> typedef SiPositionInfo<VeloChannelID> toolInfo
 *   One can get from a toolInfo object following information:

 *   1) channel ID of a strip that is closest to the centre of a given cluster
 *      and floored
 *   For example: let us consider two clusters (a and b) with two strips
 *                 
 *       a)    40 ADC ||         || 10 ADC        b)  10 ADC ||         || 40 ADC
 *            strip 0 ^   strip 1^                   strip 0 ^   strip 1^
 *   
 *   Centre of the first cluster is much closer to the strip designated as 'strip 0'
 *   and otherwise for the second cluster. However, for both cases channel ID of the
 *   strip number 0 will be returned. This behaviour was implementet in order to follow
 *   the data format defined for the output stream of the TELL1 board.
 *
 *   2) fractional cluster's centre position in units of pitch; this value is always
 *      within range (0, 1)
 *
 *   3) fractional error of the cluster's centre position, also, in units of pitch
 *
 *  @author Tomasz Szumlak
 *  @date   2005-09-30
 */

class DeVelo;

class VeloClusterPosition : public GaudiTool, virtual public IVeloClusterPosition {
public:

  // typedefs for object returned by tool
  typedef LHCb::SiPositionInfo<LHCb::VeloChannelID> toolInfo;
  typedef std::pair<double, double> Pair;
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
  /*
   Standard constructor
  */
  VeloClusterPosition( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~VeloClusterPosition( ); ///< Destructor
  /*
   The position method returns toolInfo structure for a given cluster
   that is passed as the method argument
  */
  virtual toolInfo position(const LHCb::VeloCluster* cluster) const;
  /*
   Returns toolInfo structure for a given cluster, in the case of
   Phi cluster user can pass also radius of the cluster, if no
   radius is provided the method will provide the value as mean
   radius of the inner or outer strip 
  */
  virtual toolInfo position(const LHCb::VeloCluster* cluster,
                            double radiusOfCluster) const;
  virtual toolInfo position(const LHCb::VeloCluster* cluster,
                            Pair& userInfo) const;
  virtual Pair fractionalPosMean(
                   const LHCb::VeloCluster* cluster) const;
  virtual Pair fractionalPosEta(
                   const LHCb::VeloCluster* cluster,
                   double alphaOfTrack) const;
  virtual Pair etaFrac(
                   const LHCb::VeloCluster* cluster,
                   double fracPosTrue=0.) const;
  
protected:

  virtual toolInfo weightedMeanPos(
                   const LHCb::VeloCluster* cluster,
                   Pair& userInfo) const;
  virtual toolInfo etaFitPos(
                   const LHCb::VeloCluster* cluster,
                   Pair& userInfo) const;
  virtual void posAndError(
               const LHCb::VeloCluster* cluster,
               const LHCb::VeloChannelID& intDistanceID,
               const double& fractionalPos,
               Pair& userInfo, Pair& toolInfo) const;
  virtual double resolution(const double& pitch,
                            const Pair& resInfo) const;
  virtual void createResParTable();
  virtual void createEtaParTable();
  
private:

  DeVelo* m_veloDet;
  bool m_printInfo;
  bool m_useWeightedMean;
  bool m_useEtaFit;
  std::vector<double> m_paraTypes;
  std::vector<double> m_defaultRSensorResolution;
  std::vector<double> m_defaultPhiSensorResolution;
  std::vector<double> m_defaultEtaPar;
  // parametrisations for error calculation
  std::vector<double> m_resParameters;
  std::vector<double> m_etaParameters;
  ResTable m_resTable;
  EtaTable m_etaTable;
  
};
#endif // VELOCLUSTERPOS_H
