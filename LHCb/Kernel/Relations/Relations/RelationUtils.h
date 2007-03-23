// $Id: RelationUtils.h,v 1.7 2007-03-23 16:40:17 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
#ifndef RELATIONS_RELATIONUTILS_H 
#define RELATIONS_RELATIONUTILS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"    // definition of type InterfaceID 
#include "GaudiKernel/ClassID.h"       // definition of type CLID 
// ============================================================================

namespace Relations
{
  /** Function to create the uniqie relation 
   *  interfaceID from interface name, CLIDs and types 
   *  
   *  @code
   * 
   *  InterfaceID id = Relations::interfaceID( System::typeinfoName( typeid(TYPE) ) ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   28/01/2002
   * 
   *  @param name     class name 
   *  @return unique(?) interface ID 
   */
  InterfaceID interfaceID ( const std::string& name ) ;
  
  /** Function to create the unique relation clid from the object name
   *  
   *  @code
   * 
   *  CLID id = Relations::clid( System::typeinfoName( typeid(TYPE) ) ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   28/01/2002
   * 
   *  @param name     object name
   *  @return unique(?) interface ID 
   */
  CLID        clid   ( const std::string&  name  ) ;
  
  /** Simple function to convers CLID (Gaudi unique class identifier) 
   *  to GUID (POOL unique class identifier)
   *
   *  The actual code is stollen from Markus Frank's lines 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-30
   *
   *  @param clid Gaudi unique class identifier 
   *  @return POOL unique class identifier 
   */
  std::string guid ( const CLID& clID ) ;

  /** Simple funtion to convers CLID (Gaudi unique class identifier) 
   *  to GUID (POOL unique class identifier)
   *
   *  The actual code is stollen from Markus Frank's lines 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-30
   *
   *  @param clid class name 
   *  @return POOL unique class identifier 
   */
  std::string guid ( const std::string& name ) ;
  
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
