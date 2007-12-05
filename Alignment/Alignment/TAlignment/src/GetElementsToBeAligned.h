// $Id: GetElementsToBeAligned.h,v 1.2 2007-12-05 15:51:14 janos Exp $
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
  const IGetElementsToBeAligned::Range& rangeElements() const;

  // Virtual in IGetElementsToBeAligned
  // Return constraints
  const IGetElementsToBeAligned::Constraints& constraints() const;

  // Virtual in IGetElementsToBeAligned
  // Return method that finds an alignment element for a given LHCb id
  const AlignmentElement* findElement(LHCb::LHCbID anLHCbID) const;

protected:

private:

  void getElements(const IDetectorElement* parent);
  bool                                             m_groupElems;
  std::map<std::string, std::vector<std::string> > m_elemsToBeAligned; ///< Elemenst : Path to elements
  RegExs                                           m_regexs;           ///< List of regular expresions
  size_t                                           m_depth;            ///< How deep?
  size_t                                           m_index;            ///< index
  mutable std::vector<const DetectorElement*>      m_elements;         ///< Flat vector of detector elements
  mutable IGetElementsToBeAligned::Elements        m_alignElements;      ///< Flat vector of alignment elements
  IGetElementsToBeAligned::Range                   m_rangeElements; ///< Range of elements to be aligned
  IGetElementsToBeAligned::Constraints             m_constraints;        ///< Constraints
  mutable FindAlignmentElement                     m_findElement;        ///< instance of FindAlignmentElement
};

#endif // GETELEMENTSTOBEALIGNED_H
