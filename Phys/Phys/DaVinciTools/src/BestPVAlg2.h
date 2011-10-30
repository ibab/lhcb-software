// $Id$
#ifndef BESTPVALG2_H 
#define BESTPVALG2_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from DaVinci
#include "Kernel/Particle2VertexBase.h"

class IOnOffline;
class IRelatedPVFinder;

/** @class BestPVAlg2 BestPVAlg2.h
 *  
 * Temporary test!
 *
 *  @author Juan Palacios
 *  @date   2010-08-25
 */
class BestPVAlg2 : public GaudiAlgorithm {
public: 
  /// Standard constructor
  BestPVAlg2( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BestPVAlg2( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  

  template <typename T>
  inline T* 
  i_get(const std::string& location) const
  {
    return exist<T>(location) ? get<T>( location ) : 0 ;
  }

  void tables() const;
  
  void tablesFromTables() const;

  std::string tableLocation(const std::string& location) const;
  
  void checkTable(const DaVinci::Map::Particle2VertexBase* table,
                  const std::string& tableLoc) const;

private:

  std::string m_PVInputLocation;
  std::vector<std::string> m_particleInputLocations;
  std::vector<std::string> m_P2PVInputLocations;
  bool m_useTables;
  IOnOffline* m_OnOffline ; ///< context switch tool. To be deprecated.
  IRelatedPVFinder* m_pvRelator ; ///< Tool that relates the Particle to a PV


};
#endif // PVRELATORALG2_H
