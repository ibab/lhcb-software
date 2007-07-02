// $Id: GiGaAssembly.h,v 1.4 2007-07-02 08:48:54 gcorti Exp $
#ifndef GIGACNV_GIGAASSEMBLY_H 
#define GIGACNV_GIGAASSEMBLY_H 1

// Include files
// STD & STL 
#include  <vector>
#include  <string>

// GaudiKernel
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/StatusCode.h" 

// GiGaCnv
#include "GiGaCnv/GiGaVolumePair.h"

/** @class GiGaAssembly GiGaAssembly.h GiGaCnv/GiGaAssembly.h
 *  
 *  GiGa representation of Logical Assembly  
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   17/01/2002
 */

class GiGaAssembly
{
  
public:
  
  /** useful typedefs 
   */
  typedef std::pair<G4LogicalVolume*,std::string>  NickV  ;
  //  typedef std::pair<NickV,HepTransform>          Volume ;
  typedef std::pair<NickV,Gaudi::Transform3D>          Volume ;
  //  typedef std::pair<G4LogicalVolume*,HepTransform3D> Volume  ;
  typedef std::vector<Volume>                      Volumes ;
  
public:
  
  /** Standard constructor
   *  @param assembly name
   */
  GiGaAssembly  ( const std::string& Name = "" );

  /** copy constructor 
   *  @param right object to be copied 
   */
  GiGaAssembly  ( const GiGaAssembly& right    );
  
  /// destructor 
  virtual ~GiGaAssembly (); 
  
  /** accessor to internal structure - volumes
   *  @return volumes 
   */
  inline const Volumes& volumes () const { return m_volumes ; }
  
  /** accessor to the assembly name 
   *  @return name of the assembly
   */
  const std::string& name() const { return m_name ; }
  
  /** set/reset name for the assembly 
   *  @param value  new name for the assembly 
   */
  inline void setName( const std::string& value = "" ) { m_name = value ; }
  
  /** add "volume" to the assembly. 
   *  @param value the pair to be added 
   *  @param name  pair name to be appended 
   *  @return status code 
   */
  StatusCode  addVolume( const GiGaVolumePair& value      , 
                         const std::string&    name  = "" ) ;

private:
  
  std::string m_name    ;
  Volumes     m_volumes ;
  
};

// ============================================================================
// End 
// ============================================================================
#endif // GIGACNV_GIGAASSEMBLY_H
// ============================================================================
