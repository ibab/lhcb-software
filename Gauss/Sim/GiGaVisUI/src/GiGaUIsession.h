// $Id: GiGaUIsession.h,v 1.1.1.1 2002-12-12 14:46:26 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/12/04 21:17:38  ibelyaev
//  add UI and Vis stuff
// 
// ============================================================================
#ifndef GIGA_GIGAUISESSION_H 
#define GIGA_GIGAUISESSION_H 1
// Include files
#include "GiGa/IGiGaUIsession.h"
#include "GiGa/GiGaBase.h"
// forward declarations
template <class TYPE> class GiGaFactory;

/** @class GiGaUIsession GiGaUIsession.h 
 *
 *  Simple implementation of IGiGaUIsession interface 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-04
 */

class GiGaUIsession : 
  public virtual IGiGaUIsession , 
  public          GiGaBase      
{
  /// friend factory for instantiation
  friend class GiGaFactory<GiGaUIsession>;
  
public:

  /** initialization method 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         initialize  ()        ;
  
  /** get the pointer to G4 user interface
   *  @see IGiGaUIsession
   *  @return pointer to G4 user interface
   */
  virtual G4UIsession* session () const ;
  
protected:
  
  /** standard constructor 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaUIsession
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaUIsession() ;

private:
  
  /// the default constructor is private 
  GiGaUIsession();
  /// copy constructor is private 
  GiGaUIsession           ( const GiGaUIsession& );
  /// assignement operator is private 
  GiGaUIsession& operator=( const GiGaUIsession& );
  
  
private:
  
  typedef std::vector<std::string>  Sessions   ;
  
  Sessions                          m_sessions ;
  G4UIsession*                      m_session  ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAUISESSION_H
// ============================================================================
