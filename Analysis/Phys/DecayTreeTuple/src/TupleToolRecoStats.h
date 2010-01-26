// $Id: TupleToolRecoStats.h,v 1.3 2010-01-26 15:39:26 rlambert Exp $
#ifndef TUPLETOOLRECOSTATS_H 
#define TUPLETOOLRECOSTATS_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IEventTupleTool.h"            // Interface

/** @class TupleToolRecoStats TupleToolRecoStats.h
 *  
 *  Fill number of protoparticles, tracks, ...
 *
 *  @author Patrick Koppenburg
 *  @date   2009-02-11
 */
class TupleToolRecoStats : public TupleToolBase, virtual public IEventTupleTool {
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

