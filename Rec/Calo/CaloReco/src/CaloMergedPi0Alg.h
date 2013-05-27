// ============================================================================
#ifndef CALORECO_CaloMergedPi0Alg_H 
#define CALORECO_CaloMergedPi0Alg_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "CaloCorrectionBase.h"
#include "CaloInterfaces/ICaloClusterTool.h"
#include "CaloInterfaces/ICaloHypoTool.h"
#include "Event/CaloCluster.h"
// ============================================================================

/** @class CaloMergedPi0Alg CaloMergedPi0Alg.h
 *  
 *  Merged pi0 reconstruction with Iterativ Method
 *
 *  @author Olivier Deschamps 
 *  @date   05/10/2002
 */

class CaloMergedPi0Alg : public GaudiAlgorithm
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloMergedPi0Alg>;
  
public:
  
  /** standard algorithm initialization 
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @return status code 
   */
  virtual StatusCode finalize  ();   
  
protected:

  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloMergedPi0Alg( const std::string& name   , 
              ISvcLocator*       svcloc );
  
  /** destructor
   */
  virtual ~CaloMergedPi0Alg();
  
protected:
  
  /** helper function to calculate number of digits 
   *  in the cluster with given status 
   *  @param cluster pointet to the cluster object
   *  @param status  digit statsu to be checked
   *  @return number of digits with given status.
   *       In the case of errors it returns -1
   */
  long numberOfDigits ( const LHCb::CaloCluster*             cluster ,
                        const LHCb::CaloDigitStatus::Status& status  ) const ;

  // L-correction
  double BarZ ( const double e,
                const double eprs,
                const unsigned int area,
                const double x,
                const double y,
                const double z );

  // S-correction
  double BarXY (const int axis,
                const unsigned int area,
                const double e[3][3]);


  // Shower Transverse fit
  double TrShape( const int Neig,
                  const unsigned int area,
                  const double SpdHit,
                  const double D3D);
  
    
private:
  std::string m_inputData ;
  std::string m_outputData;
  std::string m_detData   ;
  
  /** NTuple 
   */
  std::string         m_ntupleLUN ; ///< logical file unit for NTuple
  NTuple::Item<float>  m_mas ; 
  NTuple::Item<long>   m_cls ; 
  NTuple::Item<long>   m_iter ; 
  std::string          m_nameOfSplitPhotons  ;
  std::string          m_nameOfSplitClusters ;


  // tools to be applied for all hypotheses 
  typedef std::vector<std::string>     Names ;
  typedef std::vector<ICaloHypoTool*>  Tools ;
  Names        m_toolTypeNames               ;
  Tools        m_tools                       ;
  Names        m_pi0toolTypeNames               ;
  Tools        m_pi0tools                       ;
  


  // Input Parameters
  bool m_createClusterOnly;
  
private:
  CaloCorrectionBase* m_trSh;
  CaloCorrectionBase* m_sCor;
  CaloCorrectionBase* m_lCor;
  double m_eT_Cut  ;
  int    m_mX_Iter ;
  DeCalorimeter* m_det;
  ICaloClusterTool*    m_cov         ; ///< tool 
  ICaloClusterTool*    m_spread      ;  ///< tool 

  /// configuration of covarinace tool 
  double               m_a           ; ///< calorimeter resolution
  double               m_gainErr     ; ///< error in gain 
  double               m_noiseIn     ; ///< incoherent noise 
  double               m_noiseCo     ; ///< coherent noise

};
// ============================================================================
#endif // CaloMergedPi0Alg_H
