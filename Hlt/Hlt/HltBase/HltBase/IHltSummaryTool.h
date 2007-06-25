// $Id: IHltSummaryTool.h,v 1.5 2007-06-25 20:40:11 hernando Exp $
#ifndef HLTBASE_IHLTSUMMARYTOOL_H 
#define HLTBASE_IHLTSUMMARYTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class HltSummary;
  class Track;
  class RecVertex;
};

static const InterfaceID IID_IHltSummaryTool ( "IHltSummaryTool", 1, 0 );

/** @class IHltSummaryTool IHltSummaryTool.h HltBase/IHltSummaryTool.h
 *
 *  functionality: 
 *      tool to simplify common task asked to the HltSummary  
 *
 *  @author Jose A. Hernando
 *  @date   2007-02-08
 */
class IHltSummaryTool : virtual public IAlgTool {
public: 
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltSummaryTool; }

  virtual const LHCb::HltSummary& summary() = 0;
  
  virtual bool decision() = 0;
  
  virtual bool decisionType(const std::string& name) = 0;

  virtual bool hasSelection(const std::string& name) = 0;

  virtual bool selectionDecision(const std::string& name) = 0;

  virtual bool selectionDecisionType(const std::string& name,
                                     const std::string& type) = 0;

  virtual std::vector<std::string> selections() = 0;

  virtual std::vector<std::string> selectionFilters(const std::string& name) = 0;

  virtual std::vector<std::string> selectionAncestors(const std::string& name) = 0;

  virtual std::vector<LHCb::Track*> selectionTracks(const std::string& name)= 0;

  virtual std::vector<LHCb::RecVertex*> selectionVertices(const std::string& name)= 0;

  virtual bool isInSelection(const std::string& name,
                             const LHCb::Track& track ) = 0;

  virtual std::vector<std::string> confKeys() = 0;
  
  virtual std::string confString(const std::string& name) = 0;
  
  virtual std::vector<std::string> confStringVector(const std::string& name)= 0;

  // virtual bool isInSelection(const std::string& name,
  //                           const LHCb::RecVectex& vertex) = 0;
  
};
#endif // HLTBASE_IHLTSUMMARYTOOL_H
