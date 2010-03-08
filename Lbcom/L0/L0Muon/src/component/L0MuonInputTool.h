// $Id: L0MuonInputTool.h,v 1.1 2010-03-08 14:18:15 jucogan Exp $
#ifndef COMPONENT_L0MUONINPUTTOOL_H 
#define COMPONENT_L0MUONINPUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "L0Interfaces/IL0MuonInputTool.h"            // Interface


/** @class L0MuonInputTool L0MuonInputTool.h component/L0MuonInputTool.h
 *  Tool to retrieve the muon tiles in input of the L0Muon trigger
 *
 *  @author Julien Cogan
 *  @date   2010-02-25
 */
class L0MuonInputTool : public GaudiTool, virtual public IL0MuonInputTool {
public: 
  /// Standard constructor
  L0MuonInputTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~L0MuonInputTool( ); ///< Destructor
  StatusCode getTiles(std::vector<LHCb::MuonTileID> & tiles, std::string rootInTes="");
  StatusCode getPads(std::vector<LHCb::MuonTileID> & pads, std::string rootInTes="");
  
protected:

private:
  
  // Options
  std::string m_l0Context; ///< Suffix appended to data location in TES

};
#endif // COMPONENT_L0MUONINPUTTOOL_H
