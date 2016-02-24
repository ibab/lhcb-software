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
  friend struct ToolFactory<CaloECorrection>;
  
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

  /// input variables calculated once in process() and passed to all calcECorrection() calls
  struct ECorrInputParams {
    LHCb::CaloCellID cellID;
    Gaudi::XYZPoint  seedPos;
    double           eSpd;
    double           dtheta;
    unsigned int     area;
  };

  /// Jacobian elements and intermediate variables sometimes returned from calcECorrection() to process()
  class ECorrOutputParams {
  public:
    ECorrOutputParams() : dEcor_dXcl(0), dEcor_dYcl(0), dEcor_dEcl(0),
      alpha(0), beta(0), Asx(0), Asy(0), aG(0), aE(0), aB(0), aX(0), aY(0), gC(0), gT(0), betaC_flag(false) {}

    // output Jacobian elements returned from calcECorrection() to process()
    double dEcor_dXcl;
    double dEcor_dYcl;
    double dEcor_dEcl;

    // intermediate variables calculated by calcECorrection() needed for debug printout inside process()
    double alpha;
    double beta;
    double Asx;
    double Asy;
    double aG;
    double aE;
    double aB;
    double aX;
    double aY;
    double gC;
    double gT;

    bool   betaC_flag;
  };

  /// calculate corrected CaloHypo energy depending on CaloCluster position, energy, and Prs energy
  double calcECorrection( double xBar, double yBar, double eEcal, double ePrs,
                          const struct CaloECorrection::ECorrInputParams &_params,
                          CaloECorrection::ECorrOutputParams             *_results ) const;
};
#endif // CALORECO_CALOECORRECTION_H
