/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/07/24 07:11:04  ibelyaev
/// new methods for object deletion in GiGaUtil namespace
///
/// Revision 1.1  2001/07/23 20:53:45  ibelyaev
/// reorganization of GiGaUtil namespace
/// 
/// ===========================================================================
#ifndef GIGA_GIGAUTIL_H 
#define GIGA_GIGAUTIL_H 1
/// ===========================================================================
/// STD & STL 
#include <string>
#include <functional>
/// GaudiKernel 
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"
/// forward declarations 
class IObjManager;
class ISvcLocator;
class IFactory;
class IGiGaPhysList;
class IGiGaEventAction;
class IGiGaStepAction;
class IGiGaStackAction;
class IGiGaTrackAction;
class IGiGaSensDet;
class IGiGaMagField;

/** @namespace  GiGaUtil GiGaUtil.h GiGa/GiGaUtil
 *  
 *  collection of simple helper utilities for GiGa package 
 * 
 *  @author Ivan Belyaev
 *  @date   23/07/2001
 */

namespace GiGaUtil 
{
  
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
  StatusCode SplitTypeAndName ( const std::string& TypeAndName ,
                                std::string      & Type        , 
                                std::string      & Name        );
  
  /** useful utility to extract the object type name 
   *  @param obj pointer to object of type "TYPE"
   *  @return type name of the object
   */ 
  template <class TYPE>
  inline const std::string ObjTypeName( TYPE obj )
  { 
    return 
      obj ? 
      std::string( System::typeinfoName( typeid(*obj) ) ) : 
      std::string( "UNKNOWN_object"                     ) ; 
  };
  
  /** @class Delete 
   * 
   *  useful utility to delete the object 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  class Delete
  { 
  public:
    /* delete the object 
     *  @param obj pointer to object to be deleted 
     *  @return NULL pointer 
     */
    template <class TYPE>
    inline TYPE* operator() ( TYPE* obj ) const 
    { 
      if( 0 != obj ) { delete obj ; obj = 0 ; }
      return obj ;
    };
  };
  
  /** @class Eraser
   * 
   *  useful utility to delete the object 
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */ 
  template <class TYPE>
  class Eraser: public std::unary_function<TYPE*,TYPE*>
  { 
  public:
    /* delete the object 
     *  @param obj pointer to object to be deleted 
     *  @return NULL pointer 
     */
    inline TYPE* operator() ( TYPE* obj ) const 
    { 
      if( 0 != obj ) { delete obj ; obj = 0 ; }
      return obj ;
    };
  };
  
  /** useful utility to delete the object through the pointer  
   *
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   *
   *  @param obj pointer to object to be deleted 
   *  @return NULL pointer 
   */ 
  template <class TYPE>
  inline TYPE* Delete_Ptr( TYPE* obj )
  { 
    if( 0 != obj ) { delete obj ; obj = 0 ; }
    return obj ;
  };

  /** @class Creator 
   *  
   *  Helper class to make the instantiation of 
   *  different objects more transparent 
   *  
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */
  class Creator
  {    
  protected:
    
    /** constructor 
     *  @param  ObjMgr   pointer to Object Manager 
     *  @param  SvcLoc   pointer to Service Locator 
     */
    Creator( IObjManager* ObjMgr ,
             ISvcLocator* SvcLoc );
    
    /** copy constructor 
     *  @param right object to be copied 
     */
    Creator( const Creator& right );
    
    /// destructor 
    virtual ~Creator();
    ///
  protected:
    
    /** accessor to Object Manager object 
     *  @return pointer to Object Manager object 
     */  
    inline IObjManager* objMgr() const { return m_objMgr ; }
    
    /** accessor to Service Locator  object 
     *  @return pointer to Service Locator object 
     */  
    inline ISvcLocator* svcLoc() const { return m_svcLoc ; }
    
    /** perform  the check
     *  @return status code 
     */
    virtual StatusCode  isOK() const;
    
    /** locate the abstract factory for object type
     *  @param type  type of the object
     *  @return pointer to abstract factory 
     */
    const IFactory*  Factory( const std::string& type ) const;
    
  private:
    
    /// no default constructor
    Creator();
    /// no assignment 
    Creator& operator=( const Creator& ) ;
    
  private:
    ///
    IObjManager* m_objMgr; ///< pointer to Object Manager 
    ISvcLocator* m_svcLoc; ///< pointer to ServiceLocator 
    ///
  };
  
  /** @class PhysListCreator 
   *  
   *  Helper class to make the instantiation of 
   *  IGiGaPhysList object more transparent 
   *  
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */
  class PhysListCreator: 
    public GiGaUtil::Creator , public 
  std::binary_function<const std::string&,const std::string&,IGiGaPhysList*>
  {
  public:
    
    /** constructor 
     *  @param  ObjMgr   pointer to Object Manager 
     *  @param  SvcLoc   pointer to Service Locator 
     */
    PhysListCreator( IObjManager* ObjMgr ,
                     ISvcLocator* SvcLoc )
      : Creator( ObjMgr , SvcLoc ) {};
    
    /** instantiate IGiGaPhysList object of given type and name 
     *  @param type  type of the IGiGaPhysList object
     *  @param name  name of the IGiGaPhysList object 
     *  @return pointer to new IgiGaPhysList object 
     */
    IGiGaPhysList* operator() ( const std::string& type ,
                                const std::string& name ) const;
    ///
  };

  /** @class EventActionCreator 
   *  
   *  Helper class to make the instantiation of 
   *  IGiGaEventAction object more transparent 
   *  
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */
  class EventActionCreator: 
    public GiGaUtil::Creator , public 
  std::binary_function<const std::string&,const std::string&,IGiGaEventAction*>
  {
  public:
    
    /** constructor 
     *  @param  ObjMgr   pointer to Object Manager 
     *  @param  SvcLoc   pointer to Service Locator 
     */
    EventActionCreator( IObjManager* ObjMgr ,
                        ISvcLocator* SvcLoc )
      : Creator( ObjMgr , SvcLoc ) {};
    
    /** instantiate IGiGaEventAction object of given type and name 
     *  @param type  type of the IGiGaEventAction object
     *  @param name  name of the IGiGaEventAction object 
     *  @return pointer to new IGiGaEventAction object 
     */
    IGiGaEventAction* operator() ( const std::string& type ,
                                   const std::string& name ) const;
    ///
  };
  
  /** @class StepActionCreator 
   *  
   *  Helper class to make the instantiation of 
   *  IGiGaStepAction object more transparent 
   *  
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */
  class StepActionCreator: 
    public GiGaUtil::Creator , public 
  std::binary_function<const std::string&,const std::string&,IGiGaStepAction*>
  {
  public:
    
    /** constructor 
     *  @param  ObjMgr   pointer to Object Manager 
     *  @param  SvcLoc   pointer to Service Locator 
     */
    StepActionCreator( IObjManager* ObjMgr ,
                       ISvcLocator* SvcLoc )
      : Creator( ObjMgr , SvcLoc ) {};
    
    /** instantiate IGiGaStepAction object of given type and name 
     *  @param type  type of the IGiGaStepAction object
     *  @param name  name of the IGiGaStepAction object 
     *  @return pointer to new IGiGaStepAction object 
     */
    IGiGaStepAction* operator() ( const std::string& type ,
                                  const std::string& name ) const;
    ///
  };
  
  /** @class StackActionCreator 
   *  
   *  Helper class to make the instantiation of 
   *  IGiGaStackAction object more transparent 
   *  
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */
  class StackActionCreator: 
    public GiGaUtil::Creator , public 
  std::binary_function<const std::string&,const std::string&,IGiGaStackAction*>
  {
  public:
    
    /** constructor 
     *  @param  ObjMgr   pointer to Object Manager 
     *  @param  SvcLoc   pointer to Service Locator 
     */
    StackActionCreator( IObjManager* ObjMgr ,
                        ISvcLocator* SvcLoc )
      : Creator( ObjMgr , SvcLoc ) {};
    
    /** instantiate IGiGaStackAction object of given type and name 
     *  @param type  type of the IGiGaStackAction object
     *  @param name  name of the IGiGaStackAction object 
     *  @return pointer to new IGiGaStackAction object 
     */
    IGiGaStackAction* operator() ( const std::string& type ,
                                   const std::string& name ) const;
    ///
  };

  /** @class TrackActionCreator 
   *  
   *  Helper class to make the instantiation of 
   *  IGiGaTrackAction object more transparent 
   *  
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */
  class TrackActionCreator: 
    public GiGaUtil::Creator , public 
  std::binary_function<const std::string&,const std::string&,IGiGaTrackAction*>
  {
  public:
    
    /** constructor 
     *  @param  ObjMgr   pointer to Object Manager 
     *  @param  SvcLoc   pointer to Service Locator 
     */
    TrackActionCreator( IObjManager* ObjMgr ,
                        ISvcLocator* SvcLoc )
      : Creator( ObjMgr , SvcLoc ) {};
    
    /** instantiate IGiGaTrackAction object of given type and name 
     *  @param type  type of the IGiGaTrackAction object
     *  @param name  name of the IGiGaTrackAction object 
     *  @return pointer to new IGiGaTrackAction object 
     */
    IGiGaTrackAction* operator() ( const std::string& type ,
                                   const std::string& name ) const;
    ///
  };

  /** @class SensDetCreator 
   *  
   *  Helper class to make the instantiation of 
   *  IGiGaSensDet object more transparent 
   *  
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */
  class SensDetCreator: 
    public GiGaUtil::Creator , public 
  std::binary_function<const std::string&,const std::string&,IGiGaSensDet*>
  {
  public:
    
    /** constructor 
     *  @param  ObjMgr   pointer to Object Manager 
     *  @param  SvcLoc   pointer to Service Locator 
     */
    SensDetCreator( IObjManager* ObjMgr ,
                    ISvcLocator* SvcLoc )
      : Creator( ObjMgr , SvcLoc ) {};
    
    /** instantiate IGiGaSensDet object of given type and name 
     *  @param type  type of the IGiGaSensDet object
     *  @param name  name of the IGiGaSensDet object 
     *  @return pointer to new IGiGaSensDet object 
     */
    IGiGaSensDet* operator() ( const std::string& type ,
                               const std::string& name ) const;
    ///
  };
  
  /** @class MagFieldCreator 
   *  
   *  Helper class to make the instantiation of 
   *  IGiGaMagField object more transparent 
   *  
   *  @author Vanya Belyaev
   *  @date 23/07/2001
   */
  class MagFieldCreator: 
    public GiGaUtil::Creator , public 
  std::binary_function<const std::string&,const std::string&,IGiGaMagField*>
  {
  public:
    
    /** constructor 
     *  @param  ObjMgr   pointer to Object Manager 
     *  @param  SvcLoc   pointer to Service Locator 
     */
    MagFieldCreator( IObjManager* ObjMgr ,
                     ISvcLocator* SvcLoc )
      : Creator( ObjMgr , SvcLoc ) {};
    
    /** instantiate IGiGaSensDet object of given type and name 
     *  @param type  type of the IGiGaSensDet object
     *  @param name  name of the IGiGaSensDet object 
     *  @return pointer to new IGiGaSensDet object 
     */
    IGiGaMagField* operator() ( const std::string& type ,
                                const std::string& name ) const;
    ///
  };
  

}; ///< end of namespace 

/// ===========================================================================
#endif ///< GIGA_GIGAUTIL_H
/// ===========================================================================
