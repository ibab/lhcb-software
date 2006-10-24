// $Id: CombineTaggersNNet.h,v 1.2 2006-10-24 10:16:44 jpalac Exp $
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
 *  @date   2006-10-02
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
  
  int combineTaggers(LHCb::FlavourTag& theTag, std::vector<LHCb::Tagger*>& );

private:

  double m_ProbMin;

};
#endif // COMBINETAGGERSNNET_H
