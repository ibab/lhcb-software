// $Id: $
#ifndef COMPONENT_L0MUONMODIFYINPUTTOOL_H 
#define COMPONENT_L0MUONMODIFYINPUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "L0Interfaces/IL0MuonModifyInputTool.h"            // Interface
#include "GaudiKernel/RndmGenerators.h"

/** @class L0MuonModifyInputTool L0MuonModifyInputTool.h component/L0MuonModifyInputTool.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2013-01-21
 */
class L0MuonModifyInputTool : public GaudiTool, virtual public IL0MuonModifyInputTool {
public: 
  /// Standard constructor
  L0MuonModifyInputTool( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~L0MuonModifyInputTool( ); ///< Destructor

  StatusCode modifyInput(std::vector<LHCb::MuonTileID> &digits);
  StatusCode initialize();
  StatusCode finalize(){return StatusCode::SUCCESS;};
  
protected:
  
private:

  IRndmGenSvc * m_randSvc;
  //double m_params[5][4];
  std::vector<double> m_params;
  
};
#endif // COMPONENT_L0MUONMODIFYINPUTTOOL_H
