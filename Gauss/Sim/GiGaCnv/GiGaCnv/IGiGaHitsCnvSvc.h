/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef GIGACNV_IGIGAHITSCNVSVC_H
#define GIGACNV_IGIGAHITSCNVSVC_H 1 
/// ===========================================================================

/// GiGa 
#include "GiGa/IGiGaCnvSvc.h" 
/// GiGaCnv 
#include "GiGaCnv/IIDIGiGaHitsCnvSvc.h"
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
  static const InterfaceID& interfaceID() { return IID_IGiGaHitsCnvSvc; }
  
  /** retrieve the relation table between Geant4 track/trajectory 
   *  identifiers and the converted MCParticle objects 
   *  @return the reference to relation table  
   */
  virtual GiGaKineRefTable&      table() = 0 ;
  
  virtual ~IGiGaHitsCnvSvc(){} ; 
  ///
};
///


/// ===========================================================================
#endif   //  GIGACNV_IGIGAHITSCNVSVC_H
/// ===========================================================================
