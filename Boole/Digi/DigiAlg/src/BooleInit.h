// $Id: BooleInit.h,v 1.14 2010-05-05 09:33:14 albrecht Exp $
#ifndef BOOLEINIT_H 
#define BOOLEINIT_H 1

// Include files
#include "Kernel/LbAppInit.h"
#include "GaudiKernel/RndmGenerators.h"
#include <string>
#include <vector>
#include "Event/ODIN.h"

class IGenericTool;

/** @class BooleInit BooleInit.h
 *  First TopAlg for Boole. Initializes random number and fills memory histogram
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-15
 */
class BooleInit : public LbAppInit {
public: 
  /// Standard constructor
  BooleInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BooleInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  virtual  void modifyOdin(LHCb::ODIN* odin);     ///< fills some Odin fields
  virtual  void simpleOdin(LHCb::ODIN* odin);     ///< sets reasonable defaults for some Odin fields
  
private:
  IGenericTool* m_memoryTool; ///< Pointer to (private) memory histogram tool
  IGenericTool* m_odinTool;   ///< Pointer to odin encoding tool

  bool m_modifyOdin;
  Rndm::Numbers m_FlatDist;
  std::string  m_genCollisionLocation ;
  std::vector<double> m_thresInteraction;
  std::vector<double> m_thresDiffractive;
  std::vector<double> m_thresElastic;
  double m_threstrigger;
};
#endif // BOOLEINIT_H
