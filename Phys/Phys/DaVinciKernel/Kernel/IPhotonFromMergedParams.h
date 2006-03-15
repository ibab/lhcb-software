// $Id: IPhotonFromMergedParams.h,v 1.5 2006-03-15 13:34:02 pkoppenb Exp $
// ====================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ====================================================
// 
// ===================================================
#ifndef DAVINCIKERNEL_IPHOTONFROMMERGEDPARAMS_H 
#define DAVINCIKERNEL_IPHOTONFROMMERGEDPARAMS_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// DaVinciTools 
#include "Kernel/IIDIPhotonFromMergedParams.h"
// forward declarations 
#include "Event/Particle.h"
#include "Event/PrimVertex.h"

/** @class IPhotonFromMergedParams IPhotonFromMergedParams.h 
 * Kernel/IPhotonFromMergedParams.h
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
  virtual StatusCode process ( LHCb::Particle*            photon , 
                               const CaloHypo*      thehypo,
                               const PrimLHCb::Vertex*    vertex ) const = 0;
  
  /** (Re)evaluate the photon's paramters using the knowledge 
   *  of photon production  vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the ptoton's production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( LHCb::Particle*            photon , 
                               const CaloHypo*      thehypo,
                               const LHCb::Vertex*        vertex ) const = 0;
  
  /** (Re)evaluate the photon's parameters using the knowledge 
   *  of photon production vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  point   position of the photon production vertex 
   *  @param  error   covariance matrix of photon production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( LHCb::Particle*            photon , 
                               const CaloHypo*      thehypo,
                               const Gaudi::XYZPoint&    point  , 
                               const Gaudi::Matrix3x3&  error) const = 0;
  
protected:
  
  /// destructor 
  virtual ~IPhotonFromMergedParams(){};

};

// =================================================
// The END 
// =================================================
#endif // DAVINCIKERNEL_IPHOTONFROMMERGEDPARAMS_H
// =================================================
