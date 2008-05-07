// $Id: MCTruthFullMonitor.h,v 1.3 2008-05-07 09:54:20 gcorti Exp $
#ifndef MCTRUTHMONITOR_H 
#define MCTRUTHMONITOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/NTuple.h"

/** @class MCTruthFullMonitor MCTruthFullMonitor.h
 *  Prepare some histograms and ntuple for checking 
 *  MCParticles and MCVertices
 *
 *  @author Gloria CORTI
 *  @date   2004-01-27
 */
class MCTruthFullMonitor : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MCTruthFullMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCTruthFullMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  NTuple::Tuple*        m_ntuple;
  NTuple::Item<long>    m_pType;
  NTuple::Item<float>   m_pxOvtx, m_pyOvtx, m_pzOvtx;
  NTuple::Item<long>    m_typeOvtx;
  NTuple::Item<float>   m_xOvtx, m_yOvtx, m_zOvtx, m_tOvtx;
  NTuple::Item<long>    m_parent;
  NTuple::Item<long>    m_daughVtx, m_daughPart;

  double  m_zVolMin;
  double  m_zVolMax;
  
};
#endif // MCTRUTHMONITOR_H
