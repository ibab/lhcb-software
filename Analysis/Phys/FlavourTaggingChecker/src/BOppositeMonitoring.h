// $Id: BOppositeMonitoring.h,v 1.1 2009-10-19 11:23:21 musy Exp $
#ifndef BOPPOSITEMONITORING_H 
#define BOPPOSITEMONITORING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciHistoAlgorithm.h"

#include "Event/RecHeader.h"
//#include "MCInterfaces/IForcedBDecayTool.h"
#include "IBDecayTool.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

/** @class BOppositeMonitoring BOppositeMonitoring.h
 *  
 *
 *  @author Marc Grabalosa Gandara
 *  @date   2009-10-08
 */
class BOppositeMonitoring : public DaVinciHistoAlgorithm {
public: 
  /// Standard constructor
  BOppositeMonitoring( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BOppositeMonitoring( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IBDecayTool*    m_forcedBtool;

  int  nmcpb; //mcp with B
  int nB511, nB521, nB531, nB541; //B
  int nB513, nB523, nB533, nB543; //B*
  int nB515, nB525, nB535, nB545; //B**
  int nBarion; //Barions

};
#endif // BOPPOSITEMONITORING_H
