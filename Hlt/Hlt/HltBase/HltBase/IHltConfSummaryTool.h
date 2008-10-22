// $Id: IHltConfSummaryTool.h,v 1.10 2008-10-22 15:52:43 hernando Exp $
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
  class Particle;
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
  
  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltConfSummaryTool; }

  /// returns the HLT summary
  virtual const LHCb::HltSummary& summary() = 0;
  
  //// returns the HLT decision
  virtual bool decision() = 0;
  
  //// do not use
  virtual bool decisionType(const std::string& name) = 0;

  //// returns true if a given HLT selection is in the summary
  virtual bool hasSelection(const std::string& name) = 0;

  //// returns the decision of a given HLT selecion
  virtual bool selectionDecision(const std::string& name) = 0;

  //// do not use
  virtual bool selectionDecisionType(const std::string& name,
                                     const std::string& type) = 0;

  /// returns the list of selection stored in the summmary
  virtual std::vector<std::string> selections() = 0;

  /// returns selections matching the name, which admits the '*' 
  virtual std::vector<std::string> selections(const std::string& name) = 0;

  /// returns the number of candidates of a given selection
  virtual size_t selectionNCandidates(const std::string& name) = 0;
  
  /// returns a list of string that code the filter/cuts applied in a selection
  virtual std::vector<std::string> selectionFilters(const std::string& name) = 0;

  /// returns the type of candidates of a given selection, i.e Track, Vertex, Particle
  virtual std::string selectionType(const std::string& name) = 0;

  /// returns a list of input selections required to produce this selection
  virtual std::vector<std::string> selectionInputSelections(const std::string& name) = 0;
  
  /// returns a list of track candidates for this selection
  virtual std::vector<LHCb::Track*> selectionTracks(const std::string& name)= 0;
  /// returns a list of vertices candidates for this selection
  virtual std::vector<LHCb::RecVertex*> selectionVertices(const std::string& name)= 0;

  /// returns a list of particles candidates for this selection
  virtual std::vector<const LHCb::Particle*> selectionParticles(const std::string& name)= 0;

  /// do not use
  virtual bool isInSelection(const std::string& name,
                             const LHCb::Track& track ) = 0;

  /// returns all the keys stored in the HLT configuration dictionary
  virtual std::vector<std::string> confKeys() = 0;

  /// returns the int value asociated to a HLT configuration key
  virtual int confInt(const std::string& name) = 0;
  
  /// returns the double value asociated to a HLT configuration key
  virtual double confDouble(const std::string& name) = 0;
  
  /// returns the string value asociated to a HLT configuration key
  virtual std::string confString(const std::string& name) = 0;
  
  /// returns a list of strings asociated to a HLT configuration key
  virtual std::vector<std::string> confStringVector(const std::string& name)= 0;

  // virtual bool isInSelection(const std::string& name,
  //                           const LHCb::RecVectex& vertex) = 0;
  
};
#endif // HLTBASE_IHLTSUMMARYTOOL_H
