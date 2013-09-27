#ifndef PATPIXELHITMANAGER_H 
#define PATPIXELHITMANAGER_H 1

#include "TMath.h"
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
#include "PatPixelHit.h"
#include "PatPixelModule.h"

static const InterfaceID IID_PatPixelHitManager("PatPixelHitManager", 1, 0);

/** @class PatPixelHitManager PatPixelHitManager.h
 *  Tool to handle the VP geometry and hits
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */

class PatPixelHitManager : public GaudiTool, public IIncidentListener {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_PatPixelHitManager;}

  /// Standard constructor
  PatPixelHitManager(const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  /// Destructor
  virtual ~PatPixelHitManager();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void buildHits();
  void clearHits();

  void handle(const Incident& incident);

  PatPixelHits& hits(const unsigned int n) const {
    return m_modules[n]->hits();
  }
  PatPixelModule* module(const unsigned int n) const {
    return m_modules[n];
  }

  unsigned int firstModule() const {return m_firstModule;}
  unsigned int lastModule() const  {return m_lastModule;}

  /// Return the number of hits in the pool.
  unsigned int nbHits() const {return m_nextInPool - m_pool.begin();} 
  /// Return the number of hits associated to a track.
  unsigned int nbHitsUsed() const {
    unsigned int nUsed = 0;
    std::vector<PatPixelHit>::const_iterator itH;
    for (itH = m_pool.begin(); itH != m_nextInPool; ++itH) {
      if ((*itH).isUsed()) ++nUsed; 
    }
    return nUsed; 
  }

  void useSlopeCorrection(const bool flag) {m_useSlopeCorrection = flag;}
  int maxSize() const {return m_maxSize;}
  /// Calculate X,Y,Z-position for pixel ChannelID with fractional interpolation.
  Gaudi::XYZPoint position(LHCb::VPChannelID id, double dx, double dy);
  /// Recompute the geometry in case of change
  StatusCode rebuildGeometry();                                  
  /// Sort hits by X within every module (to speed up the search).
  void sortByX();                                                

private:
  /// Detector element
  DeVP* m_vp;
  // List of hits: here are the hits stored
  std::vector<PatPixelHit> m_pool;             
  /// Next free place in the hit list
  std::vector<PatPixelHit>::iterator m_nextInPool;       
  /// List of pointers to modules (which contain pointers to their hits)
  std::vector<PatPixelModule*> m_modules;          

  /// Indices of first and last module
  unsigned int m_firstModule;
  unsigned int m_lastModule;

  /// Flag to use position correction based on track slope or not
  bool m_useSlopeCorrection;
 
  /// Max. number of hits per event
  int m_maxSize;
  /// Flag whether hits are ready for use
  bool m_eventReady;
};

#endif // PATPIXELHITMANAGER_H

