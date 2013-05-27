// $Id: L0Filter.h,v 1.6 2010-05-26 10:46:21 odescham Exp $
#ifndef L0FILTER_H 
#define L0FILTER_H 1

// Include files
#include "L0Base/L0AlgBase.h"


/** @class L0Filter L0Filter.h
 *  
 * L0 decision(s) Filtering
 *
 *  configuration properties:
 *
 *   * FILTERING DECISION : 
 *      - TriggerBit : the decision bit to be used  L0 / TimingTrigger bit / Force bit [def = L0 ]
 *
 *   * IF TriggerBit = "L0"
 *     - Decision mask : Physics=1 / Beam1=2 / Beam2=4 / Any=7 [ def = Physics ]
 *
 *     - orChannels :  select the (TCK-dependent) list of L0channels [ def = {} ]
 *            syntax :   orChannels += { [TCK] : { [CHANNELS] }
 *                       TCK = 'ALL'  or 'OTHER' is allowed
 *                       CHANNEL = 'ANY'  (filter on global decision(mask) ) or 'PASS' (filter anyway) is allowed
 *                eg : orChannels += { "0x1810" : {"CALO","MUON"},
 *                                     "0x2009" : {"ANY"},
 *                                     "OTHER"  : {"PASS} };
 *
 *                     orChannels += { "ALL" : {"PASS} }  ==> NO FILTERING
 *
 *                     orChannels += { "0x1810" : {"PASS} , "OTHER" : {} }  ==> FILTERING on TCK =0x1810
 *
 *
 *
 *
 *     - orSubTriggers : select the list of subTriggers  [ def = {} ]
 *            TCK-independent predefined subTriggers :    'L0Ecal'/'L0Hcal'/'L0Muon'/'Other'
 *            syntax :  orSubTrigger += { "L0Ecal" };  
 *
 *     - if orChannels and orSubTriggers are EMPTIES : filter on global decision ( ==>  orChannels += {"ALL" : {"ANY"} };
 *
 *   
 *
 *
 *    + Revert : revert the filtering (default: event is ACCEPTED according to the filtering criteria )
 *
 *
 *  @author Olivier Deschamps
 *  @date   2010-05-26
 */
class L0Filter : public L0AlgBase {
public: 
  /// Standard constructor
  L0Filter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0Filter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string toUpper(std::string str);
  bool isAnother(std::string tck);

  std::string m_l0Location ;              ///< Location of L0 decision
  std::map<std::string,std::vector<std::string> > m_l0channels ; ///< required (tck,[channels]) (Or-ed)
  std::string m_trig;
  unsigned long m_count;
  unsigned long m_sel;
  int m_mask;
  std::vector<std::string> m_l0triggers;
  bool m_revert;
  
};
#endif // L0FILTER_H
