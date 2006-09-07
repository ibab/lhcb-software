
//=============================================================================================
/** @file RichDAQHeaderPDBase.h
 *
 *  Header file for RICH DAQ utility class : RichDAQHeaderPDBase
 *
 *  CVS Log :-
 *  $Id: RichDAQHeaderPDBase.h,v 1.4 2006-09-07 17:14:09 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RichDAQHeaderPDBase_H
#define RICHDAQ_RichDAQHeaderPDBase_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichDet/RichDAQDefinitions.h"

/** @class RichDAQHeaderPDBase RichDAQHeaderPDBase.h
 *
 *  Base class for RICH HPD headers. Implements basic common functions
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
class RichDAQHeaderPDBase
{

public:

  /// Type for header words
  typedef std::vector<RichDAQ::LongType> HeaderWords;

public:

  /// Constructor with number of header words
  explicit RichDAQHeaderPDBase ( const RichDAQ::ShortType nWords = 1 )
    : m_headerWords(nWords,0) { }

  /// Copy constructor
  RichDAQHeaderPDBase ( const RichDAQHeaderPDBase & header )
    : m_headerWords(header.headerWords()) { }

  /// Constructor from raw header word(s)
  explicit RichDAQHeaderPDBase( const HeaderWords & words )
    : m_headerWords(words) { }

  /// Destructor
  ~RichDAQHeaderPDBase ( ) { }

public: // methods

  /// Returns the number of data words in the header
  inline unsigned int nHeaderWords() const
  {
    return headerWords().size();
  }

  /// Read only access to header words
  inline const HeaderWords & headerWords() const { return m_headerWords; }

  /// Read correct number of data words from given stream
  /// Note, after this call data pointer is incremented to the next word after the header
  inline void readFromDataStream( const RichDAQ::LongType *& data )
  {
    for ( unsigned int i = 0; i < nHeaderWords(); ++i )
    {
      headerWords()[i] = *(data++);
    }
  }

protected: // methods

  /// Set the data value using the given mask and shift values
  inline bool set( const RichDAQ::ShortType value,
                   const RichDAQ::ShortType shift,
                   const RichDAQ::LongType  mask,
                   const RichDAQ::ShortType wordNumber = 0 )
  {
    headerWords()[wordNumber] = ( ((value << shift) & mask) | (headerWords()[0] & ~mask) );
    return true;
  }

  /// tests whether a given value is in range for a given data field
  inline bool dataInRange( const RichDAQ::ShortType value,
                           const RichDAQ::ShortType max ) const
  {
    return ( value <= max );
  }

  /// Read/Write access to header words
  inline HeaderWords & headerWords() { return m_headerWords; }

private: // data

  /// The header words
  HeaderWords m_headerWords;

};

#endif // RICHDAQ_RichDAQHeaderPDBase_H
