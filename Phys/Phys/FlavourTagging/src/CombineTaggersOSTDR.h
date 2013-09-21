// $Id: CombineTaggersOSTDR.h,v 1.3 2006-10-24 10:16:44 jpalac Exp $
#ifndef COMBINETAGGERSOSTDR_H
#define COMBINETAGGERSOSTDR_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"

#include "ICombineTaggersTool.h"            // Interface


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

  int combineTaggers(LHCb::FlavourTag& theTag, std::vector<LHCb::Tagger*>&, int, bool );

private:

};
#endif // COMBINETAGGERSOSTDR_H
