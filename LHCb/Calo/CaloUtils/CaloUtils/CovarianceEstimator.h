// $Id: CovarianceEstimator.h,v 1.8 2006-05-26 12:49:17 cattanem Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef CALOALGS_COVARIANCEESTIMATOR_H
#define CALOALGS_COVARIANCEESTIMATOR_H 1
// ===========================================================================
// Include files
#include <functional>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/StatusCode.h"

// forward declaration
class  LHCb::CaloCluster    ;
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
  public std::unary_function<LHCb::CaloCluster*,StatusCode>
{
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
  CovarianceEstimator( const DeCalorimeter* Det     = 0    ,
                       const double         A       = 0.10 ,
                       const double         GainS   = 0.01 ,
                       const double         NoiseIn = 0.0  ,
                       const double         NoiseCo = 0.0  , 
                       const double         ResB    = 0.01 );
  
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
  inline StatusCode calculateCovarianceMatrix ( LHCb::CaloCluster* cluster ) const
  { return (*this)( cluster ); }
  
  
  /** set new value for calorimeter 
   *  @param Det pointer to calorimeter detector 
   */
  inline void setDetector( const DeCalorimeter* Det ) 
  { m_detector = Det ; }
  
  /** simple accessor to DeCalorimeter object
   *  @return pointer to detector 
   */
  inline const DeCalorimeter* detector    () const
  { return m_detector    ; }
  
  /** set new resolution parameter
   *  @param A calorimeter resolution 
   */
  inline void setA ( const double A ) 
  { m_a2GeV = A * A * Gaudi::Units::GeV ; }
  
  /** calorimeter resolution (A*A*GeV)
   *  @return A*A*GeV resolution parameter 
   */
  inline double a2GeV        () const
  { return m_a2GeV  ; }

  /** set new resolution parameter
   *  @param B calorimeter resolution 
   */
  inline void setB ( const double B ) 
  { m_b2  = B * B ; }
  
  /** calorimeter resolution (B*B)
   *  @return B*B resolution parameter 
   */
  inline double b2           () const
  { return m_b2   ; }

  /** set error in gain 
   *  @param GainS error in relative gain 
   */
  inline void  setGainS( const double GainS ) 
  { m_s2gain = GainS * GainS ; }

  /** get dispersion  of relative gain error
   *  @return dispersion of relative gain error 
   */
  inline double s2gain       () const
  { return m_s2gain ; }

  /** get  dispersion of noise (both coherent and incoherent
   *  @return overall noise dispersion 
   */
  inline double s2noise      () const
  { return ( m_s2coherent + m_s2incoherent ) ; }
  
  /** set new error in incoherent noise 
   *  @param NoiseIn error in incoherent noise 
   */
  inline void setNoiseIn( const double NoiseIn ) 
  { m_s2incoherent = NoiseIn * NoiseIn ;}
  
  /** get the dispersion of incoherent noise
   *  @return dispersion of incoherent noise
   */ 
  inline double s2incoherent () const
  { return m_s2incoherent ; }

  /** set new error in coherent noise 
   *  @param NoiseCo error in incoherent noise 
   */
  inline void setNoiseCo( const double NoiseCo ) 
  { m_s2coherent = NoiseCo * NoiseCo ;}
  
  /**  dispersion of coherent  noise
   *  @return dispersion of coherent noise
   */   
  inline double s2coherent   () const
  { return m_s2coherent   ; }
  
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
  
  const DeCalorimeter* m_detector ; ///< pointer to DeCalorimeter object
  double m_a2GeV                  ; ///< calorimeter resolution ((A**2)*GeV)
  double m_b2                     ; ///< calorimeter resolution ((B**2))
  double m_s2gain                 ; ///< relative gain dispersion
  double m_s2incoherent           ; ///< incoherent noise dispersion
  double m_s2coherent             ; ///< coherent noise dispersion
  
};

/** printout operator to standard gaudi stream 
 *  @see MsgStream 
 *  @param stream the reference to the standard stream 
 *  @param object object to be printed  
 *  @return the reference to the standard stream 
 */
inline MsgStream&    operator<<( MsgStream&                 stream , 
                                 const CovarianceEstimator& object ) 
{ return object.printOut( stream );};

/** printout operator to standard gaudi stream 
 *  @param stream the reference to the standard stream 
 *  @param object object to be printed  
 *  @return the reference to the standard stream 
 */
inline std::ostream& operator<<( std::ostream&              stream , 
                                 const CovarianceEstimator& object ) 
{ return object.printOut( stream );};

// ===========================================================================
// The End 
// ===========================================================================
#endif ///< CALOALGS_COVARIANCEESTIMATOR_H
// ===========================================================================
