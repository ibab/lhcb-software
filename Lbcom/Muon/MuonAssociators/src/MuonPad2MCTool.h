#ifndef MUONPAD2MCTOOL_H 
#define MUONPAD2MCTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MCInterfaces/IMuonPad2MCTool.h"            // Interface

#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/MuonDAQHelper.h"

/** @class MuonPad2MCTool MuonPad2MCTool.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-12-06
 */
class MuonPad2MCTool : public GaudiTool, virtual public IMuonPad2MCTool {
public: 
  /// Standard constructor
  MuonPad2MCTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~MuonPad2MCTool( ); ///< Destructor
  StatusCode initialize();
  LHCb::MCParticle* Pad2MC(LHCb::MuonTileID value);
  
  bool isXTalk(LHCb::MuonTileID value,LHCb::MCParticle*& pp);
//  virtual LHCb::MCParticle* PadXtalk2MC(LHCb::MuonTileID value,
//                                  std::vector<LHCb::MuonTileID>  & 
//                                        list_digit);
  LHCb::MCParticle* PadNoXtalk2MC(LHCb::MuonTileID value);

protected:

private:
  StatusCode XtalkStrip(LHCb::MuonTileID tile,LHCb::MCParticle*& pp);
   StatusCode XtalkPad(LHCb::MuonTileID tile,LHCb::MCParticle*& pp);
  
 DeMuonDetector*   m_muonDetector;


};
#endif // MUONPAD2MCTOOL_H
