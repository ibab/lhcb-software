// $Id: $
#ifndef VERTEXMONITOR_H 
#define VERTEXMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FastPVMonitor FastPVMonitor.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2011-11-22
 */
class FastPVMonitor : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FastPVMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FastPVMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  double m_minIPForTrack;
  double m_maxIPForTrack;
  int m_nEvent;
  int m_nVertices;
  int m_nLargeIP;

  double m_s0;
  double m_sx;
  double m_sy;
  double m_sz;
  double m_sx2;
  double m_sy2;
  double m_sz2;
};
#endif // VERTEXMONITOR_H
