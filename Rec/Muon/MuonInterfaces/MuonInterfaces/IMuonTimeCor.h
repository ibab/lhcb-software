// $Id: IMuonTimeCor.h,v 1.1.1.1 2010-02-09 12:58:35 asatta Exp $
#ifndef MUONTOOLS_IMUONTIMECOR_H 
#define MUONTOOLS_IMUONTIMECOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/MuonTileID.h"

static const InterfaceID IID_IMuonTimeCor ( "IMuonTimeCor", 1, 0 );

/** @class IMuonTimeCor IMuonTimeCor.h MuonTools/IMuonTimeCor.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2009-12-22
 */
class IMuonTimeCor : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonTimeCor; }
  virtual StatusCode getCorrection(LHCb::MuonTileID tile,int & cor)=0;
  virtual StatusCode getOutCorrection(LHCb::MuonTileID tile,int & cor)=0;
  virtual StatusCode setOutCorrection(LHCb::MuonTileID tile,int  cor)=0;
  virtual StatusCode writeOutCorrection()=0;
  virtual StatusCode writeCorrection()=0;
  

protected:

private:

};
#endif // MUONTOOLS_IMUONTIMECOR_H
