// $Id: $
#ifndef INEUTRALIDTOOL_H 
#define INEUTRALIDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/CaloHypo.h"
#include "ICaloHypoEstimator.h"

static const InterfaceID IID_INeutralIDTool ( "INeutralIDTool", 2, 0 );

/** @class INeutralIDTool INeutralIDTool.h
 *  
 *
 *  @author Mostafa HOBALLAH --
 *  @date   2013-07-25
 */
class INeutralIDTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_INeutralIDTool; }

  virtual StatusCode initialize()=0;
  virtual StatusCode finalize()=0;

  virtual double isNotE(const LHCb::CaloHypo* hypo,ICaloHypoEstimator* e=NULL)=0;
  virtual double isNotH(const LHCb::CaloHypo* hypo,ICaloHypoEstimator* e=NULL)=0;

  virtual double isNotE(const double* v)=0;
  virtual double isNotH(const double* v)=0;

  virtual void Variables(const LHCb::CaloHypo* hypo,  double& clmatch, double& prse, 
                         double& e19, double& hclecl, double& prse19,double& prse49, double& sprd, double& prse4mx, 
                         double& prsm, double& spdm)=0;

  
protected:

  virtual ~INeutralIDTool(){}             
private:

};
#endif // INEUTRALIDTOOL_H
