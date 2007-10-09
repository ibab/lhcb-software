// $Id: KsFitParams.h,v 1.1.1.1 2007-10-09 18:50:43 smenzeme Exp $
#ifndef KSFITPARAMS_H 
#define KSFITPARAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/Point3DTypes.h"

#include "FwdParameters.h"
#include "FitTool.h"

/** @class KsFitParams KsFitParams.h
 *  PArameterize the KShort tracks
 *
 *  @author Olivier Callot
 *  @date   2002-11-02
 */
class KsFitParams : public GaudiTupleAlg {
public:
  /// Standard constructor
  KsFitParams( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~KsFitParams( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  FitTool* m_fitTool;
  std::string m_tupleName;
  double      m_zTT1;
  double      m_zRef;

  std::vector<double> m_zMagParams;
  std::vector<double> m_momParams;

  FwdParameters m_zMagPar;
  FwdParameters m_momPar;

  int m_nEvent;
  int m_nTrack;
};
#endif // KSFITPARAMS_H
