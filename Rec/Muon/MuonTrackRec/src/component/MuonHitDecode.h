// $Id: MuonHitDecode.h,v 1.4 2009-09-10 08:57:34 ggiacomo Exp $
#ifndef LIB_MUONHITDECODE_H 
#define LIB_MUONHITDECODE_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonTrackRec/IMuonHitDecode.h"            // Interface
class IMuonRawBuffer;
class DeMuonDetector;

class MuonHitDecode : public GaudiTool, virtual public IMuonHitDecode , virtual public IIncidentListener{
public: 
  MuonHitDecode( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~MuonHitDecode( ); ///< Destructor

  // main methods
  virtual StatusCode decodeRawData();      
  virtual const std::vector<MuonLogHit*>* hits()
  { 
    if (! m_hitsDecoded) decodeRawData();
    return (const std::vector<MuonLogHit*>*) (&m_hits);}

  // specific for Online Monitoring, not implemented here (just avoid compil. warnings)
  virtual int banksSize(LHCb::RawBank::BankType bankType, std::vector<int> &sizes)
  { return 0* (bankType == bankType) * sizes.size();}
  virtual int bankVersion() {return 0;}
  virtual void dumpRawBanks() {}
  virtual void dumpFrame(int Tell1, int ODE) { Tell1=ODE;}
  virtual float tdc2ns(float TDCtime) {return 0.*TDCtime;}
  virtual float tdc2ns_diff(float TDCdelta) {return 0.*TDCdelta;}
  virtual bool mappingIsOld() {return false;}
  virtual int l0id() {return 0;}
  virtual int bcn() {return 0;}
  virtual int cbcn() {return 0;}
  virtual void setMultiBunch(int bxPerSide) {bxPerSide=0;}
  virtual void unsetMultiBunch() {}
  virtual bool multiBunch()  {return false;}
  virtual int mbExtraBXPerside() {return 0;}
  virtual bool centralBX() {return true;}
  virtual bool firstBX() {return true;}
  virtual bool lastBX() {return true;}
  virtual LHCb::MuonTileID* tileFromODE(int ODEnumber,
                                        int ODEchannel) {
    return (0*ODEnumber*ODEchannel);}
  virtual int odeIndex(int ODEnumber) {return 0*ODEnumber;}
  virtual int channelsPerQuadrant(int station,
				  int region) {return 0*station*region;}
  virtual int nPadX(int s) {return 0*s;}
  virtual int nPadY(int s) {return 0*s;}
  virtual int nPadXvy(int s, int r) {return 0*s*r;}
  virtual int nPadYvx(int s, int r) {return 0*s*r;}
  virtual float padSizeX(int station,			    
                         int region)  {return 0.*station*region;}
  virtual float padSizeY(int station,			    
                         int region)  {return 0.*station*region;}
  virtual float padSizeXvy(int station,			    
			   int region)  {return 0.*station*region;}
  virtual float padSizeYvx(int station,			    
			   int region)  {return 0.*station*region;}
  virtual LHCb::MuonTileID* tileFromLogCh(unsigned int q, 
                                          unsigned int s, 
                                          unsigned int r, 
                                          short int io, 
                                          unsigned int ch) 
  {return (0*q*s*r*io*ch);}
  virtual std::string& ecsChamberName(int region,
                                      int chamberNumber) 
  {int x=region*chamberNumber;x=0;
    nullstring="";
    return nullstring;}
  virtual bool completeEvent() {return true;}

  // from GaudiTool
  virtual StatusCode 	initialize ();
  virtual StatusCode 	finalize ();
  
  // from IIncidentListener
  virtual void handle ( const Incident& incident );   
protected:


private:
  void clearHits();
  IMuonRawBuffer* m_recTool;
  DeMuonDetector* m_muonDetector;
  std::vector<MuonLogHit*> m_hits;
  bool m_hitsDecoded;
  std::string nullstring;
};
#endif // LIB_MUONHITDECODE_H
