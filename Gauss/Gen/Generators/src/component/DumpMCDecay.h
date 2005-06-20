// $Id: DumpMCDecay.h,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// 
// ============================================================================
#ifndef GENERATORS_DUMPMCDECAY_H 
#define GENERATORS_DUMPMCDECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================


/** @class DumpMCDecay DumpMCDecay.h Algorithms/DumpMCDecay.h
 *
 *  Dump the decays of certain particles   
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-18
 */
class DumpMCDecay : public GaudiAlgorithm 
{  
  // factory for instantiation 
  friend class AlgFactory<DumpMCDecay> ;
  
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
  virtual StatusCode initialize ();
  
  /** execution of the algoritm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode execute    ();

protected:
  
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
  DumpMCDecay( const std::string& name , 
               ISvcLocator*       isvc ) ;
  
  /// dectructor 
  virtual ~DumpMCDecay();

private:
  
  // default constructor   is disabled 
  DumpMCDecay();
  // copy constructor      is disabled 
  DumpMCDecay           ( const DumpMCDecay& );
  // assigenemtn operator  is disabled 
  DumpMCDecay& operator=( const DumpMCDecay& );
  
private:
  
  // addresses of HepMC events
  Addresses                     m_addresses ;
  
  // particles to be printed
  PIDs                          m_particles ;
  
  // quarks to be printes
  PIDs                          m_quarks    ;
  
  // pointer to particle property service 
  mutable IParticlePropertySvc* m_ppSvc     ;
  
};


// ============================================================================
// The END 
// ============================================================================
#endif // GENERATORS_DUMPMCDECAY_H
// ============================================================================
