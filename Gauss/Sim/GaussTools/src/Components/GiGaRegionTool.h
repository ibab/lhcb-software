// $Id: GiGaRegionTool.h,v 1.2 2007-01-12 15:36:53 ranjard Exp $
#ifndef GAUSSTOOLS_GIGAREGIONTOOL_H 
#define GAUSSTOOLS_GIGAREGIONTOOL_H 1

// Include files
#include "GiGa/GiGaToolBase.h"


/** @class GiGaRegionTool GiGaRegionTool.h Components/GiGaRegionTool.h
 *
 *  The simple tool that creates and configures the 'Region' 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-05-30
 */
class GiGaRegionTool : public GiGaToolBase 
{
  // friend factory for instantiation 
  //  friend class GiGaFactory<GiGaRegionTool>;  
public:
  
  /** the only one method
   *  @see GiGaToolBase
   *  @see IGiGaTool
   *  @param region region name  
   *  @return status code 
   */
  virtual StatusCode process ( const std::string& region  = "" ) const ;
  
  //protected:
  
  /** standard constructor
   *  @see GiGaToolBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaRegionTool
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ;
  
  /// virtual destructor 
  virtual ~GiGaRegionTool(){};
  
protected:

  // default constructor  is disabled 
  GiGaRegionTool();
  // copy    constructor  is disabled 
  GiGaRegionTool           ( const GiGaRegionTool& ) ;
  // assignement operator is disabled 
  GiGaRegionTool& operator=( const GiGaRegionTool& ) ;
  
private:

  typedef std::vector<std::string> Volumes ;

  std::string  m_region  ;
  Volumes      m_volumes ;

  // cut for gammas 
  double m_gamma     ;
  // cut for electrons 
  double m_electron  ;
  // cut for positrons  
  double m_positron  ;
  
  bool   m_overwrite ;
};

// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENTS_GIGAREGIONTOOL_H
// ============================================================================
