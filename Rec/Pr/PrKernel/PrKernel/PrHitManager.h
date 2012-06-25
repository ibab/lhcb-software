// $Id: $
#ifndef PRKERNEL_PRHITMANAGER_H 
#define PRKERNEL_PRHITMANAGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "PrKernel/PrHit.h"
#include "PrKernel/PrHitZone.h"

static const InterfaceID IID_PrHitManager ( "PrHitManager", 1, 0 );

/** @class PrHitManager PrHitManager.h PrKernel/PrHitManager.h
 *  Hit manager for the PrHits
 *
 *  @author Olivier Callot
 *  @date   2012-03-13
 */
class PrHitManager : public GaudiTool, public IIncidentListener {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PrHitManager; }

  /// Standard constructor
  PrHitManager( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~PrHitManager( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual void buildGeometry() {};
  virtual void decodeData()    {};
  
  /// Clear the zones, and reset the hit pointer. Store the maximum size.
  void clearHits() {
    int lastSize = m_nextInPool - m_pool.begin();
    if ( lastSize > m_maxSize ) m_maxSize = lastSize;
    
    for ( PrHitZones::iterator itS = m_zones.begin(); m_zones.end() != itS; ++itS ) {
      if ( 0 != *itS) (*itS)->reset();
    }
    m_nextInPool = m_pool.begin();
  };

  /// Handle the incident 'BEginEvent': Invalidate the decoding
  void handle ( const Incident& incident ) {
    if ( IncidentType::BeginEvent == incident.type() ){
      this->clearHits();
      m_eventReady = false;
    }
  };

  /// Return a new hit. Increase the pool size if needed
  PrHit* newHit( ) { 
    if ( m_nextInPool == m_pool.end() ) {
      unsigned int nbPrev = m_nextInPool - m_pool.begin();
      while ( m_pool.size() < nbPrev + 100 ) {
        PrHit* tmp = new PrHit();
        m_pool.push_back( tmp );
      }
      m_nextInPool = m_pool.begin() + nbPrev;
    }
    return *(m_nextInPool++);
  }

  /// Return a new hit assigned to the specified zone
  PrHit* newHitInZone( int lay ) {
    PrHit* tmp = newHit();
    zone(lay)->hits().push_back( tmp );
    return tmp;
  }
  
  /// Shortcut to access the hits of a given zone.
  PrHits& hits( unsigned int zone ) { return m_zones[zone]->hits(); }

  /// Access a zone. Create it if needed...
  PrHitZone* zone( unsigned int n ) { 
    while( m_zones.size() <= n ) {
      PrHitZone* tmp = new PrHitZone( m_zones.size() );
      m_zones.push_back( tmp );
    }
    return m_zones[n]; 
  }

  /// Return the current number of zones
  unsigned int nbZones()   const { return m_zones.size();   }

protected:

private:
  PrHits           m_pool;
  PrHits::iterator m_nextInPool;
  PrHitZones       m_zones;
  int              m_maxSize;
  bool             m_eventReady;
};
#endif // PRKERNEL_PRHITMANAGER_H
