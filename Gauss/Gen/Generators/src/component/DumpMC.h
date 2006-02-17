// $Id: DumpMC.h,v 1.2 2006-02-17 13:24:45 robbep Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
//
// ============================================================================
#ifndef GENERATORS_DUMPMC_H
#define GENERATORS_DUMPMC_H 1
// ============================================================================
// include files
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================

class DumpMC : public GaudiAlgorithm 
{
  // factory for instantiation 
  friend class AlgFactory<DumpMC> ;
  
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
  DumpMC ( const std::string& name , 
           ISvcLocator*       isvc ) ;
  /// destructor 
  virtual ~DumpMC();
  
private:
  
  // default constructor   is disabled 
  DumpMC();
  // copy constructor      is disabled 
  DumpMC           ( const DumpMC& );
  // assigenemtn operator  is disabled 
  DumpMC& operator=( const DumpMC& );
  
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
#endif // GENERATORS_DUMPMC_H
// ============================================================================
