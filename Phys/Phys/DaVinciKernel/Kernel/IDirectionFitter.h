// $
#ifndef DAVINCIKERNEL_IDIRECTIONFITTER_H 
#define DAVINCIKERNEL_IDIRECTIONFITTER_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
class StatusCode;


static const InterfaceID IID_IDirectionFitter("IDirectionFitter", 1 , 0); 

/** @class IDirectionFitter 
 *
 *  @author benoit VIAUD
 *  @date   5/08/2002
 */
class IDirectionFitter : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IDirectionFitter; }
  /// Method that performs the Direction fit
  virtual StatusCode fitWithDirection( Particle& ,Vertex& , 
                                Particle& ,Vertex& ) = 0;
  virtual StatusCode fitWithDirection( Particle& ,Vertex& ,
                                Particle& ,Vertex&, Vertex& ) = 0;

};
#endif // DAVINCIKERNEL_IDIRECTIONFITTER_H
