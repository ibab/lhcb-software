// $Id: CombineTaggersNNet.h,v 1.1 2005-07-06 00:36:02 musy Exp $
#ifndef COMBINETAGGERSNNET_H 
#define COMBINETAGGERSNNET_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "ICombineTaggersTool.h"            // Interface

/** @class CombineTaggersNNet CombineTaggersNNet.h CombineTaggersNNet.h
 *  
 *  v1.3
 *  @author Marco Musy
 *  @date   2005-07-07
 */
class CombineTaggersNNet : public GaudiTool, 
	virtual public ICombineTaggersTool {
public:
  /// Standard constructor
  CombineTaggersNNet( const std::string& type, 
	    const std::string& name,
	    const IInterface* parent );

  virtual ~CombineTaggersNNet( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  int combineTaggers(FlavourTag& theTag, std::vector<Tagger*>& );

private:

  double m_ProbMin;

};
#endif // COMBINETAGGERSNNET_H
