#ifndef CALOTOOLS_CALOLCORRECTIONSIMPLE_H
#define CALOTOOLS_CALOLCORRECTIONSIMPLE_H 1
// Include files
#include "GaudiKernel/AlgTool.h"
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"

/** @class CaloLCorrectionSimple CaloLCorrectionSimple.h
 *
 *  Logitudinal correction with parameters set as property
 *  (based on vanya adaptation of olivier deschamp code)
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   20/06/2002
 */
class CaloLCorrectionSimple : public CaloTool,
                              virtual public ICaloHypoTool {
  /// friend factory for instantiation
  friend class ToolFactory<CaloLCorrectionSimple>;

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
  CaloLCorrectionSimple
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloLCorrectionSimple() ;
  
private:
  double m_Cste,m_Zecal,m_LogCste;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTOOLS_CALOLCORRECTIONSIMPLE_H
// ============================================================================
