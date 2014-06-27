// $Id: CaloClusterCovarianceAlg.h,v 1.6 2009-02-12 10:59:37 cattanem Exp $
// ===========================================================================
#ifndef CALORECO_CALOCLUSTERCOVARIANCEALG_H 
#define CALORECO_CALOCLUSTERCOVARIANCEALG_H 1
/// ===========================================================================
// Include files
// from STL
#include <string>
#include "GaudiAlg/GaudiAlgorithm.h"

class ICaloClusterTool   ;
class SubClusterSelectorTool ;

/** @class CaloClusterCovarianceAlg CaloClusterCovarianceAlg.h
 *  
 *   Simple algorithm for evaluation of covariance matrix
 *   for CaloCluster object 
 *
 *  @author Vanya BElyaev Ivan Belyaev
 *  @date   04/07/2001
 */

class CaloClusterCovarianceAlg : public GaudiAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterCovarianceAlg>;
  
protected:

  /** Standard constructor
   *  @param   name          algorith name
   *  @param   pSvcLocator   pointer to Service Locator
   */
  CaloClusterCovarianceAlg
  ( const std::string& name  , 
    ISvcLocator*       pSvcLocator );
  
  /// virtual destructor 
  virtual ~CaloClusterCovarianceAlg();
  
public:
  
  /** standard initialization method 
   *  @see GaudiAlgorithm
   *  @see     Algorithm 
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** standard execution method 
   *  @see GaudiAlgorithm
   *  @see     Algorithm 
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute    () ;
  
  /** standard finalization method 
   *  @see GaudiAlgorithm
   *  @see     Algorithm 
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:  
  
  inline ICaloClusterTool*       cov    () const { return m_cov    ; }
  inline ICaloClusterTool*       spread () const { return m_spread ; }
  inline SubClusterSelectorTool* tagger () const { return m_tagger ; }
    
  
private:
  
  /// default constructor is private
  CaloClusterCovarianceAlg();
  
  /** copy constructor is private
   *  @param copy object to be copied 
   */
  CaloClusterCovarianceAlg
  ( const  CaloClusterCovarianceAlg& copy );
  
  /** assignement operator is private
   *  @param copy object to be copied 
   */
  CaloClusterCovarianceAlg& operator=
  ( const  CaloClusterCovarianceAlg& copy );

private:
  
  bool                 m_copy      ;  /// copy flag 
  
  /// configuration of covarinace tool 
  double               m_a           ; ///< calorimeter resolution
  double               m_gainErr     ; ///< error in gain 
  double               m_noiseIn     ; ///< incoherent noise 
  double               m_noiseCo     ; ///< coherent noise
  
  
  // tool used for covariance matrix calculation    
  std::string          m_covType     ; ///< type 
  std::string          m_covName     ; ///< name 
  ICaloClusterTool*    m_cov         ; ///< tool 
  SubClusterSelectorTool* m_tagger   ;
  
  // tool used for cluster spread estimation 
  std::string          m_spreadType  ;  ///< type 
  std::string          m_spreadName  ;  ///< name 
  ICaloClusterTool*    m_spread      ;  ///< tool 

  std::string m_outputData;
  std::string m_inputData;
  std::string m_detData;
  std::string m_condition;
  std::vector<std::string>          m_taggerE  ;  
  std::vector<std::string>          m_taggerP  ;  
  const DeCalorimeter* m_detector;
  // collection of known cluster shapes
  std::map<std::string , std::string> m_clusterShapes;  
};

#endif // CALORECO_CALOCLUSTERCOVARIANCEALG_H
