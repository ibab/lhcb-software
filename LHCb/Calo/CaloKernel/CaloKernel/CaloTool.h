// $Id: CaloTool.h,v 1.14 2004-02-17 11:55:55 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOKERNEL_CALOTOOL_H 
#define CALOKERNEL_CALOTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiTools
// ============================================================================
#include "GaudiTools/GaudiTool.h"
// ============================================================================

// forward declarations 
template <class TYPE>
class ToolFactory      ;
class IChronoStatSvc   ;
class IToolSvc         ;
class DeCalorimeter    ;
class IDataProviderSvc ;

/** @class CaloTool CaloTool.h CaloKernel/CaloTool.h
 *   
 *  Useful base class for all Calorimeter tools  
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   01/11/2001
 */

class CaloTool: public GaudiTool
{  
public:  
  
  /** standard initialization method 
   *  @see GaudiTool 
   *  @see   AlgTool 
   *  @see  IAlgTool
   *  @return status code 
   */
  virtual StatusCode    initialize ();
  
  /** standard finalization method 
   *  @see GaudiTool 
   *  @see   AlgTool 
   *  @see  IAlgTool
   *  @return status code 
   */
  virtual StatusCode    finalize   ();
  
protected:
  
  /** Standard constructor
   *  @see AlgTool
   *  @param type tool type (useless)
   *  @param name tool name
   *  @param parent pointer to parent object (service, algorithm or tool)  
   */
  CaloTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ); 
  
  /// destructor, virtual and protected 
  virtual ~CaloTool();
  
protected:
  
  /** accessor to associated calorimeter detector  (const version)
   *  @return pointer to associated calorimeter detector 
   */
  inline const DeCalorimeter* det       () const { return m_det ;}
  
  /** set new detector 
   *  @param Det pointer to new calorimeter detector
   */
  inline const DeCalorimeter* setDet ( const DeCalorimeter* Det )
  { m_det = Det ; return det() ; }
  
  /** accessor to detector name 
   *  @return name of the detector
   */
  inline const std::string&   detName() const { return m_detName; }
  
  /** set new name of the detector 
   *  @param newV new detector name
   */
  inline const std::string&   setDetName( const std::string& newV ) 
  { m_detName = newV ; return detName() ; }
  
private:
  
  /// default constructor  is private!
  CaloTool();
  /// copy constructor     is private!
  CaloTool
  ( const CaloTool& copy );
  /// assignement operator is private!
  CaloTool& operator=
  ( const CaloTool&  );
  
private:
  
  const DeCalorimeter*   m_det       ;
  std::string            m_detName   ;
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CALOKERNEL_CALOTOOL_H
// ============================================================================
