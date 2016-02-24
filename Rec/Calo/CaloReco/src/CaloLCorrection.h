// $Id: CaloLCorrection.h,v 1.5 2007-03-02 15:31:46 cattanem Exp $
// ============================================================================
#ifndef CALORECO_CALOLCORRECTION_H 
#define CALORECO_CALOLCORRECTION_H 1
// Include files
// from STL
#include <string>
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloCorrectionBase.h"



/** @class CaloLCorrection CaloLCorrection.h
 *  
 *
 *  @author Deschamps Olivier
 *  @date   2003-03-10
 *  revised 2010
 */

class CaloLCorrection : 
  public virtual ICaloHypoTool ,
  public              CaloCorrectionBase{
  /// friend factory for instantiation 
  friend struct ToolFactory<CaloLCorrection>;
  
public:
  
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const ;
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const ;
  
public:

  virtual StatusCode initialize () ;
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @see GaudiTool 
   *  @see  AlgTool 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent  tool parent 
   */
  CaloLCorrection ( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent ) ;
  
  /// destructor
  virtual ~CaloLCorrection () ;
  
private:
  
  CaloLCorrection () ;
  CaloLCorrection           ( const CaloLCorrection& ) ;
  CaloLCorrection& operator=( const CaloLCorrection& ) ;
  
private:  

};
// ============================================================================
#endif // CALORECO_CALOLCORRECTION_H
