// $Id: ReadLHE.h,v 1.2 2007-07-31 12:49:53 ibelyaev Exp $
// ============================================================================
#ifndef LBPYTHIA_READLHE_H
#define LBPYTHIA_READLHE_H 1 
// ============================================================================
// Include files 
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Generators  
// ============================================================================
#include "Generators/F77Utils.h"
// ============================================================================
// LbPythia
// ============================================================================
#include "LbPythia/Pypars.h"
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"
// ============================================================================
// Local 
// ============================================================================
#include "LbPythia/ReadFile.h"
// ============================================================================
namespace LbPythia 
{
  // ==========================================================================
  /** @class ReadLHE RadLHE.h ReadLHE.cpp
   */
  class ReadLHE: public LbPythia::ReadFile
  {
    // ========================================================================
    /// friend factory for instantiation
    friend class ToolFactory<LbPythia::ReadLHE> ;
    // ========================================================================
  public:
    // ========================================================================
    /// tool initialization 
    virtual StatusCode initialize () ;
    /// tool finalization 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param type tool type (?)
     *  @param name tool name 
     *  @param parent tool parent 
     */
    ReadLHE
    ( const std::string& type,
      const std::string& name,
      const IInterface* parent ) ;
    // destructor
    virtual ~ReadLHE () ;
    // ========================================================================
  private:
    // ========================================================================
    // the default constructor is disabled 
    ReadLHE () ;
    // the copy constructor is disabled 
    ReadLHE( const ReadLHE& ) ;
    // assignement operator is disabled 
    ReadLHE& operator=( const ReadLHE& ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// logical file unit  (F77)
    int         m_LUN     ;                         // logical file unit  (F77)
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LbPythia
// ============================================================================
// The END 
// ============================================================================
#endif // LBPYTHIA_READLHE_H 1 
// ============================================================================
