// $Id: DumpHepMCDecay.h,v 1.2 2009-01-08 09:44:37 cattanem Exp $
// ============================================================================
#ifndef SIM_DUMPHEPMCDECAY_H 
#define SIM_DUMPHEPMCDECAY_H 1
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h" 
// ============================================================================
#include <string>
#include <vector>

namespace LHCb {
  class IParticlePropertySvc;
}

namespace HepMC {
  class GenParticle;
  class GenVertex;
}


/** @class DumpHepMCDecay DumpHepMCDecay.h
 *
 *  Dump the decays of certain particles
 *  This class was previously called DumpMCDecay (Gen/Generators package)
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-18
 */
class DumpHepMCDecay : public GaudiAlgorithm 
{  
  // factory for instantiation 
  friend class AlgFactory<DumpHepMCDecay> ;  
public:
  
  /// the actual type of container with addresses 
  typedef std::vector<std::string> Addresses ;
  /// the actual type of list of PIDs 
  typedef std::vector<int>         PIDs      ;
public:
  /** initialization of the algoritm
   *  @see GaudiAlgorithm
   *  @see      Algorithm
   *  @see     IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  /** execution of the algoritm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode execute    ();
public:
  /** print the decay tree of the particle 
   *  @param particle pointer to teh particle to be printed 
   *  @param stream   output stream 
   *  @param level    decay level
   *  @return statsu code 
   */
  StatusCode printDecay 
  ( const HepMC::GenParticle* particle              , 
    std::ostream&             stream    = std::cout , 
    unsigned int              level     = 0         ) const ;
  /** get the particle name in the string fixed form
   *  @param particle pointer to the particle
   *  @param particle name 
   */
  std::string particleName 
  ( const HepMC::GenParticle* particle ) const ;
protected:  
  /** standard constructor 
   *  @see GaudiAlgorithm
   *  @see      Algorithm
   *  @see      AlgFactory
   *  @see     IAlgFactory 
   *  @param name algorithm instance's name 
   *  @param iscv pointer to Service Locator 
   */
  DumpHepMCDecay
  ( const std::string& name , 
    ISvcLocator*       isvc ) ;
  /// dectructor 
  virtual ~DumpHepMCDecay(){};
private:
  // default constructor   is disabled 
  DumpHepMCDecay();
  // copy constructor      is disabled 
  DumpHepMCDecay           ( const DumpHepMCDecay& );
  // assigenemtn operator  is disabled 
  DumpHepMCDecay& operator=( const DumpHepMCDecay& );
protected:
  // addresses of HepMC events
  Addresses                     m_addresses ;
  // particles to be printed
  PIDs                          m_particles ;
  // quarks to be printes
  PIDs                          m_quarks    ;
  // maximal number of levels 
  int                           m_levels    ;
private:
  // pointer to particle property service 
  mutable LHCb::IParticlePropertySvc* m_ppSvc;
} ;
// ============================================================================
// The END 
// ============================================================================
#endif // SIM_DUMPHEPMCDECAY_H
// ============================================================================
