// $Id: CaloSCorrection.h,v 1.4 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
#ifndef CALORECO_CALOSCORRECTION_H 
#define CALORECO_CALOSCORRECTION_H 1
// Include files
// Include files
#include <string>
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloCorrectionBase.h"



/** @class CaloSCorrection CaloSCorrection.h
 *  
 *
 *   @author Deschamps Olivier
 *  @date   2003-03-10
 */

class CaloSCorrection : 
  public virtual ICaloHypoTool ,
  public              CaloCorrectionBase{
  /// friend factory for instantiation 
  friend struct ToolFactory<CaloSCorrection>;
  
public:
  
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const ;
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const ;
  
public:

  virtual StatusCode initialize () ;
  virtual StatusCode finalize   () ;
  
protected:
  
  CaloSCorrection ( const std::string& type   , 
                    const std::string& name   ,
                    const IInterface*  parent ) ;
  
  virtual ~CaloSCorrection () ;
  
private:
  
  CaloSCorrection () ;
  CaloSCorrection           ( const CaloSCorrection& ) ;
  CaloSCorrection& operator=( const CaloSCorrection& ) ;


  /// input variables calculated once in process() and passed to all calcSCorrection() calls
  struct SCorrInputParams {
    LHCb::CaloCellID  cellID;
    Gaudi::XYZPoint  seedPos;
    double                 z;
  };

  /// Jacobian elements returned from calcSCorrection() to process()
  struct SCorrOutputParams {
    double dXhy_dXcl;
    double dYhy_dYcl;
  };


  /// calculate corrected CaloHypo position depending on CaloCluster position
  void calcSCorrection( double  xBar, double  yBar, double &xCor, double &yCor,
                        const struct SCorrInputParams                  &params,
                        struct SCorrOutputParams                       *results ) const;
  
private:
  
};
// ============================================================================
#endif // CALORECO_CALOSCORRECTION_H
