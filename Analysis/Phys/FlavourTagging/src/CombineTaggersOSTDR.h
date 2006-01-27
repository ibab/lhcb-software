// $Id: CombineTaggersOSTDR.h,v 1.1 2006-01-27 16:39:01 pkoppenb Exp $
#ifndef COMBINETAGGERSOSTDR_H 
#define COMBINETAGGERSOSTDR_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "ICombineTaggersTool.h"            // Interface
#include "Event/Tagger.h"

/** @class CombineTaggersOSTDR CombineTaggersOSTDR.h CombineTaggersOSTDR.h
 *  
 *  v1.3
 *  @author Chris Barnes
 *  @date   2005-27-09
 */
class CombineTaggersOSTDR : public GaudiTool, 
	virtual public ICombineTaggersTool {
public:
  /// Standard constructor
  CombineTaggersOSTDR( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent );

  virtual ~CombineTaggersOSTDR( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  int combineTaggers(FlavourTag& theTag, std::vector<Tagger*>& );

private:

};
#endif // COMBINETAGGERSOSTDR_H
