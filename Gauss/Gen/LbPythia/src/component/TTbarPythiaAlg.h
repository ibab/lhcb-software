// $Id: TTbarPythiaAlg.h,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
#ifndef LBPYTHIA_TTBARPYTHIAALG_H 
#define LBPYTHIA_TTBARPYTHIAALG_H 1

// Include files
// from STL
#include <string>

#include "LbPythia/PythiaAlg.h"

class ParticleProperty ;


/** @class TTbarPythiaAlg TTbarPythiaAlg.h
 *  
 *  class to generate t, tbar events with Pythia (mainly reimplementation
 *  of selectEvt function to filter at generator level interesting
 *  events)
 *
 *  @author Patrick Robbe
 *  @date   2004-12-16
 */
class TTbarPythiaAlg : public PythiaAlg {
public:
  /// Standard constructor
  TTbarPythiaAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TTbarPythiaAlg( ); ///< Destructor
  
  /** Initialization
   *  Obtain from particle property service the Id of the 
   *  particles to request in acceptance
   *  @return StatusCode
   */
  StatusCode genInitialize( ) ;

  /** Fill HepMC::GenEvent from HEPEVT Common block
   *  @return StatusCode
   *  @param[out] evt the converted HepMC::GenEvent
   */
  StatusCode fillEvt( HepMC::GenEvent * evt ) ;

  /** Analyse the event 
   *  It is the only function modified with respect to the PythiaAlg
   *  algorithm, to be able to select and process only events with 
   *  b and bbar quarks in the LHCb acceptance
   *  @retval true if the HepMC::GenEvent pass the generator level cuts
   *  @param[in] evt The HepMC::Event to study
   */
  virtual bool selectEvt( HepMC::GenEvent * evt ) ;

protected:

private:
  ParticleProperty * m_bId , * m_bbarId , * m_tId, * m_tbarId ;
};
#endif // LBPYTHIA_TTBARPYTHIAALG_H
