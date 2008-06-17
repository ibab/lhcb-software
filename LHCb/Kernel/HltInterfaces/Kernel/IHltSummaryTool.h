// $Id: IHltSummaryTool.h,v 1.1 2008-06-17 21:41:35 pkoppenb Exp $
#ifndef HLTBASE_IHLTSUMMARYTOOL_H 
#define HLTBASE_IHLTSUMMARYTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class HltSummary;
  class Track;
  class RecVertex;
  class Particle ;
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
  
  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltSummaryTool; }

  /// returns the summary 
  virtual const LHCb::HltSummary& summary() = 0;
  
  /// returns true the HLT decision
  virtual bool decision() = 0;
  
  /// returns the HLT decision type (see Event/HltEnums.h for types)
  virtual bool decisionType(const std::string& name) = 0;

  /// returns true if the HLT summary has this selection
  virtual bool hasSelection(const std::string& name) = 0;

  /// returns the selection decision of the HLT 
  virtual bool selectionDecision(const std::string& name) = 0;

  /// returns the selection decision type of the HLT
  virtual bool selectionDecisionType(const std::string& name,
                                     const std::string& type) = 0;

  /// returns the names of the selections of the HLT
  virtual std::vector<std::string> selections() = 0;


  /// returns the number of candidates of this selection
  virtual size_t selectionNCandidates(const std::string& name) = 0;

  /// returns the tracks of a given selection
  virtual std::vector<LHCb::Track*> selectionTracks(const std::string& name)= 0;
  /// returns the vertices of a given selection
  virtual std::vector<LHCb::RecVertex*> selectionVertices(const std::string& name)= 0;

  /// returns a list of particles candidates for this selection
  virtual std::vector<const LHCb::Particle*> selectionParticles(const std::string& name)= 0;

  /// returns true if this track is in the selection
  virtual bool isInSelection(const std::string& name,
                             const LHCb::Track& track ) = 0;

  /// returns the keys of the HLT configuration
  virtual std::vector<std::string> confKeys() = 0;
  
  /// returns a int value of this key of the HLT configuration
  virtual int confInt(const std::string& name) = 0;
  
  /// returns a double value of this key of the HLT configuration
  virtual double confDouble(const std::string& name) = 0;
  
  /// returns a string value of this key of the HLT configuration
  virtual std::string confString(const std::string& name) = 0;
  
  /// returns a vector of strings of this key of the HLT configuration
  virtual std::vector<std::string> confStringVector(const std::string& name)= 0;

  // virtual bool isInSelection(const std::string& name,
  //                           const LHCb::RecVectex& vertex) = 0;
  
};
#endif // HLTBASE_IHLTSUMMARYTOOL_H
