// $Id: PythiaHiggs.h,v 1.2 2005-12-31 17:35:36 robbep Exp $
#ifndef LBPYTHIA_PYTHIAHIGGS_H 
#define LBPYTHIA_PYTHIAHIGGS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "Generators/IGenCutTool.h"

/** @class PythiaHiggs PythiaHiggs.h component/PythiaHiggs.h
 *  
 *  Tool to select events with 2 b quarks from Higgs in acceptance
 *
 *  @author Patrick Robbe
 *  @date   2005-11-21
 */
class PythiaHiggs : public GaudiTool , virtual public IGenCutTool {
 public:
  /// Standard constructor
  PythiaHiggs( const std::string & type , const std::string & name ,
               const IInterface * parent ) ;
  
  virtual ~PythiaHiggs( ); ///< Destructor

  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent ,
                         const LHCb::GenCollision * theCollision ) const ;


 private:
  double m_thetaMax ;
};
#endif // COMPONENT_PYTHIAHIGGS_H
