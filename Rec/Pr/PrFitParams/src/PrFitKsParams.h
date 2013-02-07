#ifndef KSFITPARAMS_H 
#define KSFITPARAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/Point3DTypes.h"

#include "PrFitParameters.h"
#include "PrFitTool.h"

/** @class PrFitKsParams PrFitKsParams.h
 *  Parameterize the KShort tracks
 *
 *  @author Olivier Callot
 *  @date   2012-07-03
 *  @modification on 2013-01-23  : Yasmine Amhis
 *  Adapt to work with Fiber Tracker and UT
 */
class PrFitKsParams : public GaudiTupleAlg {
public:
  /// Standard constructor
  PrFitKsParams( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrFitKsParams( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  PrFitTool*  m_fitTool;
  std::string m_tupleName;
  double      m_zTT1;
  double      m_zRef;

  std::vector<double> m_zMagParams;
  std::vector<double> m_momParams;

  PrFitParameters m_zMagPar;
  PrFitParameters m_momPar;

  int m_nEvent;
  int m_nTrack;
};
#endif // KSFITPARAMS_H
