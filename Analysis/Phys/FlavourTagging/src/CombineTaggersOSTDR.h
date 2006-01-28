// $Id: CombineTaggersOSTDR.h,v 1.2 2006-01-28 22:43:54 musy Exp $
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
 *  Tool to do the combined OS tagging (independently from SS)
 *  @author Chris Barnes
 *  @date   2006-01-28
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
