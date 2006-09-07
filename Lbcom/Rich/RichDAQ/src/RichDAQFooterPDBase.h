
//=============================================================================================
/** @file RichDAQFooterPDBase.h
 *
 *  Header file for RICH DAQ utility class : RichDAQFooterPDBase
 *
 *  CVS Log :-
 *  $Id: RichDAQFooterPDBase.h,v 1.1 2006-09-07 17:14:09 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

#ifndef RICHDAQ_RichDAQFooterPDBase_H
#define RICHDAQ_RichDAQFooterPDBase_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichDet/RichDAQDefinitions.h"

//=============================================================================================
/** @class RichDAQFooterPDBase RichDAQFooterPDBase.h
 *
 *  Base class for RICH HPD footers. Implements basic common functions
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================
class RichDAQFooterPDBase
{

public:

  /// Type for header words
  typedef std::vector<RichDAQ::LongType> FooterWords;

public:

  /// Constructor with number of header words
  explicit RichDAQFooterPDBase ( const RichDAQ::ShortType nWords = 0 )
    : m_footerWords(nWords,0) { }

  /// Copy constructor
  RichDAQFooterPDBase ( const RichDAQFooterPDBase & footer )
    : m_footerWords(footer.footerWords()) { }

  /// Constructor from raw footer word(s)
  explicit RichDAQFooterPDBase( const FooterWords & words )
    : m_footerWords(words) { }

  /// Destructor
  ~RichDAQFooterPDBase ( ) { }

public: // methods

  /// Returns the number of data words in the footer
  inline unsigned int nFooterWords() const
  {
    return footerWords().size();
  }

  /// Read only access to footer words
  inline const FooterWords & footerWords() const { return m_footerWords; }

  /// Read given number of data words from stream
  /// Note, after this call data pointer is incremented to the next word after the footer
  inline void readFromDataStream( const RichDAQ::LongType *& data )
  {
    for ( unsigned int i = 0; i < nFooterWords(); ++i )
    {
      footerWords()[i] = *(data++);
    }
  }

protected: // methods

  /// Set the data value using the given mask and shift values
  inline bool set( const RichDAQ::ShortType value,
                   const RichDAQ::ShortType shift,
                   const RichDAQ::LongType  mask )
  {
    footerWords()[0] = ( ((value << shift) & mask) | (footerWords()[0] & ~mask) );
    return true;
  }

  /// tests whether a given value is in range for a given data field
  inline bool dataInRange( const RichDAQ::ShortType value,
                           const RichDAQ::ShortType max ) const
  {
    return ( value <= max );
  }

  /// Read/Write access to footer words
  inline FooterWords & footerWords() { return m_footerWords; }

private: // data

  /// The footer words
  FooterWords m_footerWords;

};

#endif // RICHDAQ_RichDAQFooterPDBase_H
