// $Id: IGiGaCnvSvc.h,v 1.3 2002-01-22 18:24:42 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/08/12 17:24:50  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.1  2001/07/25 17:19:30  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.10  2001/07/23 13:11:44  ibelyaev
// the package restructurisation(II)
// 
// ============================================================================
#ifndef           GIGACNV_IGIGACNVSVC_H 
#define           GIGACNV_IGIGACNVSVC_H  1 
// ============================================================================
// STL 
#include <vector> 
// GaudiKernel
#include "GaudiKernel/IConversionSvc.h" 
#include "GaudiKernel/IService.h" 
//
#include "GiGaCnv/IIDIGiGaCnvSvc.h"
//
class IGiGaSvc               ;
class IGiGaSetUpSvc          ;
class IParticlePropertySvc   ;
class GiGaLeaf               ;
//

/** @class IGiGaCnvSvc IGiGaCnvSvc.h GiGa/IGiGaCnvSvc.h
 *
 *  definition of abstract interface to Geant 4 conversion service 
 *  which is responsible for conversion from/to  
 *  Gaudi representation to GEANT4 primary event structures 
 *  
 *  @author Vanya Belyaev
 */

class IGiGaCnvSvc: virtual public IConversionSvc ,
                   virtual public IService   
{  
public: 
  
  /** Retrieve unique interface ID
   *  @return unique interface ID 
   */
  static const InterfaceID& interfaceID() { return IID_IGiGaCnvSvc; }
  
  /** get accesor to GiGa service 
   *  @return pointer to GiGa Service 
   */
  virtual IGiGaSvc*      gigaSvc  ()  const = 0 ;  
  
  /** get accesor to GiGa SetUp service 
   *  @return pointer to GiGa SetUp Service 
   */
  virtual IGiGaSetUpSvc* setupSvc ()  const = 0 ; 
  
  /** declare the object/converter to conversion service  
   *  @param leaf object/converter parametres 
   */
  virtual StatusCode     declareObject( const GiGaLeaf& leaf ) = 0 ;
  
  /** register all declared leaves 
   *  @return status code 
   */
  virtual StatusCode     registerGiGaLeaves() = 0 ;
  
  /** virtual desctructor 
   */
  virtual ~IGiGaCnvSvc(){}; 
  
};

// ============================================================================
#endif ///<  GIGACNV_IGIGACNVSVC_H 
// ============================================================================




