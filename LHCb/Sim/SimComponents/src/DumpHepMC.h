// $Id: DumpHepMC.h,v 1.1 2008-07-23 17:38:05 cattanem Exp $ 
// ============================================================================
#ifndef SIM_DUMPHEPMC_H
#define SIM_DUMPHEPMC_H 1
// ============================================================================
// include files
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================

class DumpHepMC : public GaudiAlgorithm 
{
  // factory for instantiation 
  friend class AlgFactory<DumpHepMC> ;
  
public:
  
  /// the actual type of container with addresses 
  typedef std::vector<std::string> Addresses ;
  
public:
  
  /** execution of the algoritm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode execute();
  
protected:
  
  /** standard constructor 
   *  @see GaudiAlgorithm
   *  @see      Algorithm
   *  @see      AlgFactory
   *  @see     IAlgFactory 
   *  @param name algorithm instance's name 
   *  @param iscv pointer to Service Locator 
   */
  DumpHepMC ( const std::string& name , 
              ISvcLocator*       isvc ) ;
  /// destructor 
  virtual ~DumpHepMC();
  
private:
  
  // default constructor   is disabled 
  DumpHepMC();
  // copy constructor      is disabled 
  DumpHepMC           ( const DumpHepMC& );
  // assigenemtn operator  is disabled 
  DumpHepMC& operator=( const DumpHepMC& );
  
private:
  
  Addresses  m_addresses ;

  /// Print HepMC::GenEvent ordering particles according to barcodes
  void orderedPrint( const HepMC::GenEvent * theEvent , 
                     std::ostream & ostr ) const ;

  /// Print HepMC::GenVertex ordering particles according to barcodes
  void orderedVertexPrint( HepMC::GenVertex * theVertex , 
                           std::ostream & ostr ) const ;
};

// ============================================================================
// The END 
// ============================================================================
#endif // SIM_DUMPHEPMC_H
// ============================================================================
