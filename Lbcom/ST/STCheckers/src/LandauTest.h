// $Id: LandauTest.h,v 1.2 2006-12-21 17:54:48 jvantilb Exp $
#ifndef LandauTest_H
#define LandauTest_H 1

#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

/** class LandauTest, package ITCheckers
 *  Algorithm for checking Landau generator behave as they should
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   21/12/2006
 */

class LandauTest: public GaudiHistoAlg {

public:

  /// Constructer
  LandauTest(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructer
  ~LandauTest() = default;

  /// intialize
  virtual StatusCode initialize();

  virtual StatusCode execute();

private:

  SmartIF<IRndmGen> m_landauDist;

};
 
#endif // LandauTest_H









