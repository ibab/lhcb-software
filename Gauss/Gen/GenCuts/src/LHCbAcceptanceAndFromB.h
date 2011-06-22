// $Id: LHCbAcceptanceAndFromB.h,v 1.1 2007-11-26 13:44:34 jonrob Exp $
#ifndef GENCUTS_LHCBACCEPTANCEANDFROMB_H
#define GENCUTS_LHCBACCEPTANCEANDFROMB_H 1

#include "LHCbAcceptance.h"

/** @class LHCbAcceptanceAndFromB LHCbAcceptanceAndFromB.h
 *
 *  Inherits from LHCbAcceptance adding the additional requirement
 *  the the signal be ultimately from a b decay
 *
 *  @author Chris Jones
 *  @date   22/11/2007
 */

class LHCbAcceptanceAndFromB : public LHCbAcceptance,
                               virtual public IGenCutTool
{

public:

  /// Standard constructor
  LHCbAcceptanceAndFromB( const std::string& type,
                          const std::string& name,
                          const IInterface* parent );

  virtual ~LHCbAcceptanceAndFromB( ); ///< Destructor

  /// Initialization
  virtual StatusCode initialize();

  /** Accept events in LHCb acceptance (defined by angle)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector ,
                         const HepMC::GenEvent * thGeneEvent ,
                         const LHCb::GenCollision * theCollision ) const ;

private: // data

  /// From a b cut tool
  const IGenCutTool * m_fromBcuts;

};

#endif // GENCUTS_LHCBACCEPTANCEANDFROMB_H
