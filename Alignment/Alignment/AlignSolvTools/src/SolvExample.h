// $Id: SolvExample.h,v 1.3 2007-06-28 16:33:56 ahicheur Exp $
#ifndef SOLVEXAMPLE_H 
#define SOLVEXAMPLE_H 1

/** @class SolvExample SolvExample.h
 *  
 *
 *  @author Adlene Hicheur
 *  @date   2007-03-07
 */

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


//Solver
#include "AlignmentInterfaces/IAlignSolvTool.h"

class IAlignSolvTool;

class SolvExample : public GaudiAlgorithm {
public: 
  /// Standard constructor
  SolvExample( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SolvExample( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  IAlignSolvTool* m_solver; ///< Interface to solve tool
  std::string m_solvtoolname; ///< Solver Name

};
#endif // SOLVEXAMPLE_H
