#ifndef LBPOWHEG_POWHEGPRODUCTIONBB_H 
#define LBPOWHEG_POWHEGPRODUCTIONBB_H 1
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
#include "PowhegProduction.h"
// ============================================================================
/** @class PowhegProductionbb PowhegProductionbb.h
 */
class PowhegProductionbb : public PowhegProduction
{
  // =========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<PowhegProductionbb> ;

public:
  // =========================================================================
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  PowhegProductionbb
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) ;

  virtual ~PowhegProductionbb() ; ///< destructor 
  
   virtual StatusCode initialize( ) ;   ///< Initialize method

   virtual StatusCode finalize( ) ; ///< Finalize method
  
} ;
// ============================================================================
// The END 
// ============================================================================
#endif  //LBPOWHEG_POWHEGPRODUCTIONBB_H 
