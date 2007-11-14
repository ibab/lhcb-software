// $Id: IHltConfSummaryTool.h,v 1.5 2007-11-14 13:34:18 hernando Exp $
#ifndef HLTBASE_IHLTCONFSUMMARYTOOL_H 
#define HLTBASE_IHLTCONFSUMMARYTOOL_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//forward declarations
namespace LHCb {
  class HltSummary;
  class Track;
  class RecVertex;
  // class Particle;
};

static const InterfaceID IID_IHltConfSummaryTool ( "IHltConfSummaryTool", 1, 0 );

/** @class IHltConfSummaryTool IHltConfSummaryTool.h HltBase/IHltConfSummaryTool.h
 *
 *  functionality: 
 *      tool to simplify common task asked to the HltSummary  
 *
 *  @author Jose A. Hernando
 *  @date   2007-02-08
 */
class IHltConfSummaryTool : virtual public IAlgTool {
public: 
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltConfSummaryTool; }

  virtual const LHCb::HltSummary& summary() = 0;
  
  virtual bool decision() = 0;
  
  virtual bool decisionType(const std::string& name) = 0;

  virtual bool hasSelection(const std::string& name) = 0;

  virtual bool selectionDecision(const std::string& name) = 0;

  virtual bool selectionDecisionType(const std::string& name,
                                     const std::string& type) = 0;

  virtual std::vector<std::string> selections() = 0;

  virtual std::vector<std::string> selectionFilters(const std::string& name) = 0;

  virtual std::string selectionType(const std::string& name) = 0;

  virtual std::vector<std::string> selectionInputSelections(const std::string& name) = 0;
  
  virtual std::vector<LHCb::Track*> selectionTracks(const std::string& name)= 0;

  virtual std::vector<LHCb::RecVertex*> selectionVertices(const std::string& name)= 0;

  //  virtual std::vector<LHCb::Particle*> selectionParticles(const std::string& name)= 0;

  virtual bool isInSelection(const std::string& name,
                             const LHCb::Track& track ) = 0;

  virtual std::vector<std::string> confKeys() = 0;

  virtual int confInt(const std::string& name) = 0;
  
  virtual double confDouble(const std::string& name) = 0;
  
  virtual std::string confString(const std::string& name) = 0;
  
  virtual std::vector<std::string> confStringVector(const std::string& name)= 0;

  // virtual bool isInSelection(const std::string& name,
  //                           const LHCb::RecVectex& vertex) = 0;
  
};
#endif // HLTBASE_IHLTSUMMARYTOOL_H
