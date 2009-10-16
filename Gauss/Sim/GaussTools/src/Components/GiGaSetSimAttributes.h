// $Id: GiGaSetSimAttributes.h,v 1.4 2009-10-16 15:02:00 silviam Exp $
#ifndef GAUSSTOOLS_GIGASETSIMATTRIBUTES_H 
#define GAUSSTOOLS_GIGASETSIMATTRIBUTES_H 1

// Include files
#include "GiGa/GiGaToolBase.h"
// SimSvc
class ISimulationSvc    ;
class G4LogicalVolume   ;
class GaussG4USerLimits ;
// ============================================================================

/** @class GiGaSetSimAttributes GiGaSetSimAttributes.h
 *  
 *  The simplest tool which attach the simulation attributes 
 *  for G4 Logical Volume
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class GiGaSetSimAttributes : public GiGaToolBase 
{
  // friend factory for instantiation 
  //  friend class GiGaFactory<GiGaSetSimAttributes>;  
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
  GiGaSetSimAttributes
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ; 
  
  /// destructor
  virtual ~GiGaSetSimAttributes( ); 

protected:

  /** accessor to simulation service
   *  @return pointer to simulation service
   */
  ISimulationSvc* simSvc() const  { return m_simSvc ; }
  
  /** find g4 volume by name 
   *  @param name name of the volume 
   */
  G4LogicalVolume* g4volume         
  ( const std::string& adddress ) const ;

  /** set user Limits for the given logical volume 
   *  and propagate it to all daughetr volumes 
   *  @param lv logicla volume 
   *  @param ul user limits 
   *  @return status code 
   */
  StatusCode setUserLimits 
  ( G4LogicalVolume*         lv , 
    const GaussG4UserLimits& ul ) const ;
  
  // policy for overwtiting the existing limits 
  inline bool overwrite() const { return m_overwrite ; }

private:
  
  std::string     m_simSvcName ;
  ISimulationSvc* m_simSvc     ;
  
  // flag to overwrite the existing limits 
  bool            m_overwrite  ;
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // GAUSSTOOLS_GIGASETSIMATTRIBUTES_H
// ============================================================================
