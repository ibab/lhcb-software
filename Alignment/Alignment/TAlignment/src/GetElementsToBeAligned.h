// $Id: GetElementsToBeAligned.h,v 1.17 2010-02-19 08:54:36 rlambert Exp $
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
  StatusCode finalize();

  // Virtual in IGetElementsToBeAligned
  // Return pair of begin iter and end iter
  const Elements& elements() const { return m_elements ; }

  // Virtual in IGetElementsToBeAligned
  // Return method that finds an alignment element for a given LHCb id
  virtual const AlignmentElement* findElement(const LHCb::LHCbID& anLHCbID) const ;

  // Return method that finds an alignment element for a given Measuerment
  virtual const AlignmentElement* findElement(const LHCb::Node& node) const ;

  // Return method that finds an alignment element for a given Measuerment
  virtual const AlignmentElement* findElement(const LHCb::Measurement& meas) const;

  // Return method that finds an alignment element for a given detector element
  virtual const AlignmentElement* findElement(const DetectorElement& element) const ;

  // Find the list of elements corresponding to a path (which can ba rehulare expression)
  StatusCode findElements(const std::string& path, 
                          std::vector<const AlignmentElement*>& alignelements) const ; 

  // initialize the alignment frames now. if time=0, it will use the
  // current time. it will issue a warning, return failure and not do
  // anything if m_initTime != 0
  StatusCode initAlignmentFrame( Gaudi::Time now=0 ) ;

  // return the time with which the geometry was initialized
  Gaudi::Time initTime() const { return m_initTime ; } 
  // initialize an Al::Equations object
  void initEquations(Al::Equations& ) const ;

private:
  enum e_DoFs {Tx, Ty, Tz, Rx, Ry, Rz};
  
  void getElements(const IDetectorElement* parent, const RegExs& expressions,
                   size_t depth, std::vector<const DetectorElement*>& detelements) const ;
  
  struct ElementSorter
  {
    // this is very tricky: how do you sort elements in a tree if you
    // don't even know the tree( because of grouping). the following
    // should cover all cases (I hope).
    
    bool operator()(const AlignmentElement* lhs,
                    const AlignmentElement* rhs) const {
      return (
              lhs->basename() < rhs->basename() ||
              (lhs->basename() == rhs->basename() &&
               (lhs->detelements().front()->name() < rhs->detelements().front()->name() ||
                (lhs->detelements().front()->name() == rhs->detelements().front()->name() &&
                 lhs->elementsInTree().size() > rhs->elementsInTree().size())
                ) 
               ) 
              );
      
      //return lhs->basename() < rhs->basename() ||
      //	(lhs->basename() == rhs->basename() &&
      //	 lhs->detelements().front()->name() < rhs->detelements().front()->name() ) ;
   
      // all varant below do not work or do not give result I wanted.
      //       return lhs->detelements().front()->name() < 
      // 	rhs->detelements().front()->name() ||
      // 	(lhs->detelements().front()->name() == rhs->detelements().front()->name() &&  lhs->elementsInTree().size() > rhs->elementsInTree().size() ) ;
      
      //       bool rhsIsLhsOffSpring = lhs->isOffspring( *rhs ) ;
      //       bool lhsIsRhsOffSpring = rhs->isOffspring( *lhs ) ;
      
      //       return (!rhsIsLhsOffSpring && !lhsIsRhsOffSpring) ?
      // 	lhs->detelements().front()->name() < 
      // 	rhs->detelements().front()->name() :
      // 	rhsIsLhsOffSpring ;
      
      //       return lhs->elementsInTree().size() > rhs->elementsInTree().size() ||
      // 	(lhs->elementsInTree().size() == rhs->elementsInTree().size() &&
      // 	 lhs->detelements().front()->name() < 
      // 	 rhs->detelements().front()->name() ) ; 
      // anything based on the 'isOffspring' routine cannot work.
      // this does not work
      //return lhs->isOffspring( *rhs )  ||
      //( ! rhs->isOffspring( *lhs ) &&
      //lhs->detelements().front()->name() < 
      //rhs->detelements().front()->name() ) ; 
    }
  } ;
  
private:
  bool                                                              m_useLocalFrame;    ///< Use local frame as alignmentframe
  bool                                                              m_upgrade;          ///< Use for upgrade alignment
  std::vector<std::string>                                          m_elemsToBeAligned; ///< Elemenst : Path to elements
  mutable IGetElementsToBeAligned::Elements                         m_elements;    ///< Flat vector of alignment elements
  typedef std::map<const DetectorElement*, const AlignmentElement*> ElementMap;
  ElementMap                                                        m_elementMap;       ///< Map of detector elements to alignment element
  Gaudi::Time m_initTime ;
};

#endif // GETELEMENTSTOBEALIGNED_H
