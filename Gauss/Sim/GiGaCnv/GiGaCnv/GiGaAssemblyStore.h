// $Id: GiGaAssemblyStore.h,v 1.2 2003-01-23 09:20:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:24:41  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
// 
// ============================================================================
#ifndef GIGACNV_GIGAASSEMBLYSTORE_H 
#define GIGACNV_GIGAASSEMBLYSTORE_H 1
// Include files
// STD & STL 
#include <vector>
// GaudiKernel
#include "GaudiKernel/StatusCode.h"
//  forward declaration
class GiGaAssembly ;

/** @class GiGaAssemblyStore GiGaAssemblyStore.h GiGaCnv/GiGaAssemblyStore.h
 *  
 *  (Static) storage for GiGaAssembly objects
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   19/01/2002
 */

class GiGaAssemblyStore 
{
public:
  
  /** friend 
   */
  friend class GiGaAssembly;
  
  typedef std::vector<GiGaAssembly*> Assemblies;
  
 protected:
  
  /** Standard constructor
   */
  GiGaAssemblyStore(); 
  
 public:

  /** destructor 
   */
  ~GiGaAssemblyStore();
  
 public:
  
  /** get the instance of the store
   *  return pointer to the store 
   */
  static GiGaAssemblyStore* store() ;
  
  /** get the assembly by name 
   *  @param name name of the assembly
   */
  GiGaAssembly*  assembly( const std::string& Name ) ;
  
  /** clear the store
   *  @return status code 
   */
  StatusCode     clear() ;
  
protected:
  
  /** add assembly to the store 
   *  @param assembly to be added 
   *  @return status code 
   */
  StatusCode addAssembly    ( GiGaAssembly* assembly ) ;
  
  /** remove assembly from the store 
   *  @param assembly to be removed 
   *  @return status code 
   */
  StatusCode removeAssembly ( GiGaAssembly* assembly ) ;
 
private:
  
  GiGaAssemblyStore
  ( const GiGaAssemblyStore& ) ;
  
  GiGaAssemblyStore& operator=
  ( const GiGaAssemblyStore& ) ;

private:
  
  Assemblies                  m_assemblies ;
};

// ============================================================================
// End 
// ============================================================================
#endif // GIGACNV_GIGAASSEMBLYSTORE_H
// ============================================================================
