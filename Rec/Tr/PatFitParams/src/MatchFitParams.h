// $Id: MatchFitParams.h,v 1.1.1.1 2007/10/09 18:50:43 smenzeme Exp $
#ifndef MATCHFITPARAMS_H 
#define MATCHFITPARAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/Point3DTypes.h"

#include "Event/MCTrackInfo.h"

#include "FwdParameters.h"
#include "FitTool.h"

/** @class MatchFitParams MatchFitParams.h
 *  Parameterize the PatMatch tracks
 *
 *  Algorithm to calculate the different parameters in PatMatch, similar to FwdFitParams it tries to fit polynomial dependencies. Uses purely MC information from MCHits ie this will only run when they are available (XDIGI, XDST)
 *  Only Velo and T-station information is used.
 *  Some TT information is calculated, but not used at the moment.
 *
 *  - NTupleName: Name of the output nTuple
 *  - ZTT1: z-position of reference point in TT
 *  - ZRef: z-position of reference plane in T-stations
 *  - ZVelo: z-position of reference plane in Velo
 *  - zMagnetParams: Initial parameters for calculation of z of 'kink-position' of magnet
 *  - momentumParams: Initial paramters to calculate the momentum.
 *  - bendYParams: Initial paramters to calculate the bending in y.
 *  - MaxZVertex: Maximum z-position of PV to take tracks into account.
 *  - MinMomentum: Minimum momentum to take tracks into account.
 *  - zMatchY: z-position of matching in Y.
 *  - RequireTTHits: Require presence of TT hits for the calculation
 *  - MagnetScaleFactor: The scale factor for the magnet. Can be obtained with the field service, but hardcoded here such that it can be run without. 
 *  Note the only as many paramters are used for the caluclation as are given as input, although internally more might be defined.
 *  The momentum paramters are not directly set in 'PatMatchTool', but in 'FastMomentumEstimate' 
 *  Nothing else from the Brunel sequence is needed when running this code, a possible way to run it would be:
 *
 *  @code
 *  def doIt():
 *      GaudiSequencer("BrunelSequencer").Members =  [ MatchFitParams("MatchFitParams", ZRef = 8520, zMagnetParams = [ ... ], momentumParams = [ ... ], bendYParams = [ ... ]) ]
 * 
 *  appendPostConfigAction( doIt )
 *  @endcode
 *
 *
 *  @author Michel De Cian
 *  @date   2014-12-21
 */
class MatchFitParams : public GaudiTupleAlg {
public:
  /// Standard constructor
  MatchFitParams( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MatchFitParams( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  
  FitTool* m_fitTool;
  std::string m_tupleName;
  double      m_zTT1;
  double      m_zRef;
  double      m_zVelo;
  double      m_maxZVertex;
  double      m_minMomentum;
  double      m_zMatchY;
  bool        m_requireTTHits;
  double      m_magnetScaleFactor;

  std::vector<double> m_zMagParams;
  std::vector<double> m_momParams;
  std::vector<double> m_bendYParams;
  

  FwdParameters m_zMagPar;
  FwdParameters m_momPar;
  FwdParameters m_bendParamY;
  

  int m_nEvent;
  int m_nTrack;
};
#endif // MATCHFITPARAMS_H
