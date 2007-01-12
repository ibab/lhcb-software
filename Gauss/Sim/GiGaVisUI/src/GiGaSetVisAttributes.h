// $Id: GiGaSetVisAttributes.h,v 1.2 2007-01-12 15:48:35 ranjard Exp $
#ifndef GIGAVISUI_GIGASETVISATTRIBUTES_H 
#define GIGAVISUI_GIGASETVISATTRIBUTES_H 1

// Include files
// GiGa 
#include "GiGa/GiGaToolBase.h"
// VisSvc
class IVisualizationSvc ;
class ILVolume          ;
class G4LogicalVolume   ;


/** @class GiGaSetVisAttributes GiGaSetVisAttributes.h
 *
 *  The simplest tool which set the visualisation attributes 
 *  for G4 visualization
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class GiGaSetVisAttributes : public GiGaToolBase 
{
  // friend factory for instantiation 
  //  friend class GiGaFactory<GiGaSetVisAttributes>;  
public:
  
  /** the only one method 
   *  @see IGiGaTool
   *  @param volume volume name 
   *  @return status code 
   */
  virtual StatusCode process ( const std::string& volume ) const ;
  
  /** initialize the object 
   *  @see GiGaToolBase
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize  () ;
  
  /** finalize the object  
   *  @see GiGaToolBase
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize    () ;
  
  //protected:
  
  /** standard constructor
   *  @see GiGaToolBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaSetVisAttributes
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ; 
  
  /// destructor 
  virtual ~GiGaSetVisAttributes() ;

protected:
  
  /** get ILVolume by its address
   *  @return ILVolume object 
   */
  const ILVolume*  volume
  ( const std::string& address ) const ;

  /** set visualization attribute for the volume 
   *  @param  vol pointer to volume 
   *  @return statsu code 
   */
  StatusCode       setVisAttributes
  ( const ILVolume*    vol ) const ;
  
  /** find g4 volume by name 
   *  @param name name of the volume 
   */
  G4LogicalVolume* g4volume         
  ( const std::string& adddress ) const ;
  
protected:
  
  /** accessor to visualization service 
   */ 
  IVisualizationSvc* visSvc() const { return m_visSvc ; }
  
private:
  
  std::string                      m_visSvcName ;
  IVisualizationSvc*               m_visSvc     ;
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // GIGAVISUI_GIGASETVISATTRIBUTES_H
// ============================================================================
