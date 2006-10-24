// $Id: CheatedSelection.h,v 1.2 2006-10-24 10:21:06 jpalac Exp $
#ifndef USER_CheatedSelection_H 
#define USER_CheatedSelection_H 1

// from Event 
#include "Event/SelResult.h"
#include "Event/GenHeader.h"

//#include "GenEvent/HepMCEvent.h"

// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IDebugTool.h"

// from Associators
#include "DaVinciAssociators/Particle2MCLinker.h"

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
  void SignalTree( const LHCb::Particle::ConstVector& ,
		   const LHCb::MCParticle*, LHCb::MCParticle::Vector& , 
		   LHCb::Particle::ConstVector& );

  //StatusCode setDecayToFind( const int evtCode );
  
  double m_BHEPm;
  IDebugTool* m_debug;

  Particle2MCLinker* m_linkLinks; ///< Pointer to associator using links
  std::vector<std::string> m_setInputData;

  //properties
  double m_BMassWindow;

  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* );

/*   bool            m_fromData;        ///< flag read event code from data */
/*   int             m_evtCode;         ///< event code to test */
/*   bool            m_setDecay;        ///< Flag is decay has been set */
/*   IEvtTypeSvc*    m_evtTypeSvc;      ///< Pointer to service */
/*   IMCDecayFinder* m_mcFinder;        ///< Pointer to tool */

};
//===========================================================================//
#endif // USER_CheatedSelection_H
