// $Id: IMuonIDDLLTool.h,v 1.1 2003-05-30 09:16:31 gcorti Exp $
#ifndef DAVINCITOOLS_IMUONIDDLLTOOL_H 
#define DAVINCITOOLS_IMUONIDDLLTOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "Event/MuonID.h"

static const InterfaceID IID_IMuonIDDLLTool("IMuonIDDLLTool", 1 , 0); 

/** @class IMuonIDDLLTool IMuonIDDLLTool.h DaVinciTools/IMuonIDDLLTool.h
 *  Interface for the Muon ID Likelihood Calculation Tool. 
 *
 *  @author Joao R. Torres de Mello Neto, Miriam Gandelman
 *  @date   24/04/2003
 */

class IMuonIDDLLTool : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonIDDLLTool; }

  /// Calculates the Likelihood given a MuonID
  /// Inputs: a MuonID object
  virtual StatusCode calcMuonDLL( const MuonID* , double& ) = 0;

};

#endif // DAVINCITOOLS_IMUONIDDLLTOOL_H
