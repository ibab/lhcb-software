// $Id: $
#ifndef FASTVELOHITMANAGER_H 
#define FASTVELOHITMANAGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "FastVeloSensor.h"
#include "VeloDet/DeVelo.h"

static const InterfaceID IID_FastVeloHitManager ( "FastVeloHitManager", 1, 0 );

/** @class FastVeloHitManager FastVeloHitManager.h
 *  Tool to handle the Fast velo geometry and hits
 *
 *  @author Olivier Callot
 *  @date   2010-09-08
 */
class FastVeloHitManager : public GaudiTool, public IIncidentListener {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_FastVeloHitManager; }

  /// Standard constructor
  FastVeloHitManager( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~FastVeloHitManager( ); ///< Destructor

  virtual StatusCode initialize();

  void buildFastHits();

  void clearHits();

  void handle ( const Incident& incident );

  FastVeloHits& hits( unsigned int sensor, int zone ) { return m_sensors[sensor]->hits(zone); }

  FastVeloSensor* sensor( unsigned int n ) { return m_sensors[n]; }

  unsigned int firstRSensor()   const { return m_firstR;   }
  unsigned int lastRSensor()    const { return m_lastR;    }
  unsigned int firstPhiSensor() const { return m_firstPhi; }
  unsigned int lastPhiSensor()  const { return m_lastPhi;  }

  double cosPhi( unsigned int zone ) const { return m_cosPhi[zone]; }
  double sinPhi( unsigned int zone ) const { return m_sinPhi[zone]; }

  int nbHits() const { return m_nextInPool - m_pool.begin(); }
  int maxSize() const { return m_maxSize; }

  StatusCode rebuildGeometry();  ///< Recompute the geometry in case of change

  FastVeloHit* hitByLHCbID( LHCb::LHCbID id );

  void resetUsedFlags ( );
  
protected:

private:
  DeVelo* m_velo;
  std::vector<FastVeloHit>  m_pool;
  std::vector<FastVeloHit>::iterator m_nextInPool;
  std::vector<FastVeloSensor*> m_sensors;
  unsigned int m_firstR;
  unsigned int m_lastR;
  unsigned int m_firstPhi;
  unsigned int m_lastPhi;
  std::vector<double> m_cosPhi;
  std::vector<double> m_sinPhi;
  int m_maxSize;
  bool m_eventReady;
  double m_lastXOffsetRight;
  double m_lastXOffsetLeft;
};
#endif // FASTVELOHITMANAGER_H
