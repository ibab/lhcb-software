// $Id: IL0MuonPtTool.h,v 1.1 2012-04-25 jucogan Exp $
#ifndef COMPONENT_IL0MUONPTTOOL_H 
#define COMPONENT_IL0MUONPTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/L0MuonCandidate.h"
#include "Kernel/MuonTileID.h"

static const InterfaceID IID_IL0MuonPtTool ( "IL0MuonPtTool", 1, 0 );

/** @class IL0MuonPtTool IL0MuonPtTool.h component/IL0MuonPtTool.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2012-04-17
 */
class IL0MuonPtTool : virtual public IAlgTool {
public: 
  typedef std::map<LHCb::MuonTileID,int> m1ptMap;
  typedef std::map< LHCb::MuonTileID, m1ptMap > m2m1Lut;

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0MuonPtTool; }

  virtual StatusCode readFromFile(std::string filename)=0;
  
  virtual void getCandidatePads(LHCb::L0MuonCandidate * l0candidate, LHCb::MuonTileID &m2, LHCb::MuonTileID &m1)=0;
  virtual int getPTFromLUT( LHCb::MuonTileID m2, LHCb::MuonTileID m1)=0;
  virtual int getCandidatePT(LHCb::L0MuonCandidate * l0candidate)=0;

protected:

private:


};
#endif // COMPONENT_IL0MUONPTTOOL_H
