// $Id: CaloTrackIdEval.cpp,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// local
#include "CaloTrackIdEval.h"


// ============================================================================
/** @file 
 *
 *  Implementation file for class CaloTrackIdEval
 *
 *  @author Ivan Belyaev
 *  @date   2002-11-10
 */
// ============================================================================

// ============================================================================
/** @var CaloTrackIdEvalFactory 
 *  Declaration of the Tool Factory
 */
// ============================================================================
//static const  ToolFactory<CaloTrackIdEval>         s_Factory ;
//const        IToolFactory&CaloTrackIdEvalFactory = s_Factory ;
// ============================================================================


// ============================================================================
/** Standard constructor
 *  @see   CaloTool
 *  @see    AlgTool 
 *  @param type    type of the tool  (?)
 *  @param name    name of the concrete instance 
 *  @param parent  pointer to parent object (algorithm, service xor tool)
 */
// ============================================================================
CaloTrackIdEval::CaloTrackIdEval( const std::string& type   ,
                                  const std::string& name   ,
                                  const IInterface*  parent )
  : CaloTool ( type, name , parent ){}
// ============================================================================



// ============================================================================
// The END
// ============================================================================
