// ===========================================================================
#ifndef CALOALGS_COVARIANCEESTIMATOR_H
#define CALOALGS_COVARIANCEESTIMATOR_H 1
// ===========================================================================
// Include files
#include <functional>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/StatusCode.h"

// forward declarations
namespace LHCb {
  class  CaloCluster;
}
class  DeCalorimeter  ;
class  MsgStream      ;

/** @class CovarianceEstimator CovarianceEstimator.h
 *
 *  simple helper class for estimation of covariance matrix
 *  for CaloCluster object.
 *
 *   Model:   ("M{}" means "expectation", "D{}" means "dispersion")
 *
 *   All cluster quantities could be calculated if one knows the matrix
 *
 *   Cov{i,j} = M{(e(i)-M{e(i)})*(e(j)-M{e(j)})}
 *
 *   where "e(i)" is energy deposited in cell with index "i":
 *
 *   e(i) =  Sg * E(i)  + Gain * ( noise1 + noise2 )
 *
 *   where: "Sg"     - relative gain fluctuations;
 *          "E(i)"   - intrinsic energy (with stochastic flustuations);
 *          "Gain"   - gain for given channel;
 *          "noise1" - incoherent noise;
 *          "noise2" - coherent noise;
 *
 *   This model results in following exporession for covariance matrix:
 *   (diagonal elements)
 *
 *   Cov{i,i} =   D{e(i)} = M{(e(i)-M{e(i)})*(e(i)-M{e(i)})}
 *
 *            =   E(i)      * E(i)      * D{Sg}     +
 *                M{Sg}     * M{Sg}     * D{E(i)}   +
 *                M{Gain}   * M{Gain}   * D{noise1} +
 *                M{Gain}   * M{Gain}   * D{noise2} +
 *                M{noise1} * M{noise1} * M{Gain}   +
 *                M{noise2} * M{noise2} * M{Gain}
 *
 *   for non-diagonal elements (i != j ) one has:
 *
 *   Cov{i,j} =   Cov{j,i} = M{(e(i)-M{e(i)})*(e(j)-M{e(j)})}
 *
 *            =   M{ Gain } * M{ Gain } * D(noise2)
 *
 *   According to the obvious definitions:
 *
 *      "M{ Sg     }"     = 1
 *
 *      "M{ noise1 }" = 0, "mean" value of noise is sero
 *
 *      "M{ noise2 }" = 0, "mean" value of noise is zero
 *
 *
 *   other parameters could be extractes from external parametrisation:
 *
 *      "D{ E(i)   }" = E(i) * "A" * "A" * GeV
 *
 *      "D{ Sg     }" = relative gain sigma squared
 *
 *      "D{ noise1 }" = sigma of incoherent noise squared
 *
 *      "D{ noise2 }" = sigma on   coherent noise squared
 *
 *      "M{ Gain   }" is extracted for each cell from DeCalorimeter object
 *
 *  At next step one is able to calculate calculate intermediate values:
 *
 *     Total cluster energy         "Etot" = Sum_i{ 1.0  * e(i) }  
 *
 *     Energy weighted X            "Ex"   = Sum_i{ x(i) * e(i) } 
 *
 *     Energy weighted Y            "Ey"   = Sum_i{ y(i) * e(i) } 
 *
 *  Since transformation from "e(i)" to ("Etot","Ex","Ey")
 *  id a linear transformation,  the covariance matrix 
 *  for ("Etot","Ex","Ey") quantities could be calculated 
 *  in an easy and transparent way: 
 *
 *  Cov{ Etot, Etot } = Sum_ij { 1.0  * Cov{i,j} * 1.0  }
 *
 *  Cov{ Etot, Ex   } = Sum_ij { x(i) * Cov{i,j} * 1.0  }
 *
 *  Cov{ Ex  , Ex   } = Sum_ij { x(i) * Cov{i,j} * x(i) }
 *
 *  Cov{ Etot, Ey   } = Sum_ij { y(i) * Cov{i,j} * 1.0  }
 *
 *  Cov{ Ex  , Ey   } = Sum_ij { x(i) * Cov{i,j} * y(i) }
 *
 *  Cov{ Ey  , Ey   } = Sum_ij { y(i) * Cov{i,j} * y(i) }
 *
 *  And the last step: we calculate the final quantities:
 *
 *   total energy of cluster:  "Ecl" = Etot              
 *    
 *   X-position of barycenter: "Xcl" = Ex/Etot          
 * 
 *   Y-position of barycenter: "Ycl" = Ey/Etot           
 *   
 *  The calculation of covariance matrix for final values 
 *  is a little bit tedious, since transformation from 
 *  ("Etot","Ex","Ey") to ("Ecl","Xcl","Ycl") is not linear,
 *  but it could be done using analytical expansion:
 * 
 *  Cov{ Ecl , Ecl } =              Cov{ Etot , Etot }
 *
 *  Cov{ Ecl , Xcl } =              Cov{ Etot , Ex   } / Ecl -
 *                            Xcl * Cov{Etot,Etot}/Ecl
 *
 *  Cov{ Xcl , Xcl } =              Cov{ Ex   , Ex   } / Ecl / Ecl +
 *                      Xcl * Xcl * Cov{ Etot , Etot } / Ecl / Ecl -
 *                            Xcl * Cov{ Etot , Ex   } / Ecl / Ecl -
 *                            Xcl * Cov{ Etot , Ex   } / Ecl / Ecl
 *
 *  Cov{ Ecl , Ycl } =              Cov{ Etot , Ey   } / Ecl -
 *                            Ycl * Cov{ Etot , Etot } / Ecl
 *
 *  Cov{ Xcl , Ycl } =              Cov{ Ex   , Ey   } / Ecl / Ecl +
 *                      Xcl * Ycl * Cov{ Etot , Etot } / Ecl / Ecl -
 *                            Xcl * Cov{ Etot , Ey   } / Ecl / Ecl -
 *                            Ycl * Cov{ Etot , Ex   } / Ecl / Ecl
 *
 *  Cov{ Ycl , Ycl } =              Cov{ Ey   , Ey   } / Ecl / Ecl +
 *                      Ycl * Ycl * Cov{ Etot , Etot } / Ecl / Ecl -
 *                            Ycl * Cov{ Etot , Ey   } / Ecl / Ecl -
 *                            Ycl * Cov{ Etot , Ey   } / Ecl / Ecl
 *
 *  @author Ivan Belyaev
 *  @date   06/07/2001
 */
class CovarianceEstimator:
  public std::unary_function<LHCb::CaloCluster*,StatusCode>{
public:

  typedef DeCalorimeter* DET;
  
  /** standard constructor
   *   @param  Det      pointer to DeCalorimeter object
   *  @param  A        intrinsic calo resolution
   *  @param  GainS    sigma relative error in gain
   *  @param  NoiseIn  sigma on incoherent noise
   *  @param  NoiseCo  sigma on coherent   noise
   *  @param  ResB     intrinsic additions to the constant term 
   *                   (nonuniformities, leakages)  
   */
  CovarianceEstimator( const DeCalorimeter* Det     = 0 );
  
  /// (virtual) Destructor
  virtual ~CovarianceEstimator();

  /** calculation of covariance matrix for cluster 
   *
   *  Error codes:
   *   - 221 - invalid source of detector information  
   *   - 223 - accumulated energy <= 0 !
   *
   *  @param cluster pointer to cluster object 
   *  @return status code
   */
  StatusCode operator()( LHCb::CaloCluster* cluster ) const ;

  /** calculate covariance matrix for cluster
   *  @param cluster pointer to cluster
   *   @return status code
   */
  inline StatusCode calculateCovarianceMatrix ( LHCb::CaloCluster* cluster ) const  { 
    return (*this)( cluster ); 
  }
  
  
  /** set new value for calorimeter 
   *  @param Det pointer to calorimeter detector 
   */
  inline void setDetector( const DeCalorimeter* Det ){ 
    m_detector = Det ; 
  }
  
  /** simple accessor to DeCalorimeter object
   *  @return pointer to detector 
   */
  inline const DeCalorimeter* detector    () const{ 
    return m_detector    ; 
  }
  
  /** set new resolution parameter
   *  @param A calorimeter resolution 
   */
  inline void setStochastic ( const std::vector<double> A ){ 
    m_A = A ;
  }


  /** calorimeter resolution (A*A*GeV)
   *  @return A*A*GeV resolution parameter 
   */
  inline double a2GeV        (const LHCb::CaloCellID id) const { 
    if( id.area() >= m_A.size() )return 0.;
    return m_A[id.area()]*m_A[id.area()]* Gaudi::Units::GeV  ; 
  }


  /** set error in gain 
   *  @param GainS error in relative gain 
   */
  inline void  setGainError( const std::vector<double> GainS ){ 
    m_GainError = GainS ; 
  }

  /** get dispersion  of relative gain error
   *  @return dispersion of relative gain error 
   */
  inline double s2gain       (const LHCb::CaloCellID id) const{ 
    if( id.area() >= m_GainError.size() )return 0.;
    return m_GainError[id.area()]* m_GainError[id.area()] ; 
}

  /** get  dispersion of noise (both coherent and incoherent
   *  @return overall noise dispersion 
   */
  inline double s2noise(const LHCb::CaloCellID id) const{ 
    return  s2incoherent(id) + s2coherent(id);
  }
  
  /** set new error in incoherent noise 
   *  @param NoiseIn error in incoherent noise 
   */
  inline void setIncoherentNoise( const std::vector<double> NoiseIn ){ 
    m_IncoherentNoise = NoiseIn;
  }
  
  /** get the dispersion of incoherent noise
   *  @return dispersion of incoherent noise
   */ 
  inline double s2incoherent (const LHCb::CaloCellID id) const{ 
    if( id.area() >= m_IncoherentNoise.size() )return 0.;
    return m_IncoherentNoise[id.area()]*m_IncoherentNoise[id.area()] ; 
  }

  /** set new error in coherent noise 
   *  @param NoiseCo error in incoherent noise 
   */
  inline void setCoherentNoise( const std::vector<double> NoiseCo ){ 
    m_CoherentNoise = NoiseCo;
  }
  
  /**  dispersion of coherent  noise
   *  @return dispersion of coherent noise
   */   
  inline double s2coherent   (const LHCb::CaloCellID id) const{
    if( id.area() >= m_CoherentNoise.size() )return 0.;
    return m_CoherentNoise[id.area()]*m_CoherentNoise[id.area()]   ; 
  }

  inline void setConstantE( const std::vector<double> constE ){ 
    m_ConstantE = constE;
  }
  inline double s2E(const LHCb::CaloCellID id) const{
    if( id.area() >= m_ConstantE.size() )return 0.;
    return m_ConstantE[id.area()]*m_ConstantE[id.area()]   ;
  }
  
  inline void setConstantX( const std::vector<double> constX ){ 
    m_ConstantX = constX;
  }
  inline double s2X(const LHCb::CaloCellID id) const{
    if( id.area() >= m_ConstantX.size() )return 0.;
    return m_ConstantX[id.area()]*m_ConstantX[id.area()]   ;
  }
  
  inline void setConstantY( const std::vector<double> constY ){ 
    m_ConstantY = constY;
  }
  inline double s2Y(const LHCb::CaloCellID id) const{
    if( id.area() >= m_ConstantY.size() )return 0.;
    return m_ConstantY[id.area()]*m_ConstantY[id.area()]   ;
  }
  

  
  /** printout to standard gaudi stream 
   *  @see MsgStream 
   *  @param log the reference to the standard stream 
   *  @return the reference to the standard stream 
   */
  MsgStream&     printOut ( MsgStream&    log             ) const ;
  
  /** printout to standard gaudi stream 
   *  @param  log  the reference to the standard stream 
   *  @return the reference to the standard stream 
   */
  std::ostream&  printOut ( std::ostream& log = std::cout ) const ;
  
private:
  
  const DeCalorimeter* m_detector          ; ///< pointer to DeCalorimeter object
  std::vector<double> m_A                  ; ///< calorimeter resolution ((A**2)*GeV)
  std::vector<double> m_GainError          ; ///< relative gain dispersion
  std::vector<double> m_IncoherentNoise    ; ///< incoherent noise dispersion
  std::vector<double> m_CoherentNoise      ; ///< coherent noise dispersion
  std::vector<double> m_ConstantE ; // global constant term to Cov(EE)
  std::vector<double> m_ConstantX ; // global constant term to Cov(EE)
  std::vector<double> m_ConstantY ; // global constant term to Cov(EE)
  
};

/** printout operator to standard gaudi stream 
 *  @see MsgStream 
 *  @param stream the reference to the standard stream 
 *  @param object object to be printed  
 *  @return the reference to the standard stream 
 */
inline MsgStream&    operator<<( MsgStream&                 stream , 
                                 const CovarianceEstimator& object ){ 
return object.printOut( stream );
}

/** printout operator to standard gaudi stream 
 *  @param stream the reference to the standard stream 
 *  @param object object to be printed  
 *  @return the reference to the standard stream 
 */
inline std::ostream& operator<<( std::ostream&              stream , 
                                 const CovarianceEstimator& object ){ 
return object.printOut( stream );
}

// ===========================================================================
#endif ///< CALOALGS_COVARIANCEESTIMATOR_H
// ===========================================================================
