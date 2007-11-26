// $Id: PrintMCDecayTreeAlg.h,v 1.1 2007-11-26 17:22:24 cattanem Exp $
#ifndef PRINTMCDECAYTREEALG_H 
#define PRINTMCDECAYTREEALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


// forward declaration
class IPrintMCDecayTreeTool;

/** @class PrintMCDecayTreeAlg PrintMCDecayTreeAlg.h
 *  Prints a dump of the MC event tree, using the PrintMCDecayTreeTool.
 *
 *  @author Vladimir Gligorov, adapted by Marco Cattaneo
 *  @date   26/11/2007
 */
class PrintMCDecayTreeAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrintMCDecayTreeAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrintMCDecayTreeAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IPrintMCDecayTreeTool *m_printTool;
  std::string m_printToolName;
};
#endif // PRINTMCDECAYTREEALG_H
