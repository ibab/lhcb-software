#ifndef BEAMGASVERTEXMONITOR_H
#define BEAMGASVERTEXMONITOR_H 1

#include <GaudiAlg/GaudiTupleAlg.h>
#include <VeloDet/DeVelo.h>
#include <Event/RecVertex.h>
#include <Event/MCVertex.h>


class BeamGasVertex {
public:
  BeamGasVertex();
  BeamGasVertex(const LHCb::HltObjectSummary& recvtx_hos, const DeVelo* velo);
  BeamGasVertex(const LHCb::RecVertex& recvtx);
  BeamGasVertex(const LHCb::MCVertex& mcvtx);

  double x, y, z, chi2;
  int ndof, ntr, ntrbw;

private:
  bool isBackwardTrack(const LHCb::HltObjectSummary& track_hos, const DeVelo* velo);
};

class BeamGasCandidate {
public:
  BeamGasCandidate() : pv(), sv1(), sv2(), selections() {};
  BeamGasCandidate(const BeamGasVertex& pv) : pv(pv), sv1(), sv2(), selections() {};
  BeamGasCandidate(const BeamGasVertex& pv, const BeamGasVertex& sv1, const BeamGasVertex& sv2)
  : pv(pv), sv1(sv1), sv2(sv2), selections() {};

  BeamGasVertex pv, sv1, sv2;
  std::set<std::string> selections;
};

typedef std::vector<BeamGasCandidate> BeamGasCandidates;
typedef std::vector<std::pair<std::string, BeamGasCandidates>> BeamGasCandidatesSet;


/** @class BeamGasVertexMonitor BeamGasVertexMonitor.h
 *  Algorithm to write beam-gas candidates to a tuple
 *
 *  @author Rosen Matev
 *  @date   2015-05-08
 */
class BeamGasVertexMonitor : public GaudiTupleAlg {
public:

  /// Standard constructor
  BeamGasVertexMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BeamGasVertexMonitor( ); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution

private:


  std::pair<double, BeamGasCandidates::const_iterator> matchCandidate(
    const BeamGasVertex& vertex, const BeamGasCandidates& candidates);

  BeamGasCandidates getRecoCandidates(const LHCb::RecVertices& splitVertices);
  BeamGasCandidates getMCCandidates(const SmartRefVector<LHCb::MCVertex>& mcVertices);
  BeamGasCandidates getHltCandidates(const LHCb::HltSelReports& selReports);


  bool fillOdin(const Tuple& tuple, const LHCb::ODIN* odin);
  bool fillVertex(const Tuple& tuple, const std::string& prefix, const BeamGasVertex& vertex, bool all);

  DeVelo* m_velo;

  // properties
  std::string m_reportLocation;
  std::vector<std::string> m_selNames;
  std::vector<std::string> m_svLocations;
  std::vector<std::string> m_svPrefixes;
  double m_maxMatchDeltaZ;

};
#endif // BEAMGASVERTEXMONITOR_H
