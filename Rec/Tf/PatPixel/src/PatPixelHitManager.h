#ifndef PATPIXELHITMANAGER_H 
#define PATPIXELHITMANAGER_H 1

// Include files
// from Gaudi
#include "PatPixelHit.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

#include "VPDet/DeVP.h"
#include "Event/VPLiteCluster.h"

#include "PatPixelSensor.h"

static const InterfaceID IID_PatPixelHitManager ( "PatPixelHitManager", 1, 0 );

/** @class PatPixelHitManager PatPixelHitManager.h
 *  Tool to handle the Pixel velo geometry and hits, from FastVelo
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */
class PatPixelHitManager : public GaudiTool, public IIncidentListener {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatPixelHitManager; }

  /// Standard constructor
  PatPixelHitManager( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~PatPixelHitManager( ); ///< Destructor

  virtual StatusCode initialize();

  virtual StatusCode finalize();

  void buildHits();

  void clearHits();

  void handle ( const Incident& incident );

  PatPixelHits& hits( unsigned int sensor ) { return m_sensors[sensor]->hits(); }

  PatPixelSensor* sensor( unsigned int n ) { return m_sensors[n]; }

  unsigned int firstSensor()   const { return m_firstSensor;   }
  unsigned int lastSensor()    const { return m_lastSensor;    }

  int nbHits()     const { return m_nextInPool - m_pool.begin(); }  // number of hits in the pool
  int nbHitsUsed() const
  { unsigned int Count = 0;
    for(std::vector<PatPixelHit>::const_iterator itH = m_pool.begin(); itH != m_nextInPool; ++itH )
    { if((*itH).isUsed()) Count++; }
    return Count; }

  int maxSize() const { return m_maxSize; }

  StatusCode rebuildGeometry();                                  // Recompute the geometry in case of change

  void sortByX();                                                // sort hits by X within every sensor (to speed up the search).

  void print(void)
  { for( std::vector<PatPixelSensor*>::const_iterator itS=m_sensors.begin(); itS != m_sensors.end(); ++itS)
    { (*itS)->print(); }
  }

protected:

private:
  DeVP*                              m_veloPix;          // to convert clusters into 3-D positions
  std::vector<PatPixelHit>           m_pool;             // list of hits: here are the hits stored, PatPixelSensor contains list of pointers to hits
  std::vector<PatPixelHit>::iterator m_nextInPool;       // next free place in the hit list
  std::vector<PatPixelSensor*>       m_sensors;          // list of (pointers to) sensors: every sensor contains a list of pointers to its hits.
  unsigned int                       m_firstSensor;      // index of the first sensor
  unsigned int                       m_lastSensor;       // index of the last sensor
  int                                m_maxSize;
  bool                               m_eventReady;
};

#endif // PATPIXELHITMANAGER_H

