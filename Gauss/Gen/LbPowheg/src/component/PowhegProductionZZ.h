#ifndef LBPOWHEG_POWHEGPRODUCTIONZZ_H 
#define LBPOWHEG_POWHEGPRODUCTIONZZ_H 1
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
/** @class PowhegProductionZZ PowhegProductionZZ.h
 *
 *  Interface tool to produce events with Powheg- BOX
 *
 *  @author Bartek Rachwal  bartlomiej.rachwal@ifj.edu.pl
 *  @date 2013-04-16
 */
class PowhegProductionZZ : public PowhegProduction
{
  // =========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<PowhegProductionZZ> ;

public:
  // =========================================================================
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  PowhegProductionZZ
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) ;

  virtual ~PowhegProductionZZ() ; ///< destructor 
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
 
  virtual StatusCode finalize( ) ; ///< Finalize method
  
} ;
// ============================================================================
// The END 
// ============================================================================
#endif  //LBPOWHEG_POWHEGPRODUCTIONZZ_H 