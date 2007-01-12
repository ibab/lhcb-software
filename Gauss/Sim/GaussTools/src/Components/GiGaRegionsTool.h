// $Id: GiGaRegionsTool.h,v 1.2 2007-01-12 15:36:54 ranjard Exp $
#ifndef GAUSSTOOLS_GIGAREGIONSTOOL_H 
#define GAUSSTOOLS_GIGAREGIONSTOOL_H 1
// Include files
#include "GiGa/GiGaToolBase.h"

// forward declarations 
class ISimulationSvc ;

/** @class GiGaRegionsTool GiGaRegionsTool.h Components/GiGaRegionsTool.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-06-04
 */

class GiGaRegionsTool : public GiGaToolBase 
{
  // friend factory for instantiation 
  //  friend class GiGaFactory<GiGaRegionsTool>;  
public:
  
  /** the only one method
   *  @see GiGaToolBase
   *  @see IGiGaTool
   *  @param region region name  
   *  @return status code 
   */
  virtual StatusCode process ( const std::string& region  = "" ) const ;
  
  /** standard initialization of the tool 
   *  @see  GiGaToolBase
   *  @see  GiGaBase
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @return status code
   */
  virtual StatusCode initialize () ;
  
  /** standard finalization of the tool 
   *  @see  GiGaToolBase
   *  @see  GiGaBase
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @return status code
   */
  virtual StatusCode finalize   () ;

  //protected:
  
  /** standard constructor
   *  @see GiGaToolBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaRegionsTool
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ;
  
  /// virtual destructor 
  virtual ~GiGaRegionsTool(){};

protected:
  
  /// accessor to Simulation Service 
  inline const ISimulationSvc* simSvc() const { return m_simSvc ; }
  
private:
  
  // default constructor  is disabled 
  GiGaRegionsTool();
  // copy    constructor  is disabled 
  GiGaRegionsTool           ( const GiGaRegionsTool& ) ;
  // assignement operator is disabled 
  GiGaRegionsTool& operator=( const GiGaRegionsTool& ) ;

private:
  
  std::string     m_simSvcName ;
  ISimulationSvc* m_simSvc     ;

  bool            m_overwrite  ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENTS_GIGAREGIONSTOOL_H
// ============================================================================
