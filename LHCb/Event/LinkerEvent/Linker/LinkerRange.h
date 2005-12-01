// $Id: LinkerRange.h,v 1.3 2005-12-01 08:35:21 ocallot Exp $
#ifndef LINKER_LINKERRANGE_H 
#define LINKER_LINKERRANGE_H 1

// Include files
#include "Linker/LinkerEntry.h"

/** @class LinkerRange LinkerRange.h Linker/LinkerRange.h
 *  Holds a range, a collection of LinkerEntry  
 *
 *  @author Olivier Callot
 *  @date   2005-01-19
 */

template <class SOURCE, class TARGET> class LinkerRange {
public: 

  typedef typename std::vector<LinkerEntry<SOURCE,TARGET> >::const_iterator iterator;
  
  /// Standard constructor
  LinkerRange( ) {}; 

  virtual ~LinkerRange( ) {}; ///< Destructor

  /** Add a LinkerEntry inside the range
   *  @param src    pointer to the SOURCE of the entry
   *  @param tgt    pointer to the TARGET of the entry
   *  @param weight Weight of the relation
   */
  void addEntry( const SOURCE* src, const TARGET* tgt, double weight ) {
    LinkerEntry<SOURCE,TARGET> tmp( src, tgt, weight );
    m_entries.push_back( tmp );
  }

  /** returns an iterator to the beginning of the table of entries = range
   *  @return  iterator
   */
  iterator begin() const { return m_entries.begin(); }

  /** returns an iterator to the end of the table of entries = range
   *  @return  iterator
   */
  iterator end()   const { return m_entries.end(); }

  /** returns the size of the range, the number of entries
   *  @return  size
   */
  int size()       const { return m_entries.size(); }

  /** returns true if the range is empty
   *  @return  is range empty ?
   */
  bool empty()     const { return m_entries.empty(); }

  /** clear the range, remove all entries
   */
  void clear()           { m_entries.clear(); }

  /** returns a reference to the first element
   */
  LinkerEntry<SOURCE,TARGET>& front() const { return m_entries.front(); }

  /** returns a reference to the last element
   */
  LinkerEntry<SOURCE,TARGET>& back() const { return m_entries.back(); }

protected:

private:
  std::vector<LinkerEntry<SOURCE,TARGET> > m_entries;
};
#endif // LINKER_LINKERRANGE_H
