// $Id: MooreInit.h,v 1.2 2010-04-11 15:23:34 graven Exp $
#ifndef MOOREINIT_H 
#define MOOREINIT_H 1

// Include files
#include "Kernel/LbAppInit.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/ToolHandle.h"


class IGenericTool;

/** @class MooreInit MooreInit.h
 *  First TopAlg for Moore. Initializes random number and fills memory histogram
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-15
 */
class MooreInit : public LbAppInit {
public: 
  /// Standard constructor
  MooreInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MooreInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm execution

private:
  ToolHandle<IGenericTool> m_odinTool;   ///< Pointer to odin encoding tool

};
#endif // MooreINIT_H
