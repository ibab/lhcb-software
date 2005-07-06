// $Id: CombineTaggersTDR.h,v 1.1 2005-07-06 00:36:03 musy Exp $
#ifndef COMBINETAGGERSTDR_H 
#define COMBINETAGGERSTDR_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "ICombineTaggersTool.h"            // Interface
#include "Event/Tagger.h"

/** @class CombineTaggersTDR CombineTaggersTDR.h CombineTaggersTDR.h
 *  
 *  v1.3
 *  @author Marco Musy
 *  @date   2005-07-07
 */
class CombineTaggersTDR : public GaudiTool, 
	virtual public ICombineTaggersTool {
public:
  /// Standard constructor
  CombineTaggersTDR( const std::string& type, 
	    const std::string& name,
	    const IInterface* parent );

  virtual ~CombineTaggersTDR( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  int combineTaggers(FlavourTag& theTag, std::vector<Tagger*>& );

private:

};
#endif // COMBINETAGGERSTDR_H
