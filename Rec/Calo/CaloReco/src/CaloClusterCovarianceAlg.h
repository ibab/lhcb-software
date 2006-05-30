// $Id: CaloClusterCovarianceAlg.h,v 1.4 2006-05-30 09:42:01 odescham Exp $
// ===========================================================================
// $CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/11/07 12:12:41  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.2  2003/06/23 13:11:54  ibelyaev
//  fix for gcc3.2
//
// Revision 1.1.1.1  2002/11/13 20:46:39  ibelyaev
// new package 
//
// Revision 1.6  2002/04/05 17:12:20  ibelyaev
//  remove bugs, bugs, bugs ....
//
// Revision 1.5  2002/04/02 11:06:30  ibelyaev
// update for new event model
//
// Revision 1.4  2001/11/25 16:25:30  ibelyaev
//  update for newer CaloKernel package
//
// ===========================================================================
#ifndef CALORECO_CALOCLUSTERCOVARIANCEALG_H 
#define CALORECO_CALOCLUSTERCOVARIANCEALG_H 1
/// ===========================================================================
// Include files
// from STL
#include <string>
#include "GaudiAlg/GaudiAlgorithm.h"

class ICaloClusterTool   ;
class ICaloSubClusterTag ;

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
   *  @param   name  algorith name
   *  @param   svc   pointer to Service Locator
   */
  CaloClusterCovarianceAlg
  ( const std::string& name  , 
    ISvcLocator*       svc   );
  
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
  
  inline ICaloSubClusterTag* tagger () const { return m_tagger ; }
  
  inline ICaloClusterTool*   cov    () const { return m_cov    ; }
  
  inline ICaloClusterTool*   spread () const { return m_spread ; }
  
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
  
  // tool used for subcluster selections    
  std::string          m_taggerType  ;  ///< type 
  std::string          m_taggerName  ;  ///< name 
  ICaloSubClusterTag*  m_tagger      ;  ///< tool
  
  // tool used for covariance matrix calculation    
  std::string          m_covType     ; ///< type 
  std::string          m_covName     ; ///< name 
  ICaloClusterTool*    m_cov         ; ///< tool 
  
  // tool used for cluster spread estimation 
  std::string          m_spreadType  ;  ///< type 
  std::string          m_spreadName  ;  ///< name 
  ICaloClusterTool*    m_spread      ;  ///< tool 

  std::string m_outputData;
  std::string m_inputData;
  std::string m_detData;
  
};

/// ===========================================================================
#endif // CALORECO_CALOCLUSTERCOVARIANCEALG_H
/// ===========================================================================
