// $Id: CaloClusterResetFractions.cpp,v 1.2 2001-12-09 14:33:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/11/08 20:07:04  ibelyaev
//  new tools are added into  the package
// 
// ============================================================================
// include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
// CaloEvent 
#include "CaloEvent/CaloCluster.h"
// local
#include "CaloClusterResetFractions.h"

// ============================================================================
/** @file  CaloClusterResetFractions.cpp
 *  
 *  Implementation file for class : CaloClusterResteFractions
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 07/11/2001 
 */
// ============================================================================

// ============================================================================
/** Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloClusterResetFractions>         s_Factory ;
const        IToolFactory&CaloClusterResetFractionsFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param type   tool type 
 *  @param name   tool name 
 *  @param parent tool parent 
 */
// ============================================================================
CaloClusterResetFractions::
CaloClusterResetFractions ( const std::string& type   ,
                            const std::string& name   ,
                            const IInterface*  parent )
  : CaloTool( type , name , parent )
{
  /// declare the available interfaces
  declareInterface<ICaloClusterTool>(this);
};

// ============================================================================
/** destructor, protected and virtual
 */
// ============================================================================
CaloClusterResetFractions::~CaloClusterResetFractions() {};

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    
CaloClusterResetFractions::initialize ()
{ return CaloTool::initialize() ; }

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterResetFractions::finalize   ()
{ return CaloTool::initialize() ; }

// ============================================================================
/** query interface method  
 *  @param  iiD  unique interface identifier 
 *  @param  pI   placeholder for interface 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterResetFractions::queryInterface 
( const InterfaceID& iiD ,
  void**             pI  )
{
  /// check the validity of the placeholder 
  if( 0 == pI ) {       return StatusCode::FAILURE   ; }
  ///
  if      ( iiD == ICaloClusterTool:: interfaceID   () ) 
    { *pI = static_cast<ICaloClusterTool*> (this)    ; }
  else if ( iiD == IAlgTool::         interfaceID   () ) 
    { *pI = static_cast<IAlgTool*>         (this)    ; }
  else if ( iiD == IInterface::       interfaceID   () )
    { *pI = static_cast<IInterface*>       (this)    ; }
  else { return CaloTool::queryInterface( iiD , pI ) ; }
  ///
  addRef();
  ///
  return StatusCode::SUCCESS ;
  ///
};

// ============================================================================
/** The main processing method 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloClusterResetFractions::process ( CaloCluster* cluster ) const
{ return (*this) (cluster) ; };

// ============================================================================
/** The main processing method with hypothesis 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @param hypo    processing hypothesis 
 *  @return status code 
 */  
// ============================================================================
StatusCode
CaloClusterResetFractions::process    
( CaloCluster* cluster                         , 
  const CaloHypotheses::Hypothesis& /* hypo */ ) const
{
  ///
  Warning(" The hypotheses dispatcher it not implemented yet!");
  ///
  return (*this) (cluster);
};

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */
// ============================================================================
StatusCode 
CaloClusterResetFractions::operator() ( CaloCluster* cluster ) const
{
  /// check the arguments 
  if( 0 == cluster ) { return Error("CaloCluster* points to NULL!"); }
  /// loop over all digits and reset the fractions 
  for( CaloCluster::Digits::iterator idigit = cluster->digits().begin() ;
       cluster->digits().end() != idigit ; ++idigit )
    {
      CaloDigitStatus& status = idigit->second;
      status.setFraction() ;
    }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
// The End 
// ============================================================================
