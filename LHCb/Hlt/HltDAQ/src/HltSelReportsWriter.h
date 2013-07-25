// $Id: HltSelReportsWriter.h,v 1.2 2010-05-19 21:09:15 graven Exp $
#ifndef HLTSELREPORTSWRITER_H 
#define HLTSELREPORTSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/HltObjectSummary.h"


#include "Kernel/IANNSvc.h"


/** @class HltSelReportsWriter HltSelReportsWriter.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-07-25
 *
 *  Algorithm to convert HltSelReports and HltObjectSummarys containers on TES to HltSelCandidates Raw Bank
 *
 */
class HltSelReportsWriter : public GaudiAlgorithm {
public: 

  enum HeaderIDs { kVersionNumber=1 };

  /// Standard constructor
  HltSelReportsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelReportsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// location of input H
  StringProperty m_inputHltSelReportsLocation;

  /// location of output
  StringProperty m_outputRawEventLocation;


  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;


  typedef std::set<unsigned int> LhcbidSequence;
  typedef std::vector< LhcbidSequence* > LhcbidSequences;

  void addToLhcbidSequences( LhcbidSequence* set2,
                             LhcbidSequences & lhcbidSequences ) const;
  
  /// for sorting ranked selections
  // static bool sortByCLID( const LHCb::HltObjectSummary*  elem1, const LHCb::HltObjectSummary*  elem2);

  class sortByCLID {
   public:
     bool operator() (const LHCb::HltObjectSummary*  elem1,
                      const LHCb::HltObjectSummary*  elem2 ) const
        {
          if( (elem1==0) || (elem2==0) || elem1==elem2) return false;
          return  elem1->summarizedObjectCLID() > elem2->summarizedObjectCLID();
        }
   };


};


#endif // HLTSELREPORTSWRITER_H
