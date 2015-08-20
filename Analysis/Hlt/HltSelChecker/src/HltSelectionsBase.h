// $Id: HltSelectionsBase.h,v 1.1 2009-10-15 12:32:16 pkoppenb Exp $
#ifndef HLTSELECTIONSDECISION_H 
#define HLTSELECTIONSDECISION_H 1

// Include files
// from DaVinci, this is a specialized HltAlgorithm
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IANNSvc.h"
/** @class HltSelectionsBase HltSelectionsBase.h
 *
 *  Common base class for HltSelectionsDecision and HltCorrelation.
 *  Just does a vector of Selections to be considered.
 *
 *  This algorithm would profit from sharing code with HltDecision. 
 *
 *  @author Patrick Koppenburg
 *  @date   2007-03-29
 */
typedef std::vector<std::string> strings;
//// This is a std::vector<std::pair<string, int> >
typedef std::vector< IANNSvc::minor_value_type > hltPairs ;

class HltSelectionsBase : public GaudiAlgorithm {
public: 

  /// Standard constructor
  HltSelectionsBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelectionsBase( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

protected:

  const hltPairs& selections(unsigned int TCK);    ///< fill map of selections

  bool m_isTrigger;

  unsigned int m_tck = 0;
  hltPairs m_selections ; ///< list of Hlt2 selections
  std::string m_hlt2SelectionID ; ///< Major key for Hlt2

};
#endif // HLTSELECTIONSDECISION_H
