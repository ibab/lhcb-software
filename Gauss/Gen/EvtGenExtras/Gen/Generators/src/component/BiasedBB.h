// $Id: BiasedBB.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
#ifndef GENERATORS_BIASEDBB_H 
#define GENERATORS_BIASEDBB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IGenCutTool.h"

/** @class BiasedBB BiasedBB.h 
 *  
 *  Tool to keep events passing cuts defining the maximum biased BB sample
 *  for DC06. 
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class BiasedBB : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  BiasedBB( const std::string& type, const std::string& name,
            const IInterface* parent);
  
  virtual ~BiasedBB( ); ///< Destructor

  /** Accept events with a cut on eta, pT, v=pT+5.360v and ctau
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theGenEvent , 
                         const LHCb::GenCollision * theCollision ) const ;
  
 private:
  /// Maximum value of eta (set by options) 
  double m_etaMax ;
  /// Minimum value of eta (set by options) 
  double m_etaMin ;
  /// Minimum value of pT (set by options) 
  double m_ptMin ;
  /// Minimum value of v=pT+5.360eta (set by options) 
  double m_vMin ;
  /// Minimum value of ctau
  double m_ctauMin ;
};
#endif // GENERATORS_BIASEDBB_H
