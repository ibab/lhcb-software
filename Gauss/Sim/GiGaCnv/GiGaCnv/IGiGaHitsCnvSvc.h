// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/08/12 17:24:50  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.2  2001/07/25 17:19:31  ibelyaev
/// all conversions now are moved from GiGa to GiGaCnv
///
/// Revision 1.1  2001/07/24 11:13:54  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
#ifndef GIGACNV_IGIGAHITSCNVSVC_H
#define GIGACNV_IGIGAHITSCNVSVC_H 1 
// ============================================================================

/// GiGa 
/// GiGaCnv 
#include "GiGaCnv/IGiGaCnvSvc.h" 
/// forward declaration
class GiGaKineRefTable;

/** @class IGiGaHitsCnvSvc     IGiGaHitsCnvSvc.h GiGa/IGiGaHitsCnvSvc.h
 *
 *  definition of abstract interface to Geant 4 conversion service 
 *  which is responsible for conversion from Geant4 hits into
 *  Gaudi Hits 
 *  
 *  @author Vanya Belyaev
*/


class IGiGaHitsCnvSvc: virtual public IGiGaCnvSvc
{
  ///
 public:

  /** retrieve the unique interface identifier 
   *  @return unique interface identifier 
   */
  static const InterfaceID& interfaceID() ;
  
  /** retrieve the relation table between Geant4 track/trajectory 
   *  identifiers and the converted MCParticle objects 
   *  @return the reference to relation table  
   */
  virtual GiGaKineRefTable&      table() = 0 ;

protected: 
  
  virtual ~IGiGaHitsCnvSvc() ; 
  ///
};
///


// ============================================================================
#endif   //  GIGACNV_IGIGAHITSCNVSVC_H
// ============================================================================
