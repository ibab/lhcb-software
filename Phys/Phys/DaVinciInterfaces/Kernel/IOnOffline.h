// $Id: IOnOffline.h,v 1.3 2009-08-13 10:32:33 ibelyaev Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_IONOFFLINE_H 
#define DAVINCIKERNEL_IONOFFLINE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
/** @class IOnOffline IOnOffline.h Kernel/IOnOffline.h
 *  
 *  Tool that tells the application if one runs in On or Offline Mode
 *  Now uses context. Can only be used as private tool.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 *
 *  @author Juan PALACIOS
 *  @date   2008-09-29
 */
class GAUDI_API IOnOffline : virtual public IAlgTool 
{
public: 
  // ==========================================================================
  DeclareInterfaceID(IOnOffline, 5, 0);
  // ==========================================================================
public:
  // ==========================================================================
  /// On or Offline ?
  virtual bool online() const = 0 ;  
  /** Return name of IDistanceCalculator implementation 
   *  @see IDdictanceCalculator
   */
  virtual const std::string& distanceCalculatorType () const = 0 ;
  /** Return name of unconstrained vertex fitter
   *  @see IVertexFit
   */
  virtual const std::string& vertexFitterType       () const = 0 ;
  /** Return name of particle combiner 
   *  @see IParticleCombiner
   */
  virtual const std::string& particleCombinerType   () const = 0 ;
  /** Return name of IRelatedPVFinder implementation
   *  @see IRelatedPVFinder
   */
  virtual const std::string& relatedPVFinderType    () const = 0 ;
  /// Return location of default primary vertices
  virtual const std::string& primaryVertexLocation  () const = 0 ;
  /** Return root of a TES location.
   *  Typical values could be "Phys" or "Hlt"
   *  Parent code would then append to "/Event/" to create
   *  absolute TES trunk location for data under control
   *  of the parent.
   */
  virtual const std::string& trunkOnTES () const = 0 ;
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IONOFFLINE_H
// ============================================================================
