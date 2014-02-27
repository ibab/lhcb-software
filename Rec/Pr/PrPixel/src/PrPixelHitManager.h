#ifndef PRPIXELHITMANAGER_H 
#define PRPIXELHITMANAGER_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
// LHCb
// Event/DigiEvent
#include "Event/VPLiteCluster.h"
// Det/VPDet
#include "VPDet/DeVP.h"
// Local
#include "PrPixelHit.h"
#include "PrPixelModule.h"

namespace LHCb {
  class RawEvent;
}

static const InterfaceID IID_PrPixelHitManager("PrPixelHitManager", 1, 0);

/** @class PrPixelHitManager PrPixelHitManager.h
 *  Tool to handle the VP geometry and hits
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */

class PrPixelHitManager : public GaudiTool, public IIncidentListener {

public: 

  /// Useful constants
  enum { 
    MODULE_SENSORS = 4,
    SENSOR_CHIPS = 3,
    CHIP_ROWS = 256,
    CHIP_COLUMNS = 256,
    CHIP_0_END = 255,
    CHIP_1_START = 256,
    CHIP_1_END = 511,
    CHIP_2_START = 512,
    SENSOR_ROWS = 256, 
    SENSOR_COLUMNS = 768, 
    SENSOR_PIXELS = SENSOR_ROWS*SENSOR_COLUMNS,
    TOT_MODULES = 52,
    TOT_SENSORS = TOT_MODULES*MODULE_SENSORS
  };

  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_PrPixelHitManager;}

  /// Standard constructor
  PrPixelHitManager(const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  /// Destructor
  virtual ~PrPixelHitManager();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void buildHitsFromSPRawBank();
  void buildHits();
  void clearHits();

  void handle(const Incident& incident);

  PrPixelHits& hits(const unsigned int n) const {
    return m_modules[n]->hits();
  }
  PrPixelModule* module(const unsigned int n) const {
    return m_modules[n];
  }

  unsigned int firstModule() const {return m_firstModule;}
  unsigned int lastModule() const  {return m_lastModule;}

  /// Return the number of hits in the pool.
  unsigned int nbHits() const {return m_nHits;} 
  /// Return the number of hits associated to a track.
  unsigned int nbHitsUsed() const {
    unsigned int nUsed = 0;
    for (unsigned int iH = 0; iH < m_nHits; ++iH) {
      if (m_pool[iH].isUsed()) { ++nUsed; } 
    }
    return nUsed; 
  }

  /// Set maximum cluster size.
  void setMaxClusterSize(const unsigned int size) { m_maxClusterSize = size; }
  /// Set slope correction flag.
  void useSlopeCorrection(const bool flag) {m_useSlopeCorrection = flag;}
  int maxSize() const {return m_maxSize;}
  /// Calculate X,Y,Z-position for pixel ChannelID with fractional interpolation.
  //Gaudi::XYZPoint position(LHCb::VPChannelID id, double dx, double dy);
  /// Recompute the geometry in case of change
  StatusCode rebuildGeometry();                                  
  /// Sort hits by X within every module (to speed up the search).
  void sortByX();                                                

private:

  void cacheSPPatterns();

private:
  /// Detector element
  DeVP* m_vp;
  // List of hits: here are the hits stored
  std::vector<PrPixelHit> m_pool;             
  /// Number of hits in this event.
  unsigned int m_nHits;
  /// List of pointers to modules (which contain pointers to their hits)
  std::vector<PrPixelModule*> m_modules;          
  std::vector<PrPixelModule> m_module_pool;          

  /// Indices of first and last module
  unsigned int m_firstModule;
  unsigned int m_lastModule;

  /// Flag to use position correction based on track slope or not
  bool m_useSlopeCorrection;
 
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
  double m_sp_fx[512];
  double m_sp_fy[512];

  // Clustering buffers
  unsigned char m_buffer[SENSOR_PIXELS];
  std::vector<uint32_t> m_pixel_idx;
  std::vector<uint32_t> m_stack;

  /// Maximum allowed cluster size (no effect when running on lite clusters).
  unsigned int m_maxClusterSize;

  /// Cache of local to global transformations, 16 stride aligned.
  double m_ltg[16*TOT_SENSORS]; // 16*208 = 16*number of sensors

  /// pointers to local x coordinates and pitches
  const double *m_local_x;
  const double *m_x_pitch;

  /// pixel size in y; this is constant for all pixels on a sensor
  double m_pixel_size;
};

#endif // PRPIXELHITMANAGER_H

