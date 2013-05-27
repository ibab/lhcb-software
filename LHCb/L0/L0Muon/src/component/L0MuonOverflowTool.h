// $Id: L0MuonOverflowTool.h,v 1.1 2010-03-09 16:21:31 jucogan Exp $
#ifndef COMPONENT_L0MUONOVERFLOWTOOL_H 
#define COMPONENT_L0MUONOVERFLOWTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "L0Interfaces/IL0MuonOverflowTool.h"            // Interface


/** @class L0MuonOverflowTool L0MuonOverflowTool.h component/L0MuonOverflowTool.h
 *  
 *  Tool to get the candidate's overflow seen by the L0Muon
 *
 *  @author Julien Cogan
 *  @date   2010-03-09
 */
class L0MuonOverflowTool : public GaudiTool, virtual public IL0MuonOverflowTool {
public: 
  /// Standard constructor
  L0MuonOverflowTool( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~L0MuonOverflowTool( ); ///< Destructor

  StatusCode getQuarters(std::vector<int> & quarters, std::string rootInTes="");
  StatusCode getBCSUs(std::vector<LHCb::MuonTileID> & bcsus, std::string rootInTes="");
  StatusCode getPUs(std::vector<LHCb::MuonTileID> & pus, std::string rootInTes="");

private:

  void getOverflows(std::vector<LHCb::MuonTileID> & overflows, std::string location);
  
  // Options
  std::string m_l0Context; ///< Suffix appended to data location in TES

  
};
#endif // COMPONENT_L0MUONOVERFLOWTOOL_H
