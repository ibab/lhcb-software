// $Id $
#ifndef _LandauTest_H
#define _LandauTest_H


#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

#include <string>

/** class LandauTest, package ITCheckers
 *  Algorithm for checking Landau generator behave as they should
 *
 *  @author M.Needham
 *  @date   21/10/2000
*/

class LandauTest: public GaudiHistoAlg {

public:

  /// Constructer
  LandauTest(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructer
  ~LandauTest();

  /// intialize
  virtual StatusCode initialize();

  /// execute
  virtual StatusCode execute();


private:


  SmartIF<IRndmGen> m_landauDist;

};
 
#endif // LandauTest_H









