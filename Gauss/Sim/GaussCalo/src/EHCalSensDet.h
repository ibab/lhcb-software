// $Id: EHCalSensDet.h,v 1.5 2007-03-18 21:33:19 gcorti Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2007/01/12 15:24:46  ranjard
// v9r0 - use GAUDI v19r0
//
// Revision 1.3  2006/01/17 15:52:57  odescham
// v8r0 - Adapt to new Event Model & LHCb v20 migration
//
// Revision 1.2  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.1  2003/07/07 08:21:08  ibelyaev
//  split the general CaloSensDet class
//
// ============================================================================
#ifndef       GAUSSCALO_EHCalSensDet_H
#define       GAUSSCALO_EHCalSensDet_H 1 
// ============================================================================
// GaudiKernel
/// Ntupel Svc 
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/GiGaHashMap.h"
/// local
#include "CaloSim.h"
#include "CaloHit.h"
#include "CaloSubHit.h"
#include "CaloSensDet.h"
/// forward declarations
class DeCalorimeter ;

/** @class EHCalSensDet EHCalSensDet.h
 *
 *  The common base class fro Ecal/Hcal sensitive detectors.
 *  It includes the common timing treatment. 
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Patrick Robbe robbe@lal.in2p3.fr 
 *
 *  @date    23/01/2001
 */

class EHCalSensDet: public CaloSensDet 
{ 
public: 
  
  /** standard constructor 
   *  @see CaloSensDet 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  EHCalSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~EHCalSensDet() {};
  
private:
  
  // no default constructor 
  EHCalSensDet() ; 
  // no copy constructor
  EHCalSensDet           ( const EHCalSensDet& ); 
  // no assignement  
  EHCalSensDet& operator=( const EHCalSensDet& ) ;
  
protected:  
  
  /** The fractions of energy deposited in consequitive time-bins 
   *  in the given Ecal/Hcal cell
   *  @see CaloSensDet
   *  @param time global time of energy deposition
   *  @param cell cellID of the cell 
   *  @param slot (out) the first time bin 
   *  @param fracs (out) the vector of fractions for subsequent time-slots;
   *  @return StatuscCode 
   */
  virtual StatusCode timing 
  ( const double            time      , 
    const LHCb::CaloCellID&       cell      ,
    CaloSubHit::Time&       slot      ,
    CaloSensDet::Fractions& fractions ) const ;
  
protected:
  
  /// width of time-slot (25ns)
  inline double slotWidth() const { return m_slotWidth ; }
  
private:
  
  /// width of time-slot (25ns) 
  double  m_slotWidth ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GAUSSCALO_EHCalSensDet_H
// ============================================================================
