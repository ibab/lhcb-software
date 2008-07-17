// $Id: GetElementsToBeAligned.h,v 1.10 2008-07-17 09:27:09 wouter Exp $
#ifndef GETELEMENTSTOBEALIGNED_H
#define GETELEMENTSTOBEALIGNED_H 1

// Include files
// from STL
#include <string>
#include <list>
#include <vector>
#include <map>
#include <utility>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "IGetElementsToBeAligned.h"

// from BOOST
#include "boost/tokenizer.hpp"
#include "boost/regex.hpp"

/** @class GetElementsToBeAligned GetElementsToBeAligned.h
 *
 *
 *  @author Jan Amoraal
 *  @date   2007-10-08
 */

class GetElementsToBeAligned : public GaudiTool, virtual public IGetElementsToBeAligned {

public:
  typedef IDetectorElement::IDEContainer::const_iterator IDetIter;     ///< IDetector element iterator
  typedef boost::char_separator<char>                    Separator;    ///< Char separator
  typedef boost::tokenizer<Separator>                    Tokenizer;    ///< List of tokens
  typedef std::list<boost::regex>                        RegExs;       ///< List of regular expressions

  /// Standard constructor
  GetElementsToBeAligned(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);

  virtual ~GetElementsToBeAligned( ); ///< Destructor

  StatusCode initialize();

  // Virtual in IGetElementsToBeAligned
  // Return pair of begin iter and end iter
  const IGetElementsToBeAligned::ElementRange& rangeElements() const;

  // Virtual in IGetElementsToBeAligned
  // Return method that finds an alignment element for a given LHCb id
  virtual const AlignmentElement* findElement(const LHCb::LHCbID& anLHCbID) const ;

  // Return method that finds an alignment element for a given Measuerment
  virtual const AlignmentElement* findElement(const LHCb::Measurement& meas) const;

private:
  enum e_DoFs {Tx, Ty, Tz, Rx, Ry, Rz};
  
  void getElements(const IDetectorElement* parent);
  
  bool                                                              m_useLocalFrame;    ///< Use local frame as alignmentframe
  std::vector<std::string>                                          m_elemsToBeAligned; ///< Elemenst : Path to elements
  RegExs                                                            m_regexs;           ///< List of regular expresions
  size_t                                                            m_depth;            ///< How deep?
  mutable std::vector<const DetectorElement*>                       m_elements;         ///< Flat vector of detector elements
  mutable IGetElementsToBeAligned::Elements                         m_alignElements;    ///< Flat vector of alignment elements
  IGetElementsToBeAligned::ElementRange                             m_rangeElements;    ///< Range of elements to be aligned
  typedef std::map<const DetectorElement*, const AlignmentElement*> ElementMap;
  ElementMap                                                        m_elementMap;       ///< Map of detector elements to alignment element
   
};

#endif // GETELEMENTSTOBEALIGNED_H
