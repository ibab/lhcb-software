// $Id: CnvPhotonParticleMaker.cpp,v 1.2 2004-03-11 13:02:14 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/08 17:22:27  ibelyaev
//  CnvPhotonParticleMaker: new creator of converted photons
// 
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// local
#include "CnvPhotonParticleMaker.h"
// ============================================================================

// ============================================================================
/** @file
 *  
 *  Implementation file for class CnvPhotonParticleMaker
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-01-19
 */
// ============================================================================

// ============================================================================
/** @var CnvPhotonParticleMakerFactory
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CnvPhotonParticleMaker>         s_Factory ;
const        IToolFactory&CnvPhotonParticleMakerFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name 
 *  @param parent tool parent
 */
// ============================================================================
CnvPhotonParticleMaker::CnvPhotonParticleMaker
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : PhotonParticleMaker ( type, name , parent ) 
{
  setConverted     ( true  ) ;
  setUseAllPhotons ( false ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CnvPhotonParticleMaker::~CnvPhotonParticleMaker(){}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
