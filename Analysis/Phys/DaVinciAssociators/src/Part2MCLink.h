// $Id: Part2MCLink.h,v 1.1 2002-05-10 15:08:21 phicharp Exp $
#ifndef Part2MCLink_H 
#define Part2MCLink_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


// local
#include "DaVinciAssociators/Part2MCPartAsct.h"

/** @class Part2MCLink Part2MCLink.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/05/2002
 */
class Part2MCLink : public Algorithm {
  friend AlgFactory<Part2MCLink>;
  
public:
  /// Standard constructor
  Part2MCLink( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Part2MCLink( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// Location of table in TES
  std::string outputData() { return m_outputData; }
  
protected:

private:
  std::string m_inputData;  ///< location of Particles to associate
  std::string m_outputData; ///< location of relations table  
};
#endif // Part2MCLink_H
