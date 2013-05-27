// $Id: IMuonPadRec.h,v 1.1 2010-02-10 19:07:50 ggiacomo Exp $
#ifndef MUONMONITOR_IMUONPADREC_H 
#define MUONMONITOR_IMUONPADREC_H 1

#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward decl.
class MuonLogHit;
class MuonLogPad;


static const InterfaceID IID_IMuonPadRec ( "IMuonPadRec", 1, 0 );

/** @class IMuonPadRec IMuonPadRec.h MuonTrackRec/IMuonPadRec.h
 *  
 *
 *  @author 
 *  @date   2008-01-25
 */
class IMuonPadRec : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonPadRec; }

  virtual StatusCode buildLogicalPads(const std::vector<MuonLogHit*>* myhits ) = 0;
  virtual const std::vector<MuonLogPad*>* pads() = 0;

protected:

private:

};
#endif // MUONMONITOR_IMUONPADREC_H
