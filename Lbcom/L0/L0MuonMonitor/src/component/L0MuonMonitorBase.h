// $Id: L0MuonMonitorBase.h,v 1.1 2008-04-08 11:31:03 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITORBASE_H 
#define COMPONENT_L0MUONMONITORBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class L0MuonMonitorBase L0MuonMonitorBase.h component/L0MuonMonitorBase.h
 *  
 *
 *  @author 
 *  @date   2008-04-08
 */
class L0MuonMonitorBase : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonMonitorBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitorBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:


  std::vector<int> m_time_slots;
  std::vector<int> m_stations;
  std::vector<int> m_quarters;
  std::vector<int> m_regions;

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

};
#endif // COMPONENT_L0MUONMONITORBASE_H
