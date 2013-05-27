// $Id: IMuonHitDecode.h,v 1.3 2010-03-25 16:58:33 ggiacomo Exp $
#ifndef MUONMONITOR_IMUONHITDECODE_H 
#define MUONMONITOR_IMUONHITDECODE_H 1

#include <vector>
#include <sstream>
#include <string>

#include "GaudiKernel/IAlgTool.h"
#include "Event/RawBank.h"
#include "Kernel/MuonTileID.h"

// forward decl.
class MuonLogHit;

static const InterfaceID IID_IMuonHitDecode ( "IMuonHitDecode", 1, 0 );

/** @class IMuonHitDecode IMuonHitDecode.h MuonTrackRec/IMuonHitDecode.h
 *  
 *
 *  @author 
 *  @date   2008-01-25
 */
class IMuonHitDecode : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonHitDecode; }

  // main methods
  virtual StatusCode decodeRawData() = 0;      
  /// list of decoded hits
  virtual const std::vector<MuonLogHit*>* hits() = 0;
  /// list of fake hits (unconnected channels)
  virtual const std::vector<MuonLogHit*>* fakehits() = 0;

  virtual float tdc2ns(float TDCtime) {
    return  (float) ( (TDCtime - 7.5) * 25. / 16.)  ;
  }
  virtual float tdc2ns_diff(float TDCdelta) {
    return  (float) ( TDCdelta  * 25. / 16.)  ;
  }
  // specific for Online Monitoring
  virtual int banksSize(LHCb::RawBank::BankType bankType,  std::vector<int> &sizes) = 0;
  virtual unsigned int odeErrorWord(int T1, int BX) =0;
  virtual int bankVersion() = 0;
  virtual void dumpRawBanks() = 0;  
  virtual void dumpFrame(int Tell1, int ODE) = 0;
  virtual bool mappingIsOld()  = 0;
  virtual int l0id() = 0;
  virtual int bcn() = 0;
  virtual int cbcn() = 0;
  virtual void setMultiBunch(int bxPerSide)  = 0;
  virtual void unsetMultiBunch()  = 0;
  virtual bool multiBunch()  = 0;
  virtual int mbExtraBXPerside() = 0;
  virtual bool centralBX() = 0;
  virtual bool firstBX() = 0;
  virtual bool lastBX() = 0;
  virtual LHCb::MuonTileID* tileFromODE(int ODEnumber,
                                        int ODEchannel)  = 0;
  virtual int odeIndex(int ODEnumber) =0;
  virtual int channelsPerQuadrant(int station,
				  int region) =0;
  virtual int nPadX(int s) =0;
  virtual int nPadY(int s) =0;
  virtual int nPadXvy(int s, int r) =0;
  virtual int nPadYvx(int s, int r) =0;
  virtual float padSizeX(int station,			    
                         int region)  =0;
  virtual float padSizeY(int station,			    
                         int region)  =0;
  virtual float padSizeXvy(int station,			    
			   int region)  =0;
  virtual float padSizeYvx(int station,			    
			   int region)  =0;
  virtual LHCb::MuonTileID* tileFromLogCh(unsigned int q, 
                                          unsigned int s, 
                                          unsigned int r, 
                                          short int io, 
                                          unsigned int ch)  =0;
  virtual std::string& ecsChamberName(int region,
                                      int chamberNumber)=0;
  virtual bool completeEvent() =0;
  virtual void setCompleteEventMask(int mask) {m_ceMask=mask;}
protected:
  inline std::string locBX(int iX) {
    std::stringstream sloc;
    if(iX < 0) 
      sloc << "Prev"<< -iX << "/";
    if(iX > 0) 
      sloc << "Next"<<  iX << "/";
    return sloc.str();
  }
  int m_ceMask;
private:

};
#endif // MUONMONITOR_IMUONHITDECODE_H
