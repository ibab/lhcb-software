// $Id: RelationUtils.h,v 1.1.1.1 2004-07-21 07:57:26 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RELATIONUTILS_H 
#define RELATIONS_RELATIONUTILS_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL
#include <string>
#include <map>
// GaudiKernel
#include "GaudiKernel/IInterface.h"    // definition of type InterfaceID 
#include "GaudiKernel/ClassID.h"       // definition of type CLID 

namespace Relations
{
  /**
   *  funtion to create the uniqie relation 
   *  interfaceID from interface name, CLIDs and types 
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   28/01/2002
   * 
   *  @param name     interface name
   *  @param idFrom   ID for the first ("FROM") object  
   *  @param idTo     ID for the second("TO")   object
   *  @param weight   ID of weight class 
   *  @param major    interface major version 
   *  @param minor    interface minor version 
   */
  InterfaceID interfaceID ( const std::string&  id           ,
                            const unsigned long idFrom       ,
                            const unsigned long idTo         ,
                            const unsigned long weight  = 0  ,
                            const unsigned long major   = 0  , 
                            const unsigned long minor   = 0  ) ;
  
  /**
   *  function to create the uniqie relation 
   *  clid from object name, CLIDs and types 
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   28/01/2002
   * 
   *  @param name     object name
   *  @param idFrom   ID for the first ("FROM") object  
   *  @param idTo     ID for teh second("TO")   object
   *  @param weight   ID of weight class 
   *  @param major    major version 
   *  @param minor    minor version 
   */
  CLID        clid        ( const std::string&  id           ,
                            const unsigned long idFrom   = 0 ,
                            const unsigned long idTo     = 0 ,
                            const unsigned long weight   = 0 , 
                            const unsigned long major    = 0 ,
                            const unsigned long minor    = 0 ) ;

  ///
  class InstanceCounter
  {
  public:
    typedef long                          counter  ;
    typedef std::map<std::string,counter> Counters ;
  public:
    
    /// the accessor to static istance 
    static InstanceCounter& instance() ;

    /** increment the counter
     *  @param type object type 
     *  @return the current value of counter
     */
    counter increment ( const std::string& type )       ;
    
    /** increment the counter
     *  @param type object type 
     *  @return the current value of counter
     */
    counter decrement ( const std::string& type )       ;
    
    /** get the current value of the counter
     *  @param type object type 
     *  @return the current value of counter
     */
    counter count     ( const std::string& type ) const ;
    
    /** make a report 
     *  @return total number  
     */
    counter report    () const ;

  protected:
    /// the default constructor is private 
    InstanceCounter();
    /// the copy constructor is private 
    InstanceCounter( const InstanceCounter& );
  public:
    /// the destructor is public ( due to Win32) 
    ~InstanceCounter();
  private:
    mutable Counters m_counters ;
  };


}; // end of namespace Relations

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RELATIONUTILS_H
// ============================================================================
