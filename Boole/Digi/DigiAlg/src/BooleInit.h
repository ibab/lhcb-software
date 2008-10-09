// $Id: BooleInit.h,v 1.12 2008-10-09 06:28:36 cattanem Exp $
#ifndef BOOLEINIT_H 
#define BOOLEINIT_H 1

// Include files
#include "Kernel/LbAppInit.h"
#include "GaudiKernel/RndmGenerators.h"
#include <string>
#include <vector>

class IGenericTool;
class IEventTimeDecoder;

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
  virtual  void modifyOdin(unsigned int *odin);     ///< fills some Odin fields
  
private:
  IGenericTool*    m_memoryTool; ///< Pointer to (private) memory histogram tool
  IEventTimeDecoder* m_odinTool; ///< Pointer to odin creation tool

  bool m_modifyOdin;
  Rndm::Numbers m_FlatDist;
  std::string  m_genCollisionLocation ;
  std::vector<double> m_thresInteraction;
  std::vector<double> m_thresDiffractive;
  std::vector<double> m_thresElastic;
  double m_threstrigger;
};
#endif // BOOLEINIT_H
