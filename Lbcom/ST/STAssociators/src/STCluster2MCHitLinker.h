// $Id: STCluster2MCHitLinker.h,v 1.1.1.1 2005-12-19 15:42:42 mneedham Exp $
#ifndef STCLUSTER2MCHITALG_H
#define STCLUSTER2MCHITALG_H 1

#include <string>
#include <map>
#include <utility>
#include <vector>

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

//Event
#include "Event/MCHit.h"

/** @class STCluster2MCHitAlg STCluster2MCHitLinker.h
 *  
 *
 *  @author Matt Needham
 *  @date   26/04/2002
 */

class STCluster2MCHitLinker : public GaudiAlgorithm {

  friend class AlgFactory<STCluster2MCHitLinker>;
  
public:

  typedef std::pair<const LHCb::MCHit*,double> hitPair;

  /// Standard constructor
  STCluster2MCHitLinker( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~STCluster2MCHitLinker(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  StatusCode refsToRelate(std::vector<hitPair>& selectedRefs,
                          const std::map<const LHCb::MCHit*,double>& hitMap,
                          const double& totCharge,
                          LHCb::MCHits* hits) const;

  StatusCode associateToTruth(const LHCb::STCluster* aCluster,
                              std::map<const LHCb::MCHit*,double>& hitMap);

 
  bool m_addSpillOverHits;
  double m_minFrac;  
  bool m_oneRef;

  std::string m_inputData;
  std::string m_outputData;
  std::string m_detType;
  std::string m_digitLocation;
  std::string m_hitLocation;

};

inline std::string STCluster2MCHitLinker::outputData() const {
  return  m_outputData;
}


#endif // STCLUSTER2MCHITALG_H







