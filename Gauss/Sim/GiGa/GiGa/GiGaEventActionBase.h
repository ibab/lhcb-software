// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/07/23 13:11:41  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef     GIGA_GiGaEventActionBase_H
#define     GIGA_GiGaEventActionBase_H 1 
// ============================================================================

// base class 
#include "GiGa/IGiGaEventAction.h" 
#include "GiGa/GiGaBase.h" 

class G4EventManager;
class G4Event; 

/** @class GiGaEventActionBase GiGaEventActionBase.h GiGa/GiGaEventActionBase.h
 *
 *  Base class for implementation of concrete Event Action for GiGa
 *  
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaEventActionBase: virtual public IGiGaEventAction   ,
                           public  GiGaBase
{
  ///
protected:
  /// constructor 
  GiGaEventActionBase( const std::string& , ISvcLocator* );
  /// virtual destructor 
  virtual ~GiGaEventActionBase();
  ///
public:
  /// ident 
  virtual const std::string& name       () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual StatusCode  initialize () ; 
  /// finalize 
  virtual StatusCode  finalize   () ; 
  /// Query Interface
  virtual StatusCode  queryInterface ( const InterfaceID& , void** ) ;
  ///
public:
  ///
  virtual void BeginOfEventAction ( const G4Event* );
  virtual void EndOfEventAction   ( const G4Event* );
  ///
protected: 
  ///
  inline const G4EventManager*   evtMgr() const; 
  ///
private:
  ///
  GiGaEventActionBase () ; ///< no default constructor!                   
  GiGaEventActionBase ( const GiGaEventActionBase& ); ///< no copy!
  GiGaEventActionBase& operator=( const GiGaEventActionBase& ); ///< no = ! 
  ///
private:
  ///
};
///
#include "GiGa/GiGaEventActionBase.icpp"
///

// ============================================================================
#endif   // GIGA_GiGaEventActionBase_H
// ============================================================================














