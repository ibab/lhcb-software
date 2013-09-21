// $Id: INNetTool.h,v 1.6 2010-02-02 19:29:08 musy Exp $
#ifndef NNETTOOL_INNETTOOL_H 
#define NNETTOOL_INNETTOOL_H 1

// Include files
// from STL
#include <string>
#include <TROOT.h>
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
  virtual double MLPvtx(std::vector<double>&)=0;
  virtual double MLPmTMVA(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPeTMVA(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPkaonTMVA(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPvtxTMVA(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPmTMVA_MC(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPeTMVA_MC(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPkaonTMVA_MC(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPvtxTMVA_MC(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPpSTMVA_MC(std::vector<std::string>&, std::vector<double>&  )=0;
  virtual double MLPkSTMVA_MC(std::vector<std::string>&, std::vector<double>&  )=0;

};
#endif // NNETTOOL_INNETTOOL_H
