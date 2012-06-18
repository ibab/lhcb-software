// $Id: $
#ifndef PRKERNEL_PRHITMANAGER_H 
#define PRKERNEL_PRHITMANAGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "PrKernel/PrHit.h"
#include "PrKernel/PrHitLayer.h"

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
  
  /// Clear the layers, and reset the hit pointer. Store the maximum size.
  void clearHits() {
    int lastSize = m_nextInPool - m_pool.begin();
    if ( lastSize > m_maxSize ) m_maxSize = lastSize;
    
    for ( PrHitLayers::iterator itS = m_layers.begin(); m_layers.end() != itS; ++itS ) {
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

  /// Return a new hit assigned to the specified layer
  PrHit* newHitInLayer( int lay ) {
    PrHit* tmp = newHit();
    layer(lay)->hits().push_back( tmp );
    return tmp;
  }
  
  /// Shortcut to access the hits of a given layer.
  PrHits& hits( unsigned int layer ) { return m_layers[layer]->hits(); }

  /// Access a layer. Create it if needed...
  PrHitLayer* layer( unsigned int n ) { 
    while( m_layers.size() <= n ) {
      PrHitLayer* tmp = new PrHitLayer( m_layers.size() );
      m_layers.push_back( tmp );
    }
    return m_layers[n]; 
  }

  /// Return the current number of layers
  unsigned int nbLayers()   const { return m_layers.size();   }

protected:

private:
  PrHits           m_pool;
  PrHits::iterator m_nextInPool;
  PrHitLayers      m_layers;
  int              m_maxSize;
  bool             m_eventReady;
};
#endif // PRKERNEL_PRHITMANAGER_H
