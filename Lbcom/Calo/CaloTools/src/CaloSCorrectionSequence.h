#ifndef CALOTOOLS_CALOSCORRECTIONSEQUENCE_H
#define CALOTOOLS_CALOSCORRECTIONSEQUENCE_H 1
// Include files
#include "GaudiKernel/AlgTool.h"
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"

// Calo S correction tool caled here
#include "CaloInterfaces/ICaloSCorrectionFunction.h"

/** @class CaloSCorrectionSequence CaloSCorrectionSequence.h
 *
 *  Generic S correction tool which can be process on 
 *  a hypo. It create the 3*3 matrix and called
 *  required tool for adapted area
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   20/06/2002
 */
class CaloSCorrectionSequence : public CaloTool,
                                virtual public ICaloHypoTool {
  /// friend factory for instantiation
  friend class ToolFactory<CaloSCorrectionSequence>;

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
  CaloSCorrectionSequence
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloSCorrectionSequence() ;
  
private:

  // name of CaloSCorrections in area
  std::vector<std::string> m_ISCorrectionNameX;
  std::vector<std::string> m_ISCorrectionNameY;
  // pointers to these tools
  std::vector<ICaloSCorrectionFunction*> m_ISCorrectionX;
  std::vector<ICaloSCorrectionFunction*> m_ISCorrectionY;

  // name of CaloSCorrection for border
  std::string m_ISCorrectionBorderName;
  // pointer to this tools
  ICaloSCorrectionFunction* m_ISCorrectionBorder;
  // flaf for border
  bool m_Border;

  // flaf for X/Y separation
  bool m_SameXY;
};
#endif // CALOTOOLS_CALOSCORRECTIONSEQUENCE_H
