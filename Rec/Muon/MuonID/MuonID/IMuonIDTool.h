#ifndef MUONID_IMUONIDTOOL_H 
#define MUONID_IMUONIDTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class MuonPID;
  class Track;
}

static const InterfaceID IID_IMuonIDTool ( "IMuonIDTool", 1, 0 );

/** @class IMuonIDTool IMuonIDTool.h MuonID/IMuonIDTool.h
 *  
 *  interface for generic MuonID tool
 *
 *  @author Giacomo Graziani
 *  @date   2015-11-10
 */
class IMuonIDTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonIDTool; }

  virtual StatusCode eventInitialize() { return StatusCode::SUCCESS;}

  virtual LHCb::MuonPID* getMuonID(const LHCb::Track* track) =0;
  
  virtual double muonIDPropertyD(const LHCb::Track* , const char* , int =-1) {return 0.;}
  virtual int muonIDPropertyI(const LHCb::Track* , const char* , int =-1) {return 0;}

protected:
  unsigned nStations;
  unsigned nRegions;
  
};
#endif // MUONID_IMUONIDTOOL_H
