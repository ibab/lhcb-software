// $Id: L0Value.h,v 1.1 2007-06-20 12:17:38 hernando Exp $
#ifndef L0VALUE_H 
#define L0VALUE_H 1

// Include files
// from Gaudi

#include "GaudiAlg/GaudiTool.h"
#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
#include "Event/IL0Candidate.h"
#include "HltBase/IL0Value.h"

/** @class L0Value L0Value.h
 *
 *  functionality:
 *        monitor the L0 result
 *        make histogram of L0 decision
 *        make histograms with the input of the L0 decison (muon,calo,hadron)  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-01-16
 */
class L0Value : public GaudiTool, virtual public IL0Value {
public:
  
  /// Standard constructor
  L0Value( const std::string& type, 
           const std::string& name,
           const IInterface* parent);
  
  virtual ~L0Value( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  
  double value(int id);

protected:

  std::string m_CaloDataLocation;
  std::string m_MuonDataLocation;
  std::string m_PileUpDataLocation;
  std::string m_L0DUReportLocation;
  std::string m_L0DUChannelLocation;

protected:

  bool m_ADCconvert;
  
  IL0Candidate* m_CaloCandidate;
  IL0Candidate* m_MuonCandidate;
  IL0Candidate* m_PileUpCandidate;

};
#endif // L0ENTRY_H
