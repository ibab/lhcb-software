// $Id: TfIDTypes.h,v 1.4 2007-08-28 14:41:23 jonrob Exp $
#ifndef TFKERNEL_TFIDTYPES_H
#define TFKERNEL_TFIDTYPES_H 1

#include "TfKernel/RegionID.h"

namespace Tf
{

  /// typedef for internal numeric type
  typedef int InternalIntType;

  //---------------------------------------------------------------------------------
  /** @class NumericType TfKernel/TfIDTypes.h
   *
   *  Simple class with numeric characteristics but some additional type safety
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  //---------------------------------------------------------------------------------
  template < class TYPE >
  class NumericType
  {
  public:
    /// Constructor
    explicit NumericType ( const TYPE id = 0 ) : m_id(id) { }
    /// Retrieve the full value
    inline TYPE data() const { return m_id; }
    /// Operator ==
    inline bool operator== ( const NumericType<TYPE>& id ) const
    { return id.data() == this->data() ; }
    /// Operator !=
    inline bool operator!= ( const NumericType<TYPE>& id ) const
    { return id.data() != this->data() ; }
    /// Operator <
    inline bool operator<  ( const NumericType<TYPE>& id ) const
    { return this->data() < id.data() ; }
    /// Operator >
    inline bool operator>  ( const NumericType<TYPE>& id ) const
    { return this->data() > id.data() ; }
  public:
    /// TYPE Operator
    inline operator TYPE() const { return data(); }
    /// Overload output to ostream
    friend inline std::ostream& operator << ( std::ostream& os, const NumericType<TYPE> & id )
    { return os << id.data() ; }
    /// Operator ++   (prefix)
    inline NumericType<TYPE>& operator++()    { ++m_id; return *this; }
    /// Operator ++(int)  (postfix)
    inline NumericType<TYPE>  operator++(int) { NumericType<TYPE> tmp = *this; ++m_id; return tmp; }
    /// Operator --   (prefix)
    inline NumericType<TYPE>& operator--()    { --m_id; return *this; }
    /// Operator --(int)  (postfix)
    inline NumericType<TYPE>  operator--(int) { NumericType<TYPE> tmp = *this; --m_id; return tmp; }
  protected:
    /// Update the internal data
    inline void setData( const TYPE id ) { m_id = id; }
  private:
    TYPE m_id; ///< The data value
  };

  /** @class TStationID TfKernel/TfIDTypes.h
   *
   *  Identifier for individual T Stations.
   *
   *  Valid T Station IDs are in the range 0 - 2, 0 being the most upstream
   *  station (nearest the magent) 2 being the most downstream (nearest RICH2).
   *
   *  See Tf::RegionID for more details
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class TStationID : public NumericType<InternalIntType>
  {
  public :
    /// Constructor
    TStationID ( const InternalIntType id = 0 ) : NumericType<InternalIntType>(id) { }
  };

  /** @class TTStationID TfKernel/TfIDTypes.h
   *
   *  Identifier for individual TT Stations.
   *
   *  Valid TT Station IDs are in the range 0 - 1, 0 being the most upstream
   *  station (nearest RICH1) 1 being the most downstream (nearest the magnet).
   *
   *  See Tf::RegionID for more details
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class TTStationID : public NumericType<InternalIntType>
  {
  public :
    /// Constructor
    TTStationID ( const InternalIntType id = 0 ) : NumericType<InternalIntType>(id) { }
  };

  /** @class TLayerID TfKernel/TfIDTypes.h
   *
   *  Identifier for layers within a T Station.
   *
   *  Valid T station layer IDs are in the range 0 to 3, 0 being the most upstream
   *  station (nearest the magent) 3 being the most downstream (nearest RICH2).
   *
   *  See Tf::RegionID for more details
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class TLayerID : public NumericType<InternalIntType>
  {
  public :
    /// Constructor
    TLayerID ( const InternalIntType id = 0 ) : NumericType<InternalIntType>(id) { }
  };

  /** @class TTLayerID TfKernel/TfIDTypes.h
   *
   *  Identifier for layers within a TT Station.
   *
   *  Valid TT station layer IDs are in the range 0 - 1, 0 being the most upstream
   *  station (nearest RICH1) 1 being the most downstream (nearest the magnet).
   *
   *  See Tf::RegionID for more details
   * 
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class TTLayerID : public NumericType<InternalIntType>
  {
  public :
    /// Constructor
    TTLayerID ( const InternalIntType id = 0 ) : NumericType<InternalIntType>(id) { }
  };

  /** @class ITRegionID TfKernel/TfIDTypes.h
   *
   *  Identifier for the various regions within an individual IT station layer.
   *
   *  Valid IT layer region IDs are in the range 0 to 3.
   *
   *  See Tf::RegionID for more details
   * 
   *  @todo A little ascii diagram of the IT regions ??
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class ITRegionID : public NumericType<InternalIntType>
  {
  public :
    /// Constructor
    ITRegionID ( const InternalIntType id = 0 ) : NumericType<InternalIntType>(id) { }
  };

  /** @class TTRegionID TfKernel/TfIDTypes.h
   *
   *  Identifier for the various regions within an individual TT station layer.
   *
   *  Valid TT layer region IDs are in the range 0 to 11.
   *
   *  See Tf::RegionID for more details
   *
   *  @todo A little ascii diagram of the TT regions ??
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class TTRegionID : public NumericType<InternalIntType>
  {
  public :
    /// Constructor
    TTRegionID ( const InternalIntType id = 0 ) : NumericType<InternalIntType>(id) { }
  };

  /** @class OTRegionID TfKernel/TfIDTypes.h
   *
   *  Identifier for the various regions within an individual OT station layer.
   *
   *  valid OT region IDs are in the range 0 to 1.
   *
   *  See Tf::RegionID for more details
   *
   *  @todo A little ascii diagram of the OT regions ??
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class OTRegionID : public NumericType<InternalIntType>
  {
  public :
    /// Constructor
    OTRegionID ( const InternalIntType id = 0 ) : NumericType<InternalIntType>(id) { }
  };

  /** @class TRegionID TfKernel/TfIDTypes.h
   *
   *  Identifier for the various regions within an individual T station layer.
   *
   *  Valid T Region IDs are in the range 0 to 5. The IDs 0 and 1 are for OT regions
   *  whilst 2-5 are for IT. TRegionIDs 2-5 map to ITRegionIDs 0-3.
   *
   *  See Tf::RegionID for more details
   *
   *  @todo A little ascii diagram of the T regions ??
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class TRegionID : public NumericType<InternalIntType>
  {
  public :
    /// Constructor
    TRegionID ( const InternalIntType id = 0 )
      : NumericType<InternalIntType>(id) { }
    /// Constructor from an OTRegionID
    TRegionID ( const OTRegionID& otID )
      : NumericType<InternalIntType>(otID.data()) { }
    /// Constructor from an ITRegionID
    TRegionID ( const ITRegionID& itID )
      : NumericType<InternalIntType>(Tf::RegionID::OTIndex::kNRegions+itID.data()) { }
  public:
    /// Convert the T region number to an OT region number
    inline OTRegionID otRegionID() const { return OTRegionID(data()); }
    /// Convert the T region number to an IT region number
    inline ITRegionID itRegionID() const
    {
      return ( data() < Tf::RegionID::OTIndex::kNRegions ? ITRegionID(-1) :
               ITRegionID(data()-Tf::RegionID::OTIndex::kNRegions) );
    }
  };

  /** @class VeloSensorID TfKernel/TfIDTypes.h
   *
   *  Identifier for Velo sensors.
   *
   *  Valid velo station IDs are in the range 0 to 20.
   *  
   *  See Tf::RegionID for more details
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class VeloSensorID : public NumericType<unsigned int>
  {
  public :
    /// Constructor
    VeloSensorID ( const unsigned int id = 0 ) : NumericType<unsigned int>(id) { }
  };

  /** @class VeloHalfID TfKernel/TfIDTypes.h
   *
   *  Identifier for a Velo half
   *
   *  Valid Velo hald IDs are in the range 0 to 1.
   *
   *  See Tf::RegionID for more details
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   11/11/2005
   */
  class VeloHalfID : public NumericType<unsigned int>
  {
  public :
    /// Constructor
    VeloHalfID ( const unsigned int id = 0 ) : NumericType<unsigned int>(id) { }
  };

}

#endif // TFKERNEL_TFIDTYPES_H
