// $Id: GiGaRunActionBase.h,v 1.4 2002-05-07 12:21:29 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_GIGARUNACTIONBASE_H 
#define GIGA_GIGARUNACTIONBASE_H 1
// ============================================================================
/// STD & STL 
#include <string> 
/// GiGa
#include "GiGa/IGiGaRunAction.h"
#include "GiGa/GiGaBase.h"

/** @class GiGaRunActionBase GiGaRunActionBase.h GiGa/GiGaRunActionBase.h
 *   
 *  The base class for implementation of GiGa Run Actions 
 * 
 *  @author Ivan Belyaev
 *  @date   26/07/2001
 */
class GiGaRunActionBase : 
  public virtual IGiGaRunAction , 
  public          GiGaBase 
{
public:
  
  /** initialization method 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode         initialize  ()        ;
  
  /** finalization method 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode         finalize    ()        ;
  
protected :
  
  /** standard constructor
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaRunActionBase
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ;
  
  /// destructor 
  virtual ~GiGaRunActionBase();
  
private:

  /// no default constructor 
  GiGaRunActionBase();
  ///  no copy constructor 
  GiGaRunActionBase( const GiGaRunActionBase& );
  /// no assignment 
  GiGaRunActionBase& operator=( const GiGaRunActionBase& );
  
private:
  
};

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_GIGARUNACTIONBASE_H
// ============================================================================
