// $Id: CheatedSelection.h,v 1.10 2009-11-30 22:46:24 musy Exp $
#ifndef USER_CheatedSelection_H 
#define USER_CheatedSelection_H 1

// from Event 
#include "Event/SelResult.h"
#include "Event/GenHeader.h"

// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"
#include "MCInterfaces/IForcedBDecayTool.h"

// from Associators
#include "Kernel/Particle2MCLinker.h"
 
// from local

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

  IPrintMCDecayTreeTool* m_debug;
  Particle2MCLinker* m_linker; ///< Pointer to associator using links

  IForcedBDecayTool* m_forcedBtool;
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* ) ;
  
  std::string m_inputLocation;
  std::vector<std::string> m_setInputData;

  //properties
  double m_GenBMassWindow, m_RecBMassWindow, m_tolerance;

};
//===========================================================================//
#endif // USER_CheatedSelection_H
