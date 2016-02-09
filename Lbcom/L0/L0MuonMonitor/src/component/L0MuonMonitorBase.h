#ifndef COMPONENT_L0MUONMONITORBASE_H 
#define COMPONENT_L0MUONMONITORBASE_H 1

// Include files
#include <fstream>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"
#include "Kernel/MuonTileID.h"

#include "MuonKernel/MuonSystemLayout.h"
#include "L0MuonKernel/MonUtilities.h"

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

  void olsErrorSummary(MsgStream & msg) const;
  void olsErrorSummary(std::ofstream & fout) const;
  
  StatusCode getL0MuonTiles(std::vector<LHCb::MuonTileID> & l0muontiles);
  StatusCode getL0MuonPads(std::vector<LHCb::MuonTileID> & l0muonpads);
  StatusCode getL0MuonTilesTAE(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muontiles);
  StatusCode getL0MuonPadsTAE(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muonpads);

  void activeTS(std::vector<int> & active_slots);
  
  StatusCode getOlsInError(std::vector<LHCb::MuonTileID> & ols);

  StatusCode storeOlsInError();
  
  std::map<std::pair<LHCb::MuonTileID,int> ,bool> m_err_ols;
  std::map<LHCb::MuonTileID,int> m_err_ols_stored;
  
  void setLayouts(); // Set the layouts in use
  MuonSystemLayout  m_opt_link_layout;

  bool excludedBx(); //Return true if the current Bx is in the list of exclusion  
  bool exclusiveBx(); //Return true if the current Bx is in the list of exclusive  
  bool selectedTrigger();

  AIDA::IHistogram1D * m_h_online; /// Error counters

  bool m_shortnames; // Use shortname for histograms created by the monitoring tools  
  bool m_online;     /// Online mode (light output)

  double m_prescale; // Prescaling: fraction of events to process
  Rndm::Numbers  m_rnd;
  
  std::vector<int> m_time_slots;
  std::vector<int> m_stations;
  std::vector<int> m_quarters;
  std::vector<int> m_regions;
  std::vector<unsigned int> m_excludedBxs;
  std::vector<unsigned int> m_exclusiveBxs;
  //   std::string m_triggerType;
  int m_triggerType;
  

  inline bool quarterInUse(int qua) const
  {
    return std::find(m_quarters.begin(),m_quarters.end(),qua)!=m_quarters.end();
  }
  inline bool regionInUse(int reg) const
  {
    return std::find(m_regions.begin(), m_regions.end(),reg)!=m_regions.end();
  }
  inline bool stationInUse(int sta) const
  {
    return std::find(m_stations.begin(),m_stations.end(),sta)!=m_stations.end();
  }

};
#endif // COMPONENT_L0MUONMONITORBASE_H
