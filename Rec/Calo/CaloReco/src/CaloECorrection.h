// $Id: CaloECorrection.h,v 1.4 2010-05-20 09:47:06 odescham Exp $
// ============================================================================
#ifndef CALORECO_CALOECORRECTION_H 
#define CALORECO_CALOECORRECTION_H 1
// Include files
#include <string>
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloCorrectionBase.h"

/** @namespace CaloECorrection_Local
 */


/** @class CaloECorrection CaloECorrection.h
 *  
 *
 *  @author Deschamps Olivier

 *  @date   2003-03-10
 */
class CaloECorrection : 
  public virtual ICaloHypoTool ,
  public              CaloCorrectionBase{
  /// friend factory for instantiation 
  friend class ToolFactory<CaloECorrection>;
  
public:
  
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const ;
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const ;
  
public:

  virtual StatusCode initialize () ;
  virtual StatusCode finalize   () ;
  
protected:
  CaloECorrection ( const std::string& type   , 
                    const std::string& name   ,
                    const IInterface*  parent ) ;
  
  virtual ~CaloECorrection () ;
  
private:
  
  CaloECorrection () ;
  CaloECorrection           ( const CaloECorrection& ) ;
  CaloECorrection& operator=( const CaloECorrection& ) ;
  
private:
  int m_pFilt;
  int m_sFilt;
};
#endif // CALORECO_CALOECORRECTION_H
