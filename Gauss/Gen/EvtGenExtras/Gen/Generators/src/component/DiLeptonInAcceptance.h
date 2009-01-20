// $Id: DiLeptonInAcceptance.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
#ifndef GENERATORS_DILEPTONINACCEPTANCE_H 
#define GENERATORS_DILEPTONINACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "Generators/IFullGenEventCutTool.h"


/** @class DiLeptonInAcceptance DiLeptonInAcceptance.h "DiLeptonInAcceptance.h"
 *  component/DiLeptonInAcceptance.h
 *  
 *  Cut on events with two leptons in LHCb acceptance + minimum pT.
 *  and mass of lepton pair in sepcified range
 * 
 *  Implementation of IFullGenEventCutTool.
 *
 *  @author T Blake
 *  @date   2007-01-31
 */

class DiLeptonInAcceptance : public GaudiTool , 
                           virtual public IFullGenEventCutTool {
 public:
  /// Standard constructor
  DiLeptonInAcceptance( const std::string& type, const std::string& name,
			const IInterface* parent ) ;
  
  virtual ~DiLeptonInAcceptance( ); ///< Destructor
  
  /** Apply cut on full event.
   *  Keep events with a pair of leptons in angular region around
   *  z axis (forward) and with a minimum pT.
   *  Implements IFullGenEventCutTool::studyFullEvent.
   */
  virtual bool studyFullEvent( LHCb::HepMCEvents * theEvents ,
                               LHCb::GenCollisions * theCollisions ) const ;

  
  
  
 private:
  
  bool isPositiveLepton( const HepMC::GenParticle* ) const;
  bool isNegativeLepton( const HepMC::GenParticle* ) const;
  bool isInAcceptance( const HepMC::GenParticle* ) const;

  bool isCombination( const HepMC::GenParticle*, 
                      const HepMC::GenParticle* ) const;
  
  
  /// Maximum value for theta angle of lepton (set by options)
  double m_chargedThetaMax ;

  /// Minimum value for theta angle of lepton (set by options)
  double m_chargedThetaMin ;
  
  /// Minimum pT of lepton (set by options)
  double m_ptMin ;
  
  /// Di Lepton mass range - Lower (set by options)
  double m_minMass; 

  /// Di Lepton mass range - Upper (set by options)
  double m_maxMass; 

  /// Allowed lepton - Plus (set by options)
  int m_leptonMinusPDG ; 

  /// Allowed lepton - Minus (set by options)
  int m_leptonPlusPDG ;  

  /// Allow combinations with the Plus/Minus pdg id interchanged ( set by options, default = true )  
  bool m_ccFlag; 
  

};

#endif // GENERATORS_DILEPTONINACCEPTANCE_H
