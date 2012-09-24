// $Id: VeloPixCluster2MCHitLinker.h,v 1.1.1.1 2009-12-04 14:34:46 marcin Exp $
#ifndef VELOPIXCLUSTER2MCHITLINKER_H
#define VELOPIXCLUSTER2MCHITLINKER_H 1
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event
#include "Event/MCHit.h"
#include "Event/VeloPixDigit.h"

/** @class VeloPixCluster2MCHitLinker VeloPixCluster2MCHitLinker.h
 *  @Based on ST code 
 * 
 *  @author Marcin Kucharczyk
 *  @date   01/12/2009
 */

class VeloPixCluster2MCHitLinker : public GaudiAlgorithm {

      friend class AlgFactory<VeloPixCluster2MCHitLinker>;
  
public:
  /// Standard constructor
  VeloPixCluster2MCHitLinker(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VeloPixCluster2MCHitLinker(); ///< Destructor
  virtual StatusCode initialize();       ///< Algorithm initialization
  virtual StatusCode execute   ();       ///< Algorithm execution
  virtual StatusCode finalize  ();       ///< Algorithm finalise
  typedef std::pair<const LHCb::MCHit*,double> HitPair;
  typedef std::map<const LHCb::MCHit*,double> HitMap;
  // Path to put table
  std::string outputData() const;
 
protected:

private:

  void refsToRelate(std::vector<HitPair>& selectedRefs, 
                    const HitMap& hitMap,
                    LHCb::MCHits* hits) const;
  StatusCode associateToTruth(const LHCb::VeloPixCluster* aCluster,
                              HitMap& hitMap);
  LHCb::VeloPixDigits* m_digitCont;
  std::string m_inputData;
  std::string m_hitLocation;
  std::string m_asctLocation;
  std::string m_outputData;
  bool m_addSpillOverHits;
  double m_minFrac;
  bool m_oneRef;
  std::string m_digitLocation;
  double m_conversion;
  double m_scaleFactor;
};

inline std::string VeloPixCluster2MCHitLinker::outputData() const {
  return  m_outputData;
}

#endif // VELOPIXCLUSTER2MCHITLINKER_H
