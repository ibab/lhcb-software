#ifndef PRPIXELHITMANAGER_H
#define PRPIXELHITMANAGER_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

// LHCb
// Kernel/LHCbKernel
#include "Kernel/VPConstants.h"
#include "Kernel/VPChannelID.h"
// DAQ/DAQKernel
#include "DAQKernel/DecoderToolBase.h"
// Det/VPDet
#include "VPDet/DeVP.h"
// Local
#include "PrPixelHit.h"
#include "PrPixelModule.h"
#include "PrPixelUtils.h"

namespace LHCb {
class RawEvent;
class RawBank;
}

static const InterfaceID IID_PrPixelHitManager("PrPixelHitManager", 1, 0);

/** @class PrPixelHitManager PrPixelHitManager.h
 *  Tool to handle the VP geometry and hits
 *
 *  @author Olivier Callot
 *  @author Kurt Rinnert
 *  @date   2012-01-05
 */

class PrPixelHitManager : public Decoder::ToolBase, public IIncidentListener {

 public:
  // Return the interface ID
  static const InterfaceID &interfaceID() { return IID_PrPixelHitManager; }

  /// Standard constructor
  PrPixelHitManager(const std::string &type, const std::string &name,
                    const IInterface *parent);
  /// Destructor
  virtual ~PrPixelHitManager();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  bool buildHitsFromRawBank();
  bool buildHitsFromClusters();
  void clearHits();

  void handle(const Incident &incident);

  PrPixelHits &hits(const unsigned int n) const { return m_modules[n]->hits(); }
  PrPixelModule *module(const unsigned int n) const { return m_modules[n]; }

  unsigned int firstModule() const { return m_firstModule; }
  unsigned int lastModule() const { return m_lastModule; }

  /// Return the number of hits in the pool.
  unsigned int nbHits() const { return m_nHits; }
  /// Return the number of hits associated to a track.
  unsigned int nbHitsUsed() const {
    unsigned int nUsed = 0;
    for (unsigned int iH = 0; iH < m_nHits; ++iH) {
      if (m_pool[iH].isUsed()) {
        ++nUsed;
      }
    }
    return nUsed;
  }

  /// Set maximum cluster size.
  void setMaxClusterSize(const unsigned int size) { m_maxClusterSize = size; }
  /// Set trigger flag
  void setTrigger(const bool triggerFlag) { m_trigger = triggerFlag; }
  /// Set cluster location
  void setClusterLocation(const std::string &loc) { m_clusterLocation = loc; }
  int maxSize() const { return m_maxSize; }
  /// Recompute the geometry in case of change
  StatusCode rebuildGeometry();
  /// Sort hits by X within every module (to speed up the search).
  void sortByX();
  /// Wrapper for storing clusters on TES (trigger or offline)
  bool storeClusters();

 private:
  /// Cache Super Pixel patterns for isolated Super Pixel clustering.
  void cacheSPPatterns();

 private:
  /// Detector element
  DeVP *m_vp;
  // List of hits: here are the hits stored
  std::vector<PrPixelHit> m_pool;
  /// Number of hits handed to the tracking in this event.
  unsigned int m_nHits;
  /// Number of clusters in this event.
  unsigned int m_nClusters;
  /// List of pointers to modules (which contain pointers to their hits)
  std::vector<PrPixelModule *> m_modules;
  std::vector<PrPixelModule> m_module_pool;

  /// Indices of first and last module
  unsigned int m_firstModule;
  unsigned int m_lastModule;

  /// Max. number of hits per event
  unsigned int m_maxSize;
  /// Flag whether hits are ready for use
  bool m_eventReady;

  // quick test for message level
  bool m_isDebug;

  // SP pattern buffers for clustering, cached once.
  // There are 256 patterns and there can be at most two
  // distinct clusters in an SP.
  unsigned char m_sp_patterns[256];
  unsigned char m_sp_sizes[256];
  float m_sp_fx[512];
  float m_sp_fy[512];

  // Clustering buffers
  unsigned char m_buffer[VP::NPixelsPerSensor];
  std::vector<uint32_t> m_pixel_idx;
  std::vector<uint32_t> m_stack;

  /// Maximum allowed cluster size (no effect when running on lite clusters).
  unsigned int m_maxClusterSize;

  /// Are we running in the trigger?
  bool m_trigger;

  /// Where to store clusters
  std::string m_clusterLocation;

  /// Cache of local to global transformations, 16 stride aligned.
  float m_ltg[16 * VP::NSensors]; // 16*208 = 16*number of sensors

  /// pointers to local x coordinates and pitches
  const double *m_local_x;
  const double *m_x_pitch;

  /// pixel size in y; this is constant for all pixels on a sensor
  float m_pixel_size;

  /// Storage for pixels contributing to clusters. Not used in trigger.
  std::vector<std::vector<LHCb::VPChannelID> > m_channelIDs;
  /// Storage for x fractions of all clusters. Not used in trigger.
  std::vector<float> m_xFractions;
  /// Storage for y fractions of all clusters. Not used in trigger.
  std::vector<float> m_yFractions;
  /// Storage for 3D points of all clusters. Not used in trigger.
  std::vector<PrPixelHit> m_allHits;

  /// Decode super pixel raw banks.
  void buildHitsFromSPRawBank(const std::vector<LHCb::RawBank *> &tBanks);
  /// Store trigger clusters on TES.
  void storeTriggerClusters();
  /// Store offline (all) clusters on TES.
  void storeOfflineClusters();
};

#endif  // PRPIXELHITMANAGER_H
