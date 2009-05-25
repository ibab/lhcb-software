// $Id: PVRelatorAlg.h,v 1.3 2009-05-25 08:48:16 jpalac Exp $
#ifndef PVRELATORALG_H 
#define PVRELATORALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IOnOffline;
class IRelatedPVFinder;

/** @class PVRelatorAlg PVRelatorAlg.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2009-05-23
 */
class PVRelatorAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PVRelatorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PVRelatorAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  template <typename T>
  inline T* 
  i_get(const std::string& location) const
  {
    return (exist<T>( location )) ? get<T>( location ) : 0 ;
  }

  Particle2Vertex::Table* table() const;
  
  Particle2Vertex::Table* tableFromTable() const;

  /// @todo remove with LHCb v27r1
  void getPVsFromVertexBases(const LHCb::VertexBase::ConstVector& v,
                             LHCb::RecVertex::ConstVector& pvs) const;
  

private:

  std::string m_particleInputLocation;
  std::string m_PVInputLocation;
  std::string m_P2PVInputLocation;
  std::string m_P2PVOutputLocation;
  bool m_useTable;
  IOnOffline* m_OnOffline ; ///< context switch tool. To be deprecated.
  IRelatedPVFinder* m_pvRelator ; ///< Tool that relates the Particle to a PV


};
#endif // PVRELATORALG_H
