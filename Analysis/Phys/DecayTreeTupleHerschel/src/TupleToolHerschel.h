#ifndef TUPLETOOLHERSCHEL_H
#define TUPLETOOLHERSCHEL_H 1

// Include files
// from Gaudi
#include "Kernel/IEventTupleTool.h"            // Interface
#include "DecayTreeTupleBase/TupleToolBase.h"

/** @class TupleToolHerschel TupleToolHerschel.h
 *
 *  Fills Herschel information.
 *
 *  @author Dan Johnson
 *  @date   2015-07-08
 */
class TupleToolHerschel : public TupleToolBase, 
                           virtual public IEventTupleTool
{

public:

  /// Standard constructor
  TupleToolHerschel( const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  virtual ~TupleToolHerschel( ); ///< Destructor
  virtual StatusCode fill( Tuples::Tuple& );///< Fill tuple

};

#endif 

