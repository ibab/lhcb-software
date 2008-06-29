// $Id: SeedFitParams.h,v 1.2 2008-06-29 21:54:49 mschille Exp $
#ifndef SEEDFITPARAMS_H 
#define SEEDFITPARAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

#include "FwdParameters.h"
#include "FitTool.h"

/** @class SeedFitParams SeedFitParams.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2006-12-08
 */
class SeedFitParams : public GaudiTupleAlg {
public: 
  /// Standard constructor
  SeedFitParams( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SeedFitParams( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  FitTool* m_fitTool;
  std::string m_tupleName;
  double      m_zRef;
  double      m_zSeed;
  double      m_zTT;
  int         m_nEvent;
  int         m_nTrack;

  std::vector<double> m_momentumScaleParams;
  std::vector<double> m_initialArrowParams;
  std::vector<double> m_zMagParams;

  FwdParameters m_momentumScalePar;
  FwdParameters m_initialArrowPar;
  FwdParameters m_zMagPar;

  std::vector<double> m_dRatio;
  FwdParameters m_dRatioPar;

  std::vector<double> m_yCorrection;
  FwdParameters m_yCorrectionPar;
};
#endif // SEEDFITPARAMS_H
