#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackStateInitTool.h"

/** @class TrackStateInitAlg TrackStateInitAlg.h 
 *
 * Algorithm to initialize all tracks in container.
 * Uses the TrackStateInitTool.
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-02
 */

class TrackStateInitAlg : public GaudiAlgorithm
{
public:
  TrackStateInitAlg(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~TrackStateInitAlg();
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

private:
  bool clearStates;
  std::string m_trackLocation;
  ToolHandle<ITrackStateInit> m_trackTool;
} ;
