// $Id: INNetTool.h,v 1.3 2005-07-04 15:46:04 pkoppenb Exp $
#ifndef NNETTOOL_INNETTOOL_H 
#define NNETTOOL_INNETTOOL_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_INNetTool("INNetTool", 1 , 0); 

/** @class INNetTool INNetTool.h 
 *  
 *  v1.2
 *  @author Marco Musy (Milano)
 *  @date   2004-12-14
 */
class INNetTool : virtual public IAlgTool {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_INNetTool; };
 
  virtual double MLPm(double x1,double x2,double x3,
		      double x4,double x5,double x6,
		      double x7,double x8)=0;
  virtual double MLPe(double x1,double x2,double x3,
		      double x4,double x5,double x6,
		      double x7,double x8)=0;
  virtual double MLPk(double x1,double x2,double x3,
		      double x4,double x5,double x6,
		      double x7,double x8)=0;
  virtual double MLPkS(double x1,double x2,double x3,
		       double x4,double x5,double x6,
		       double x7,double x8,
		       double x10,double x11,double x12)=0;
  virtual double MLPpS(double x1,double x2,double x3,
		       double x4,double x5,double x6,
		       double x7,double x8,
		       double x10,double x11,double x12)=0;

};
#endif // NNETTOOL_INNETTOOL_H
