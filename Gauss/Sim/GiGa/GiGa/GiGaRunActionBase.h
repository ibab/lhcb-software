/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef GIGA_GIGARUNACTIONBASE_H 
#define GIGA_GIGARUNACTIONBASE_H 1
/// ===========================================================================
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
class GiGaRunActionBase : virtual public IGiGaRunAction , 
                          public          GiGaBase 
{
public:
  
  /** identification 
   *  @return name of concrete inteface instance 
   */
  virtual const std::string& name        () const  ;
  
  /** initialization method 
   *  @return status code 
   */
  virtual StatusCode         initialize  ()        ;
  
  /** finalization method 
   *  @return status code 
   */
  virtual StatusCode         finalize    ()        ;
  
  /** perform begin-of-run action
   *  @param run  pointer to Geant4 Run object
   */
  virtual void BeginOfRunAction ( const G4Run* run );
  
  /** perform end-of-run action
   *  @param run  pointer to Genat4 Run object
   */
  virtual void EndOfRunAction   ( const G4Run* run );
  
protected :
  
  /** standard constructor
   *  @param Name name of this concrete instance of Run Action Object 
   *  @param Loc  pointer to Service Locator 
   */
  GiGaRunActionBase( const std::string Name ,
                     ISvcLocator*      Loc  ); 
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

/// ===========================================================================
#endif ///< GIGA_GIGARUNACTIONBASE_H
/// ===========================================================================
