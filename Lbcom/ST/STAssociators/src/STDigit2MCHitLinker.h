// $Id: STDigit2MCHitLinker.h,v 1.1.1.1 2005-12-19 15:42:42 mneedham Exp $
#ifndef STDIGIT2MCHITLINKER_H
#define STDIGIT2MCHITLINKER_H 1

#include <string>
#include <map>
#include <utility>
#include <vector>

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MCHit.h"

class ISTDigitSelector;

/** @class STDigit2MCHitLinker STDigit2MCHitLinker.h
 *  
 *
 *  @author Matt Needham
 *  @date   26/04/2002
 */
class STDigit2MCHitLinker : public GaudiAlgorithm {

  friend class AlgFactory<STDigit2MCHitLinker>;
  
public:

  typedef std::pair<const LHCb::MCHit*,double> hitPair;

  /// Standard constructor
  STDigit2MCHitLinker( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~STDigit2MCHitLinker(); 

  /// initialize
  virtual StatusCode initialize(); 

  /// execute
  virtual StatusCode execute();    

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  double totalCharge(const std::map<const LHCb::MCHit*,double>& hitMap) const;
  StatusCode refsToRelate(std::vector<hitPair>& selectedRefs,
                          const std::map<const LHCb::MCHit*,double>& hitMap,
                          const double& totCharge,
                          LHCb::MCHits* hits) const;
  
  std::string m_outputData;
  std::string m_inputData;
  bool m_addSpillOverHits;
  double m_minFrac;  
  bool m_oneRef;
 
  std::string m_hitLocation;
  std::string m_detType;
  std::string m_selectorName;
  ISTDigitSelector* m_selector;
};

inline std::string STDigit2MCHitLinker::outputData() const {
  return m_outputData;
}

#endif // STDIGIT2MCHITLINKER_H
