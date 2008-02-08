// $Id: L0MuonMonitor.h,v 1.3 2008-02-08 11:17:00 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITOR_H 
#define COMPONENT_L0MUONMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include "MuonKernel/MuonSystemLayout.h"


/** @class L0MuonMonitor L0MuonMonitor.h component/L0MuonMonitor.h
 *  
 *
 *  @author 
 *  @date   2008-01-22
 */
class L0MuonMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum Channel_type {Pad, StripH, StripV , nb_channel_types};

protected:
 
private:
  
  static inline std::string channelTypeName(L0MuonMonitor::Channel_type e)
  { 
    switch ( e ) {   
    case L0MuonMonitor::Pad    : return "Pad";
    case L0MuonMonitor::StripH : return "StripH";
    case L0MuonMonitor::StripV : return "StripV";
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

  std::string histoName(L0MuonMonitor::Channel_type ch_type, int reg, int sta, int qua)
  {
    return quarterName(qua)+"_"+stationName(sta)+"_"+regionName(reg)+"_"+channelTypeName(ch_type);
  };
  
  void setLayouts(); // Set the layouts in use

  std::map<Channel_type , MuonSystemLayout > m_channel_layout;


  // Properties
  std::string m_extension;
  

};

inline  L0MuonMonitor::Channel_type operator++( L0MuonMonitor::Channel_type &rs, int ) 
{  
  return rs = (L0MuonMonitor::Channel_type)(rs + 1); 
};
  

#endif // COMPONENT_L0MUONMONITOR_H
