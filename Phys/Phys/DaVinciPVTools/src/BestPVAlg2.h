// $Id$
#ifndef BESTPVALG2_H 
#define BESTPVALG2_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from DaVinci
#include "Kernel/Particle2VertexBase.h"

// from LHCb
#include "Kernel/IRelatedPVFinder.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/Particle2Vertex.h"
#include "Relations/Get.h"
// DaVinci
#include "Kernel/DaVinciStringUtils.h"
#include "Kernel/DefaultDVToolTypes.h"

/** @class BestPVAlg2 BestPVAlg2.h
 *  
 * Temporary test!
 *
 *  @author Juan Palacios
 *  @date   2010-08-25
 */
class BestPVAlg2 : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  BestPVAlg2( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BestPVAlg2( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  template <typename T>
  inline T* 
  i_get(const std::string& location) const
  {
    return getIfExists<T>( location );
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
  std::string m_pvRelatorName;   ///< The name of the PV relator to use
  IRelatedPVFinder* m_pvRelator ; ///< Tool that relates the Particle to a PV


};
#endif // PVRELATORALG2_H
