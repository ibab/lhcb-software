// $Id: IPhotonFromMergedParams.h,v 1.1 2003-04-25 18:17:33 gcorti Exp $
// ====================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ====================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/01/22 16:43:22  ibelyaev
//  new tools for PhotonFromMergeds
// 
// ===================================================
#ifndef DAVINCITOOLS_IPHOTONFROMMERGEDPARAMS_H 
#define DAVINCITOOLS_IPHOTONFROMMERGEDPARAMS_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// DaVinciTools 
#include "DaVinciTools/IIDIPhotonFromMergedParams.h"
// forward declarations 
class Particle     ;
class PrimVertex   ;
class Vertex       ;
class HepPoint3D   ;
class HepSymMatrix ;

/** @class IPhotonFromMergedParams IPhotonFromMergedParams.h 
 * DaVinciTools/IPhotonFromMergedParams.h
 *  
 *  An abstract interface for (re)evaluation of photon parameters 
 *  using the knowledge of the photon production vertex 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2003-01-19
 */
class IPhotonFromMergedParams : public virtual IAlgTool                          
{  
public:
  /** static interface identification 
   *  @see IInterface
   *  @return unique interface identifier
   */
  static const InterfaceID&interfaceID(){return IID_IPhotonFromMergedParams;}
  
  /** (Re)evaluate the photon's paramters using the knowledge 
   *  of the primary vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the primary vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const CaloHypo*      thehypo,
                               const PrimVertex*    vertex ) const = 0;
  
  /** (Re)evaluate the photon's paramters using the knowledge 
   *  of photon production  vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the ptoton's production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const CaloHypo*      thehypo,
                               const Vertex*        vertex ) const = 0;
  
  /** (Re)evaluate the photon's parameters using the knowledge 
   *  of photon production vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  point   position of the photon production vertex 
   *  @param  error   covariance matrix of photon production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const CaloHypo*      thehypo,
                               const HepPoint3D&    point  , 
                               const HepSymMatrix&  error) const = 0;
  
protected:
  
  /// destructor 
  virtual ~IPhotonFromMergedParams(){};

};

// =================================================
// The END 
// =================================================
#endif // DAVINCITOOLS_IPHOTONFROMMERGEDPARAMS_H
// =================================================
