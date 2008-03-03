// $Id: FilterToFixOppositeBFractions.h,v 1.3 2008-03-03 17:14:30 pkoppenb Exp $
#ifndef FILTERTOFIXOPPOSITEBFRACTIONS_H 
#define FILTERTOFIXOPPOSITEBFRACTIONS_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/GenHeader.h"
#include "Event/RecHeader.h"
#include "Event/MCParticle.h"
/** @class FilterToFixOppositeBFractions FilterToFixOppositeBFractions.h
 *   
 * This Filter is created to fix the Bug2 observed on DC06 data
 * and reported by Patrick Robbe on the 13 feb 2007.
 * This bug affects the B composition of the opposite side with respect to the
 * signal B. This filter will remove specific fraction of events where 
 * abs(BsigID) equal abs(BoppoID) depending on events type.
 * It checks for signal B, using HepMC, looks for opposite B using the MC
 * originVertex() method. The case with more than 2 B's coming from the same
 * vertex is not handled properly for the time being.
 * To use it, create a sequence with all the algorithms 
 * that have to be processed 
 * each events. The FilterToFixOppositeBFractions has to be the first in the
 * list. Example :
 * @code
 * ApplicationMgr.TopAlg += { "GaudiSequencer/MySeq" };
 * MySeq.Members += { "FilterToFixOppositeBFractions" };
 * FilterToFixOppositeBFractions.ActivateCorrection = true;
 * @endcode 
 *
 *  @author Stephane Poss
 *  @date   2007-02-22
 */
class FilterToFixOppositeBFractions : public GaudiAlgorithm {
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

  double percent(int , double );
  double errorp(int , double);
  double r(double);
  

  //cout opposite fraction
  int m_coutSameB,m_coutBd,m_coutBs,m_coutBu,m_coutOt,m_rejected;
  int m_coutBdAC,m_coutBsAC,m_coutBuAC,m_coutOtAC;
  
  double  m_coutevt,m_coutevtAC;
  double  m_BsBC,m_BdBC,m_BuBC,m_OtBC;
  double  m_BsAC,m_BdAC,m_BuAC,m_OtAC;
  double  m_errBsBC,m_errBdBC,m_errBuBC,m_errOtBC;
  double  m_errBsAC,m_errBdAC,m_errBuAC,m_errOtAC;
  
  bool   m_activate;
  int    m_ievt;
  bool   m_print;
  bool   m_printevt;
  std::map< int, std::pair<double,double> > m_evtGxFx;
  
};
#endif // FILTERTOFIXOPPOSITEBFRACTIONS_H
