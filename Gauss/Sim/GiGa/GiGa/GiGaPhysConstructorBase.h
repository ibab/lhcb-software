// ============================================================================
#ifndef       GIGA_GiGaPhysConstructorBase_H
#define       GIGA_GiGaPhysConstructorBase_H 1 
// ============================================================================
/// base classes 
#include "GiGa/IGiGaPhysConstructor.h"
#include "GiGa/GiGaBase.h"


class GiGaPhysConstructorBase: 
  public virtual IGiGaPhysConstructor   , 
  public          GiGaBase
{
protected:
  
  /** standard constructor
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaPhysConstructorBase( const std::string& type   , 
                    const std::string& name   , 
                    const IInterface*  parent ) ;
  ///
  virtual ~GiGaPhysConstructorBase();

public:
  
  /** initialize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize     () ; 
  
  /** finalize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize       () ; 
  
  ///
private:
  ///
  GiGaPhysConstructorBase() ; ///< no default constructor 
  GiGaPhysConstructorBase           ( const GiGaPhysConstructorBase& ) ; //< no copy
  GiGaPhysConstructorBase& operator=( const GiGaPhysConstructorBase& ) ; //< no assignment  
  ///

};
// ============================================================================
 
// ============================================================================
#endif    ///<  GIGA_GiGaPhysConstructorBase_H
// ============================================================================





