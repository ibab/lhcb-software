// $Id: TupleToolRecoStats.h,v 1.2 2009-07-30 14:54:47 pkoppenb Exp $
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
#include "Number.icpp"


};
#endif // TUPLETOOLRECOSTATS_H

