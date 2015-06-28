// $Id: HltVertexReportsWriter.h,v 1.2 2009-11-26 13:06:07 tskwarni Exp $
#ifndef HLTVERTEXREPORTSWRITER_H
#define HLTVERTEXREPORTSWRITER_H 1

// Include files
#include "boost/optional.hpp"
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IANNSvc.h"


/** @class HltvertexReportsWriter HltvertexReportsWriter.h
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-05
 *
 *  Algorithm to translate HltSummary  into HltVertexReports
 *
 */
class HltVertexReportsWriter : public GaudiAlgorithm {
public:

  enum HeaderIDs { kVersionNumber=2 };


  enum SourceIDs { kSourceID_Dummy=0,
		   kSourceID_Hlt=kSourceID_Dummy,
		   kSourceID_Hlt1=1,
		   kSourceID_Hlt2=2,
		   kSourceID_Max=7,
		   kSourceID_BitShift=13,
		   kSourceID_MinorMask=0x1FFF,
		   kSourceID_MajorMask=0xE000
  };


  /// Standard constructor
  HltVertexReportsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  ~HltVertexReportsWriter( ) override = default; ///< Destructor

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution

private:

  // ----------------------- methods

  boost::optional<int> selectionNameToInt(const IANNSvc& ann, const std::string& name) const;

  // ----------------------- data members

  /// location of input
  StringProperty m_inputHltVertexReportsLocation;

  /// location of output
  StringProperty m_outputRawEventLocation;

  /// SourceID to insert in the bank header
  IntegerProperty m_sourceID;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc = nullptr;

};

#endif // HLTVERTEXREPORTSWRITER_H
