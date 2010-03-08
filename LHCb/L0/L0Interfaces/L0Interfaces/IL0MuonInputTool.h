// $Id: IL0MuonInputTool.h,v 1.1 2010-03-08 13:40:24 jucogan Exp $
#ifndef L0INTERFACES_IL0MUONINPUTTOOL_H 
#define L0INTERFACES_IL0MUONINPUTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/MuonTileID.h"

static const InterfaceID IID_IL0MuonInputTool ( "IL0MuonInputTool", 1, 0 );

/** @class IL0MuonInputTool IL0MuonInputTool.h L0Interfaces/IL0MuonInputTool.h
 *  Interface to retrieve the muon tiles in input of the L0Muon trigger
 *
 *  @author Julien Cogan
 *  @date   2010-02-25
 */
class IL0MuonInputTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0MuonInputTool; }
  // Fill the list of logical channels seen in input of the L0Muon trigger
  virtual StatusCode getTiles(std::vector<LHCb::MuonTileID> & tiles, std::string rootInTes="")=0;
  // Fill the list of logical pads seen in input of the L0Muon trigger
  virtual StatusCode getPads(std::vector<LHCb::MuonTileID> & pads, std::string rootInTes="")=0;
  
protected:

private:

};
#endif // L0INTERFACES_IL0MUONINPUTTOOL_H
