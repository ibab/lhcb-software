#ifndef FWDFITPARAMS_H 
#define FWDFITPARAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

#include "FwdParameters.h"

/** @class FwdFitParams FwdFitParams.h
 *  Analyse the MC to produce Ltracking parameters
 *
 *  @author Olivier Callot
 *  @date   2005-07-01
 */
class FwdFitParams : public GaudiTupleAlg {
public:
  /// Standard constructor
  FwdFitParams( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FwdFitParams( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  bool solveMatrix( double m[10][10], double d[10] );

protected:

private:
  // Parameters to control the algorithm, accessible by jobOptions

  double m_minMomentum;               ///< Momentum cut to consider tracks
  double m_maxZVertex;                ///< Maximum Z vertex.
  double m_zVelo;                     ///< Z where initial track is defined
  double m_zMag;                      ///< Z of the field center
  double m_zRef;                      ///< Z of a reference plane.
  double m_zRefExt;                   ///< Z to define the state for the Muon
  double m_zRefM;                     ///< Position of the Muon test point, i.e. M1
  bool   m_veloFromMC;

  std::vector<double> m_zMagParams;
  std::vector<double> m_xParams;
  std::vector<double> m_yParams;
  std::vector<double> m_y1Params;

  std::vector<double> m_momParams;
  std::vector<double> m_XsParams;
  
  std::string m_tupleName;
  std::string m_tupleName2;

  double m_zBeforeST2;            ///< Separation between ST1 and ST2
  double m_zBeforeST3;            ///< Separation between ST2 and ST3

  // For function parameter computation...


  FwdParameters m_ZmPar;
  FwdParameters m_xPar;
  FwdParameters m_yPar;
  FwdParameters m_y1Par;
  FwdParameters m_y2Par;

  FwdParameters m_momPar;
  FwdParameters m_XsPar;
  
  // Local variables and counters

  int m_count[3];

  std::string m_veloTracksLocation;

};
#endif // FWDFITPARAMS_H
