// $Id: IMuonIDNSharedHitsTool.h,v 1.1 2005-09-30 08:53:52 pkoppenb Exp $
#ifndef DAVINCITOOLS_IMUONIDNSHAREDHITSTOOL_H 
#define DAVINCITOOLS_IMUONIDNSHAREDHITSTOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "Event/MuonID.h"

static const InterfaceID IID_IMuonIDNSharedHitsTool("IMuonIDNSharedHitsTool", 1 , 0);

/** @class IMuonIDNSharedHitsTool IMuonIDNSharedHitsTool.h DaVinciTools/IMuonIDNSharedHitsTool.h
 * Interface for the Muon ID Tool used to calculate the number of muonID's that share a hit
 * with a given MuonID and are closest to the hit than this one.
 *
 *  @author Andre Massafferri, Erica Polycarpo, Miriam Gandelman
 *  @date   16/09/2005
 */

class IMuonIDNSharedHitsTool : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonIDNSharedHitsTool; }

  /// Calculates the number of shared hits given a MuonID
  /// Inputs: a MuonID object
  virtual StatusCode calcSharedHits( const MuonID* muonid, int& ) = 0;
};

#endif // DAVINCITOOLS_IMUONIDNSHAREDHITSTOOL_H
