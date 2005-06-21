// $Id: HiggsPythiaAlg.h,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
#ifndef LBPYTHIA_HIGGSPYTHIAALG_H 
#define LBPYTHIA_HIGGSPYTHIAALG_H 1

// Include files
// from STL
#include <string>

#include "LbPythia/PythiaAlg.h"

class ParticleProperty ;


/** @class HiggsPythiaAlg HiggsPythiaAlg.h
 *  
 *  class to generate Higgs events with Pythia (mainly reimplementation
 *  of selectEvt function to filter at generator level interesting
 *  events)
 *
 *  @author Patrick Robbe
 *  @date   2004-12-16
 */
class HiggsPythiaAlg : public PythiaAlg {
public:
  /// Standard constructor
  HiggsPythiaAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HiggsPythiaAlg( ); ///< Destructor
  
  /** Initialization
   *  Obtain from particle property service the Id of the 
   *  particles to request in acceptance
   *  @return StatusCode
   */
  virtual StatusCode genInitialize( ) ;

  /** Fill HepMC::GenEvent from HEPEVT Common block
   *  @return StatusCode
   *  @param[out] evt the converted HepMC::GenEvent
   */
  virtual StatusCode fillEvt( HepMC::GenEvent * evt ) ;

  /** Analyse the event 
   *  It is the only function modified with respect to the PythiaAlg
   *  algorithm, to be able to select and process only events with 
   *  b, bbar quarks and letpon in the LHCb acceptance
   *  @retval true if the HepMC::GenEvent pass the generator level cuts
   *  @param[in] evt The HepMC::Event to study
   */
  virtual bool selectEvt( HepMC::GenEvent * evt ) ;

protected:

private:
  /// flag to request or not lepton from W or b quarks in acceptance
  bool m_useLepton , m_usebb ; 
  ParticleProperty * m_HiggsId , * m_WpId , * m_WmId , * m_bId , * m_bbarId , 
    * m_epId , * m_emId , * m_mupId , * m_mumId , *m_Z0Id ;
};
#endif // LBPYTHIA_HIGGSPYTHIAALG_H
