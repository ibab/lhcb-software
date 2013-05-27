#ifndef PRFITFWDPARAMS_H 
#define PRFITFWDPARAMS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

#include "PrFitParameters.h"

/** @class PrFitFwdParams PrFitFwdParams.h
 *  Analyse the MC to produce Ltracking parameters
 *
 *  @author Olivier Callot
 *  @date   2005-07-01
 *  @modification on 2013-01-23  : Yasmine Amhis
 *  Adapt to work with Fiber Tracker and VP or VL
 */
class PrFitFwdParams : public GaudiTupleAlg {
public:
  /// Standard constructor
  PrFitFwdParams( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrFitFwdParams( ); ///< Destructor

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
  //  double m_zRefY;                     ///< idem for the Y view
  double m_zRefExt;                   ///< Z to define the state for the Muon
  double m_zRefM;                     ///< Positikon of the Muon test point, i.e. M1
  bool   m_veloFromMC;
  bool   m_useVeloPix ; 
  std::vector<double> m_zMagParams;
  std::vector<double> m_xParams;
  std::vector<double> m_byParams;
  std::vector<double> m_cyParams;
  std::vector<double> m_momParams;
  std::vector<double> m_XsParams;
  
  std::string m_tupleName;
  std::string m_tupleName2;

  double m_zBeforeST2;            ///< Separation between ST1 and ST2
  double m_zBeforeST3;            ///< Separation between ST2 and ST3

  // For function parameter computation...


  PrFitParameters m_ZmPar;
  PrFitParameters m_xPar;
  PrFitParameters m_byPar;
  PrFitParameters m_cyPar;
  PrFitParameters m_momPar;
  PrFitParameters m_XsPar;
  
  // Local variables and counters

  int m_count[3];

  std::string m_veloTracksLocation;

};
#endif // PRFITFWDPARAMS_H
