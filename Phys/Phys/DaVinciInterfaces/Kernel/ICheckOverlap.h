// $Id: ICheckOverlap.h,v 1.3 2010-06-20 15:49:19 ibelyaev Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_ICHECKOVERLAP_H 
#define DAVINCIKERNEL_ICHECKOVERLAP_H 1
// ============================================================================
// Include files
// ============================================================================
// STL&STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// Event/PhysEvent 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
/** @class ICheckOverlap  Kernel/ICheckOverlap.h
 *  
 *  Tool to check if more than one particle originate 
 *  from the same source 
 *
 *  @author Jose Helder Lopes, redesigned by P. Koppenburg  
 *                           & modified a bit by Vanya Belyaev 
 *  @date   28/06/2002
 */
class GAUDI_API ICheckOverlap : virtual public IAlgTool 
{
public:
  // ==========================================================================
  DeclareInterfaceID(ICheckOverlap, 4, 0);
  // ==========================================================================
public:
  // ==========================================================================
  /** Check for duplicate use of a protoparticle to produce particles.
   *  @param parts is a vector of pointers to particles.  
   */
  virtual bool foundOverlap( const LHCb::Particle::ConstVector & parts  ) = 0;
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const LHCb::Particle* ) = 0;
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const LHCb::Particle*, 
                             const LHCb::Particle* ) = 0;
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const LHCb::Particle*, 
                             const LHCb::Particle*, 
                             const LHCb::Particle*) = 0;
  /// Check for duplicate use of a protoparticle to produce particles.  
  virtual bool foundOverlap( const LHCb::Particle*,
                             const LHCb::Particle*,
                             const LHCb::Particle*,
                             const LHCb::Particle*) = 0;
  // ==========================================================================
public:
  // ==========================================================================
  /** Check for duplicate use of a protoparticle to produce decay tree of
   *  any particle in vector. Removes found particles from vector.
   */
  virtual StatusCode removeOverlap( LHCb::Particle::ConstVector& ) = 0;
  // ==========================================================================  
  /** Check for the overlap for the arbitrary sequence of the objects, 
   *  implicitely convertible to <c>const LHCb::Particle*</c>.
   *  E.g. any STL container with <c>const LHCb::Particle*</c> or 
   *  <c>LHCb::Particle*</c>, <c>SmartRefVector<LHCb::Particle></c> 
   *  and <c>LHCb::Particle::Container</c> are OK.
   *  The various container adapters (e.g. <c>LoKi::Range_</c>)
   *  also fit nicely into the concept
   *  @param begin  begin-iterator of the input sequence of "particles"
   *  @param end    end-iterator   of the input sequence of "particles" 
   *  @return true of overlarps are found 
   */
  template <class PARTICLE>
  bool foundOverlap 
  ( PARTICLE begin ,
    PARTICLE end   ) 
  { return foundOverlap ( LHCb::Particle::ConstVector ( begin, end ) ) ; }
  // ==========================================================================  
  /** Remove the overlap for the arbitrary sequence of the objects, 
   *  implicitely convertible to <c>const LHCb::Particle*</c>.
   *  E.g. any STL container with <c>const LHCb::Particle*</c> or 
   *  <c>LHCb::Particle*</c>, <c>SmartRefVector<LHCb::Particle></c> and 
   *  <c>LHCb::Particle::Container</c> are OK.
   *  The various container adapters (e.g. <c>LoKi::Range_</c>)
   *  also fit nicely into the concept
   *  @param begin  begin-iterator of the input sequnce of "particles"
   *  @param end    end-iterator   of the input sequnce of "particles" 
   *  @param result the resulting (output) vector with no overlaps
   *  @return status code 
   */
  template <class PARTICLE>
  StatusCode  removeOverlap 
  ( PARTICLE begin                      ,
    PARTICLE end                        , 
    LHCb::Particle::ConstVector& result ) 
  { 
    result =  LHCb::Particle::ConstVector ( begin, end ) ;
    return removeOverlap ( result ) ; 
  }
  // ==========================================================================  
protected:
  // ==========================================================================
  /// virtual and protected destructors 
  virtual ~ICheckOverlap() ;               // virtual and protected destructors
  // ==========================================================================
};
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DAVINCIKERNEL_ICHECKOVERLAP_H
// ============================================================================
