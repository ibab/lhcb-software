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
 *  @date 2003-10-31
 *
 *  @todo Once SICB retires return to full functionality
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

  /// Retrieve the full 32 bit integer key
  inline RichSmartCode::LongType key() const        { return m_key; }

  // Retained for backward compatibility (to be removed)
  inline RichSmartCode::LongType Key() const        { return key(); }

  // Retained for backward compatibility (to be removed)
  inline RichSmartCode::LongType index() const      { return key(); }

  /// Update the full 32 bit integer key
  inline void setKey( RichSmartCode::LongType key ) { m_key = key;  }

  /// operator to convert to full 32 bit value
  inline operator RichSmartCode::LongType() const   { return key(); }

  /// Returns only the data bits (strips the validity bits)
  inline RichSmartCode::LongType dataBitsOnly() const
  {
    return (key() & ( RichSmartCode::MaskRich + RichSmartCode::MaskPanel +
                      RichSmartCode::MaskPDRow + RichSmartCode::MaskPDCol +
                      RichSmartCode::MaskPixelRow + RichSmartCode::MaskPixelCol +
                      RichSmartCode::MaskSubPixel ) );
  }

  /// Test if the RichSmartID is valid
  inline bool isValid() const
  {
    return ((key() & (RichSmartCode::MaskRichOK + RichSmartCode::MaskPanelOK +
                      RichSmartCode::MaskPDRowOK + RichSmartCode::MaskPDColOK +
                      RichSmartCode::MaskPixelRowOK + RichSmartCode::MaskPixelColOK +
                      RichSmartCode::MaskSubPixelOK)) !=0 );
  }

  /// Test if the RICH detector field is set.
  inline bool richIsSet() const
  {
    // Disable until SICB retires due to lack of bits
    // return (((key() & RichSmartCode::MaskRichOK) >> RichSmartCode::ShiftRichOK) != 0 );
    return true;
  }

  /// Test if the RICH panel field is set.
  inline bool panelIsSet() const
  {
    // Disable until SICB retires due to lack of bits
    // return (((key() & RichSmartCode::MaskPanelOK) >> RichSmartCode::ShiftPanelOK) != 0 );
    return true;
  }

  /// Test if the RICH photon detector row field is set.
  inline bool pdRowIsSet() const
  {
    return (((key() & RichSmartCode::MaskPDRowOK) >> RichSmartCode::ShiftPDRowOK) != 0 );
  }

  /// Test if the RICH photon detector column field is set.
  inline bool pdColIsSet() const
  {
    return (((key() & RichSmartCode::MaskPDColOK) >> RichSmartCode::ShiftPDColOK) != 0 );
  }

  /// Test if the RICH pixel row field is set.
  inline bool pixelRowIsSet() const
  {
    return (((key() & RichSmartCode::MaskPixelRowOK) >> RichSmartCode::ShiftPixelRowOK) != 0 );
  }

  /// Test if the RICH pixel column field is set.
  inline bool pixelColIsSet() const
  {
    return (((key() & RichSmartCode::MaskPixelColOK) >> RichSmartCode::ShiftPixelColOK) != 0 );
  }

  /// Test if the RICH sub-pixel field is set.
  inline bool subPixelIsSet() const
  {
    return (((key() & RichSmartCode::MaskSubPixelOK) >> RichSmartCode::ShiftSubPixelOK) != 0 );
  }

  /// Decoding function to extract RICH detector identifier
  inline const RichSmartCode::ShortType rich() const
  {
    return (key() & RichSmartCode::MaskRich) >> RichSmartCode::ShiftRich;
  }

  /// Decoding function to extract RICH panel identifier
  inline const RichSmartCode::ShortType panel() const
  {
    return (key() & RichSmartCode::MaskPanel) >> RichSmartCode::ShiftPanel;
  }

  /// Decoding function to extract RICH photon detector row identifier
  inline const RichSmartCode::ShortType PDRow() const
  {
    return (key() & RichSmartCode::MaskPDRow) >> RichSmartCode::ShiftPDRow;
  }

  /// Decoding function to extract RICH photon detector column identifier
  inline const RichSmartCode::ShortType PDCol() const
  {
    return (key() & RichSmartCode::MaskPDCol) >> RichSmartCode::ShiftPDCol;
  }

  /// Decoding function to extract RICH pixel row identifier
  inline const RichSmartCode::ShortType pixelRow() const
  {
    return (key() & RichSmartCode::MaskPixelRow) >> RichSmartCode::ShiftPixelRow;
  }

  /// Decoding function to extract RICH pixel column identifier
  inline const RichSmartCode::ShortType pixelCol() const
  {
    return (key() & RichSmartCode::MaskPixelCol) >> RichSmartCode::ShiftPixelCol;
  }

  /// Decoding function to extract sub-pixel identifier
  inline const RichSmartCode::ShortType subPixel() const
  {
    return (key() & RichSmartCode::MaskSubPixel) >> RichSmartCode::ShiftSubPixel;
  }

  /// update RICH detector identifier
  inline bool setRich ( const RichSmartCode::ShortType Rich )
  {
    if ( !dataInRange(Rich,RichSmartCode::MaxRich) ) {
      std::cout << "RichSmartID ERROR : Rich " << Rich << " out of range" << std::endl;
      return false;
    }
    return set( Rich, RichSmartCode::ShiftRich,
                RichSmartCode::MaskRich, RichSmartCode::SetRichOK );
  }

  /// update RICH panel identifier
  inline bool setPanel ( const RichSmartCode::ShortType Panel )
  {
    if ( !dataInRange(Panel,RichSmartCode::MaxPanel) ) {
      std::cout << "RichSmartID ERROR : Panel " << Panel << " out of range" << std::endl;
      return false;
    }
    return set( Panel, RichSmartCode::ShiftPanel,
                RichSmartCode::MaskPanel, RichSmartCode::SetPanelOK );
  }

  /// update RICH photon detector row identifier
  inline bool setPDRow ( const RichSmartCode::ShortType PDRow )
  {
    if ( !dataInRange(PDRow,RichSmartCode::MaxPDRow) ) {
      std::cout << "RichSmartID ERROR : PDRow " << PDRow << " out of range" << std::endl;
      return false;
    }
    return set( PDRow, RichSmartCode::ShiftPDRow,
                RichSmartCode::MaskPDRow, RichSmartCode::SetPDRowOK );
  }

  /// update RICH photon detector column identifier
  inline bool setPDCol( const RichSmartCode::ShortType PDCol )
  {
    if ( !dataInRange(PDCol,RichSmartCode::MaxPDCol) ) {
      std::cout << "RichSmartID ERROR : PDCol " << PDCol << " out of range" << std::endl;
      return false;
    }
    return set( PDCol, RichSmartCode::ShiftPDCol,
                RichSmartCode::MaskPDCol, RichSmartCode::SetPDColOK );
  }

  /// update RICH pixel row identifier
  inline bool setPixelRow( const RichSmartCode::ShortType PixelRow )
  {
    if ( !dataInRange(PixelRow,RichSmartCode::MaxPixelRow) ) {
      std::cout << "RichSmartID ERROR : PixelRow " << PixelRow << " out of range" << std::endl;
      return false;
    }
    return set( PixelRow, RichSmartCode::ShiftPixelRow,
                RichSmartCode::MaskPixelRow, RichSmartCode::SetPixelRowOK );
  }

  /// update RICH pixel column identifier
  inline bool setPixelCol( const RichSmartCode::ShortType PixelCol )
  {
    if ( !dataInRange(PixelCol,RichSmartCode::MaxPixelCol) ) {
      std::cout << "RichSmartID ERROR : PixelCol " << PixelCol << " out of range" << std::endl;
      return false;
    }
    return set( PixelCol, RichSmartCode::ShiftPixelCol,
                RichSmartCode::MaskPixelCol, RichSmartCode::SetPixelColOK );
  }

  /// update sub-pixel identifier
  inline bool setSubPixel( const RichSmartCode::ShortType subPixel )
  {
    if ( !dataInRange(subPixel,RichSmartCode::MaxSubPixel) ) {
      std::cout << "RichSmartID ERROR : SubPixel " << subPixel << " out of range" << std::endl;
      return false;
    }
    return set( subPixel, RichSmartCode::ShiftSubPixel,
                RichSmartCode::MaskSubPixel, RichSmartCode::SetSubPixelOK );
  }

  /// Decoding function to return Pixel RichSmartID only
  inline const RichSmartID pixelID() const
  {
    return ( key() & ~(RichSmartCode::MaskSubPixel+RichSmartCode::MaskSubPixelOK) );
  }

  /// Decoding function to return PD RichSmartID only
  inline const RichSmartID pdID() const
  {
    return ( key() & ( RichSmartCode::MaskPanel + RichSmartCode::MaskRich +
                       RichSmartCode::MaskPDCol + RichSmartCode::MaskPDRow +
                       RichSmartCode::MaskRichOK + RichSmartCode::MaskPanelOK +
                       RichSmartCode::MaskPDRowOK + RichSmartCode::MaskPDColOK ) );
  }

  /// Decoding function to return Panel RichSmartID only
  inline const RichSmartID panelID() const
  {
    return ( key() & ( RichSmartCode::MaskPanel + RichSmartCode::MaskRich +
                       RichSmartCode::MaskRichOK + RichSmartCode::MaskPanelOK ) );
  }

  /// Decoding function to return Rich RichSmartID only
  inline const RichSmartID richID() const
  {
    return ( key() & (RichSmartCode::MaskRich + RichSmartCode::MaskRichOK) );
  }

  /// Comparision operator
  inline bool operator < ( const RichSmartCode::LongType & ID ) const
  {
    return ( key() < ID );
  }

  /// Comparison operator
  inline bool operator < ( const RichSmartID & ID ) const
  {
    return ( key() < ID.key() );
  }

  /// Equality operator using contents
  inline bool operator == ( const RichSmartID & ID ) const
  {
    return ( key() == ID.key() );
  }

  /// non-equality operator using contents
  inline bool operator != ( const RichSmartID & ID ) const
  {
    return !( ID == *this ) ;
  }

  /// Serialize the object for writing
  virtual StreamBuffer & serialize( StreamBuffer & s ) const;

  /// Serialize the object for reading
  virtual StreamBuffer & serialize( StreamBuffer & s );

  /// Fill the ASCII output stream the data fields
  virtual std::ostream & printDataFields( std::ostream & s ) const;

  /// Fill the ASCII output stream with all fields (data + validity)
  virtual std::ostream & printAllFields( std::ostream & s ) const;

private: // methods

  // Set the data
  inline bool set( const RichSmartCode::LongType  value,
                   const RichSmartCode::ShortType shift,
                   const RichSmartCode::LongType  mask,
                   const RichSmartCode::LongType  okMask )
  {
    setKey( ((value << shift) & mask) | (key() & ~mask) | okMask );
    return true;
  };

  // tests whether a given value is in range for a given data field
  inline bool dataInRange( const RichSmartCode::ShortType value,
                           const RichSmartCode::ShortType max ) const
  {
    return ( value <= max );
  }

private: //data

  /// The internal representation of the RichSmartSmartID
  RichSmartCode::LongType m_key;

};

// Methods associated with messaging and serialisation

inline std::ostream & RichSmartID::printDataFields( std::ostream & s ) const
{
  s << "'Rich=" << rich() << " Panel=" << panel() << " PD(r/c)="
    << PDRow() << "/" << PDCol() << " Pixel(r/c)=" << pixelRow()
    << "/" << pixelCol() << " SubPixel=" << subPixel() << "'";
  return s;
}

inline std::ostream & RichSmartID::printAllFields( std::ostream & s ) const
{
  s << "data " << rich() << " " << panel() << " " << PDRow() << " " << PDCol()
    << " " << pixelRow() << " " << pixelCol() << " " << subPixel()
    << " valid "
    << richIsSet() << " " << panelIsSet() << " " << pdRowIsSet() << " "
    << pdColIsSet() << " " << pixelRowIsSet() << " " << pixelColIsSet() << " "
    << subPixelIsSet();
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
  id.printDataFields(os); return os;
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
