// $Id: L0MuonOLErrorTool.h,v 1.1 2010-03-08 14:18:16 jucogan Exp $
#ifndef COMPONENT_L0MUONOLERRORTOOL_H 
#define COMPONENT_L0MUONOLERRORTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "L0Interfaces/IL0MuonOLErrorTool.h"            // Interface

#include "MuonKernel/MuonSystemLayout.h"


/** @class L0MuonOLErrorTool L0MuonOLErrorTool.h component/L0MuonOLErrorTool.h
 *  Tool to access the list of optical link in error in input of the L0muon trigger
 *
 *  @author Julien Cogan
 *  @date   2010-02-25
 */
class L0MuonOLErrorTool : public GaudiTool, virtual public IL0MuonOLErrorTool {
public: 
  /// Standard constructor
  L0MuonOLErrorTool( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~L0MuonOLErrorTool( ); ///< Destructor

  StatusCode getTiles(std::vector<LHCb::MuonTileID> & ols, std::string rootInTes="");

protected:

private:

  // Options
  std::string m_l0Context; ///< Suffix appended to data location in TES

  // Misc
  MuonSystemLayout m_opt_link_layout ; // Layout of the optical links

};
#endif // COMPONENT_L0MUONOLERRORTOOL_H
