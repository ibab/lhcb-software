// $Id: IMuonRawBuffer.h,v 1.10 2008-10-23 13:42:16 asatta Exp $
#ifndef MUONDAQ_IMUONRAWBUFFER_H 
#define MUONDAQ_IMUONRAWBUFFER_H 1

// Include files
#include "Kernel/MuonTileID.h"

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "Event/RawEvent.h"
#include "Event/MuonPPEventInfo.h"
#include "Event/MuonODEData.h"
#include "Event/MuonTell1Header.h"
#include "Event/RawBankReadoutStatus.h"
#include <vector>
#include <utility>

/** @class IMuonRawBuffer IMuonRawBuffer.h MuonTools/IMuonRawBuffer.h
 *  
 *  Interface for the tools to convert MuonTileID to coordinates 
 *
 *  @author David Hutchcroft
 *  @date   11/03/2002
 */
static const InterfaceID IID_IMuonRawBuffer( "IMuonRawBuffer" , 1 , 0 );

class IMuonRawBuffer : public virtual IAlgTool {
public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { 
    return  IID_IMuonRawBuffer; 
  }
  //to get all Muon data in one shot
  virtual StatusCode getTile(std::vector<LHCb::MuonTileID>& tile)=0;
  virtual StatusCode getTileAndTDC(std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC)=0;
  virtual StatusCode getPads(std::vector<LHCb::MuonTileID>& pads)=0; 
  virtual StatusCode getNZSupp(std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC)=0;
private:
  // to get muon data belonging to one particular raw bank
  virtual StatusCode getTile(const LHCb::RawBank* r,std::vector<LHCb::MuonTileID>& tile)=0;
  virtual StatusCode getTileAndTDC(const LHCb::RawBank* r,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC)=0;
  virtual StatusCode getPads(const LHCb::RawBank* r,std::vector<LHCb::MuonTileID>& pads)=0;

  virtual MuonTell1Header getHeader(const LHCb::RawBank* r)=0;
public:
  virtual LHCb::RawBankReadoutStatus status()=0;
  virtual void putStatusOnTES()=0;

  virtual std::vector<std::pair<MuonTell1Header, unsigned int> > getHeaders(LHCb::RawEvent* raw)=0;
  virtual  std::vector<std::pair<MuonTell1Header, unsigned int> > getHeaders()=0;





  virtual StatusCode getNZSupp(const LHCb::RawBank* r,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC)=0;
  virtual StatusCode dumpNZSupp(const LHCb::RawBank* r,unsigned int 
ode_num)=0;
  virtual MuonPPEventInfo getPPInfo(const LHCb::RawBank* r,unsigned int pp_num)=0;
  virtual MuonPPEventInfo getPPInfo(unsigned int tell1Number,unsigned int pp_num)=0;
  virtual StatusCode getPads( LHCb::RawEvent* raw,std::vector<LHCb::MuonTileID>& pads,std::string offset)=0;
 
  virtual StatusCode getTile( LHCb::RawEvent* raw,std::vector<LHCb::MuonTileID>& tile,std::string offset)=0;
  virtual StatusCode getTileAndTDC( LHCb::RawEvent* raw,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC,std::string offset)=0;
  virtual StatusCode getNZSupp( LHCb::RawEvent* raw,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC,std::string offset)=0;
  virtual StatusCode getPads( int tell1,std::vector<LHCb::MuonTileID>& pads)  =0;
  virtual StatusCode getPads( int tell1)  =0;
  virtual StatusCode getPadsInStation( int station,std::vector<std::vector<LHCb::MuonTileID>* >& pads)=0;
  
  virtual void forceReset()=0; 	
  virtual MuonODEData getODEData(const LHCb::RawBank* r,unsigned int link)=0;
  virtual unsigned int BXCounter(unsigned int  tell1Number)=0;
  //  virtual unsigned int 
  virtual bool PPReachedHitLimit(unsigned int Tell1Number,int pp_num)=0;
  virtual bool LinkReachedHitLimit(unsigned int Tell1Number,int link_num)=0;
  
};
#endif // MUONDAQ_IMUONRAWBUFFER_H
