// $Id: IL0MuonOLErrorTool.h,v 1.1 2010-03-08 13:40:24 jucogan Exp $
#ifndef COMPONENT_IL0MUONOLERRORTOOL_H 
#define COMPONENT_IL0MUONOLERRORTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/MuonTileID.h"

static const InterfaceID IID_IL0MuonOLErrorTool ( "IL0MuonOLErrorTool", 1, 0 );

/** @class IL0MuonOLErrorTool IL0MuonOLErrorTool.h component/IL0MuonOLErrorTool.h
 *  Interface to access the list of optical link error in input of the L0Muon trigger
 *
 *  @author Julien Cogan
 *  @date   2010-02-25
 */
class IL0MuonOLErrorTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0MuonOLErrorTool; }

  // Fill the list of MuonTileIDs with the optical link in error 
  virtual StatusCode getTiles(std::vector<LHCb::MuonTileID> & ols, std::string rootInTes="")=0;

protected:

private:

};
#endif // COMPONENT_IL0MUONOLERRORTOOL_H
