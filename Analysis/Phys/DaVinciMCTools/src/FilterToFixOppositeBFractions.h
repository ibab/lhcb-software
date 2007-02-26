// $Id: FilterToFixOppositeBFractions.h,v 1.1 2007-02-26 09:46:43 sposs Exp $
#ifndef FILTERTOFIXOPPOSITEBFRACTIONS_H 
#define FILTERTOFIXOPPOSITEBFRACTIONS_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

#include "Event/GenHeader.h"
#include "Event/RecHeader.h"
#include "Event/MCParticle.h"
/** @class FilterToFixOppositeBFractions FilterToFixOppositeBFractions.h
 *   
 *
 *  @author Stephane Poss
 *  @date   2007-02-22
 */
class FilterToFixOppositeBFractions : public DVAlgorithm {
public: 
  /// Standard constructor
  FilterToFixOppositeBFractions( const std::string& name, 
                                 ISvcLocator* pSvcLocator );

  virtual ~FilterToFixOppositeBFractions( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* );

  float percent(int , float );
  float errorp(int , float);
  float r(float);
  

  //cout opposite fraction
  int m_coutSameB,m_coutBd,m_coutBs,m_coutBu,m_coutOt,m_rejected;
  int m_coutBdAC,m_coutBsAC,m_coutBuAC,m_coutOtAC;
  
  float  m_coutevt,m_coutevtAC;
  float  m_BsBC,m_BdBC,m_BuBC,m_OtBC;
  float  m_BsAC,m_BdAC,m_BuAC,m_OtAC;
  float  m_errBsBC,m_errBdBC,m_errBuBC,m_errOtBC;
  float  m_errBsAC,m_errBdAC,m_errBuAC,m_errOtAC;
  
  bool   m_evttype;
  int    m_ievt;
  bool   m_print;
  bool   m_printevt;
  int    m_et[56];
  float  m_gx[56];
  float  m_fx[56];
  
};
#endif // FILTERTOFIXOPPOSITEBFRACTIONS_H
