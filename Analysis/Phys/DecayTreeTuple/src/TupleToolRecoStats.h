// $Id: TupleToolRecoStats.h,v 1.1 2009-02-11 18:02:35 pkoppenb Exp $
#ifndef TUPLETOOLRECOSTATS_H 
#define TUPLETOOLRECOSTATS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"
#include "Kernel/IEventTupleTool.h"            // Interface

/** @class TupleToolRecoStats TupleToolRecoStats.h
 *  
 *  Fill number of protoparticles, tracks, ...
 *
 *  @author Patrick Koppenburg
 *  @date   2009-02-11
 */
class TupleToolRecoStats : public GaudiTupleTool, virtual public IEventTupleTool {
public: 
  /// Standard constructor
  TupleToolRecoStats( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~TupleToolRecoStats( ); ///< Destructor
  virtual StatusCode fill( Tuples::Tuple& );///< Fill tuple

protected:

private:
  /// generic templated method to extract the number of entries in a given location.
  /// usage int n = number<LHCb::Particles>('/Event/Phys/MyParts/Particles')
  template<class CLASS> 
  int number(std::string location){
    if (exist<CLASS>(location)){
      return (get<CLASS>(location))->size() ;
    } else return -1 ;
  }
  


};
#endif // TUPLETOOLRECOSTATS_H

