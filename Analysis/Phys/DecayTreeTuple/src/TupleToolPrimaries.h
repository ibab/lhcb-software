// $Id: TupleToolPrimaries.h,v 1.4 2009-11-17 12:33:12 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLPRIMARIES_H
#define JBOREL_TUPLETOOLPRIMARIES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IEventTupleTool.h"            // Interface

/** @class TupleToolPrimaries TupleToolPrimaries.h jborel/TupleToolPrimaries.h
 *
 * \brief Primary vertices properties for DecayTreeTuple
 *
 * Tuple columns:
 * - coordinates PVX, PVY, PVZ
 * - errors PVXERR, PVYERR, PVZERR
 * - vertex chi2 PVCHI@
 * - vertex ndf PVNDOF
 * - Nb of tracks used to do the vertex PVNTRACKS
 *
 * \sa DecayTreeTuple
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolPrimaries : public GaudiTool, virtual public IEventTupleTool {
public:
  /// Standard constructor
  TupleToolPrimaries( const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  virtual ~TupleToolPrimaries( ){}; ///< Destructor
  virtual StatusCode initialize();

  virtual StatusCode fill( Tuples::Tuple& );

private:
  std::string m_pvLocation ; ///<  PV location to be used. If empty, take context-dependent default

};
#endif // JBOREL_TUPLETOOLPRIMARIES_H
