// $Id: ICLTool.h,v 1.2 2009-07-07 22:02:41 polye Exp $
#ifndef ICLTOOL_H 
#define ICLTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ICLTool ( "ICLTool", 1, 0 );

/** @class ICLTool ICLTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-02
 */
class ICLTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICLTool; }

  // virtual ~ICLTool() {};

  virtual double minRange() = 0;

  virtual double maxRange() = 0;


//   virtual StatusCode  cl(double value, double& cls, double& clb,
//                          double& clratio, double range = 0.) = 0;
  

  virtual StatusCode  cl(double value, double& cls, double& clb,
                         double& clratio, double range = 0., int region = 0) = 0;


private:

};
#endif // ICLTOOL_H
