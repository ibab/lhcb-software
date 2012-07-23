// $Id: IGiGaCnvSvc.h,v 1.5 2004-02-20 19:27:26 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/12/07 14:36:25  ibelyaev
//  see $GIGACNVROOT/doc/release.notes
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

class IGiGaSvc               ;
class IGiGaSetUpSvc          ;
namespace LHCb {
  class IParticlePropertySvc   ;
}
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
  static const InterfaceID& interfaceID()  ;
  
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

protected:
  
  /** virtual desctructor 
   */
  virtual ~IGiGaCnvSvc() ; 
  
};

// ============================================================================
#endif ///<  GIGACNV_IGIGACNVSVC_H 
// ============================================================================




