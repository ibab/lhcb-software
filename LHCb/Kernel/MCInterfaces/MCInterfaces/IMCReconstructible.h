
//-----------------------------------------------------------------------------
/** @file IMCReconstructible.h
 *
 *  Header file for class : IMCReconstructible
 *
 *  CVS Log :-
 *  $Id: IMCReconstructible.h,v 1.3 2007-06-04 09:58:09 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/02/2007
 */
//-----------------------------------------------------------------------------

#ifndef MCINTERFACES_IMCEFFRECONSTRUCTIBLE_H 
#define MCINTERFACES_IMCEFFRECONSTRUCTIBLE_H 1

// from STL
#include <string>

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event Model
namespace LHCb
{
  class MCParticle;
}

/// Interface ID
static const InterfaceID IID_IMCReconstructible ( "IMCReconstructible", 1, 0 );

/** @class IMCReconstructible IMCReconstructible.h MCInterfaces/IMCReconstructible.h
 *  
 *  Returns if an MCParticle should be reconstructible or not
 *
 *  @author Christian Jacoby
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-04-28
 */
class IMCReconstructible : virtual public IAlgTool
{

public: 

  /// Reconstructability categories
  enum RecCategory 
    { 
      NoClassification=-1,  ///< No MC classification possible (e.g. NO MC)
      OutsideAcceptance=0,  ///< Outside detector acceptance
      NotReconstructible,   ///< In acceptance but not reconstructible
      ChargedLong,          ///< Reconstructible as a Long charged track
      ChargedDownstream,    ///< Reconstructible as a Downstream charged track
      ChargedUpstream,      ///< Reconstructible as an Upstream charged track
      ChargedTtrack,        ///< Reconstructible as a T charged track
      ChargedVelo,          ///< Reconstructible as a VELO charged track
      Neutral=50            ///< Reconstructible as a Neutral particle (To Be Extended)
    };

  /// Convert type enum to string
  static std::string text( const RecCategory cat );

public:

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMCReconstructible; }

  /** Get the reconstruction category for the given MCParticle
   *
   *  @param mcPart Pointer to the MCParticle
   *
   *  @return The Reconstructibility category
   */  
  virtual IMCReconstructible::RecCategory reconstructible( const LHCb::MCParticle* mcPart ) const = 0;

  /** Is the MCParticle in the detector acceptance or not
   *
   *  Acceptance criteria depend on the particle type
   *
   *  @param mcPart Pointer to the MCParticle
   *
   *  @return boolean indicating if the MCParticle is in acceptance or not
   */
  virtual bool inAcceptance( const LHCb::MCParticle* mcPart ) const = 0;
 
  /** Check if given MCParticle can be reconstructed as given type
  *
  *  @param mcPart Pointer to the MCParticle
  *  @param category type to check
  *
  *  @return The Reconstructibility category
  */  
  virtual bool isReconstructibleAs(const IMCReconstructible::RecCategory& category,
                                   const LHCb::MCParticle* mcPart ) const = 0;
  
};

//-----------------------------------------------------------------------------
// Implementation of enum printout
//-----------------------------------------------------------------------------

inline std::string IMCReconstructible::text( const RecCategory cat )
{
  switch ( cat )
  {
  case IMCReconstructible::OutsideAcceptance  : return "Outside Detector Acceptance";
  case IMCReconstructible::NotReconstructible : return "Not Reconstructible";
  case IMCReconstructible::ChargedLong        : return "Charged Long Particle";
  case IMCReconstructible::ChargedDownstream  : return "Charged Downstream Particle";
  case IMCReconstructible::ChargedUpstream    : return "Charged Upstream Particle";
  case IMCReconstructible::ChargedTtrack      : return "Charged Ttrack Particle";
  case IMCReconstructible::ChargedVelo        : return "Charged VELO Particle";
  case IMCReconstructible::Neutral            : return "Neutral Particle";
  case IMCReconstructible::NoClassification   : return "No MC Classification";
  default                                     : return "UNKNOWN TYPE";
  }
}


#endif // MCINTERFACES_IMCEFFRECONSTRUCTIBLE_H
