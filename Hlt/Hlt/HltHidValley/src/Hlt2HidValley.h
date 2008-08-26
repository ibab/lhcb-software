#ifndef Hlt2HidValley_H
#define Hlt2HidValley_H 1
// Include files:
// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"

/** @class Hlt2HidValley Hlt2HidValley.h
 *
 *
 *  @author Marcin Kucharczyk
 *  @date   2008-08-21
 */

class Hlt2HidValley : public DVAlgorithm {
public:
  Hlt2HidValley(const std::string& name, ISvcLocator* pSvcLocator);
  // Destructor
  virtual ~Hlt2HidValley();
  // Algorithm initialization
  virtual StatusCode initialize();
  // Algorithm execution
  virtual StatusCode execute();
  // Algorithm finalization
  virtual StatusCode finalize();

private:
  std::string m_inputTracks;
  std::string m_inputPrimaryVertices;
  std::string m_inputDisplacedVertices;

};
#endif // Hlt2HidValley_H
