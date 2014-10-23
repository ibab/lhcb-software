#ifndef VERTEXISOLATIONDETACHED_H
#define VERTEXISOLATIONDETACHED_H 1

#include "Kernel/IRelatedInfoTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "CaloUtils/CaloParticle.h"

#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
#include "Kernel/IVertexFit.h"
class IDVAlgorithm;
class IDistanceCalculator;
class IVertexFit;

namespace LHCb {
  class Particle;
  class Vertex;
}

/** @class TupleToolVtxIsoln TupleToolVtxIsoln.h
 *
 * \brief Fill isolation information for DecayTreeTuple
 *
 * - head_NOPARTWITHINDCHI2WDW : no. of non-signal particles that when added to vertex give delta chi2 < specified window
 * - head_NOPARTWITHINCHI2WDW : no. of non-signal particles that when added to vertex give chi2 < specified window
 * head_SMALLESTCHI2: chi2 of smallest chi2 combination with any of the input Particles
 * head_SMALLESTDELTACHI2: delta chi2 of smallest delta chi2 combination with any of the input Particles
 *
 * \sa DecayTreeTuple
 *
 *  @todo Maybe one should get Tracks instead of Particles?
 *
 *  @author Mitesh Patel, Patrick Koppenburg
 *  @date   2008-04-15
 */
class RelInfoVertexIsolationDetached : public GaudiTool, virtual public IRelatedInfoTool
{

public:

  /// Standard constructor
  RelInfoVertexIsolationDetached( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

  virtual StatusCode initialize () ;

  virtual ~RelInfoVertexIsolationDetached( ); ///< Destructor

  virtual StatusCode calculateRelatedInfo( const LHCb::Particle*,
                                           const LHCb::Particle*);

  virtual LHCb::RelatedInfoMap* getInfo(void);

private:

  const LHCb::Vertex* originVertex( const  LHCb::Particle*
                                    , const LHCb::Particle* ) const;

  /// Check if a pure CALO Particle
  inline bool isPureNeutralCalo(const LHCb::Particle* P)const
  {
    LHCb::CaloParticle caloP(  (LHCb::Particle*) P );
    return caloP.isPureNeutralCalo();
  }  // Helpers

private:

  std::vector<std::string> m_variables;
  std::vector<short int> m_keys;

  IDVAlgorithm* m_dva;
  const IDistanceCalculator* m_dist;
  const IVertexFit* m_pVertexFit;
  double m_deltaChi2;
  double m_Chi2;
  std::string m_typeVertexFit;
  std::vector<std::string> m_inputParticles;

  LHCb::RelatedInfoMap       m_map;

};

#endif
