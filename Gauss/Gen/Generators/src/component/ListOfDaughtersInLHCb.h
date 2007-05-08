// $Id: ListOfDaughtersInLHCb.h,v 1.3 2007-05-08 13:42:00 robbep Exp $
#ifndef GENERATORS_LISTOFDAUGHTERSINLHCB_H 
#define GENERATORS_LISTOFDAUGHTERSINLHCB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IGenCutTool.h"
#include "Kernel/Transform4DTypes.h"

/** @class ListOfDaughtersInLHCb ListOfDaughtersInLHCb.h 
 *  
 *  Tool to keep events with several daughters from signal particles 
 *  in LHCb acceptance (AND of all daughters).
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class ListOfDaughtersInLHCb : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Ordered set of particle ID
  typedef std::set< int > PIDs ;
  
  /// Standard constructor
  ListOfDaughtersInLHCb( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);
  
  virtual ~ListOfDaughtersInLHCb( ); ///< Destructor

  /// Initialization
  virtual StatusCode initialize() ;

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

 private:
  /** Study a particle a returns true when all stable daughters
   *  are in LHCb acceptance
   */
  bool passCuts( const HepMC::GenParticle * theSignal ) const ;  

  /** Minimum value of angle around z-axis for charged daughters
   * (set by options) 
   */
  double m_chargedThetaMin ;

  /** Maximum value of angle around z-axis for charged daughters
   * (set by options) 
   */
  double m_chargedThetaMax ;

  /** Minimum value of angle around z-axis for neutral daughters
   * (set by options) 
   */
  double m_neutralThetaMin ;

  /** Maximum value of angle around z-axis for neutral daughters
   * (set by options) 
   */
  double m_neutralThetaMax ;

  /// PDG Id of particle to select  (set by options)
  PIDs m_daughtersPIDList ;

  /// temporary vector to read PIDs from options
  std::vector< int > m_pidVector ;
};
#endif // GENERATORS_LISTOFDAUGHTERSINLHCB_H
