#ifndef COMMONMUONHITMANAGER_H_
#define COMMONMUONHITMANAGER_H_

#include <array>
#include <vector>

#include "Event/MuonCoord.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonDet/DeMuonDetector.h"

#include "MuonID/CommonMuonHit.h"
#include "MuonID/CommonMuonStation.h"

static const InterfaceID IID_CommonMuonHitManager("CommonMuonHitManager", 1, 0);

/** @class CommonMuonHitManager CommonMuonHitManager.h
 *  Muon hit manager for Hlt1 and Offline.
 *
 *  Used to be Hlt/Hlt1Muons/Hlt1MuonHitManager.
 *
 *  @author Roel Aaij
 *  @author Kevin Dungs
 *  @date   2015-01-03
 */
class CommonMuonHitManager : public GaudiTool, public IIncidentListener {
 public:
  static const InterfaceID& interfaceID() { return IID_CommonMuonHitManager; }

  CommonMuonHitManager(const std::string& type, const std::string& name,
                       const IInterface* parent);

  ~CommonMuonHitManager() override = default;

  StatusCode initialize() override;

  void handle(const Incident& incident) override;

  virtual CommonMuonHitRange hits(double xmin, unsigned int station,
                                  unsigned int region);
  virtual CommonMuonHitRange hits(double xmin, double xmax,
                                  unsigned int station, unsigned int region);
 
  virtual unsigned int nRegions(unsigned int station) const;

  virtual const CommonMuonStation& station(unsigned int id) const;

 private:
  // Properties
  std::string m_coordLocation;

  // Data members
  DeMuonDetector* m_muonDet;

  std::vector<CommonMuonStation> m_stations;
  std::bitset<5> m_prepared;

  std::array<std::vector<const LHCb::MuonCoord*>, 5> m_coords;
  bool m_loaded;

  // Functions
  void prepareHits(unsigned int station);

  void loadCoords();
};
#endif  // COMMONMUONHITMANAGER_H_
