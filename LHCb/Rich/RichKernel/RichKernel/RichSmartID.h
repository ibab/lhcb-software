#ifndef Event_RichSmartID_H
#define Event_RichSmartID_H 1

// Include files
#include "Kernel/CLHEPStreams.h"
#include "RichKernel/RichSmartCode.h"

/** @class RichSmartID RichSmartID.h
 *
 *  Smart ID for Rich Objects
 *
 *  @author Andy Presland   (andrew.presland@cern.ch)
 *  created Tue Feb 26 09:25:55 2002
 *
 */

class RichSmartID {

public:

  typedef  long int      ContentType;

  /// Constructor from long int
  RichSmartID(const ContentType & intKey) { m_Key = intKey ; }

  /// Default Constructor
  RichSmartID(): m_Key(99999) {}

  /** constructor with all arguments
      @param   Rich      rich identifier
      @param   Panel     rich panel identifier
      @param   HPD       rich HPD identifier
      @param   PixRow    rich pixel row identifier
      @param   PixCol    rich pixel column identifier
  */
  RichSmartID (
        				const unsigned int Rich ,
                const unsigned int Panel  ,
                const unsigned int HPDRow   ,
    						const unsigned int HPDCol  ,
                const unsigned int PixelRow ,
                const unsigned int PixelCol
    )
    : m_Key(0)
  {
    setRich   	( Rich ) ;
    setPanel  	( Panel ) ;
    setHPDRow 	( HPDRow ) ;
    setHPDCol 	( HPDCol ) ;
    setPixelRow ( PixelRow ) ;
    setPixelCol ( PixelCol ) ;
  };

  /// Destructor
  virtual ~RichSmartID() {}

  /// Retrieve 32 bit integer key
  long Key() const;

  /// Update 32 bit integer key
  void setKey(ContentType& value);

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  /// long operator
  operator long() const { return m_Key; }


public:

  /// decoding function to extract index
  inline const unsigned int index() const
  {
    return
      ( contents() & RichSmartCode::MaskIndex ) >> RichSmartCode::ShiftIndex ;
  }

  /// decoding function to extract rich idenfifier
  inline const unsigned int rich () const
  {
    return
      ( contents() & RichSmartCode::MaskRich  ) >> RichSmartCode::ShiftRich  ;
  }
  /// decoding function to extract rich panel idenfifier
  inline const unsigned int panel () const
  {
    return
      ( contents() & RichSmartCode::MaskPanel  ) >> RichSmartCode::ShiftPanel  ;
  }
  /// decoding function to extract pixel row  idenfifier
  inline const unsigned int HPDRow  () const
  {
    return
      ( contents() & RichSmartCode::MaskHPDRow ) >> RichSmartCode::ShiftHPDRow ;
  }
  /// decoding function to extract pixel column   idenfifier
  inline const unsigned int HPDCol  () const
  {
    return
      ( contents() & RichSmartCode::MaskHPDCol ) >> RichSmartCode::ShiftHPDCol ;
  }
  /// decoding function to extract HPD row  idenfifier
  inline const unsigned int pixelRow () const
  {
    return
      ( contents() & RichSmartCode::MaskPixelRow  ) >> RichSmartCode::ShiftPixelRow  ;
  }
  /// decoding function to extract HPD row  idenfifier
  inline const unsigned int pixelCol () const
  {
    return
      ( contents() & RichSmartCode::MaskPixelCol  ) >> RichSmartCode::ShiftPixelCol  ;
  }
    /// decoding function to extract all bits
  inline unsigned int  all  () const
  {
    return
      ( contents() & RichSmartCode::MaskAll    ) >> RichSmartCode::ShiftAll   ;
  }

  /// decoding function to extract "the rest" - should be zero!
  inline unsigned int  rest () const
  {
    return
      ( contents() & RichSmartCode::MaskRest   ) >> RichSmartCode::ShiftRest   ;
  }

  /// decoding function to extract the full content
  inline const ContentType& contents() const { return m_Key; }

  // the main method - casting to the representation
  //inline operator ContentType () const { return contents(); }

  // comparison
  inline bool operator < ( const ContentType& ID ) const
    { return contents() <  ID ; }
  inline bool operator ==( const ContentType& ID ) const
    { return contents() == ID ; }

public:

  /// comparison operator using index.
  inline bool operator < ( const RichSmartID& ID ) const
    { return index() <  ID.index() ; }
  /// equality operator using contents
  inline bool operator ==( const RichSmartID& ID ) const
  { return contents() == ID.contents() ; }
  /// non-equality operator using contents
  inline bool operator !=( const RichSmartID& ID ) const
  { return !(ID == *this) ; }
  /// printout to std::ostream
  inline std::ostream& printOut ( std::ostream& ) const ;
  /// printout to MsgStream
//  inline MsgStream&    printOut ( MsgStream&    ) const ;
  /// return bit pattern with delimeters
  inline const std::string bits( const char delim = ',' ) const;

protected:

private:

  long int m_Key; ///<  32 bit integer key


  /// update rich identifier
  inline RichSmartID& setRich   	( const unsigned int Rich  		);
  /// update panel identifier
  inline RichSmartID& setPanel   	( const unsigned int Panel  	);
  /// update HPD row identifier
  inline RichSmartID& setPixelRow  	( const unsigned int PixelRow  	);
  inline RichSmartID& setPixelCol  	( const unsigned int PixelCol  	);
  /// update row identifier
  inline RichSmartID& setHPDRow  	( const unsigned int HPDRow 	);
  /// update col     identifier
  inline RichSmartID& setHPDCol  	( const unsigned int HPDCol 	);
  /// update sicb    identifier
  inline RichSmartID& setSicb  		( const unsigned int Calo  ,
  																	const unsigned int Sicb  		);
  /// update sicb identifier
  inline RichSmartID& set      		( const ContentType& ,
                                    const unsigned int ,
                                    const ContentType& );
  /// update index   identifier
  inline RichSmartID& setIndex 		( const unsigned int Index );


};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline long RichSmartID::Key() const
{
  return m_Key;
}

inline void RichSmartID::setKey(ContentType& value)
{
  m_Key = value;
}

inline StreamBuffer& RichSmartID::serialize(StreamBuffer& s) const
{
  s << m_Key;
  return s;
}

inline StreamBuffer& RichSmartID::serialize(StreamBuffer& s)
{
  s >> m_Key;
  return s;
}

inline std::ostream& RichSmartID::fillStream(std::ostream& s) const
{
  s << "{ "
    << " Key:\t" << m_Key << " } ";
  return s;
}

///Serialise methods
inline StreamBuffer& operator >> (StreamBuffer& s, RichSmartID& key)  {
  long longKey;
  s >> longKey;
  key.setKey(longKey);
  return s;
}

///Serialise methods
inline StreamBuffer& operator << (StreamBuffer& s, const RichSmartID& key)  {
  s << key.Key();
  return s;
}

inline RichSmartID& RichSmartID::setRich ( const unsigned int Rich )
  { return set( Rich  , RichSmartCode::ShiftRich , RichSmartCode::MaskRich ); };
inline RichSmartID& RichSmartID::setPanel ( const unsigned int Panel )
  { return set( Panel, RichSmartCode::ShiftPanel, RichSmartCode::MaskPanel ); };
inline RichSmartID& RichSmartID::setPixelRow ( const unsigned int PixelRow  )
  { return set( PixelRow, RichSmartCode::ShiftPixelRow, RichSmartCode::MaskPixelRow ) ; };
inline RichSmartID& RichSmartID::setPixelCol ( const unsigned int PixelCol  )
  { return set( PixelCol, RichSmartCode::ShiftPixelCol, RichSmartCode::MaskPixelCol ) ; };
inline RichSmartID& RichSmartID::setHPDRow  ( const unsigned int HPDRow   )
  { return set( HPDRow, RichSmartCode::ShiftHPDRow, RichSmartCode::MaskHPDRow  ) ; };
inline RichSmartID& RichSmartID::setHPDCol  ( const unsigned int HPDCol   )
  { return set( HPDCol, RichSmartCode::ShiftHPDCol, RichSmartCode::MaskHPDCol  ) ; };
inline RichSmartID& RichSmartID::setIndex( const unsigned int Index )
  { return set( Index, RichSmartCode::ShiftIndex, RichSmartCode::MaskIndex) ; };

inline RichSmartID& RichSmartID::set ( const RichSmartID::ContentType&  Value ,
                                       const unsigned int             Shift ,
                                       const RichSmartID::ContentType& Mask  )
{
  ContentType tmp1 , tmp2 ;
  tmp1  = ( Value << Shift ) &  Mask ;
  tmp2  =  contents()        & ~Mask ;
  m_Key = ( tmp1 | tmp2 ) ;
  return *this;
};




// ** convert the value of pixel ID into bit pattern
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
      if      ( 0                          			== pos )   break      ;
      if      ( 0                          			== del )   continue   ;
      if      ( RichSmartCode::ShiftRest   			== pos ) { str += del ; }
      else if ( RichSmartCode::ShiftRich   			== pos ) { str += del ; }
      else if ( RichSmartCode::ShiftPanel  			== pos ) { str += del ; }
      else if ( RichSmartCode::ShiftPixelRow    == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftPixelCol    == pos ) { str += del ; }
      else if ( RichSmartCode::ShiftHPDRow 			== pos ) { str += del ; }
      else if ( RichSmartCode::ShiftHPDCol 			== pos ) { str += del ; }
      else if ( RichSmartCode::ShiftAll    			== pos ) { str += del ; }
    }
  return str += "]";
}
//
//
//
// overloaded output to std::ostream
inline std::ostream& operator<<( std::ostream& os , const RichSmartID& ccid )
  { return ccid.printOut( os ); };

//
//
//
// overloaded output to MsgStream
//inline MsgStream&    operator<<( MsgStream&    os , const RichSmartID& ccid ){ return ccid.printOut( os ); };

//
//
//
//output to StreamBuffer
//inline StreamBuffer& operator<<( StreamBuffer& s  , const RichSmartID& ccid )
//{ return s << (RichSmartID::ContentType) ccid; };

//
//
//
// input from StreamBuffer
//inline StreamBuffer& operator>>( StreamBuffer& s  ,       RichSmartID& ccid )
//{
  //RichSmartID::ContentType value;
  //s >> value;
  //ccid.m_ccid_id = value ;
  //return s;
//};

//
//
//
// print to std::ostream
inline std::ostream& RichSmartID::printOut( std::ostream& os ) const
{ return os << " RichSmartlID=" << (RichSmartID::ContentType) *this; };

//
//
//
// print to MsgStream
//inline MsgStream&    RichSmartID::printOut( MsgStream&    os ) const
//{ return os << " RichSmartID=" << (RichSmartID::ContentType) *this; };
#endif   ///Event_RichSmartID_H
