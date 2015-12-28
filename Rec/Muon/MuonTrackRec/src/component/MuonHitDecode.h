#ifndef LIB_MUONHITDECODE_H 
#define LIB_MUONHITDECODE_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonInterfaces/IMuonHitDecode.h"            // Interface
class IMuonRawBuffer;
class DeMuonDetector;


/** @class MuonHitDecode MuonHitDecode.h 
 *    
 *  Parameters:
 *  - NumberRawLocations: Number of raw locations to look for data (default = 1). 
 *  Values larger than one will result in looking for data in Prev# / Next#
 *  - SkipHWNumber: Skip the calculation of the hardware numbers. Can be set 'true' 
 *    for reconstructing MuonTT tracks as it is faster.
 *  
 *
 */

class MuonHitDecode : public extends<GaudiTool, IMuonHitDecode , IIncidentListener >
{

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
    return &m_hits;
  }
  virtual const std::vector<MuonLogHit*>* fakehits() {
    return &m_fakehits;
  }
  // specific for Online Monitoring, not implemented here (just avoid compil. warnings)
  virtual int banksSize(LHCb::RawBank::BankType , std::vector<int>& )
  { return 0;}
  virtual unsigned int odeErrorWord(int , int ) {return 0;}
  virtual int bankVersion() {return 0;}
  virtual void dumpRawBanks() {}
  virtual void dumpFrame(int, int) {}
  virtual bool mappingIsOld() {return false;}
  virtual int l0id() {return 0;}
  virtual int bcn() {return 0;}
  virtual int cbcn() {return 0;}
  virtual void setMultiBunch(int) {}
  virtual void unsetMultiBunch() {}
  virtual bool multiBunch()  {return false;}
  virtual int mbExtraBXPerside() {return 0;}
  virtual bool centralBX() {return true;}
  virtual bool firstBX() {return true;}
  virtual bool lastBX() {return true;}
  virtual LHCb::MuonTileID tileFromODE(int, int) { return 0; }
  virtual int odeIndex(int ) {return 0;}
  virtual int channelsPerQuadrant(int, int ) {return 0;}
  virtual int nPadX(int ) {return 0;}
  virtual int nPadY(int ) {return 0;}
  virtual int nPadXvy(int , int ) {return 0;}
  virtual int nPadYvx(int , int ) {return 0;}
  virtual float padSizeX(int , int)	{return 0.;}
  virtual float padSizeY(int , int)	{return 0.;}
  virtual float padSizeXvy(int , int)	{return 0.;}
  virtual float padSizeYvx(int , int)	{return 0.;}
  virtual LHCb::MuonTileID tileFromLogCh(unsigned int,
                                         unsigned int,
                                         unsigned int,
                                         short int,
                                         unsigned int) 
  { return 0;}
  virtual bool completeEvent() {return true;}

  // from GaudiTool
  virtual StatusCode 	initialize ();
 
  // from IIncidentListener
  virtual void handle ( const Incident& incident );   

private:

  void clearHits();
  IMuonRawBuffer* m_recTool;
  DeMuonDetector* m_muonDetector;
  std::vector<std::pair<LHCb::MuonTileID,unsigned int> >  m_tilesAndTDC;
  std::vector<MuonLogHit*> m_hits;
  std::vector<MuonLogHit*> m_fakehits;
  bool m_hitsDecoded = false;
  int m_TAENum = 1;
  bool m_skipHWNumber = false;
  
};
#endif // LIB_MUONHITDECODE_H
