#ifndef RICHKERNEL_RICHSMARTID_H
#define RICHKERNEL_RICHSMARTID_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichKernel/RichSmartCode.h"

/** @class RichSmartID RichSmartID.h RichKernel/RichSmartID.h
 *
 *  Smart identifier for RICH Objects
 *
 *  @author Chris Jones     (christopher.rob.jones@cern.ch)
 *
 *  @date 2002-2-26
 *
 *  @todo Add tests for data field validity.
 */

class RichSmartID {

public:

  /// Default Constructor
  RichSmartID() : m_key( 0 ) { }

  /// Copy constructor
  RichSmartID( const RichSmartID & id ) : m_key( id.key() ) { }

  /// Constructor from RichSmartCode::LongType
  RichSmartID( const RichSmartCode::LongType key ) { m_key = key; }

  /** constructor with all arguments
      @param   Rich        rich detector identifier
      @param   Panel       rich panel identifier
      @param   PDRow       rich photo-detector row identifier
      @param   PDCol       rich photo-detector row identifier
      @param   PixelRow    rich pixel row identifier
      @param   PixelCol    rich pixel column identifier
      @param   SubPixel    rich sub-pixel identifier
  */
  RichSmartID ( const RichSmartCode::ShortType Rich,
                const RichSmartCode::ShortType Panel,
                const RichSmartCode::ShortType PDRow,
                const RichSmartCode::ShortType PDCol,
                const RichSmartCode::ShortType PixelRow,
                const RichSmartCode::ShortType PixelCol,
                const RichSmartCode::ShortType SubPixel = 0 )
    : m_key(0)
  {
    setRich     ( Rich     );
    setPanel    ( Panel    );
    setPDRow    ( PDRow    );
    setPDCol    ( PDCol    );
    setPixelRow ( PixelRow );
    setPixelCol ( PixelCol );
    setSubPixel ( SubPixel );
  };

  /// Destructor
  virtual ~RichSmartID() {}

  /// Retrieve 32 bit integer key
  RichSmartCode::LongType key() const        { return m_key; }

  // Retained for backward compatibility (to be removed)
  RichSmartCode::LongType Key() const        { return key(); }

  /// Update 32 bit integer key
  void setKey( RichSmartCode::LongType key ) { m_key = key;  }

  /// operator to convert to internal representation
  operator RichSmartCode::LongType() const   { return key(); }

  /// Test if the RichSmartID is valid
  bool isValid() const
  {
    return (key() & RichSmartCode::MaskAllOK) >> RichSmartCode::ShiftAllOK;
  }

  /// Test if the rich field is set. Not yet implemented
  bool richIsSet() const
  {
    return true; // test to be added
  }

  /// Test if the panel field is set. Not yet implemented
  bool panelIsSet() const
  {
    return true; // test to be added
  }

  /// Test if the PD row field is set. Not yet implemented
  bool pdRowIsSet() const
  {
    return true; // test to be added
  }

  /// Test if the PD column field is set. Not yet implemented
  bool pdColIsSet() const
  {
    return true; // test to be added
  }

  /// Test if the pixel row field is set. Not yet implemented
  bool pixelRowIsSet() const
  {
    return true; // test to be added
  }

  /// Test if the pixel column field is set. Not yet implemented
  bool pixelColIsSet() const
  {
    return true; // test to be added
  }

  /// Test if the sub-pixel field is set. Not yet implemented
  bool subPixelIsSet() const
  {
    return true; // test to be added
  }

  /// Decoding function to extract index
  const RichSmartCode::ShortType index() const
  {
    return (key() & RichSmartCode::MaskIndex) >> RichSmartCode::ShiftIndex;
  }

  /// Decoding function to extract rich identifier
  const RichSmartCode::ShortType rich() const
  {
    return (key() & RichSmartCode::MaskRich) >> RichSmartCode::ShiftRich;
  }

  /// Decoding function to extract rich panel identifier
  const RichSmartCode::ShortType panel() const
  {
    return (key() & RichSmartCode::MaskPanel) >> RichSmartCode::ShiftPanel;
  }

  /// Decoding function to extract PD row identifier
  const RichSmartCode::ShortType PDRow() const
  {
    return (key() & RichSmartCode::MaskPDRow) >> RichSmartCode::ShiftPDRow;
  }

  /// Decoding function to extract PD column identifier
  const RichSmartCode::ShortType PDCol() const
  {
    return (key() & RichSmartCode::MaskPDCol) >> RichSmartCode::ShiftPDCol;
  }

  /// Decoding function to extract pixel row identifier
  const RichSmartCode::ShortType pixelRow() const
  {
    return (key() & RichSmartCode::MaskPixelRow) >> RichSmartCode::ShiftPixelRow;
  }

  /// Decoding function to extract pixel row identifier
  const RichSmartCode::ShortType pixelCol() const
  {
    return (key() & RichSmartCode::MaskPixelCol) >> RichSmartCode::ShiftPixelCol;
  }

  /// Decoding function to extract pixel row identifier
  const RichSmartCode::ShortType subPixel() const
  {
    return (key() & RichSmartCode::MaskSubPixel) >> RichSmartCode::ShiftSubPixel;
  }

  /// Decoding function to extract all bits
  RichSmartCode::ShortType all() const
  {
    return (key() & RichSmartCode::MaskIndex) >> RichSmartCode::ShiftIndex;
  }

  /// update rich identifier
  bool setRich ( const RichSmartCode::ShortType Rich )
  {
    if ( !dataInRange(Rich,RichSmartCode::MaxRich) ) {
      std::cout << "RichSmartID ERROR : Rich " << Rich << " out of range" << std::endl;
      return false;
    }
    return set( Rich, RichSmartCode::ShiftRich, RichSmartCode::MaskRich );
  }

  /// update panel identifier
  bool setPanel ( const RichSmartCode::ShortType Panel )
  {
    if ( !dataInRange(Panel,RichSmartCode::MaxPanel) ) {
      std::cout << "RichSmartID ERROR : Panel " << Panel << " out of range" << std::endl;
      return false;
    }
    return set( Panel, RichSmartCode::ShiftPanel, RichSmartCode::MaskPanel );
  }

  /// update PD row identifier
  bool setPDRow ( const RichSmartCode::ShortType PDRow )
  {
    if ( !dataInRange(PDRow,RichSmartCode::MaxPDRow) ) {
      std::cout << "RichSmartID ERROR : PDRow " << PDRow << " out of range" << std::endl;
      return false;
    }
    return set( PDRow, RichSmartCode::ShiftPDRow, RichSmartCode::MaskPDRow );
  }

  /// update PD col identifier
  bool setPDCol( const RichSmartCode::ShortType PDCol )
  {
    if ( !dataInRange(PDCol,RichSmartCode::MaxPDCol) ) {
      std::cout << "RichSmartID ERROR : PDCol " << PDCol << " out of range" << std::endl;
      return false;
    }
    return set( PDCol, RichSmartCode::ShiftPDCol, RichSmartCode::MaskPDCol  );
  }

  /// update pixel row identifier
  bool setPixelRow( const RichSmartCode::ShortType PixelRow )
  {
    if ( !dataInRange(PixelRow,RichSmartCode::MaxPixelRow) ) {
      std::cout << "RichSmartID ERROR : PixelRow " << PixelRow << " out of range" << std::endl;
      return false;
    }
    return set( PixelRow, RichSmartCode::ShiftPixelRow, RichSmartCode::MaskPixelRow );
  }

  /// update pixel col identifier
  bool setPixelCol( const RichSmartCode::ShortType PixelCol )
  {
    if ( !dataInRange(PixelCol,RichSmartCode::MaxPixelCol) ) {
      std::cout << "RichSmartID ERROR : PixelCol " << PixelCol << " out of range" << std::endl;
      return false;
    }
    return set( PixelCol, RichSmartCode::ShiftPixelCol, RichSmartCode::MaskPixelCol );
  }

  /// update sub-pixel identifier
  bool setSubPixel( const RichSmartCode::ShortType subPixel )
  {
    if ( !dataInRange(subPixel,RichSmartCode::MaxSubPixel) ) {
      std::cout << "RichSmartID ERROR : SubPixel " << subPixel << " out of range" << std::endl;
      return false;
    }
    return set( subPixel, RichSmartCode::ShiftSubPixel, RichSmartCode::MaskSubPixel );
  }

  /// update sicb identifier
  bool set( const RichSmartCode::LongType  value,
            const RichSmartCode::ShortType shift,
            const RichSmartCode::LongType  mask )
  {
    setKey( ((value << shift) & mask) | (key() & ~mask) | RichSmartCode::SetAllOK );
    return true;
  };

  /// update index identifier
  bool setIndex ( const RichSmartCode::ShortType Index )
  {
    return set( Index, RichSmartCode::ShiftIndex, RichSmartCode::MaskIndex );
  }

  /// Decoding function to return Pixel RichSmartID only
  const RichSmartID pixelID() const
  {
    return ( key() & ~RichSmartCode::MaskSubPixel );
  }

  /// Decoding function to return PD RichSmartID only
  const RichSmartID pdID() const
  {
    return ( key() & ( RichSmartCode::MaskPanel + RichSmartCode::MaskRich +
                       RichSmartCode::MaskPDCol + RichSmartCode::MaskPDRow +
                       RichSmartCode::MaskAllOK ) );
  }

  /// Decoding function to return Panel RichSmartID only
  const RichSmartID panelID() const
  {
    return ( key() & ( RichSmartCode::MaskPanel + RichSmartCode::MaskRich +
                       RichSmartCode::MaskAllOK ) );
  }
  
  /// Decoding function to return Rich RichSmartID only
  const RichSmartID richID() const
  {
    return ( key() & (RichSmartCode::MaskRich + RichSmartCode::MaskAllOK) );
  }

  /// Comparision operator
  bool operator < ( const RichSmartCode::LongType & ID ) const
  {
    return ( key() < ID );
  }

  /// Comparison operator
  bool operator < ( const RichSmartID & ID ) const
  {
    return ( key() < ID.key() );
  }

  /// Equality operator using contents
  bool operator == ( const RichSmartID & ID ) const
  {
    return ( key() == ID.key() );
  }

  /// non-equality operator using contents
  bool operator != ( const RichSmartID & ID ) const
  {
    return !( ID == *this ) ;
  }

  /// Serialize the object for writing
  virtual StreamBuffer & serialize( StreamBuffer & s ) const;

  /// Serialize the object for reading
  virtual StreamBuffer & serialize( StreamBuffer & s );

  /// Fill the ASCII output stream
  virtual std::ostream & fillStream( std::ostream & s ) const;

private: // methods

  bool dataInRange( const RichSmartCode::ShortType value,
                    const RichSmartCode::ShortType max )
  {
    return ( value <= max );
  }

private: //data

  /// The internal representation of the RichSmartSmartID
  RichSmartCode::LongType m_key;

};

// Methods associated with messaging and serialisation

inline std::ostream & RichSmartID::fillStream( std::ostream & s ) const
{
  s << "'Rich=" << rich() << " Panel=" << panel() << " PD(r/c)="
    << PDRow() << "/" << PDCol() << " Pixel(r/c)=" << pixelRow()
    << "/" << pixelCol() << " SubPixel=" << subPixel() << "'";
  return s;
}

inline StreamBuffer & RichSmartID::serialize( StreamBuffer & s ) const
{
  s << m_key; return s;
}

inline StreamBuffer & RichSmartID::serialize( StreamBuffer & s )
{
  s >> m_key; return s;
}

/// Serialise >> method
inline StreamBuffer & operator >> ( StreamBuffer & s, RichSmartID & id )
{
  id.serialize(s); return s;
}

/// Serialise << method
inline StreamBuffer & operator << ( StreamBuffer & s, const RichSmartID & id )
{
  id.serialize(s); return s;
}

/// overloaded output to std::ostream
inline std::ostream & operator << ( std::ostream& os, const RichSmartID & id )
{
  id.fillStream(os); return os;
}

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream& os, const RichSmartID & id )
{
  os << "'Rich=" << id.rich() << " Panel=" << id.panel() << " PD(r/c)="
     << id.PDRow() << "/" << id.PDCol() << " Pixel(r/c)=" << id.pixelRow()
     << "/" << id.pixelCol() << " SubPixel=" << "'";
  return os;
}

#endif   // RICHKERNEL_RICHSMARTID_H
