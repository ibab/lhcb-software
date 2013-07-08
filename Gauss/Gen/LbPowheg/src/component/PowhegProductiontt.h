#ifndef LBPOWHEG_POWHEGPRODUCTIONTT_H 
#define LBPOWHEG_POWHEGPRODUCTIONTT_H 1
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
/** @class PowhegProductiontt PowhegProductiontt.h
 */
class PowhegProductiontt : public PowhegProduction
{
  // =========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<PowhegProductiontt> ;

public:
  // =========================================================================
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  PowhegProductiontt
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) ;

  virtual ~PowhegProductiontt() ; ///< destructor 
  
   virtual StatusCode initialize( ) ;   ///< Initialize method

   virtual StatusCode finalize( ) ; ///< Finalize method
  
} ;
// ============================================================================
// The END 
// ============================================================================
#endif  //LBPOWHEG_POWHEGPRODUCTIONTT_H 
