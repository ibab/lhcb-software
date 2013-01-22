// $Id: $
#ifndef L0INTERFACES_IL0MUONMODIFYINPUTTOOL_H 
#define L0INTERFACES_IL0MUONMODIFYINPUTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/MuonTileID.h"

static const InterfaceID IID_IL0MuonModifyInputTool ( "IL0MuonModifyInputTool", 1, 0 );

/** @class IL0MuonModifyInputTool IL0MuonModifyInputTool.h L0Interfaces/IL0MuonModifyInputTool.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2013-01-21
 */
class IL0MuonModifyInputTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0MuonModifyInputTool; }

  virtual StatusCode modifyInput(std::vector<LHCb::MuonTileID> &digits)=0;
  virtual StatusCode initialize()=0;
  virtual StatusCode finalize()=0;
  
protected:

private:

};
#endif // L0INTERFACES_IL0MUONMODIFYINPUTTOOL_H
