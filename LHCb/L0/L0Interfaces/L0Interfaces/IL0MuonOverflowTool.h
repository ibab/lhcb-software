// $Id: IL0MuonOverflowTool.h,v 1.1 2010-03-09 16:19:50 jucogan Exp $
#ifndef L0INTERFACES_IL0MUONOVERFLOWTOOL_H 
#define L0INTERFACES_IL0MUONOVERFLOWTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/MuonTileID.h"

static const InterfaceID IID_IL0MuonOverflowTool ( "IL0MuonOverflowTool", 1, 0 );

/** @class IL0MuonOverflowTool IL0MuonOverflowTool.h L0Interfaces/IL0MuonOverflowTool.h
 *  Interface to access the list of FPGA declaring an overflow
 *
 *  @author Julien Cogan
 *  @date   2010-03-09
 */
class IL0MuonOverflowTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0MuonOverflowTool; }

  /// Fill the list of quarters with an overflow
  virtual StatusCode getQuarters(std::vector<int> & quarters, std::string rootInTes="")=0;
  /// Fill the list of BCSUs (as MuonTileIDs) with an overflow
  virtual StatusCode getBCSUs(std::vector<LHCb::MuonTileID> & bcsus, std::string rootInTes="")=0;
  /// Fill the list of PUs (as MuonTileIDs) with an overflow
  virtual StatusCode getPUs(std::vector<LHCb::MuonTileID> & pus, std::string rootInTes="")=0;

};
#endif // L0INTERFACES_IL0MUONOVERFLOWTOOL_H
