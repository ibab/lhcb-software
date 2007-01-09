// $Id: STCluster2MCHitLinker.h,v 1.5 2007-01-09 15:04:59 jvantilb Exp $
#ifndef STCLUSTER2MCHITLINKER_H
#define STCLUSTER2MCHITLINKER_H 1

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// Event
#include "Event/MCHit.h"
#include "Event/STDigit.h"

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
                          LHCb::MCHits* hits) const;

  StatusCode associateToTruth(const LHCb::STCluster* aCluster,
                              std::map<const LHCb::MCHit*,double>& hitMap);
 
  LHCb::STDigits* m_digitCont;

  std::string m_inputData;
  std::string m_hitLocation;
  std::string m_asctLocation; ///< Location of Digit2MCHit table 

  // job options
  std::string m_outputData;   ///< Location of the STClusters linker table
  bool m_addSpillOverHits;    ///< Flag to add spill-over to linker table
  double m_minFrac;           ///< Minimal charge fraction to link to MCParticle
  bool m_oneRef;              ///< Flag to allow only 1 link for each cluster
  std::string m_digitLocation;///< Location of the STDigits
  std::string m_detType;      ///< Detector type (IT or TT)
};

inline std::string STCluster2MCHitLinker::outputData() const {
  return  m_outputData;
}

#endif // STCLUSTER2MCHITLINKER_H
