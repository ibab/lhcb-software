// $Id: CameraTestAlg.h,v 1.1.1.1 2009-02-02 11:45:34 rogers Exp $
#ifndef CAMERATESTALG_H 
#define CAMERATESTALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


// Forward declarations
class ICameraTool;
class TF1;
class TH1;
class TH2;

/** @class CameraTestAlg CameraTestAlg.h
 *  
 *
 *  @author Claus Buszello
 *  @date   2008-02-22
 */
class CameraTestAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CameraTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CameraTestAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  int m_evt;
  ICameraTool * cam;
  
  TH1D * Hflat;
  TH1D * Hangled;
  
  TH2D * H2d;
  TH2D * H2d2;

  TF1 * fflat;

  

};
#endif // CAMERATESTALG_H
