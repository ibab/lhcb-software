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
#include "PatPixelSensor.h"

static const InterfaceID IID_PatPixelHitManager("PatPixelHitManager", 1, 0);

/** @class PatPixelHitManager PatPixelHitManager.h
 *  Tool to handle the Pixel velo geometry and hits, from FastVelo
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

  PatPixelHits& hits(const unsigned int n) {
    return m_modules[n]->hits();
  }
  PatPixelSensor* sensor(const unsigned int n) {
    return m_modules[n];
  }

  unsigned int firstModule() const {return m_firstModule;}
  unsigned int lastModule() const  {return m_lastModule;}

  int nbHits()     const { return m_nextInPool - m_pool.begin(); }  // number of hits in the pool
  int nbHitsUsed() const {
    unsigned int nUsed = 0;
    std::vector<PatPixelHit>::const_iterator itH;
    for (itH = m_pool.begin(); itH != m_nextInPool; ++itH) {
      if ((*itH).isUsed()) ++nUsed; 
    }
    return nUsed; 
  }

  int maxSize() const {return m_maxSize;}

  // X,Y,Z-position for pixel ChannelID with fractional interpolation
  Gaudi::XYZPoint position(LHCb::VPChannelID id, double dx, double dy) {
    const DeVPSensor* sensor = m_vp->sensorOfChannel(id);
    std::pair<double, double> offsets(dx, dy);
    double dx_prime = dx, dy_prime = dy;
    Double_t P_offset, T_factor;
    Double_t slx, sly;
    Double_t delta_x = fabs(dx - 0.5);
    Double_t delta_y = fabs(dy - 0.5);
    Gaudi::XYZPoint Point = sensor->channelToPoint(id, offsets);
    if (dx == 0.5 && dy ==0.5) return Point;
    if (dx != 0.5) {
      slx = fabs(Point.x()/Point.z());
      P_offset =  0.31172471 +  0.15879833 * TMath::Erf(-6.78928312*slx + 0.73019077);
      T_factor =  0.43531842 +  0.3776611  * TMath::Erf( 6.84465914*slx - 0.75598833);
      dx_prime = 0.5 + (dx-0.5)/delta_x*(P_offset + T_factor *delta_x); }

    if (dy != 0.5) {
      sly = fabs(Point.y()/Point.z());
      P_offset =  0.35829374 - 0.20900493 * TMath::Erf(5.67571733*sly -0.40270243);
      T_factor =  0.29798696 + 0.47414641 * TMath::Erf(5.84419802*sly -0.40472057);
      dy_prime = 0.5 + (dy-0.5)/delta_y*(P_offset + T_factor *delta_y); 
    }
    std::pair<double, double> offsets2(dx_prime, dy_prime);
    return sensor->channelToPoint(id, offsets2);
  }
  /// Recompute the geometry in case of change
  StatusCode rebuildGeometry();                                  
  // Sort hits by X within every module (to speed up the search).
  void sortByX();                                                

private:
  /// Detector element
  DeVP* m_vp;
  // List of hits: here are the hits stored
  std::vector<PatPixelHit> m_pool;             
  /// Next free place in the hit list
  std::vector<PatPixelHit>::iterator m_nextInPool;       
  /// List of pointers to modules (which contain pointers to their hits)
  std::vector<PatPixelSensor*> m_modules;          

  /// Indices of first and last module
  unsigned int m_firstModule;
  unsigned int m_lastModule;

  /// Max. number of hits per event
  int m_maxSize;
  /// Flag whether hits are ready for use
  bool m_eventReady;
};

#endif // PATPIXELHITMANAGER_H

