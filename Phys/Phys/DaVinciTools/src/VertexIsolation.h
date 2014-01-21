#ifndef VERTEXISOLATION_H
#define VERTEXISOLATION_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
//#include "Kernel/DVAlgorithm.h"
#include "Kernel/IExtraInfoTool.h"
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
//class IVertexFit;
//class LHCb::CaloParticle ;

/** @class VertexIsolation VertexIsolation.h
 *
 * \brief Calculate vertex isolation.
 *    Take vertexed particle and add other tracks of the
 *    event, one by one, building a new vertex.
 *
 * Variables:
 *    - m_nPartChi2Win: number of particles that generate a vertex within a chi2 window
 *    - m_smallestDeltaChi2OneTrack: smallest delta chi2 when adding one track
 *    - m_smallestDeltaChi2MassOneTrack: mass of the candidate with the smallest delta chi2
 *    - m_smallestDeltaChi2TwoTracks: smallest delta chi2 when adding one track to the combination that has the smallest delta chi2 when adding one track
 *    - m_smallestDeltaChi2MassTwoTracks: mass of the candidate with the smallest delta chi2 when adding one track to the combination that has the smallest delta chi2 when adding one track
 *
 * Options:
 *    - InputParticles: containers for extra particles used for building extra vertices
 *    - MaxChi2: maximum chi2 for counting compatible vertices
 *
 *  Converted from TupleTool by A. Puig 05/08/2013
 *
 */

class VertexIsolation : public GaudiTool, virtual public IExtraInfoTool
{

public:

  /// Standard constructor
  VertexIsolation( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  virtual StatusCode initialize () ;

  virtual StatusCode calculateExtraInfo( const LHCb::Particle*,
                                         const LHCb::Particle*);

  virtual int getFirstIndex(void);

  virtual int getNumberOfParameters(void);

  virtual int getInfo(int index, double & value, std::string & name);

  virtual ~VertexIsolation( ); ///< Destructor

private:

  /// Check if a pure CALO Particle
  inline bool isPureNeutralCalo(const LHCb::Particle* P)const
  {
    LHCb::CaloParticle caloP(  (LHCb::Particle*) P );
    return caloP.isPureNeutralCalo();
  }  // Helpers

private:

  int    m_nPartDeltaChi2Win ;
  int    m_nPartChi2Win ;
  double m_smallestDeltaChi2OneTrack ;
  double m_smallestChi2OneTrack ;
  double m_smallestDeltaChi2MassOneTrack ;
  double m_smallestDeltaChi2TwoTracks ;
  double m_smallestChi2TwoTracks ;
  double m_smallestDeltaChi2MassTwoTracks ;

  /// Find all the signal particles to vertex
  void findDaughters2Vertex( const LHCb::Particle* );
  LHCb::Particle::ConstVector m_particlesToVertex ;

  // Config and vertexing
  std::vector<std::string>   m_inputParticles ;
  IDVAlgorithm*              m_dva;
  const IDistanceCalculator* m_dist;
  const IVertexFit*          m_pVertexFit;
  double                     m_chi2 ;

public:

  // Structure of isolation vars
  struct IsolationResult
  {
    // Constructor with defaults
    IsolationResult() :  nCompatibleDeltaChi2(0), nCompatibleChi2(0), 
                         smallestChi2(-1), smallestDeltaChi2(-1), 
                         bestParticle(NULL) {} 
    // Members
    int             nCompatibleDeltaChi2, nCompatibleChi2 ;
    double          smallestChi2, smallestDeltaChi2 ;
    LHCb::Particle *bestParticle ;
  };

  // Isolation calculation
  VertexIsolation::IsolationResult getIsolation( const double, LHCb::Particle::ConstVector& ) ;

};

#endif // VERTEXISOLATION_H
