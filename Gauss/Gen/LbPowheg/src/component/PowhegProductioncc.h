#ifndef LBPOWHEG_POWHEGPRODUCTIONCC_H 
#define LBPOWHEG_POWHEGPRODUCTIONCC_H 1
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
/** @class PowhegProductioncc PowhegProductioncc.h
 */
class PowhegProductioncc : public PowhegProduction
{
  // =========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<PowhegProductioncc> ;

public:
  // =========================================================================
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  PowhegProductioncc
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) ;

  virtual ~PowhegProductioncc() ; ///< destructor 
  
   virtual StatusCode initialize( ) ;   ///< Initialize method

   virtual StatusCode finalize( ) ; ///< Finalize method
  
} ;
// ============================================================================
// The END 
// ============================================================================
#endif  //LBPOWHEG_POWHEGPRODUCTIONCC_H 
