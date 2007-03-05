// $Id: CheatedSelection.h,v 1.5 2007-03-05 11:44:34 pkoppenb Exp $
#ifndef USER_CheatedSelection_H 
#define USER_CheatedSelection_H 1

// from Event 
#include "Event/SelResult.h"
#include "Event/GenHeader.h"

// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IDebugTool.h"

// from Associators
#include "Kernel/Particle2MCLinker.h"

// from local
//#include "IForcedBDecayTool.h"

/** @class CheatedSelection CheatedSelection.h 
 *  
 *  Select events looking at the MC truth
 *
 *  @author Marco Musy
 *  @date   16/11/2004
 */

class CheatedSelection : public DVAlgorithm {

 public: 
  /// Standard constructor
  CheatedSelection( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheatedSelection( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 private:
  const LHCb::MCParticle* originof( const LHCb::MCParticle* ) ;
  void SignalTree( const LHCb::MCParticle*, 
		   LHCb::MCParticle::Vector& , 
		   LHCb::Particle::ConstVector& );

  IDebugTool* m_debug;
  Particle2MCLinker* m_linker; ///< Pointer to associator using links

  //IForcedBDecayTool* m_forcedBtool;
  const LHCb::MCParticle* forcedB(void);
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* ) ;

  std::vector<std::string> m_setInputData;

  //properties
  double m_BMassWindow;

};
//===========================================================================//
#endif // USER_CheatedSelection_H
