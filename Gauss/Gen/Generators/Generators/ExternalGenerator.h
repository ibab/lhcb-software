// $Id: ExternalGenerator.h,v 1.3 2005-11-29 15:44:11 robbep Exp $
#ifndef GENERATORS_EXTERNALGENERATOR_H 
#define GENERATORS_EXTERNALGENERATOR_H 1

// Include files
#include <set>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ISampleGenerationTool.h"

class IProductionTool ;
class IDecayTool ;
class IGenCutTool ;
namespace HepMC { 
  class GenEvent ; 
  class GenParticle ;
}

/** @class ExternalGenerator ExternalGenerator.h 
 *         Generators/ExternalGenerator.h
 *  
 *  Abstract interface to methods using external generators
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */

class ExternalGenerator : public GaudiTool , 
                          public ISampleGenerationTool {
public:
  typedef std::vector< HepMC::GenParticle * > ParticleVector ;
  typedef std::set< int >                     PIDs           ;

  /// Standard constructor
  ExternalGenerator( const std::string & type , 
                     const std::string & name ,
                     const IInterface * parent ) ;

  virtual ~ExternalGenerator( ) ; ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;

protected:
  /// Decay heavy excited particles
  StatusCode decayHeavyParticles( HepMC::GenEvent * theEvent , 
                                  const double mass , const int pid )
    const ;

  /// Check the presence of a particle of given type in event
  bool checkPresence( const PIDs & pidList , 
                             const HepMC::GenEvent * theEvent ,
                             ParticleVector & particleList ) ;

  /// invert the event
  void revertEvent( HepMC::GenEvent * theEvent ) ;

  /// count the number of particles with pz > 0
  unsigned int nPositivePz( const ParticleVector & particleList ) ;

  void prepareInteraction( EventVector & theEventVector , 
                           HardVector & theHardVector , 
                           HepMC::GenEvent * & theGenEvent ,
                           HardInfo * & theHardInfo ) const ;

  IProductionTool * m_productionTool ;
  IDecayTool      * m_decayTool      ;
  IGenCutTool     * m_cutTool        ;

  inline double fraction( const unsigned int A , const unsigned int B ) const
  { return ( (double) A / (double) B ) ; }
  inline double err_fraction( const unsigned int A , const unsigned int B ) 
    const 
  { return sqrt( A * ( 1. - ( (double) A / (double) B ) ) ) / 
      ( (double) B ) ; }

private:
  std::string m_productionToolName , m_decayToolName , m_cutToolName ;

};
#endif // GENERATORS_EXTERNALGENERATOR_H
