// $Id: GiGaSensDetBase.h,v 1.8 2004-04-20 04:26:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $  
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/07 12:21:30  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef     GIGA_GiGaSensDetBase_H
#define     GIGA_GiGaSensDetBase_H 1 
// ============================================================================
/// GiGa
#include "GiGa/IGiGaSensDet.h" 
#include "GiGa/GiGaBase.h" 
///

/** @class GiGaSensDetBase GiGaSensDetBase.h GiGa/GiGaSensDetBase.h
 *
 *  Base class for implementation of concrete Sensitive Detector for GiGa
 *  
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaSensDetBase: virtual public IGiGaSensDet ,
                       public          GiGaBase
{
  ///
protected:
  
  /** standard constructor   
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaSensDetBase ( const std::string& type   , 
                    const std::string& name   , 
                    const IInterface*  parent );
  /// virtual destructor 
  virtual ~GiGaSensDetBase();
  ///
public:

  /** initialize the sensitive detector  
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         initialize () ; 

  /** finalize the sensitive detector  
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         finalize   () ; 

public:

  virtual unsigned long release() ;

protected:
  ///
  GiGaSensDetBase           ( const GiGaSensDetBase& ); /// no copy constructor!
  GiGaSensDetBase& operator=( const GiGaSensDetBase& ); /// no assignment! 
  ///
private:
  ///
  bool                m_active  ;  ///< Active Flag
  std::string         m_detPath ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif   // GIGA_GiGaSensDet_H
// ============================================================================














