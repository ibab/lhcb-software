// $Id: HltLumiOdinReader.h,v 1.1.1.1 2008-07-17 08:50:25 panmanj Exp $
#ifndef HLTLUMIODINREADER_H 
#define HLTLUMIODINREADER_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
#include "HltBase/HltAlgorithm.h"

/** @class HltLumiOdinReader HltLumiOdinReader.h
 *  
 *  functionality:
 *      Read some values from ODIN bank
 *  Options:
 *
 *  @author Jaap Panman
 *  @date   2008-07-15
 */
class HltLumiOdinReader : public HltAlgorithm {
public: 
  /// Standard constructor
  HltLumiOdinReader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltLumiOdinReader( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:


protected:


};
#endif // HLTLUMIODINREADER_H 
