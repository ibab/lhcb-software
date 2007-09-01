// $Id: FindAlignmentElement.h,v 1.1 2007-09-01 17:35:12 janos Exp $
#ifndef TALIGNMENT_FINDALIGNMENTELEMENT_H 
#define TALIGNMENT_FINDALIGNMENTELEMENT_H 1

// Include files

/** @class FindAlignmentElement FindAlignmentElement.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2007-08-31
 */

class FindAlignmentElement {
  
 public:
  typedef std::vector<AlignmentElement>::const_iterator AlignElemIter;
  
  FindAlignmentElement(){};
  
  template <typename ACTION1, typename ACTION2>
    FindAlignmentElement(AlignElemIter begin, AlignElemIter end, ACTION1 isDet, ACTION2 elemFromLHCbID) 
    : m_begin(begin),
    m_end(end),
    m_isDet(isDet),
    m_elemFromLHCbID(elemFromLHCbID) {};
  
  const AlignmentElement* operator()(const LHCb::LHCbID& anLHCbID) {
    if (!m_isDet(anLHCbID)) return static_cast<const AlignmentElement*>(0);
    const DetectorElement* elem = m_elemFromLHCbID(anLHCbID);
    AlignElemIter AlignElem = find_if(m_begin, m_end, boost::lambda::bind(&AlignmentElement::matches, 
                                                                          boost::lambda::_1, 
                                                                          elem));
    return (AlignElem != m_end) ? &(*AlignElem) : static_cast<const AlignmentElement*>(0);
  };
  
 private:
  AlignElemIter                                          m_begin;
  AlignElemIter                                          m_end;
  boost::function<bool                   (LHCb::LHCbID)> m_isDet;
  boost::function<const DetectorElement* (LHCb::LHCbID)> m_elemFromLHCbID;
};  

#endif // TALIGNMENT_FINDALIGNMENTELEMENT_H
