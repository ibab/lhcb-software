// $Id: ReadLHEfileProduction.cpp,v 1.2 2007-06-29 13:37:20 ibelyaev Exp $
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
#include "LbPythia/ReadLHE.h"
// ============================================================================
/** @class ReadLHEfileProduction ReadLHEfileProduction.cpp
 *
 *  Simple production tool which reads parton level 
 *  LHE file (Les Houches Events). The LHE standard 
 *  in described in hep-ph/0609017 CERN-LCGAPP-2006-03
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-10-03
 */
class ReadLHEfileProduction : public LbPythia::ReadLHE
{
  // =========================================================================
  /// friend factory for instantiation
  friend class ToolFactory<ReadLHEfileProduction> ;
  // =========================================================================
protected:
  // =========================================================================
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  ReadLHEfileProduction
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) 
    : LbPythia::ReadLHE ( type , name , parent ) 
  {}
  // destructor 
  virtual ~ReadLHEfileProduction() {} ///< destructor 
  // =========================================================================
private:
  // =========================================================================
  // the default constructor is disabled 
  ReadLHEfileProduction() ;
  // the copy constructor is disabled 
  ReadLHEfileProduction ( const ReadLHEfileProduction& ) ;
  // assignement operator is disabled 
  ReadLHEfileProduction& operator=( const ReadLHEfileProduction& ) ;
  // =========================================================================
} ;
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( ReadLHEfileProduction )
// ============================================================================
// The END 
// ============================================================================
