// $Id: INNetTool.h,v 1.5 2006-05-16 10:20:04 musy Exp $
#ifndef NNETTOOL_INNETTOOL_H 
#define NNETTOOL_INNETTOOL_H 1

// Include files
// from STL
#include <string>
// from Gaudi
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
 
  virtual double MLPm(std::vector<double>& )=0;
  virtual double MLPe(std::vector<double>& )=0;
  virtual double MLPk(std::vector<double>& )=0;
  virtual double MLPkS(std::vector<double>&)=0;
  virtual double MLPpS(std::vector<double>&)=0;

};
#endif // NNETTOOL_INNETTOOL_H
