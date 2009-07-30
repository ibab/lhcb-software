// $Id: TupleToolTriggerRecoStats.h,v 1.1 2009-07-30 14:54:47 pkoppenb Exp $
#ifndef TUPLETOOLTRIGGERRECOSTATS_H 
#define TUPLETOOLTRIGGERRECOSTATS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"
#include "Kernel/IEventTupleTool.h"            // Interface


/** @class TupleToolTriggerRecoStats TupleToolTriggerRecoStats.h
 *  
 *  Counts the number of particles per type
 *
 *  @author Patrick Koppenburg
 *  @date   2009-07-30
 */
class TupleToolTriggerRecoStats : public GaudiTupleTool, virtual public IEventTupleTool {
public: 
  /// Standard constructor
  TupleToolTriggerRecoStats( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);

  virtual ~TupleToolTriggerRecoStats( ); ///< Destructor
  virtual StatusCode fill( Tuples::Tuple& );///< Fill tuple

protected:

private:
#include "Number.icpp"

  std::vector<std::string> m_locations ; ///< Hlt locations to look at
  
};
#endif // TUPLETOOLTRIGGERRECOSTATS_H
