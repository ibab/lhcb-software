// $Id: MuonTileID.h,v 1.1 2002-04-18 07:41:40 cattanem Exp $
//
#ifndef      MUONKERNEL_MUONTILEID_H
#define      MUONKERNEL_MUONTILEID_H 1  

#ifndef MUONKERNEL_LOCAL
// GaudiKernel
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h" 
#endif

// Muon base
#include "MuonKernel/MuonBase.h"
#include "MuonKernel/MuonLayout.h"

/** @class MuonTileID MuonTileID.h MuonKernel/MuonTileID.h

    Class to represent basic identifier for various items of the 
    Muon system

    @author  Andrei Tsaregorodtsev
    @date    26/12/2001
*/ 

// class MuonStationLayout;
// class MuonSystemLayout;

class MuonTileID {

public:

  friend class MuonSystemLayout;

  typedef MuonBase::ContentType  ContentType ; 
  ///  Default constructor
  MuonTileID() : m_muonid(0) {}
  ///  Copy constructor
  MuonTileID(const MuonTileID& id) {
    m_muonid = id.m_muonid; 
  }
  
  /** Constructor with all arguments
      @param   station     Muon station number
      @param   layer       Layer within a station
      @param   readout     Readout: vertical or horizontal strips
      @param   layout      Muon Layout for this region
      @param   quarter     Quarter within a station
      @param   region      Region within a station
      @param   x           X index of the item
      @param   y           Y index of the item
  */ 
  MuonTileID(int station,
               int layer,
	       int readout,
	       const IMuonLayout& lay,
	       int region,
	       int quarter,
	       int x,
	       int y) : m_muonid(0) {
	   setStation(station);
	   setLayer(layer);
	   setReadout(readout);
	   setRegion(region);
	   setQuarter(quarter);
	   setX(x);
	   setY(y);
	   setLayout(MuonLayout(lay.grid(*this)));
	 }
  /** Constructor from similar MuonTileID
      @param   MuonTileID      MuonTileID containing general part of the identifier
      @param   quarter     Quarter within a station
      @param   region      Region within a station
      @param   x           X index of the item
      @param   y           Y index of the item
  */ 
  MuonTileID(const MuonTileID& id,
	 const unsigned int region,
	 const unsigned int quarter,
	 const unsigned int x,
	 const unsigned int y) : m_muonid(id.m_muonid) {
	   setQuarter(quarter);
	   setRegion(region);
	   setX(x);
	   setY(y);
	 }  
  /** Constructor from relative position in the containing "MuonTile" MuonTileID
      @param   MuonTileID      MuonTileID of the containing muon system element
      @param   layout      MuonLayout of the element which ID is being constructed
      @param   x           relative X index of the item
      @param   y           relative Y index of the item
  */ 
  MuonTileID(const MuonTileID& id,
               const IMuonLayout& layout,
	       const unsigned int x,
	       const unsigned int y) ;
	      
  /// constructor taking a long needed for the keyed container 
  MuonTileID(const long int& muonid){
    m_muonid = static_cast<unsigned int>(muonid);
  }
  /// destructor
  ~MuonTileID() {} 
  
  /// convert the MuonTileID to an int for use as a key
  operator long int() const {
    return static_cast<long int>(m_muonid);
  }
  /// Function to extract station
  inline const unsigned int station() const { 
    return  ( m_muonid & MuonBase::MaskStation ) >> MuonBase::ShiftStation ; 
  }
  /// Function to extract layer
  inline const unsigned int layer() const { 
    return  ( m_muonid & MuonBase::MaskLayer ) >> MuonBase::ShiftLayer ; 
  }
  /// Function to extract readout
  inline const unsigned int readout() const { 
    return  ( m_muonid & MuonBase::MaskReadout ) >> MuonBase::ShiftReadout ; 
  }
  /// Function to extract region
  inline const unsigned int region() const { 
    return  ( m_muonid & MuonBase::MaskRegion ) >> MuonBase::ShiftRegion ; 
  }
  /// Function to extract quarter
  inline const unsigned int quarter() const { 
    return  ( m_muonid & MuonBase::MaskQuarter ) >> MuonBase::ShiftQuarter ; 
  }
  /// Function to extract layout
  inline MuonLayout layout() const { 
    int xg = ( m_muonid & MuonBase::MaskLayoutX ) >> MuonBase::ShiftLayoutX ; 
    int yg = ( m_muonid & MuonBase::MaskLayoutY ) >> MuonBase::ShiftLayoutY ;
    return  MuonLayout(xg,yg) ; 
  }
  /// Function to extract index in X
  inline const unsigned int nX() const { 
    return  ( m_muonid & MuonBase::MaskX ) >> MuonBase::ShiftX ; 
  }
  /// Function to extract index in Y
  inline const unsigned int nY() const { 
    return  ( m_muonid & MuonBase::MaskY ) >> MuonBase::ShiftY ; 
  }
  /// Function to extract lower part of the identifier
  inline const unsigned int index() const { 
    return  ( m_muonid & MuonBase::MaskIndex ) >> MuonBase::ShiftIndex ; 
  }
  /// Function to extract lower part of the identifier with station information
  inline const unsigned int key() const { 
    return  ( m_muonid & MuonBase::MaskKey ) >> MuonBase::ShiftKey ; 
  }  

  
  /** comparison operator using key. 
   *  @param ID cell ID to be compared 
   *  @return true if own index is LESS than index of ID 
   */
  inline bool operator < ( const MuonTileID& id ) const 
    { return key() <  id.key() ; }

  /// equality operator using full contents 
  inline bool operator ==( const MuonTileID& id ) const 
  { return m_muonid == id.m_muonid ; }

  /// non-equality operator using full contents 
  inline bool operator !=( const MuonTileID& id ) const 
  { return !(id == *this) ; }
   
#ifndef MUONKERNEL_LOCAL   
  /// serialization for reading 
  inline StreamBuffer& serialize( StreamBuffer& )       ;
  
  /// serialization for writing 
  inline StreamBuffer& serialize( StreamBuffer& ) const ;
  
  /// printout to MsgStream
  inline MsgStream&    printOut ( MsgStream&    ) const ;
#endif  // MUONKERNEL_LOCAL   
  
  /// printout to std::ostream 
  inline std::ostream& printOut ( std::ostream& ) const ;
  
  /// Find the MuonTileID which is an interception of two MuonTileID's
  MuonTileID intercept(const MuonTileID& id) const;
  
  /** Find the MuonTileID of a Tile of a given layout containing this pad
    @param   layout   container layout
  */
  MuonTileID containerID(const IMuonLayout& layout) const ;
  
  /** Find the MuonTileID of a Tile which is a neighbour of this pad.
      The neighbour will be defined in terms of the same layout and region
      as this pad. It means that the returned MuonTileID might not be 
      valid.
    @param   dirX   horizontal direction in which to look for the neighbour
    @param   dirY   vertical direction in which to look for the neighbour
  */
  MuonTileID neighbourID(int dirX, int dirY) const;

  /** Get local offset X with respect to the container defined by the
      the given IMuonLayout
      @param   layout   container layout
  */  
  int localX(const IMuonLayout& layout) const ;
    
  /** Get local offset Y with respect to the container defined by the
      the given IMuonLayout
      @param   layout   container layout
  */  
  int localY(const IMuonLayout& layout) const ;   
  
  /// Check that the MuonTileID is consistent in terms of its layout
  bool isValid() const;
  
  /// Check that the MuonTileID is defined. It means that its code is not 0.
  /// It can be however not a valid id.
  bool isDefined() const;
  
private:
  /// update station identifier 
  inline void setStation  ( const unsigned int station );  
  
  /// update layer identifier 
  inline void setLayer  ( const unsigned int layer );
  
  /// update readout identifier 
  inline void setReadout  ( const unsigned int readout );  
  
  /// update layout identifier 
  inline void setLayout  ( MuonLayout layout );  
  
  /// update region identifier 
  inline void setRegion  ( const unsigned int region );  
  
  /// update quarter identifier 
  inline void setQuarter  ( const unsigned int quarter ); 
  
  /// update index in X
  inline void setX ( const unsigned int x ); 
  
  /// update index in Y
  inline void setY ( const unsigned int y ); 
  
  /// set arbitrary field of the identifier
  inline void set ( const MuonTileID::ContentType      Value ,  
                    const unsigned int  	   Shift ,
                    const MuonTileID::ContentType&     Mask  );

private:
  /// Internal representation of the identifier
  unsigned int m_muonid;
};

// Function definitions

inline void MuonTileID::setStation( const unsigned int station ){
  set( station, MuonBase::ShiftStation, MuonBase::MaskStation ) ;
}  
inline void MuonTileID::setLayer( const unsigned int layer ){
  set( layer, MuonBase::ShiftLayer, MuonBase::MaskLayer ) ;
}
inline void MuonTileID::setReadout( const unsigned int readout ){
  set( readout, MuonBase::ShiftReadout, MuonBase::MaskReadout ) ;
}
inline void MuonTileID::setRegion( const unsigned int region ){
  set( region, MuonBase::ShiftRegion, MuonBase::MaskRegion ) ;
}
inline void MuonTileID::setQuarter( const unsigned int quarter ){
  set( quarter, MuonBase::ShiftQuarter, MuonBase::MaskQuarter ) ;
}
inline void MuonTileID::setX( const unsigned int x ){
  set( x, MuonBase::ShiftX, MuonBase::MaskX ) ;
}
inline void MuonTileID::setY( const unsigned int y ){
  set( y, MuonBase::ShiftY, MuonBase::MaskY ) ;
}
inline void MuonTileID::setLayout( MuonLayout layout ){
  unsigned int lx, ly;
  lx = layout.xGrid();
  ly = layout.yGrid();
  set( lx, MuonBase::ShiftLayoutX, MuonBase::MaskLayoutX ) ;
  set( ly, MuonBase::ShiftLayoutY, MuonBase::MaskLayoutY ) ;
}

inline void MuonTileID::set ( const MuonTileID::ContentType      Value ,  
                          const unsigned int             Shift ,
                          const MuonTileID::ContentType& Mask  ) {
  ContentType tmp1 , tmp2 ;
  tmp1 = ( Value << Shift ) & Mask ; 
  tmp2 = m_muonid & ~Mask ; 
  m_muonid = ( tmp1 | tmp2 ) ;
}

// output to std::ostream 
inline std::ostream& operator<< ( std::ostream& os , const MuonTileID& id ) {
  return id.printOut( os );  
}


#ifndef MUONKERNEL_LOCAL
// output to MsgStream 
inline MsgStream& operator<<( MsgStream&    os , const MuonTileID& id ) {
  return id.printOut( os ); 
}


// output to StreamBuffer
inline StreamBuffer& operator<< ( StreamBuffer& s  , const MuonTileID& id ) { 
  return id.serialize( s ) ; 
}

// input from StreamBuffer
inline StreamBuffer& operator>>( StreamBuffer& s  ,  MuonTileID& id ) {
  return id.serialize( s ); 
}

// input from StreamBuffer
inline StreamBuffer& MuonTileID::serialize( StreamBuffer& sb ) {
  ContentType tmp ;
  sb >> tmp;
  m_muonid = tmp;
  return sb;  
}

// output to StreamBuffer 
inline StreamBuffer& MuonTileID::serialize( StreamBuffer& sb ) const {
  return sb << m_muonid; 
}

// print to MsgStream
inline MsgStream& MuonTileID::printOut( MsgStream& os ) const { 
  os << "[" ;
  return 
    os <<  layout() << ","
       <<  station() << ","  
       <<  region() << "," 
       <<  quarter() << "," 
       <<  nX() << "," 
       <<  nY() << "]" ;   
}
#endif  // MUONKERNEL_LOCAL

// print to std::ostream
inline std::ostream& MuonTileID::printOut( std::ostream& os ) const { 
  os << "[" ;
  return 
    os <<  layout() << ","
       <<  station() << ","  
       <<  region() << "," 
       <<  quarter() << "," 
       <<  nX() << "," 
       <<  nY() << "]" ;  
}

#endif  // MUONKERNEL_MUONTILEID_H 
