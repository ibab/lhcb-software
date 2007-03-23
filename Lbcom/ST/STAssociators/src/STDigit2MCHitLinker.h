// $Id: STDigit2MCHitLinker.h,v 1.4 2007-03-23 08:59:13 jvantilb Exp $
#ifndef STDIGIT2MCHITLINKER_H
#define STDIGIT2MCHITLINKER_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MCHit.h"

/** @class STDigit2MCHitLinker STDigit2MCHitLinker.h
 *  
 *
 *  @author Matt Needham
 *  @date   26/04/2002
 */
class STDigit2MCHitLinker : public GaudiAlgorithm {

  friend class AlgFactory<STDigit2MCHitLinker>;
  
public:

  typedef std::pair<const LHCb::MCHit*,double> HitPair;
  typedef std::map<const LHCb::MCHit*,double> HitMap;

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

  double totalCharge( const HitMap& hitMap ) const;
  void refsToRelate( std::vector<HitPair>& selectedRefs, const HitMap& hitMap,
                     const double& totCharge, LHCb::MCHits* hits ) const;

  std::string m_hitLocation;

  // job options
  std::string m_outputData;   ///< Location of the STDigits linker table
  std::string m_inputData;    ///< Location of the STDigits
  bool m_addSpillOverHits;    ///< Flag to add spill-over to linker table
  double m_minFrac;           ///< Minimal charge fraction to link to MCParticle
  bool m_oneRef;              ///< Flag to allow only 1 link for each digit
  std::string m_detType;      ///< Detector type (IT or TT)
};

inline std::string STDigit2MCHitLinker::outputData() const {
  return m_outputData;
}

#endif // STDIGIT2MCHITLINKER_H
