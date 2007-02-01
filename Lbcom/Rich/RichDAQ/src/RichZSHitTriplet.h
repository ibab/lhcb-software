
/** @file RichZSHitTriplet.h
 *
 *  Header file for RICH DAQ utility class : RichZSHitTriplet
 *
 *  CVS Log :-
 *  $Id: RichZSHitTriplet.h,v 1.13 2007-02-01 17:42:30 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHZSHITTRIPLET_H
#define RICHDAQ_RICHZSHITTRIPLET_H 1

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichDet/RichDAQDefinitions.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace DAQ
   *
   *  namespace for RICH DAQ software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace DAQ
  {

    /** @namespace RichZSHitTripletCode
     *
     *  Namespace for definitions related to RichZSHitTripletCode
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-06
     */
    namespace RichZSHitTripletCode 
    {

      /// Number of bits for each row or column number
      static const Rich::DAQ::ShortType BitsHit    = 5;

      // shift registers
      static const Rich::DAQ::ShortType ShiftRow0  = 0;
      static const Rich::DAQ::ShortType ShiftCol0  = ShiftRow0 + BitsHit;
      static const Rich::DAQ::ShortType ShiftRow1  = ShiftCol0 + BitsHit;
      static const Rich::DAQ::ShortType ShiftCol1  = ShiftRow1 + BitsHit;
      static const Rich::DAQ::ShortType ShiftRow2  = ShiftCol1 + BitsHit;
      static const Rich::DAQ::ShortType ShiftCol2  = ShiftRow2 + BitsHit;

      // The masks
      static const Rich::DAQ::LongType  MaskRow0   = ((1 << BitsHit)-1) << ShiftRow0;
      static const Rich::DAQ::LongType  MaskCol0   = ((1 << BitsHit)-1) << ShiftCol0;
      static const Rich::DAQ::LongType  MaskRow1   = ((1 << BitsHit)-1) << ShiftRow1;
      static const Rich::DAQ::LongType  MaskCol1   = ((1 << BitsHit)-1) << ShiftCol1;
      static const Rich::DAQ::LongType  MaskRow2   = ((1 << BitsHit)-1) << ShiftRow2;
      static const Rich::DAQ::LongType  MaskCol2   = ((1 << BitsHit)-1) << ShiftCol2;

      // the max row/column values storable
      static const Rich::DAQ::ShortType MaxRowCol  = ( 1 << BitsHit ) - 1;

    }

    /** @class RichZSHitTriplet RichZSHitTriplet.h
     *
     *  Utility class representing three hits packed into a single word
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-06
     */

    class RichZSHitTriplet 
    {

    public: // methods

      /// Copy Constructor
      RichZSHitTriplet( const RichZSHitTriplet & hits ) : m_data( hits.data() ) {}

      /// Constructor from Rich::DAQ::LongType
      RichZSHitTriplet( const Rich::DAQ::LongType data = 0 ) : m_data( data ) { }

      /// Constructor from three MCRichDigits
      RichZSHitTriplet( const LHCb::RichSmartID * digOne   ,  ///< Pointer to first LHCb::RichSmartID to store
                        const LHCb::RichSmartID * digTwo   ,  ///< Pointer to second LHCb::RichSmartID to store
                        const LHCb::RichSmartID * digThree    ///< Pointer to third LHCb::RichSmartID to store
                        )
        : m_data ( 0 )
      {
        if ( digOne ) {
          setRow0 ( digOne->pixelRow() );
          setCol0 ( digOne->pixelCol() );
        }
        if ( digTwo ) {
          setRow1 ( digTwo->pixelRow() );
          setCol1 ( digTwo->pixelCol() );
        }
        if ( digThree ) {
          setRow2 ( digThree->pixelRow() );
          setCol2 ( digThree->pixelCol() );
        }
      }

      /// Destructor
      ~RichZSHitTriplet( ) { }

      /// Retrieve the full value
      inline Rich::DAQ::LongType data() const { return m_data; }

      /// operator to convert to Rich::DAQ::LongType
      inline operator Rich::DAQ::LongType() const { return data(); }

      /// Set the first hit row number
      inline void setRow0( const Rich::DAQ::ShortType row )
      {
        dataInRange(row,RichZSHitTripletCode::MaxRowCol);
        set( row, RichZSHitTripletCode::ShiftRow0, RichZSHitTripletCode::MaskRow0 );
      }

      /// Set the first hit column number
      inline void setCol0( const Rich::DAQ::ShortType col )
      {
        dataInRange(col,RichZSHitTripletCode::MaxRowCol);
        set( col, RichZSHitTripletCode::ShiftCol0,RichZSHitTripletCode::MaskCol0 );
      }

      /// Set the second hit row number
      inline void setRow1( const Rich::DAQ::ShortType row )
      {
        dataInRange(row,RichZSHitTripletCode::MaxRowCol);
        set( row, RichZSHitTripletCode::ShiftRow1,RichZSHitTripletCode::MaskRow1 );
      }

      /// Set the second hit column number
      inline void setCol1( const Rich::DAQ::ShortType col )
      {
        dataInRange(col,RichZSHitTripletCode::MaxRowCol);
        set( col, RichZSHitTripletCode::ShiftCol1,RichZSHitTripletCode::MaskCol1 );
      }

      /// Set the third hit row number
      inline void setRow2( const Rich::DAQ::ShortType row )
      {
        dataInRange(row,RichZSHitTripletCode::MaxRowCol);
        set( row, RichZSHitTripletCode::ShiftRow2, RichZSHitTripletCode::MaskRow2 );
      }

      /// Set the third hit column number
      inline void setCol2( const Rich::DAQ::ShortType col )
      {
        dataInRange(col,RichZSHitTripletCode::MaxRowCol);
        set( col, RichZSHitTripletCode::ShiftCol2,RichZSHitTripletCode::MaskCol2 );
      }

      /// Retrieve the first hit row number
      inline Rich::DAQ::ShortType row0() const
      {
        return ( (data() & RichZSHitTripletCode::MaskRow0) >> RichZSHitTripletCode::ShiftRow0 );
      }

      /// Retrieve the second hit row number
      inline Rich::DAQ::ShortType row1() const
      {
        return ( (data() & RichZSHitTripletCode::MaskRow1) >> RichZSHitTripletCode::ShiftRow1 );
      }

      /// Retrieve the third hit row number
      inline Rich::DAQ::ShortType row2() const
      {
        return ( (data() & RichZSHitTripletCode::MaskRow2) >> RichZSHitTripletCode::ShiftRow2 );
      }

      /// Retrieve the first hit column number
      inline Rich::DAQ::ShortType col0() const
      {
        return ( (data() & RichZSHitTripletCode::MaskCol0) >> RichZSHitTripletCode::ShiftCol0 );
      }

      /// Retrieve the second hit column number
      inline Rich::DAQ::ShortType col1() const
      {
        return ( (data() & RichZSHitTripletCode::MaskCol1) >> RichZSHitTripletCode::ShiftCol1 );
      }

      /// Retrieve the third hit column number
      inline Rich::DAQ::ShortType col2() const
      {
        return ( (data() & RichZSHitTripletCode::MaskCol2) >> RichZSHitTripletCode::ShiftCol2 );
      }

    private: // methods

      /// Update the internal data
      inline void setData( const Rich::DAQ::LongType data ) { m_data = data;  }

      /// Set the data value for a given mask and shift value
      inline void set( const Rich::DAQ::ShortType value,
                       const Rich::DAQ::ShortType shift,
                       const Rich::DAQ::LongType  mask )
      {
        setData( ((value << shift) & mask) | (data() & ~mask) );
      }

      /// tests whether a given value is in range for a given data field
      inline void dataInRange( const Rich::DAQ::ShortType value,
                               const Rich::DAQ::ShortType max ) const
      {
        if ( value > max ) throw GaudiException( "Data out of range", "*RichZSHitTriplet*", StatusCode::FAILURE );
      }

    private: // data

      /// The data word
      Rich::DAQ::LongType m_data;

    };

  }
}

#endif // RICHDAQ_RICHZSHITTRIPLET_H
