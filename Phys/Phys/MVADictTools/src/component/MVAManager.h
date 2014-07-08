#ifndef COMPONENT_MVAMANAGER_H 
#define COMPONENT_MVAMANAGER_H 1

// Include files 
#include <string>
#include <map>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from DaVinci
#include "Kernel/IParticleDictTool.h"


/** @class MVAManager MVAManager.h component/MVAManager.h
 *  This is a helper class, which bundles all components needed
 *  to setup a tool chain to create an MVA classifier
 *
 *  @author Sebastian Neubert
 *  @date   2013-08-06
 */
class MVAManager : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MVAManager( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MVAManager( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  std::string m_dictname;
  IParticleDictTool* m_dictofFunctors; ///< collecting several LoKi functors into one dictionary
  std::string m_transformname;
  IParticleDictTool* m_mva;            ///< DictTransform performing the MVA

 
  

private:

};
#endif // COMPONENT_MVAMANAGER_H
