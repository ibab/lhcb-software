#ifndef CALOTOOLS_CALOSCORRECTIONFINAL_H
#define CALOTOOLS_CALOSCORRECTIONFINAL_H 1
// Include files
#include "GaudiKernel/AlgTool.h"
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"


/** @class CaloSCorrectionFinal CaloSCorrectionFinal.h
 *
 *  S correction tool which can be process on 
 *  a hypo. 
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   14/11/2002
 */
class CaloSCorrectionFinal : public CaloTool,
                             virtual public ICaloHypoTool {
  /// friend factory for instantiation
  friend class ToolFactory<CaloSCorrectionFinal>;

public:

  /** The main processing method
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloHypo* hypo  ) const ;
  
  /** standard initialization method
   *  @see CaloTool 
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;

  /** standard finalization method
   *  @see CaloTool 
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @see   CaloTool
   *  @see    AlgTool 
   *  @param type    type of the tool  (?)
   *  @param name    name of the concrete instance 
   *  @param parent  pointer to parent object (algorithm, service or tool)
   */
  CaloSCorrectionFinal
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloSCorrectionFinal() ;
  
private:

  std::vector<double> m_Coeff_area_0_X;
  std::vector<double> m_Coeff_area_0_Y;
  std::vector<double> m_Coeff_area_1_X;
  std::vector<double> m_Coeff_area_1_Y;
  std::vector<double> m_Coeff_area_2_X;
  std::vector<double> m_Coeff_area_2_Y;
};
#endif // CALOTOOLS_CALOSCORRECTIONSEQUENCE_H
