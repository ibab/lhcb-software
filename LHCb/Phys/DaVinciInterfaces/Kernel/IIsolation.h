// $Id: IIsolation.h,v 1.1 2010-03-10 13:14:27 giampi Exp $
#ifndef DAVINCIKERNEL_IISOLATION_H 
#define DAVINCIKERNEL_IISOLATION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward declarations 
// ============================================================================
#include "Event/Particle.h"
#include "Event/RecVertex.h"

static const InterfaceID IID_IIsolation ( "IIsolation", 1, 0 );

/** @class IIsolation IIsolation.h
 *  
 *
 *  @author Giampiero Mancinelli
 *  @date   2010-02-12
 */
class IIsolation : virtual public IAlgTool {
public: 
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IIsolation; }
  
  virtual StatusCode getIso(LHCb::Particle::ConstVector muons, 
                            int* iso, int isotype=0,
                            const LHCb::VertexBase* PV=0, 
                            const LHCb::VertexBase* SV=0) const = 0 ;
  
protected:
  // ==========================================================================
  virtual ~IIsolation() ;         // virtual and protected desctrustor 
};
#endif // DAVINCIKERNEL_IISOLATION_H
