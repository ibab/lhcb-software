#ifndef LBPOWHEG_POWHEGPRODUCTIONWZ_H 
#define LBPOWHEG_POWHEGPRODUCTIONWZ_H 1
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
/** @class PowhegProductionWZ PowhegProductionWZ.h
 *
 *  Interface tool to produce events with Powheg- BOX
 *
 *  @author Bartek Rachwal  bartlomiej.rachwal@ifj.edu.pl
 *  @date 2013-04-16
 */
class PowhegProductionWZ : public PowhegProduction
{
  // =========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<PowhegProductionWZ> ;

public:
  // =========================================================================
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  PowhegProductionWZ
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) ;

  virtual ~PowhegProductionWZ() ; ///< destructor 
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
 
  virtual StatusCode finalize( ) ; ///< Finalize method
  
} ;
// ============================================================================
// The END 
// ============================================================================
#endif  //LBPOWHEG_POWHEGPRODUCTIONWZ_H 