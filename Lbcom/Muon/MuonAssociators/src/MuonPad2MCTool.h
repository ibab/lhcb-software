// $Id: MuonPad2MCTool.h,v 1.1 2007-01-11 13:12:46 asatta Exp $
#ifndef MUONPAD2MCTOOL_H 
#define MUONPAD2MCTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonAssociators/IMuonPad2MCTool.h"            // Interface

#include "MuonDet/DeMuonDetector.h"

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
  StatusCode finalize();
  LHCb::MCParticle* Pad2MC(LHCb::MuonTileID value,
                             std::vector<LHCb::MuonTileID>& list_digit);
  
  bool isXTalk(LHCb::MuonTileID value,LHCb::MCParticle*& pp,
                       std::vector<LHCb::MuonTileID>& list_digit);
//  virtual LHCb::MCParticle* PadXtalk2MC(LHCb::MuonTileID value,
//                                  std::vector<LHCb::MuonTileID>  & 
//                                        list_digit);
  LHCb::MCParticle* PadNoXtalk2MC(LHCb::MuonTileID value,
                                    std::vector<LHCb::MuonTileID>& list_digit);

protected:

private:
  StatusCode XtalkStrip(LHCb::MuonTileID tile,LHCb::MCParticle*& pp,std::vector<LHCb::MuonTileID>& list_digit);
   StatusCode XtalkPad(LHCb::MuonTileID tile,LHCb::MCParticle*& pp,std::vector<LHCb::MuonTileID>& list_digit);
  
 DeMuonDetector*   m_muonDetector;


};
#endif // MUONPAD2MCTOOL_H
