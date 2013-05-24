#ifndef LBPOWHEG_POWHEGPRODUCTION_H 
#define LBPOWHEG_POWHEGPRODUCTION_H 1
// ============================================================================
// Include files 
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Local 
// ============================================================================
#include "LbPowheg/Powheg.h"
// ============================================================================
/** @class PowhegProduction PowhegProduction.h
 */
class PowhegProduction : public LbPythia::Powheg
{
  // =========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<PowhegProduction> ;
  // =========================================================================
  public:
  // =========================================================================
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  PowhegProduction
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent );
  virtual ~PowhegProduction() {} ///< destructor 
  virtual StatusCode initialize( ) ;   ///< Initialize method
  virtual StatusCode finalize( ) ; ///< Finalize method

private:
  // =========================================================================
  StatusCode generateLHE() ;
  // the default constructor is disabled 
  PowhegProduction() ;
  // the copy constructor is disabled 
  PowhegProduction ( const PowhegProduction& ) ;
  // assignement operator is disabled 
  PowhegProduction& operator=( const PowhegProduction& ) ;
  // =========================================================================
} ;
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( PowhegProduction )
// ============================================================================
// The END 
// ============================================================================

#endif // LBPOWHEG_POWHEGPRODUCTION_H 
