// $Id: STCluster2MCHitLinker.h,v 1.7 2009-01-16 08:39:37 mneedham Exp $
#ifndef STCLUSTER2MCHITLINKER_H
#define STCLUSTER2MCHITLINKER_H 1

// Gaudi
#include "Kernel/STAlgBase.h"

// Event
#include "Event/MCHit.h"
#include "Event/STDigit.h"

/** @class STCluster2MCHitAlg STCluster2MCHitLinker.h
 *  
 *
 *  @author Matt Needham
 *  @date   26/04/2002
 */

class STCluster2MCHitLinker : public ST::AlgBase {

  friend class AlgFactory<STCluster2MCHitLinker>;
  
public:

  typedef std::pair<const LHCb::MCHit*,double> HitPair;
  typedef std::map<const LHCb::MCHit*,double> HitMap;

  /// Standard constructor
  STCluster2MCHitLinker( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~STCluster2MCHitLinker(); 

  /// execute
  virtual StatusCode execute();    

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  void refsToRelate(std::vector<HitPair>& selectedRefs, const HitMap& hitMap,
                    LHCb::MCHits* hits) const;

  StatusCode associateToTruth(const LHCb::STCluster* aCluster, HitMap& hitMap);
 
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

};

inline std::string STCluster2MCHitLinker::outputData() const {
  return  m_outputData;
}

#endif // STCLUSTER2MCHITLINKER_H
