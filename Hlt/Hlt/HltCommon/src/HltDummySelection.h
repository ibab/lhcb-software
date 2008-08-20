// $Id: HltDummySelection.h,v 1.1 2008-08-20 18:01:17 graven Exp $
#ifndef HLTDUMMYSELECTION_H 
#define HLTDUMMYSELECTION_H 1

// Include files
// from Gaudi


#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"

/** @class DummyLumiAlley DummyLumiAlley.h
 *  
 *  functionality:
 *      Always pass
 *
 *  @author Jaap Panman
 *  @date   2008-07-16
 */
class HltDummySelection : public HltAlgorithm {
public: 
  /// Standard constructor
  HltDummySelection( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDummySelection( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  Hlt::SelectionContainer0 m_selection;

};
#endif // HLTDUMMYSELECTION_H
