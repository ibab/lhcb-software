// $Id: HltLumiOdinReader.h,v 1.3 2009-12-23 17:59:49 graven Exp $
#ifndef HLTLUMIODINREADER_H 
#define HLTLUMIODINREADER_H 1

// Include files
// from Gaudi


#include "HltBase/HltSelectionContainer.h"
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

private:
  Hlt::SelectionContainer0 m_selection;


};
#endif // HLTLUMIODINREADER_H 
