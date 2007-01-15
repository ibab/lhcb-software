// $Id: IMuonPad2MCTool.h,v 1.1 2007-01-15 16:11:50 cattanem Exp $
#ifndef IMUONPAD2MCTOOL_H 
#define IMUONPAD2MCTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/MuonTileID.h"
#include "GaudiKernel/IAlgTool.h"
#include "Event/MCParticle.h"


static const InterfaceID IID_IMuonPad2MCTool ( "IMuonPad2MCTool", 1, 0 );

/** @class IMuonPad2MCTool IMuonPad2MCTool.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-12-06
 */
class IMuonPad2MCTool  : public virtual IAlgTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonPad2MCTool; }


  virtual LHCb::MCParticle* Pad2MC(LHCb::MuonTileID value, 
std::vector<LHCb::MuonTileID>& list_digit)=0;


  
  virtual bool isXTalk(LHCb::MuonTileID value, LHCb::MCParticle*& pp,
std::vector<LHCb::MuonTileID>& list_digit)=0;
//  virtual LHCb::MCParticle* PadXtalk2MC(LHCb::MuonTileID value, 
//std::vector<LHCb::MuonTileID> list)=0;
  virtual LHCb::MCParticle* PadNoXtalk2MC(LHCb::MuonTileID value, 
std::vector<LHCb::MuonTileID>& list_digit)=0;
protected:

private:

};
#endif // IMUONPAD2MCTOOL_H
