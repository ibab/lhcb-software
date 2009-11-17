// $Id: MCTupleToolPrimaries.h,v 1.1 2009-11-17 12:33:12 pkoppenb Exp $
#ifndef MCTUPLETOOLPRIMARIES_H 
#define MCTUPLETOOLPRIMARIES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IEventTupleTool.h"            // Interface
/** @class MCTupleToolPrimaries MCTupleToolPrimaries.h
 *  
 * \brief Primary MC vertices properties for DecayTreeTuple
 *
 * Tuple columns:
 * - coordinates PVX, PVY, PVZ, PVT (time)
 *
 *  @author Patrick Koppenburg
 *  @date   2009-11-16
 */
class MCTupleToolPrimaries : public GaudiTool, virtual public IEventTupleTool {
public: 
  /// Standard constructor
  MCTupleToolPrimaries( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~MCTupleToolPrimaries( ); ///< Destructor
  virtual StatusCode fill( Tuples::Tuple& );

protected:

private:

};
#endif // MCTUPLETOOLPRIMARIES_H
