#ifndef _TrackComputeExpectedHits_H_
#define _TrackComputeExpectedHits_H_

/** @class TrackComputeExpectedHits TrackComputeExpectedHits.h
 *
 *
 *  @author S. Hansmann-Menzemer
 *  @date   20.07.2009
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>
#include <vector>
#include <map>

class IHitExpectation;
class IVeloExpectation;
class DeOTDetector;

class TrackComputeExpectedHits: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackComputeExpectedHits(const std::string& name,
			   ISvcLocator* pSvcLocator);
  virtual ~TrackComputeExpectedHits();

  virtual StatusCode initialize();

  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  

  IVeloExpectation* m_veloExpectation;
  IHitExpectation* m_ttExpectation;
  IHitExpectation* m_itExpectation;
  IHitExpectation* m_otExpectation;

  DeOTDetector* m_otDet;

 
};

#endif
