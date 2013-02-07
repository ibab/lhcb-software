#ifndef SEEDFITPARAMS_H 
#define SEEDFITPARAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

#include "PrFitParameters.h"
#include "PrFitTool.h"

/** @class PrFitSeedParams PrFitSeedParams.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2012-07-03
 *  @modification on 2013-01-23  : Yasmine Amhis
 *  Adapt to work with Fiber Tracker and UT

 */
class PrFitSeedParams : public GaudiTupleAlg {
public: 
  /// Standard constructor
  PrFitSeedParams( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrFitSeedParams( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  PrFitTool*  m_fitTool;
  std::string m_tupleName;
  double      m_zRef;
  double      m_zSeed;
  double      m_zTT;
  int         m_nEvent;
  int         m_nTrack;

  std::vector<double> m_momentumScaleParams;
  std::vector<double> m_initialArrowParams;
  std::vector<double> m_zMagParams;

  PrFitParameters m_momentumScalePar;
  PrFitParameters m_initialArrowPar;
  PrFitParameters m_zMagPar;

  std::vector<double> m_dRatio;
  PrFitParameters m_dRatioPar;

  std::vector<double> m_yCorrection;
  PrFitParameters m_yCorrectionPar;
};
#endif // SEEDFITPARAMS_H
