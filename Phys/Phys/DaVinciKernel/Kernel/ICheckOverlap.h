// $Id: ICheckOverlap.h,v 1.10 2007-12-03 09:17:41 ibelyaev Exp $
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
/// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ICheckOverlap("ICheckOverlap", 2, 1);
// ============================================================================
/** @class ICheckOverlap ICheckOverlap.h
 *  
 *  Tool to check if more than one particle originate 
 *  from the same source 
 *
 *  @author Jose Helder Lopes, redesigned by P. Koppenburg
 *  @date   28/06/2002
 */
class ICheckOverlap : virtual public IAlgTool 
{
  
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ICheckOverlap; }
  
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Argument: parts is a vector of pointers to particles.  
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
  
  /// Check for duplicate use of a protoparticle to produce particles.
  /// Continue a previous check using the contents of the vector of pointers 
  /// to protoparticles.(Most intended for internal use by the other methods).
  /// Arguments: parts is a vector of pointer to particles. 
  ///            proto is a vector of pointers to protoparticles.
  virtual bool foundOverlap( const LHCb::Particle::ConstVector & parts,
                             std::vector<const LHCb::ProtoParticle* > & proto ) = 0 ;
  
  /// Check for duplicate use of a protoparticle to produce decay tree of
  /// any particle in vector. Removes found particles from vector.
  virtual StatusCode removeOverlap( LHCb::Particle::ConstVector& ) = 0;
  
  /// Check for duplicate use of a protoparticle to produce decay tree of
  /// any particle in vector. Removes found particles from vector.
  virtual StatusCode removeOverlap( LHCb::Particle::Vector& ) = 0;
  // ==========================================================================  
  /** Check for the overlap for the arbitrary sequence of the objects, 
   *  implicitely convertible to <c>const LHCb::Particle*</c>.
   *  E.g. any STL container with <c>const LHCb::Particle*</c> or 
   *  <c>LHCb::Particle*</c>, <c>SmartRefVector<LHCb::Particle> 
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
   *  <c>LHCb::Particle*</c>, <c>SmartRefVector<LHCb::Particle> and 
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
  
private:
  
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_ICHECKOVERLAP_H
// ============================================================================
