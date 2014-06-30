// $Id: $
#ifndef IGAMMAPI0SEPARATIONTOOL_H 
#define IGAMMAPI0SEPARATIONTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/CaloHypo.h"

static const InterfaceID IID_IGammaPi0SeparationTool ( "IGammaPi0SeparationTool", 3, 0 );

/** @class IGammaPi0SeparationTool IGammaPi0SeparationTool.h
 *  
 *
 *  @author Miriam Calvo Gomez --
 *  @date   2010-03-29
 */
class IGammaPi0SeparationTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IGammaPi0SeparationTool; }

  virtual StatusCode initialize()=0;
  virtual StatusCode finalize()=0;

  virtual double isPhoton(const LHCb::CaloHypo* hypo)=0;
  virtual double isPhoton(const double* v)=0;
  virtual bool ClusterVariables(const LHCb::CaloHypo* hypo,
                        double& fr2, double& fasym, double& fkappa, double& fr2r4, double& etot,
                        double& Eseed, double& E2, int& area)=0;
  virtual bool PrsVariables(const LHCb::CaloHypo* hypo,
                                          double& r2PS, double& asymPS, double& kappaPS, double& r2r4PS, 
                                          double& eSumPS, double& ePrs, double& eMaxPS, double& e2ndPS, double& ecornerPS, 
                                          int& multiPS, int& multiPS15, int& multiPS30, int& multiPS45)=0;

  virtual double inputData(std::string data)=0;
  virtual std::map<std::string,double> inputDataMap()=0;
  virtual std::map<std::string,double> inputPrsDataMap()=0;
  
protected:

  virtual ~IGammaPi0SeparationTool(){}             
private:

};
#endif // IGAMMAPI0SEPARATIONTOOL_H
