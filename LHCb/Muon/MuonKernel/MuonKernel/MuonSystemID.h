// $Id: MuonSystemID.h,v 1.1.1.1 2002-01-28 08:48:24 atsareg Exp $
//
#ifndef      MUONKERNEL_MUONSYSTEMID_H
#define      MUONKERNEL_MUONSYSTEMID_H 1  
// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h" 
// Muon base
#include "MuonKernel/MuonBase.h"
#include "MuonKernel/MuonLayout.h"

/** @class MuonSystemID MuonSystemID.h MuonKernel/MuonSystemID.h

    Class to represent basic identifier for various items of the 
    Muon system

    @author  Andrei Tsaregorodtsev
    @date    26/12/2001
*/ 

// class MuonStationLayout;
// class MuonSystemLayout;

class MuonSystemID {

public:

  friend class MuonSystemLayout;

  typedef MuonBase::ContentType  ContentType ; 
  ///  Default constructor
  MuonSystemID() : m_muonid(0) {}
  ///  Copy constructor
  MuonSystemID(const MuonSystemID& id) {
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
  MuonSystemID(int station,
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
  /** Constructor from similar MuonSystemID
      @param   MuonSystemID      MuonSystemID containing general part of the identifier
      @param   quarter     Quarter within a station
      @param   region      Region within a station
      @param   x           X index of the item
      @param   y           Y index of the item
  */ 
  MuonSystemID(const MuonSystemID& id,
	 const unsigned int region,
	 const unsigned int quarter,
	 const unsigned int x,
	 const unsigned int y) : m_muonid(id.m_muonid) {
	   setQuarter(quarter);
	   setRegion(region);
	   setX(x);
	   setY(y);
	 }  
  /** Constructor from relative position in the containing "MuonTile" MuonSystemID
      @param   MuonSystemID      MuonSystemID of the containing muon system element
      @param   layout      MuonLayout of the element which ID is being constructed
      @param   x           relative X index of the item
      @param   y           relative Y index of the item
  */ 
  MuonSystemID(const MuonSystemID& id,
               const IMuonLayout& layout,
	       const unsigned int x,
	       const unsigned int y) ;
  /// destructor
  ~MuonSystemID() {} 

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
  inline const unsigned int longIndex() const { 
    return  ( m_muonid & MuonBase::MaskTile ) >> MuonBase::ShiftTile ; 
  }  
  /// This is a key for containers of MuonTile's
  inline const unsigned int key() const { 
    return  longIndex() ; 
  }
  
  /** comparison operator using index. 
   *  @param ID cell ID to be compared 
   *  @return true if own index is LESS than index of ID 
   */
  inline bool operator < ( const MuonSystemID& id ) const 
    { return index() <  id.index() ; }

  /// equality operator using full contents 
  inline bool operator ==( const MuonSystemID& id ) const 
  { return m_muonid == id.m_muonid ; }

  /// non-equality operator using full contents 
  inline bool operator !=( const MuonSystemID& id ) const 
  { return !(id == *this) ; }
   
  /// serialization for reading 
  inline StreamBuffer& serialize( StreamBuffer& )       ;
  
  /// serialization for writing 
  inline StreamBuffer& serialize( StreamBuffer& ) const ;

  /// printout to std::ostream 
  inline std::ostream& printOut ( std::ostream& ) const ;

  /// printout to MsgStream
  inline MsgStream&    printOut ( MsgStream&    ) const ;
  
  /// Find the MuonSystemID which is an interception of two MuonSystemID's
  MuonSystemID intercept(const MuonSystemID& id) const;
  
  /** Find the MuonSystemID of a Tile of a given layout containing this pad
    @param   layout   container layout
  */
  MuonSystemID getContainerID(const IMuonLayout& layout) const ;
  
//   /** Find the MuonSystemID of a Tile of a given layout containing this pad
//     @param   layout   container station layout
//   */
//   MuonSystemID getContainerID(const MuonStationLayout& layout) const ;
//   
//   /** Find the MuonSystemID of a Tile of a given layout containing this pad
//     @param   layout   container muon system layout
//   */
//   MuonSystemID getContainerID(const MuonSystemLayout& layout) const ;

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
  
  /// Check that the MuonSystemID is consistent
  bool isValid() const;
  
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
  inline void set ( const MuonSystemID::ContentType      Value ,  
                    const unsigned int  	   Shift ,
                    const MuonSystemID::ContentType&     Mask  );

private:
  /// Internal representation of the identifier
  unsigned int m_muonid;
};

// Function definitions

inline void MuonSystemID::setStation( const unsigned int station ){
  set( station, MuonBase::ShiftStation, MuonBase::MaskStation ) ;
}  
inline void MuonSystemID::setLayer( const unsigned int layer ){
  set( layer, MuonBase::ShiftLayer, MuonBase::MaskLayer ) ;
}
inline void MuonSystemID::setReadout( const unsigned int readout ){
  set( readout, MuonBase::ShiftReadout, MuonBase::MaskReadout ) ;
}
inline void MuonSystemID::setRegion( const unsigned int region ){
  set( region, MuonBase::ShiftRegion, MuonBase::MaskRegion ) ;
}
inline void MuonSystemID::setQuarter( const unsigned int quarter ){
  set( quarter, MuonBase::ShiftQuarter, MuonBase::MaskQuarter ) ;
}
inline void MuonSystemID::setX( const unsigned int x ){
  set( x, MuonBase::ShiftX, MuonBase::MaskX ) ;
}
inline void MuonSystemID::setY( const unsigned int y ){
  set( y, MuonBase::ShiftY, MuonBase::MaskY ) ;
}
inline void MuonSystemID::setLayout( MuonLayout layout ){
  unsigned int lx, ly;
  lx = layout.xGrid();
  ly = layout.yGrid();
  set( lx, MuonBase::ShiftLayoutX, MuonBase::MaskLayoutX ) ;
  set( ly, MuonBase::ShiftLayoutY, MuonBase::MaskLayoutY ) ;
}

inline void MuonSystemID::set ( const MuonSystemID::ContentType      Value ,  
                          const unsigned int             Shift ,
                          const MuonSystemID::ContentType& Mask  ) {
  ContentType tmp1 , tmp2 ;
  tmp1 = ( Value << Shift ) & Mask ; 
  tmp2 = m_muonid & ~Mask ; 
  m_muonid = ( tmp1 | tmp2 ) ;
}

// output to std::ostream 
inline std::ostream& operator<< ( std::ostream& os , const MuonSystemID& id ) {
  return id.printOut( os );  
}

// output to MsgStream 
inline MsgStream& operator<<( MsgStream&    os , const MuonSystemID& id ) {
  return id.printOut( os ); 
}

// output to StreamBuffer
inline StreamBuffer& operator<< ( StreamBuffer& s  , const MuonSystemID& id ) { 
  return id.serialize( s ) ; 
}

// input from StreamBuffer
inline StreamBuffer& operator>>( StreamBuffer& s  ,  MuonSystemID& id ) {
  return id.serialize( s ); 
}

// input from StreamBuffer
inline StreamBuffer& MuonSystemID::serialize( StreamBuffer& sb ) {
  ContentType tmp ;
  sb >> tmp;
  m_muonid = tmp;
  return sb;  
}

// output to StreamBuffer 
inline StreamBuffer& MuonSystemID::serialize( StreamBuffer& sb ) const {
  return sb << m_muonid; 
}

// print to std::ostream
inline std::ostream& MuonSystemID::printOut( std::ostream& os ) const { 
  os << "[" ;
  return 
    os <<  layout() << ","
       <<  station() << ","  
       <<  region() << "," 
       <<  quarter() << "," 
       <<  nX() << "," 
       <<  nY() << "]" ;  
}

// print to MsgStream
inline MsgStream& MuonSystemID::printOut( MsgStream& os ) const { 
  os << "[" ;
  return 
    os <<  layout() << ","
       <<  station() << ","  
       <<  region() << "," 
       <<  quarter() << "," 
       <<  nX() << "," 
       <<  nY() << "]" ;   
}

#endif  // MUONKERNEL_MUONSYSTEMID_H 
