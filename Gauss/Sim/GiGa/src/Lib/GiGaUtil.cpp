/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/07/25 17:18:09  ibelyaev
/// move all conversions from GiGa to GiGaCnv
///
/// Revision 1.1  2001/07/23 20:53:46  ibelyaev
/// reorganization of GiGaUtil namespace
/// 
/// ===========================================================================
/// STD & STL 
#include <algorithm> 
/// GaudiKernel
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IObjManager.h"
/// GiGa 
#include "GiGa/GiGaException.h"
#include "GiGa/IGiGaPhysList.h"
#include "GiGa/IGiGaPhysListFactory.h"
#include "GiGa/IGiGaRunAction.h"
#include "GiGa/IGiGaRunActionFactory.h"
#include "GiGa/IGiGaEventAction.h"
#include "GiGa/IGiGaEventActionFactory.h"
#include "GiGa/IGiGaStepAction.h"
#include "GiGa/IGiGaStepActionFactory.h"
#include "GiGa/IGiGaStackAction.h"
#include "GiGa/IGiGaStackActionFactory.h"
#include "GiGa/IGiGaTrackAction.h"
#include "GiGa/IGiGaTrackActionFactory.h"
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IGiGaSensDetFactory.h"
#include "GiGa/IGiGaMagField.h"
#include "GiGa/IGiGaMagFieldFactory.h"
#include "GiGa/GiGaUtil.h"

/// ===========================================================================
/** Implementation file for functions from  GiGaUtil namespace 
 * 
 *  @author Vanya  Belyaev
 *  @date 23/07/2001 
 */
//-----------------------------------------------------------------------------
/// ===========================================================================

/// ===========================================================================
/** split a "Type/Name" string into "Type" and "Name"
 *  
 *  rules:
 * -  "Type/Name"  =  "Type" , "Name"
 * -  "Type"       =  "Type" , "Type"
 * -  "Type/"      =  "Type" , "Type"
 * 
 *  error conditions 
 *
 *  -  empty input string 
 *  -  more than 1 separator ('/') 
 *  -  empty type ("/Name")
 *
 *  @param TypeAndName   string to be splitted 
 *  @param Type  returned "Type" 
 *  @param Name  returned "Name" 
 *  @return status code
 */  
/// ===========================================================================
StatusCode GiGaUtil::SplitTypeAndName ( const std::string& TypeAndName ,
                                        std::string      & Type        , 
                                        std::string      & Name        )
  
{
  /// clear output strings
  Type.erase();
  Name.erase();
  ///
  if( TypeAndName.empty() ) { return StatusCode::FAILURE ; } ///< RETURN
  /// 
  const unsigned int numSep = 
    std::count( TypeAndName.begin() ,
                TypeAndName.end  () , '/' );
  if      ( 1  <  numSep  ) { return StatusCode::FAILURE ; } ///< RETURN 
  else if ( 0 ==  numSep  ) 
    {
      Type = TypeAndName ;
      Name = TypeAndName ;
      return StatusCode::SUCCESS ;                           ///< RETURN
    }
  else 
    {      
      /// look for '/' in the string 
      std::string::size_type pos = TypeAndName.find('/');
      if( std::string::npos  == pos ) { return StatusCode::FAILURE ; }
      /// construct the name and the type
      Type = std::string( TypeAndName , 0   , pos );
      if( Type.empty()    ) { return StatusCode::FAILURE ; }  ///< RETURN 
      Name = std::string( TypeAndName , pos + 1 , std::string::npos ) ;
      if( Name.empty()    ) { Name = Type ; }
    }  
  ///
  return StatusCode::SUCCESS; 
  ///
};


/// ==========================================================================
/** @class Creator 
 *  
 *  Helper class to make the instantiation of 
 *  different objects more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ==========================================================================
/** constructor 
 *  @param  ObjMgr   pointer to Object Manager 
 *  @param  SvcLoc   pointer to Service Locator 
 */
/// ==========================================================================
GiGaUtil::Creator::Creator( IObjManager* ObjMgr ,
                            ISvcLocator* SvcLoc )
  : m_objMgr ( ObjMgr ) 
  , m_svcLoc ( SvcLoc )
{ 
  if( 0 != objMgr() ) { objMgr()->addRef() ; }
  if( 0 != svcLoc() ) { svcLoc()->addRef() ; }
};

/// ===========================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
/// ===========================================================================
GiGaUtil::Creator::Creator( const GiGaUtil::Creator& right )
  : m_objMgr ( right.objMgr() )
  , m_svcLoc ( right.svcLoc() ) 
{ 
  if( 0 != objMgr() ) { objMgr()->addRef() ; }
  if( 0 != svcLoc() ) { svcLoc()->addRef() ; }
};

/// ===========================================================================
/// destructor 
/// ===========================================================================
GiGaUtil::Creator::~Creator()
{ 
  if( 0 != objMgr() ) { objMgr()->release() ; m_objMgr = 0 ; }
  if( 0 != svcLoc() ) { svcLoc()->release() ; m_svcLoc = 0 ; }
};

/// ===========================================================================
/// destructor 
/// ===========================================================================
StatusCode GiGaUtil::Creator::isOK() const 
{  
  /// check for Object Manager 
  if( 0 == objMgr() ) { throw GiGaException( ObjTypeName( this )  + 
                                             "IObjManager* points to NULL!");}
  /// check for Service Locator 
  if( 0 == svcLoc() ) { throw GiGaException( ObjTypeName( this )  + 
                                             "ISvcLocator* points to NULL!");}
  ///
  return StatusCode::SUCCESS;
};

/// ===========================================================================
/** locate the abstract factory for object type
 *  @param type  type of the object
 *  @return pointer to abstract factory 
 */
/// ===========================================================================
const IFactory*  GiGaUtil::Creator::Factory( const std::string& type ) const
{
  if( isOK().isFailure() ) { return 0; }                 ///< RETURN
  /// locate the factory
  const bool exist        = objMgr()->existsObjFactory( type ) ;
  if( !exist         ) { return 0 ; }                    ///< RETURN 
  ///
  return objMgr()->objFactory      ( type );
};

/// ==========================================================================
/** @class PhysListCreator 
 *  
 *  Helper class to make the instantiation of 
 *  IGiGaPhysList object more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ===========================================================================
/** instantiate IGiGaPhysList object of given type and name 
 *  @param type  type of the IGiGaPhysList object
 *  @param name  name of the IGiGaPhysList object 
 *  @return pointer to new IGiGaPhysList object 
 */
/// ===========================================================================
IGiGaPhysList* 
GiGaUtil::PhysListCreator::operator() ( const std::string& type ,
                                        const std::string& name ) const
{
  /// locate the factory
  const IFactory* factory = Factory ( type );
  if( 0 == factory   ) { return 0 ; }                    ///< RETURN 
  /// cast to right factory type 
  const IGiGaPhysListFactory* plFactory = 
    dynamic_cast<const IGiGaPhysListFactory*> ( factory ) ;
  if( 0 == plFactory ) { return 0 ; }                    ///< RETURN
  /// instantiate the PhysList object
  return plFactory->instantiate( name , svcLoc() ) ;
};

/// ==========================================================================
/** @class RunActionCreator 
 *  
 *  Helper class to make the instantiation of 
 *  IGiGaRunAction object more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ===========================================================================
/** instantiate IGiGaRunAction object of given type and name 
 *  @param type  type of the IGiGaRunAction object
 *  @param name  name of the IGiGaRunAction object 
 *  @return pointer to new IGiGaRunAction object 
 */
/// ===========================================================================
IGiGaRunAction* 
GiGaUtil::RunActionCreator::operator() ( const std::string& type ,
                                           const std::string& name ) const
{ 
  /// locate the factory
  const IFactory* factory = Factory( type );
  if( 0 == factory   ) { return 0 ; }                    ///< RETURN 
  /// cast to right factory type 
  const IGiGaRunActionFactory* raFactory = 
    dynamic_cast<const IGiGaRunActionFactory*> ( factory ) ;
  if( 0 == raFactory ) { return 0 ; }                    ///< RETURN
  /// instantiate the EventAction object
  return raFactory->instantiate( name , svcLoc() ) ;
  ///
};
/// ==========================================================================
/** @class EventActionCreator 
 *  
 *  Helper class to make the instantiation of 
 *  IGiGaEventAction object more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ===========================================================================
/** instantiate IGiGaEventAction object of given type and name 
 *  @param type  type of the IGiGaEventAction object
 *  @param name  name of the IGiGaEventAction object 
 *  @return pointer to new IGiGaEventAction object 
 */
/// ===========================================================================
IGiGaEventAction* 
GiGaUtil::EventActionCreator::operator() ( const std::string& type ,
                                           const std::string& name ) const
{ 
  /// locate the factory
  const IFactory* factory = Factory( type );
  if( 0 == factory   ) { return 0 ; }                    ///< RETURN 
  /// cast to right factory type 
  const IGiGaEventActionFactory* eaFactory = 
    dynamic_cast<const IGiGaEventActionFactory*> ( factory ) ;
  if( 0 == eaFactory ) { return 0 ; }                    ///< RETURN
  /// instantiate the EventAction object
  return eaFactory->instantiate( name , svcLoc() ) ;
  ///
};

/// ==========================================================================
/** @class StepActionCreator 
 *  
 *  Helper class to make the instantiation of 
 *  IGiGaStepAction object more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ===========================================================================
/** instantiate IGiGaStepAction object of given type and name 
 *  @param type  type of the IGiGaStepAction object
 *  @param name  name of the IGiGaStepAction object 
 *  @return pointer to new IGiGaStepAction object 
 */
/// ===========================================================================
IGiGaStepAction* 
GiGaUtil::StepActionCreator::operator() ( const std::string& type ,
                                          const std::string& name ) const
{ 
  /// locate the factory
  const IFactory* factory = Factory( type );
  if( 0 == factory   ) { return 0 ; }                    ///< RETURN 
  /// cast to right factory type 
  const IGiGaStepActionFactory* saFactory = 
    dynamic_cast<const IGiGaStepActionFactory*> ( factory ) ;
  if( 0 == saFactory ) { return 0 ; }                    ///< RETURN
  /// instantiate the EventAction object
  return saFactory->instantiate( name , svcLoc() ) ;
  ///
};

/// ==========================================================================
/** @class StackActionCreator 
 *  
 *  Helper class to make the instantiation of 
 *  IGiGaStackAction object more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ===========================================================================
/** instantiate IGiGaStackAction object of given type and name 
 *  @param type  type of the IGiGaStackAction object
 *  @param name  name of the IGiGaStackAction object 
 *  @return pointer to new IGiGaStackAction object 
 */
/// ===========================================================================
IGiGaStackAction* 
GiGaUtil::StackActionCreator::operator() ( const std::string& type ,
                                           const std::string& name ) const
{ 
  /// locate the factory
  const IFactory* factory = Factory( type );
  if( 0 == factory   ) { return 0 ; }                    ///< RETURN 
  /// cast to right factory type 
  const IGiGaStackActionFactory* saFactory = 
    dynamic_cast<const IGiGaStackActionFactory*> ( factory ) ;
  if( 0 == saFactory ) { return 0 ; }                    ///< RETURN
  /// instantiate the EventAction object
  return saFactory->instantiate( name , svcLoc() ) ;
  ///
};

/// ==========================================================================
/** @class TrackActionCreator 
 *  
 *  Helper class to make the instantiation of 
 *  IGiGaTrackAction object more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ===========================================================================
/** instantiate IGiGaTrackAction object of given type and name 
 *  @param type  type of the IGiGaTrackAction object
 *  @param name  name of the IGiGaTrackAction object 
 *  @return pointer to new IGiGaTrackAction object 
 */
/// ===========================================================================
IGiGaTrackAction* 
GiGaUtil::TrackActionCreator::operator() ( const std::string& type ,
                                           const std::string& name ) const
{ 
  /// locate the factory
  const IFactory* factory = Factory( type );
  if( 0 == factory   ) { return 0 ; }                    ///< RETURN 
  /// cast to right factory type 
  const IGiGaTrackActionFactory* taFactory = 
    dynamic_cast<const IGiGaTrackActionFactory*> ( factory ) ;
  if( 0 == taFactory ) { return 0 ; }                    ///< RETURN
  /// instantiate the EventAction object
  return taFactory->instantiate( name , svcLoc() ) ;
  ///
};

/// ==========================================================================
/** @class SensDetCreator 
 *  
 *  Helper class to make the instantiation of 
 *  IGiGaSensDet object more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ===========================================================================
/** instantiate IGiGaSensDet object of given type and name 
 *  @param type  type of the IGiGaSensDet object
 *  @param name  name of the IGiGaSensDet object 
 *  @return pointer to new IGiGaSensDet object 
 */
/// ===========================================================================
IGiGaSensDet* 
GiGaUtil::SensDetCreator::operator() ( const std::string& type ,
                                       const std::string& name ) const
{ 
  /// locate the factory
  const IFactory* factory = Factory( type );
  if( 0 == factory   ) { return 0 ; }                    ///< RETURN 
  /// cast to right factory type 
  const IGiGaSensDetFactory* sdFactory = 
    dynamic_cast<const IGiGaSensDetFactory*> ( factory ) ;
  if( 0 == sdFactory ) { return 0 ; }                    ///< RETURN
  /// instantiate the EventAction object
  return sdFactory->instantiate( name , svcLoc() ) ;
  ///
};

/// ==========================================================================
/** @class MagFieldCreator 
 *  
 *  Helper class to make the instantiation of 
 *  IGiGaMagField object more transparent 
 *  
 *  @author Vanya Belyaev
 *  @date 23/07/2001
 */
/// ===========================================================================
/** instantiate IGiGaMagField object of given type and name 
 *  @param type  type of the IGiGaMagField object
 *  @param name  name of the IGiGaMagField object 
 *  @return pointer to new IGiGaMagField object 
 */
/// ===========================================================================
IGiGaMagField* 
GiGaUtil::MagFieldCreator::operator() ( const std::string& type ,
                                        const std::string& name ) const
{ 
  /// locate the factory
  const IFactory* factory = Factory( type );
  if( 0 == factory   ) { return 0 ; }                    ///< RETURN 
  /// cast to right factory type 
  const IGiGaMagFieldFactory* mfFactory = 
    dynamic_cast<const IGiGaMagFieldFactory*> ( factory ) ;
  if( 0 == mfFactory ) { return 0 ; }                    ///< RETURN
  /// instantiate the EventAction object
  return mfFactory->instantiate( name , svcLoc() ) ;
  ///
};

/// ===========================================================================


