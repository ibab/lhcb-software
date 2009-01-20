// $Id: SignalIsFromBDecay.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
#ifndef GENERATORS_SignalIsFromBDecay_H
#define GENERATORS_SignalIsFromBDecay_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IGenCutTool.h"

/** @class SignalIsFromBDecay SignalIsFromBDecay.h
 *
 *  Selects only signal which ultimately came from a B decay
 *
 *  Concrete implementation of IGenCutTool.
 *
 *  @author Chris Jones
 *  @date   22/11/2007
 */

class SignalIsFromBDecay : public GaudiTool,
                           virtual public IGenCutTool
{

public:

  /// Standard constructor
  SignalIsFromBDecay( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  virtual ~SignalIsFromBDecay( ); ///< Destructor

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector ,
                         const HepMC::GenEvent * theEvent ,
                         const LHCb::GenCollision * theCollision ) const;

private: // methods

  /// returns true if this particle is from a b
  bool isFromB( const HepMC::GenParticle * part,
                unsigned int tree_level = 1 ) const;

};

#endif // GENERATORS_DAUGHTERSINLHCBANDFROMB_H
