// $Id: GetElementsToAlign.h,v 1.2 2007-10-17 12:08:23 lnicolas Exp $
#ifndef TALIGNMNET_GETELEMENTSTOALIGN_H 
#define TALIGNMENT_GETELEMENTSTOALIGN_H 1

// Include files
// from STD
#include <string>
#include <list>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "DetDesc/DetectorElement.h"

// from BOOST
#include "boost/tokenizer.hpp"
#include "boost/regex.hpp"
#include "boost/function.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

#include "VeloDet/DeVelo.h"

#include "AlignmentElement.h"
#include "FindAlignmentElement.h"

/** @class GetElementsToAlign GetElementsToAlign.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-08-09
 */

class GetElementsToAlign : public GaudiAlgorithm {

 public:
  /// Some handy typedefs
  typedef IDetectorElement::IDEContainer::const_iterator IDetIter;     ///< IDetector element iterator
  typedef boost::char_separator<char>                    Separator;    ///< Char separator
  typedef boost::tokenizer<Separator>                    Tokenizer;    ///< List of tokens
  typedef std::list<boost::regex>                        RegExs;       ///< List of regular expressions
  typedef std::vector<AlignmentElement>                  Elements;     ///< Alignable detector elements 
  typedef std::vector<std::vector<double> >              Translations; ///< Vector of translations
  typedef std::vector<std::vector<double> >              Rotations;    ///< Vector of rotations
  typedef std::vector<std::vector<double> >              Pivots;       ///< Vector of pivots
  
  // Standard constructor
  GetElementsToAlign( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetElementsToAlign( );  ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization  

  virtual StatusCode execute(); ///< Algorithm initialization  
  
  /// Return forward begin iterator to alignable elements
  Elements::const_iterator elementsToAlignBegin() const {
    return m_elements.begin();
  }

  /// Return forward end iterator to alignable elements
  Elements::const_iterator elementsToAlignEnd() const {
    return m_elements.end();
  }

  /// Return reverse begin iterator to alignable elements
  Elements::const_reverse_iterator elementsToAlignRBegin() const {
    return m_elements.rbegin();
  }

  /// Return reverse end iterator to alignable elements
  Elements::const_reverse_iterator elementsToAlignREnd() const {
    return m_elements.rend();
  }
  
  const DetectorElement* veloModule(const DeVeloSensor* sensor) const {
    IDetectorElement* iElem = sensor->parentIDetectorElement();
    IDetectorElement* mod   = (dynamic_cast<const DetectorElement*>(iElem))->parentIDetectorElement();
    
    return dynamic_cast<const DetectorElement*>(mod);
  }

  const DetectorElement* itLadder(const DeSTSector* sensor) const {
    IDetectorElement* lad = sensor->parentIDetectorElement();

    return dynamic_cast<const DetectorElement*>(lad);
  }
  
  StatusCode setLocalDeltaParams(const Translations& translations,
                                 const Rotations&    rotations   ) const;

  StatusCode setLocalDeltaMatrix(const Translations& translations,
                                 const Rotations&    rotations   ) const;  

 protected:

 private:
  
  void getElements(const IDetectorElement* parent);
  
  std::string                                            m_elementsToAlign; ///< Path of elements to align
  RegExs                                                 m_regexs;          ///< list of regular expresions
  size_t                                                 m_depth;           ///< How deep does the user want to go. 
  size_t                                                 m_nElement;        ///< Number of elements 
  Elements                                               m_elements;        ///< Alignable detector elements
  Translations                                           m_translations;
  Rotations                                              m_rotations;
  boost::function<bool (LHCb::LHCbID)>                   m_isDet;
  boost::function<const DetectorElement* (LHCb::LHCbID)> m_detElemFromLHCbID;
  FindAlignmentElement                                   m_findAlignElement;
};

#endif // TALIGNMENT_GETELEMENTSTOALIGN_H
