// $Id: CombineTaggersTDR.h,v 1.4 2007-03-01 20:59:23 musy Exp $
#ifndef COMBINETAGGERSTDR_H 
#define COMBINETAGGERSTDR_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"

#include "ICombineTaggersTool.h"            // Interface

/** @class CombineTaggersTDR CombineTaggersTDR.h CombineTaggersTDR.h
 *  
 *  v1.3
 *  @author Marco Musy
 *  @date   2006-10-02
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
  
  int combineTaggers(LHCb::FlavourTag& theTag, std::vector<LHCb::Tagger*>& , int);

};
#endif // COMBINETAGGERSTDR_H
