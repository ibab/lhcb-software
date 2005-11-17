// $Id: LHCbAcceptance.h,v 1.2 2005-11-17 15:56:46 robbep Exp $
#ifndef GENERATORS_LHCBACCEPTANCE_H 
#define GENERATORS_LHCBACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IGenCutTool.h"

/** @class LHCbAcceptance LHCbAcceptance.h 
 *  
 *  Utility tool to keep events with particles only in LHCb acceptance
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class LHCbAcceptance : public GaudiTool, virtual public IGenCutTool {
public:
  /// Standard constructor
  LHCbAcceptance( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  virtual ~LHCbAcceptance( ); ///< Destructor

  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const HardInfo * theHardInfo ) const ;

protected:

private:
  double m_thetaMax ;
};
#endif // GENERATORS_LHCBACCEPTANCE_H
