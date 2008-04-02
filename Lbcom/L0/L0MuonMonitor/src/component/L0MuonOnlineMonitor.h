// $Id: L0MuonOnlineMonitor.h,v 1.1.1.1 2008-04-02 12:41:15 jucogan Exp $
#ifndef COMPONENT_L0MUONONLINEMONITOR_H 
#define COMPONENT_L0MUONONLINEMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "MuonKernel/MuonSystemLayout.h"


/** @class L0MuonOnlineMonitor L0MuonOnlineMonitor.h component/L0MuonOnlineMonitor.h
 *  
 *
 *  @author 
 *  @date   2008-04-02
 */
class L0MuonOnlineMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonOnlineMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonOnlineMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum Channel_type {Pad, StripH, StripV , nb_channel_types};

protected:

private:

  StatusCode makePads(std::vector<LHCb::MuonTileID>& tiles, std::vector<LHCb::MuonTileID>& pads);

  AIDA::IHistogram1D* m_decoding_status[4][3];

  void setLayouts(); // Set the layouts in use
  std::map<Channel_type , MuonSystemLayout > m_channel_layout;
  MuonSystemLayout m_pad_layout;

  inline bool quarterInUse(int qua) 
  {
    std::vector<int>::iterator i;
    for (i=m_quarters.begin(); i<m_quarters.end(); ++i){
      if ((*i)==qua) return true;
    }
    return false;
  }
  inline bool regionInUse(int reg) 
  {
    std::vector<int>::iterator i;
    for (i=m_regions.begin(); i<m_regions.end(); ++i){
      if ((*i)==reg) return true;
    }
    return false;
  }
  inline bool stationInUse(int sta) 
  {
    std::vector<int>::iterator i;
    for (i=m_stations.begin(); i<m_stations.end(); ++i){
      if ((*i)==sta) return true;
    }
    return false;
  }
  

  inline void flipCoord(int &x, int&y, const int qua)
  {
    switch ( qua ) {   
    case 0: 
      break;
    case 1: 
      y=-y;
      break;
    case 2: 
      x=-x;
      y=-y;
      break;
    case 3: 
      x=-x;
      break;
    };
  }
  
  
  static inline std::string channelTypeName(Channel_type e)
  { 
    switch ( e ) {   
    case Pad    : return "Pad";
    case StripH : return "StripH";
    case StripV : return "StripV";
    default : return "Unknown";
    };
  };
  
  static inline std::string quarterName(int qua)
  { 
    switch ( qua ) {   
    case 0    : return "Q1";
    case 1    : return "Q2";
    case 2    : return "Q3";
    case 3    : return "Q4";
    default : return "Unknown";
    };
  };
  
  static inline std::string regionName(int reg)
  { 
    switch ( reg ) {   
    case 0    : return "R1";
    case 1    : return "R2";
    case 2    : return "R3";
    case 3    : return "R4";
    default : return "Unknown";
    };
  };
  
  static inline std::string stationName(int sta)
  { 
    switch ( sta ) {   
    case 0    : return "M1";
    case 1    : return "M2";
    case 2    : return "M3";
    case 3    : return "M4";
    case 4    : return "M5";
    default : return "Unknown";
    };
  };
  static inline std::string timeSlot(int bx)
  {
    std::string ts;
    switch (bx) {
    case -7 : return "Prev7/";
    case -6 : return "Prev6/";
    case -5 : return "Prev5/";
    case -4 : return "Prev4/";
    case -3 : return "Prev3/";
    case -2 : return "Prev2/";
    case -1 : return "Prev1/";
    case  0 : return "";
    case  1 : return "Next1/";
    case  2 : return "Next2/";
    case  3 : return "Next3/";
    case  4 : return "Next4/";
    case  5 : return "Next5/";
    case  6 : return "Next6/";
    case  7 : return "Next7/";
    default : return "Unknown";
    };

  };

  std::vector<int> m_time_slots;
  std::vector<int> m_stations;
  std::vector<int> m_quarters;
  std::vector<int> m_regions;
  

};
inline  L0MuonOnlineMonitor::Channel_type operator++( L0MuonOnlineMonitor::Channel_type &rs, int ) 
{  
  return rs = (L0MuonOnlineMonitor::Channel_type)(rs + 1); 
};
#endif // COMPONENT_L0MUONONLINEMONITOR_H
