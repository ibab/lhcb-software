// $Id: CaloCorrection.h,v 1.1.1.1 2002-11-13 20:46:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/07 18:15:00  ibelyaev
//  preliminary version ('omega'-release)
// 
// ============================================================================
#ifndef CALOTOOLS_CALOCORRECTION_H 
#define CALOTOLLS_CALOCORRECTION_H 1
// Include files
// STD & STL
#include <string>
#include <math.h>
// CaloInterfaces 
#include "CaloInterfaces/ICaloCorrection.h"
// CaloKernel
#include "CaloKernel/CaloTool.h"

/** @class CaloCorrection CaloCorrection.h
 *  
 *  The simple base class for all "corrections" 
 * 
 *  @see AlgTool
 *  @see CaloTool
 *  @see ICaloCorrection
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   22/03/2002
 */
class CaloCorrection : 
  public virtual ICaloCorrection ,  
  public                CaloTool 
{
public:
  
  /// typedef for parameters 
  typedef std::vector<double> Parameters;
  
  /**   defintion of "Function" type!
   *
   *  - The first argument   is the uncorrected position of cluster  
   *  - The second arguments is position of the center of SEED cell
   *  - The third argument   is the SEED cell size 
   *  - The last argiment    is the function parametrisation 
   */
  typedef double (*Function) 
    ( const double       /* cluster position */ , 
      const double       /* seed center      */ , 
      const double       /* seed cell size   */ ,
      const Parameters&  /* parameters       */ );
  
protected:
  
  /** Standard constructor
   *  @param  type   type of the tool (?)
   *  @param  name   the name of this concrete instance 
   *  @param  parent the tool's parent
   *  @param  fx     function for correction of x-coordinate 
   *  @param  fxp    derivative of function for correction of x-coordinate 
   *  @param  fy     function for correction of y-coordinate 
   *  @param  fyp    derivative of function for correction of y-coordinate    
   */
  CaloCorrection( const std::string& type          ,  
                  const std::string& name          ,
                  const IInterface*  parent        ,
                  Function           fx     = 0    ,
                  Function           fxp    = 0    , 
                  Function           fy     = 0    ,
                  Function           fyp    = 0    );
  
  /// destructor, virtual of course 
  virtual ~CaloCorrection(); ///< Destructor
  
public:
  
  /** The main processing method 
   *  @see ICaloCorrection
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @param  corrected new created corrected CaloPosition object 
   *  @return status code 
   */  
  virtual StatusCode     process    
  ( const CaloPosition & position  , 
    const HepPoint3D   & seed      , 
    CaloPosition*&       corrected ) const ;
  
  /** The main processing method 
   *  @see ICaloCorrection
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @return new created corrected CaloPosition object 
   */  
  virtual CaloPosition* process    
  ( const CaloPosition & position , 
    const HepPoint3D   & seed     ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloCorrection
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @return new created corrected CaloPosition object 
   */  
  virtual CaloPosition* operator() 
    ( const CaloPosition & position , 
      const HepPoint3D   & seed     ) const ;
  
  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode initialize() ;

public:
  
  /** accessor to the function for x-correction 
   *  @return x-correction function 
   */
  Function         fx  () const { return m_fx ; }

  /** accessor to the function for y-correction 
   *  @return y-correction function 
   */
  Function         fy  () const { return m_fy ; }

  /** accessor to the derivative of function for x-correction 
   *  @return derivative of x-correction function 
   */
  Function         fxp () const { return m_fxp ; }

  /** accessor to the derivative of function for y-correction 
   *  @return derivative of y-correction function 
   */
  Function         fyp () const { return m_fyp ; }
  
  /** accessor to the cell size 
   *  @return cell size 
   */
  const double     cell() const  { return m_cell ; }
  
  /** accessor to the parameters of x-correction function
   *  @return parameters of x-correction function
   */
  const Parameters xpars() const { return m_xpars ; }
  
  /** accessor to the parameters of y-correction function
   *  @return parameters of y-correction function
   */
  const Parameters ypars() const { return m_ypars ; }
  
protected:

  Function  setFx  ( Function fun ) { m_fx  = fun ; return m_fx  ; }
  Function  setFy  ( Function fun ) { m_fy  = fun ; return m_fy  ; }
  Function  setFxp ( Function fun ) { m_fxp = fun ; return m_fxp ; }
  Function  setFyp ( Function fun ) { m_fyp = fun ; return m_fyp ; }

private:
  
  /// default constructor  is private 
  CaloCorrection();
  /// copy    constructor  is private 
  CaloCorrection             ( const CaloCorrection& );
  /// assignement operator is private 
  CaloCorrection& operator = ( const CaloCorrection& );
  
private:
  
  // parametrisation of x-function 
  Parameters            m_xpars   ; ///< paramterisation of x-function 
  // parametrisation of y-function 
  Parameters            m_ypars   ; ///< paramterisation of y-function 
  // cell size
  double                m_cell    ; ///< cell size
  
  // x-correction function
  Function              m_fx      ;  ///< x-correction function
  // x-correction function derivative 
  Function              m_fxp     ;  ///< x-correction function derivative 
  // y-correction function
  Function              m_fy      ;  ///< y-correction function
  // y-correction function derivative 
  Function              m_fyp     ;  ///< y-correction function derivative
  
  // new Z-position 
  double                m_z       ;
  // bool 
  double                m_updateZ ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTOOLS_CALOSCORRECTION_H
// ============================================================================
