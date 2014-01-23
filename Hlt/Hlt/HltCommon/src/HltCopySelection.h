// $Id: HltCopySelection.h,v 1.2 2009-12-23 16:14:31 graven Exp $
#ifndef HLTCOMMON_HLTCOPYSELECTION_H
#define HLTCOMMON_HLTCOPYSELECTION_H 1

// Include files
// from Gaudi
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"

/** @class HltCopySelection HltCopySelection.h
 *
 *  @author Gerhard Raven
 *  @date   2009-03-31
 */
template <typename T>
class HltCopySelection : public HltAlgorithm
{
  public:
    /// Standard constructor
    HltCopySelection( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~HltCopySelection(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution

  private:
    Hlt::SelectionContainer2<T, T> m_selection;
};

#endif // HLTCOPYSELECTION_H
