// $Id: DaughtersInLHCbAndFromB.h,v 1.1 2007-11-26 13:44:34 jonrob Exp $
#ifndef GENCUTS_DAUGHTERSINLHCBANDFROMB_H
#define GENCUTS_DAUGHTERSINLHCBANDFROMB_H 1

// Include files
#include "DaughtersInLHCb.h"

/** @class DaughtersInLHCbAndFromB DaughtersInLHCbAndFromB.h
 *
 *  Inherits from DaughtersInLHCb adding the additional requirement that
 *  the signal be from a b decay.
 *
 *  Concrete implementation of IGenCutTool.
 *
 *  @author Chris Jones
 *  @date   22/11/2007
 */

class DaughtersInLHCbAndFromB : public DaughtersInLHCb,
                                virtual public IGenCutTool
{

public:

  /// Standard constructor
  DaughtersInLHCbAndFromB( const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  virtual ~DaughtersInLHCbAndFromB( ); ///< Destructor

  /// Initialization
  virtual StatusCode initialize();

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector ,
                         const HepMC::GenEvent * theEvent ,
                         const LHCb::GenCollision * theCollision ) const;

private: // data

  /// From a b cut tool
  const IGenCutTool * m_fromBcuts;

};

#endif // GENCUTS_DAUGHTERSINLHCBANDFROMB_H
