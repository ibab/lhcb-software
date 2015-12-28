// $Id: MuonLogHit.h,v 1.2 2010-02-11 09:29:51 ggiacomo Exp $
#ifndef MUONLOGHIT_H
#define MUONLOGHIT_H 1
#include "Kernel/MuonTileID.h"
#include <sstream>

class MuonLogHit final {
 public:
  /// Standard constructor
  MuonLogHit() = default;
  /// constructor from tile
  explicit MuonLogHit(LHCb::MuonTileID tile ) : m_tile(tile)
  {}
  inline LHCb::MuonTileID tile() const
  { return m_tile; }
  inline int odeNumber() const
  { return m_ODEnumber;}
  inline void setOdeNumber(unsigned int& OdeNumber)
    {m_ODEnumber = (int)OdeNumber; }
  inline int odeChannel() const
  { return m_ODEchannel;}
  inline void setOdeChannel(unsigned int& OdeChannel)
  {m_ODEchannel =(int)OdeChannel; }
  inline int odeIndex() const
  { return m_ODEindex;}
  inline void setOdeIndex(short int& OdeIndex)
  {m_ODEindex =(int)OdeIndex; }
  inline float time() const
  { //avoid bias due to bins 0 and 1 being added in bin 1
    return (float) ((m_time+50*16)%16 == 1  ? m_time-0.499 : m_time);}
  inline int rawtime() const
  { return m_time;}
  inline bool intime() const
  { return (m_time>-1 && m_time <16);  }
  inline int bx()
  { return ( (m_time+160)/16 - 10); }

  inline void setTime(int& TDCtime)
  {m_time = TDCtime; }
  inline std::string odeName() const {
    std::stringstream sname;
    sname << "Q" << (m_tile.quarter()+1)
          << "M" << (m_tile.station()+1)
          << "R" << (m_tile.region() +1)
          << "_" << m_ODEindex;
    return sname.str();
  }
  inline int odeHwID()  const {
    return (m_tile.quarter()+1) *1000 +
      (m_tile.station()+1) *100 +
      (m_tile.region() +1) *10 +
      m_ODEindex;
  }
 private:
  LHCb::MuonTileID m_tile;
  int m_ODEnumber;
  int m_ODEchannel;
  int m_ODEindex;
  int m_time;
};
#endif // MUONLOGHIT_H
