#ifndef RICHKERNEL_RICHSMARTID_H
#define RICHKERNEL_RICHSMARTID_H 1

// Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichKernel/RichSmartCode.h"

/** @class RichSmartID RichSmartID.h
 *
 *  Smart identifier for RICH Objects
 *
 *  @author Andy Presland   (andrew.presland@cern.ch)
 *  @author Chris Jones     (christopher.rob.jones@cern.ch)
 *
 *  @date 2002-2-26
 */

class RichSmartID {

public:

  /// Define the internal representation
  typedef  long int  ContentType;

  /// Constructor from ContentType
  RichSmartID( const ContentType & intKey ) { m_key = intKey; }

  /// Default Constructor
  RichSmartID(): m_key(99999) {}

  /** constructor with all arguments
      @param   Rich        rich detector identifier
      @param   Panel       rich panel identifier
      @param   HPDRow      rich HPD row identifier
      @param   HPDCol      rich HPD row identifier
      @param   PixelRow    rich pixel row identifier
      @param   PixelCol    rich pixel column identifier
  */
  RichSmartID ( const unsigned int Rich,
                const unsigned int Panel,
                const unsigned int HPDRow,
                const unsigned int HPDCol,
                const unsigned int PixelRow,
                const unsigned int PixelCol )
    : m_key(0)  {
    setRich     ( Rich ) ;
    setPanel    ( Panel ) ;
    setHPDRow   ( HPDRow ) ;
    setHPDCol   ( HPDCol ) ;
    setPixelRow ( PixelRow ) ;
    setPixelCol ( PixelCol ) ;
  };

  /// Destructor
  virtual ~RichSmartID() {}

  /// Retrieve 32 bit integer key
  inline long Key() const { return m_key; }

  /// Retrieve 32 bit integer key
  inline long key() const { return m_key; }

  /// Update 32 bit integer key
  inline void setKey( ContentType& value ) { m_key = value; }

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  /// long operator
  operator long() const { return m_key; }

  /// Decoding function to extract index
  inline const unsigned int index() const
  {
    return
      ( contents() & RichSmartCode::MaskIndex ) >> RichSmartCode::ShiftIndex ;
  }

  /// Decoding function to extract rich identifier
  inline const unsigned int rich () const
  {
    return
      ( contents() & RichSmartCode::MaskRich ) >> RichSmartCode::ShiftRich ;
  }

  /// Decoding function to extract rich panel identifier
  inline const unsigned int panel () const
  {
    return
      ( contents() & RichSmartCode::MaskPanel ) >> RichSmartCode::ShiftPanel ;
  }

  /// Decoding function to return HPD RichSmartID only
  inline const RichSmartID hpdID () const
  {
    return (RichSmartID) (( contents() & ~RichSmartCode::MaskPixelRow ) 
                                       & ~RichSmartCode::MaskPixelCol );
  }

  /// Decoding function to return Panel RichSmartID only
  inline const RichSmartID panelID () const
  {
    return (RichSmartID) (((( contents() & ~RichSmartCode::MaskPixelRow ) 
                                         & ~RichSmartCode::MaskPixelCol )
                                         & ~RichSmartCode::MaskHPDRow )
                                         & ~RichSmartCode::MaskHPDCol );
  }

  /// Decoding function to return Rich RichSmartID only
  inline const RichSmartID richID () const
  {
    return (RichSmartID) ((((( contents() & ~RichSmartCode::MaskPixelRow ) 
                                          & ~RichSmartCode::MaskPixelCol )
                                          & ~RichSmartCode::MaskHPDRow )
                                          & ~RichSmartCode::MaskHPDCol )
                                          & ~RichSmartCode::MaskPanel );
  }

  /// Decoding function to extract HPD row identifier
  inline const unsigned int HPDRow  () const
  {
    return
      ( contents() & RichSmartCode::MaskHPDRow ) >> RichSmartCode::ShiftHPDRow ;
  }

  /// Decoding function to extract HPD column identifier
  inline const unsigned int HPDCol  () const
  {
    return
      ( contents() & RichSmartCode::MaskHPDCol ) >> RichSmartCode::ShiftHPDCol ;
  }

  /// Decoding function to extract pixel row identifier
  inline const unsigned int pixelRow () const
  {
    return
      ( contents() & RichSmartCode::MaskPixelRow ) >> RichSmartCode::ShiftPixelRow ;
  }

  /// Decoding function to extract pixel row identifier
  inline const unsigned int pixelCol () const
  {
    return
      ( contents() & RichSmartCode::MaskPixelCol ) >> RichSmartCode::ShiftPixelCol ;
  }

  /// Decoding function to extract all bits
  inline unsigned int all () const
  {
    return
      ( contents() & RichSmartCode::MaskAll ) >> RichSmartCode::ShiftAll ;
  }

  /// Decoding function to extract "the rest" - should be zero!
  inline unsigned int rest () const
  {
    return
      ( contents() & RichSmartCode::MaskRest ) >> RichSmartCode::ShiftRest ;
  }

  /// Decoding function to extract the full content
  inline const ContentType& contents() const { return m_key; }

  /// Comparision operator
  inline bool operator < ( const ContentType& ID ) const
  { return contents() <  ID ; }

  /// Equality operator
  inline bool operator == ( const ContentType& ID ) const
  { return contents() == ID ; }

  /// Comparison operator using index.
  inline bool operator < ( const RichSmartID& ID ) const
  { return index() <  ID.index() ; }

  /// Equality operator using contents
  inline bool operator == ( const RichSmartID& ID ) const
  { return contents() == ID.contents() ; }

  /// non-equality operator using contents
  inline bool operator !=( const RichSmartID& ID ) const
  { return !(ID == *this) ; }

  inline const std::string bits( const char delim = ',' ) const;

  /// update rich identifier
  inline RichSmartID& setRich( const unsigned int Rich );

  /// update panel identifier
  inline RichSmartID& setPanel( const unsigned int Panel );

  /// update pixel row identifier
  inline RichSmartID& setPixelRow( const unsigned int PixelRow );

  /// update pixel col identifier
  inline RichSmartID& setPixelCol( const unsigned int PixelCol );

  /// update HPD row identifier
  inline RichSmartID& setHPDRow( const unsigned int HPDRow  );

  /// update HPD col identifier
  inline RichSmartID& setHPDCol( const unsigned int HPDCol  );

  /// update sicb identifier
  inline RichSmartID& set( const ContentType&,
                           const unsigned int,
                           const ContentType& );

  /// update index   identifier
  inline RichSmartID& setIndex   ( const unsigned int Index );

protected:

private:

  /// The internal representation of the RichSmartSmartID
  ContentType m_key;

};

inline RichSmartID& RichSmartID::setRich ( const unsigned int Rich ) {
  return
    set( Rich, RichSmartCode::ShiftRich, RichSmartCode::MaskRich );
}

inline RichSmartID& RichSmartID::setPanel ( const unsigned int Panel ) {
  return
    set( Panel, RichSmartCode::ShiftPanel, RichSmartCode::MaskPanel );
}

inline RichSmartID& RichSmartID::setPixelRow ( const unsigned int PixelRow ) {
  return
    set( PixelRow, RichSmartCode::ShiftPixelRow, RichSmartCode::MaskPixelRow );
}

inline RichSmartID& RichSmartID::setPixelCol ( const unsigned int PixelCol ) {
  return
    set( PixelCol, RichSmartCode::ShiftPixelCol, RichSmartCode::MaskPixelCol );
}

inline RichSmartID& RichSmartID::setHPDRow  ( const unsigned int HPDRow ){
  return
    set( HPDRow, RichSmartCode::ShiftHPDRow, RichSmartCode::MaskHPDRow );
}

inline RichSmartID& RichSmartID::setHPDCol  ( const unsigned int HPDCol ) {
  return
    set( HPDCol, RichSmartCode::ShiftHPDCol, RichSmartCode::MaskHPDCol  );
}

inline RichSmartID& RichSmartID::setIndex( const unsigned int Index ) {
  return
    set( Index, RichSmartCode::ShiftIndex, RichSmartCode::MaskIndex);
}

inline RichSmartID& RichSmartID::set ( const RichSmartID::ContentType&  Value,
                                       const unsigned int               Shift,
                                       const RichSmartID::ContentType& Mask )
{
  ContentType tmp1 , tmp2 ;
  tmp1  = ( Value << Shift ) &  Mask ;
  tmp2  =  contents()        & ~Mask ;
  m_key = ( tmp1 | tmp2 ) ;
  return *this;
};

// Convert the value of pixel ID into bit pattern
inline const std::string RichSmartID::bits( const char del ) const
{
  std::string  str("[");
  const ContentType  one = 1;
  for( int n  = ( 0 == rest() )? RichSmartCode::BitsAll - 1 :
         RichSmartCode::BitsTotal - 1 ; n >= 0 ; --n  )
    {
      unsigned int pos = n;
      bool isNull =   ( 0 == ( ( one << pos ) & contents() ) );
      str+= isNull ? '0' : '1';
      if      ( 0                             == pos )   break      ;
      if      ( 0                             == del )   continue   ;
      if      ( RichSmartCode::ShiftRest      == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftRich      == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftPanel     == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftPixelRow  == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftPixelCol  == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftHPDRow    == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftHPDCol    == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftAll       == pos ) { str += del ; }
    }
  return str += "]";
}

inline std::ostream& RichSmartID::fillStream( std::ostream& s ) const {
  s << "{ Key:\t" << m_key << " } ";
  return s;
}

inline StreamBuffer& RichSmartID::serialize( StreamBuffer& s ) const {
  s << m_key;
  return s;
}

inline StreamBuffer& RichSmartID::serialize( StreamBuffer& s ) {
  s >> m_key;
  return s;
}

/// Serialise >> method
inline StreamBuffer& operator >> ( StreamBuffer& s, RichSmartID& key ) {
  long longKey;
  s >> longKey;
  key.setKey(longKey);
  return s;
}

/// Serialise << method
inline StreamBuffer& operator << ( StreamBuffer& s, const RichSmartID& key ) {
  s << key.key();
  return s;
}

/// overloaded output to std::ostream
inline std::ostream& operator << ( std::ostream& os, const RichSmartID& id ) {
  os << id.key();
  return os;
}

/// overloaded output to MsgStream
inline MsgStream& operator << ( MsgStream& os, const RichSmartID& id ) {
  os << "'Rich=" << id.rich() << " Panel=" << id.panel() << " HPD(r/c)=" 
     << id.HPDRow() << "/" << id.HPDCol() << " Pixel(r/c)=" << id.pixelRow() 
     << "/" << id.pixelCol() << "'";
  return os;
}

#endif   // RICHKERNEL_RICHSMARTID_H
