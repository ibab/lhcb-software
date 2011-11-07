/** @file ZooLHCbID.h Zoo level representation of LHCbIDs
 *
 * @author Manuel Schiller
 * @date 2010-01-29
 */
#ifndef ZOOLHCBID_H
#define ZOOLHCBID_H

#include <ostream>
#include <exception>
#include <TObject.h>

#ifndef __CINT__
#include <boost/static_assert.hpp>
#else
#define BOOST_STATIC_ASSERT(x)
#endif

/// Zoo representation of a LHCb::LHCbID
class ZooLHCbID
{
    public:
	/// types of ZooLHCbIDs
	enum ChannelType {
	    Velo = 1, TT, IT, OT, Rich, Calo, Muon, VeloPix
	};
    protected:
	/// handle bit fields (i.e. one or several integer fields of specified width)
	/** this class is responsible to align the individual fields inside
	 * an integer type according to the specification given by the user
	 *
	 * access to individual fields is provided via templated getters
	 * and setters, see struct Field below for examples */
	template<typename I = unsigned> class Bitfield
	{ 
	    protected:
		/// data member to hold bitfield
		I m_value;
	    public:
		/// type which holds the data internally
		typedef I value_type;
		/// conversion to integer
		operator I() const { return m_value; }
		/// get an individual field
		template<typename B> I get() const
		{ return (m_value >> B::begin) & ((I(1) << B::width) - 1); }
		/// set all fields at once from a suitable integer
		void set(I val) { m_value = val; }
		/// set an individual field
		template<typename B, typename J> void set(J val)
		{
		    const I mask = (I(1) << B::width) - I(1);
		    // zero bits corresponding to the field
		    m_value &= ~(mask << B::begin);
		    // set the field to contain the first B::width bits of val
		    m_value |= (I(val) & mask) << B::begin;
		}
		/// template from which to construct individual fields
		/**
		 * In a derived class, one can define fields within the
		 * bitfield like this:
		 * @code
		 * struct Foo : public Bitfield<>
		 * {
		 *     typedef Field<0, 4> status;    // bits 0..3
		 *     typedef Field<4, 1> error_bit; // bit 4
		 *     typedef Field<5, 12> add_info; // bit 5..16
		 * };
		 * @endcode
		 *
		 * To access these fields, one can use code similar to the following
		 * example:
		 * @code
		 * Foo foo;
		 * // set status member
		 * foo.set<Foo::status>(0xc);
		 * // imagine some code which sets other members, i.e. error_bit and add_info
		 *
		 * // read out add_info
		 * unsigned add_info = foo.get<Foo::add_info>();
		 * @endcode
		 */
		template<unsigned Begin, unsigned Width> struct Field
		{
		    enum { begin = Begin, width = Width }; 
		};
	};

	/// a generic channel ID storage format
	struct _GenericChannelID : public Bitfield<> {
	    typedef Field<0, 28> channel;
	    typedef Field<28, 4> channeltype;
	};
	/// OT channel ID storage format
	struct _OTChannelID : public Bitfield<> {
	    typedef Field<0, 8> tdc;
	    typedef Field<8, 8> straw;
	    typedef Field<16, 4> module;
	    typedef Field<20, 2> quarter;
	    typedef Field<22, 2> layer;
	    typedef Field<24, 2> station;
	    typedef Field<28, 4> channeltype;
	    // aliases for combinations
	    typedef Field<8, 18> uniquestraw;
	    typedef Field<16, 10> uniquemodule;
	    typedef Field<20, 6> uniquequarter;
	    typedef Field<22, 4> uniquelayer;
	};
	/// ST channel ID storage format
	struct _STChannelID : public Bitfield<> {
	    typedef Field<0, 10> strip;
	    typedef Field<10, 5> sector;
	    typedef Field<15, 3> region;
	    typedef Field<18, 3> layer;
	    typedef Field<21, 2> station;
	    typedef Field<23, 1> type;
	    typedef Field<28, 4> channeltype;
	    // aliases for combinations
	    typedef Field<0, 23> uniquestrip;
	    typedef Field<10, 13> uniquesector;
	    typedef Field<15, 8> uniqueregion;
	    typedef Field<18, 5> uniquelayer;
	};
	/// Velo channel ID storage format
	struct _VeloChannelID : public Bitfield<> {
	    typedef Field<0, 11> strip;
	    typedef Field<11, 8> sensor;
	    typedef Field<19, 2> type;
	    typedef Field<28, 4> channeltype;
	    // aliases for combinations
	    typedef Field<0, 19> uniquestrip;
	};
	/** union to convert seamlessly between the flat unsigned int
	 * representation, a generic (channeltype, subdetchannel)
	 * representaion and specific representations
	 *
	 * for now we have ZooOTChannelID, ZooSTChannelID, ZooVeloChannelID
	 * 
	 * others can be added as needed
	 */
	union _LHCbID {
	    UInt_t lhcbid;
	    _GenericChannelID genid;
	    _OTChannelID otid;
	    _STChannelID stid;
	    _VeloChannelID veloid;
	} m_id;

	/// default constructor for use in derived classes
	/** careful: yields invalid ZooLHCbID */
	ZooLHCbID()
	{
	    BOOST_STATIC_ASSERT(sizeof(UInt_t) == sizeof(ZooLHCbID));
	    m_id.lhcbid = 0;
	}

	/// class to throw when things go wrong
	/** this class is intentionally not public; client code must check
	 * the type before convertion to one of the derived classes, i.e.
	 * there is no use in catching this exception
	 */
	class bad_cast : public std::exception
        {
	    private:
		const char* m_what;
	    public:
		/// default constructor
		bad_cast() throw () : m_what("invalid conversion of ZooLHCbID")
	        { }
		/// constructor with a custom message
		bad_cast(const char* what) throw() : m_what(what)
	        { }
		/// return the message
		virtual const char* what() const throw()
		{ return m_what; }
	};

    public:
	/// constructor from unsigned int
	explicit ZooLHCbID(unsigned id)
	{
	    BOOST_STATIC_ASSERT(sizeof(UInt_t) == sizeof(ZooLHCbID));
	    m_id.lhcbid = id;
	}

	/// convert to unsigned
	operator unsigned() const { return m_id.lhcbid; }
	/// convert to unsigned (for compat. w. LHCb::LHCbID)
	unsigned lhcbID() const { return *this; }
	/// return type of LHCbID
	ChannelType channeltype() const
	{ return static_cast<ChannelType>(m_id.genid.get<_GenericChannelID::channeltype>()); }
	/// return an opaque channel number
	/** cast to appropriate derived class for meaningful access to known
	 * channel types */
	unsigned channel() const { return m_id.genid.get<_GenericChannelID::channel>(); }
	/// comparison
	bool operator<(const ZooLHCbID& other) const
	{ return m_id.lhcbid < other.m_id.lhcbid; }
	/// comparison
	bool operator>(const ZooLHCbID& other) const
	{ return m_id.lhcbid > other.m_id.lhcbid; }
	/// comparison
	bool operator<=(const ZooLHCbID& other) const
	{ return m_id.lhcbid <= other.m_id.lhcbid; }
	/// comparison
	bool operator>=(const ZooLHCbID& other) const
	{ return m_id.lhcbid >= other.m_id.lhcbid; }
	/// comparison
	bool operator==(const ZooLHCbID& other) const
	{ return m_id.lhcbid == other.m_id.lhcbid; }
	/// comparison
	bool operator!=(const ZooLHCbID& other) const
	{ return m_id.lhcbid != other.m_id.lhcbid; }
};

/// printing ZooLHCbIDs and derived classes
/**
 * the reason to have knowledge about derived classes in operator<<
 * for ZooLHCbID is that we want a nice printout even if we have not
 * converted a ZooLHCbID to one of the derived classes
 */
std::ostream& operator<<(std::ostream& os, const ZooLHCbID& id);

/// Outer Tracker Channel ID
/**
 * <h3>OT Channel numbering scheme</h3>
 * for details, see
 * http://cdsweb.cern.ch/record/681313/files/lhcb-2003-041.pdf
 *
 * @code
 * quarter 3 (A side)          quarter 2 (C side)
 * +--+--+--+--+--+--+--+--+--+-+--+--+--+--+--+--+--+--+
 * | 1| 2| 3| 4| 5| 6| 7| 8| 9|9| 8| 7| 6| 5| 4| 3| 2| 1|
 * |  |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  +--+-+  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  +--+    +--+  |  |  |  |  |  |  |
 * +--+--+--+--+--+--+--|          |--+--+--+--+--+--+--+
 * |  |  |  |  |  |  |  +--+    +--+  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  +-+--+  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |  |    y ^  ¬
 * |  |  |  |  |  |  |  |  | |  |  |  |  |  |  |  |  |  |      | / z
 * | 1| 2| 3| 4| 5| 6| 7| 8|9| 9| 8| 7| 6| 5| 4| 3| 2| 1| x    |/
 * +--+--+--+--+--+--+--+--+-+--+--+--+--+--+--+--+--+--+ <----*
 * quarter 1 (A side)         quarter 0 (C side)
 * @endcode
 */
class ZooOTChannelID : public ZooLHCbID
{
    public:
	/// constructor
	ZooOTChannelID(unsigned station, unsigned layer, unsigned quarter,
		unsigned module, unsigned straw, unsigned tdc);

	/// conversion from ZooLHCbID
	ZooOTChannelID(const ZooLHCbID& other);

	/// assignment from ZooLHCbID
	ZooOTChannelID& operator=(const ZooLHCbID& other);

	/// return raw TDC time
	unsigned tdc() const { return m_id.otid.get<_OTChannelID::tdc>(); }
	/// return straw number
	unsigned straw() const { return m_id.otid.get<_OTChannelID::straw>(); }
	/// return module
	unsigned module() const { return m_id.otid.get<_OTChannelID::module>(); }
	/// return quarter
	unsigned quarter() const { return m_id.otid.get<_OTChannelID::quarter>(); }
	/// return layer
	unsigned layer() const { return m_id.otid.get<_OTChannelID::layer>(); }
	/// return station
	unsigned station() const { return m_id.otid.get<_OTChannelID::station>(); }

	/// return unique straw number
	unsigned uniqueStraw() const
	{ return m_id.otid.get<_OTChannelID::uniquestraw>(); }
	/// return unique module
	unsigned uniqueModule() const
	{ return m_id.otid.get<_OTChannelID::uniquemodule>(); }
	/// return unique quarter
	unsigned uniqueQuarter() const
	{ return m_id.otid.get<_OTChannelID::uniquequarter>(); }
	/// return unique layer
	unsigned uniqueLayer() const
	{ return m_id.otid.get<_OTChannelID::uniquelayer>(); }

	/// true if two straws are the same
	bool isSameStraw(ZooOTChannelID other) const
	{ return uniqueStraw() == other.uniqueStraw(); }

	/// true if channel is in x layer
	bool isX() const { return 0 == layer() || 3 == layer(); }
	/// true if channel is in u layer
	bool isU() const { return 1 == layer(); }
	/// true if channel is in v layer
	bool isV() const { return 2 == layer(); }
	/// true if channel is in stereo layer
	bool isStereo() const { return isU() || isV(); }

	/// true if channel's quarter is above beampipe
	bool isTop() const { return quarter() > 1; }
	/// true if channel's quarter is below beampipe
	bool isBottom() const { return !isTop(); }
	/// true if channel's quarter is on A side
	bool isASide() const { return quarter() & 1; }
	/// true if channel's quarter is on C side
	bool isCSide() const { return !isASide(); }
};

/// Silicon Tracker Channel ID
/**
 * <h3>TT numbering scheme</h3>
 *
 * see http://cdsweb.cern.ch/record/1067495/files/lhcb-2007-137.pdf
 * for details on the numbering scheme used...
 *
 * TTA sensor ids/detRegion: 66 entries needed for a TTA layer
 * @code
 *  11111111111111111 22222222 33333333333333333
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ ^ y
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | |
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * | 4| 8|12|16|20|24| 6|12|18| 4| 8|12|16|20|24|
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |  |  |  |  |  |  | 5|11|17|  |  |  |  |  |  |
 * |  |  |  |  |  |  +--+--+--+  |  |  |  |  |  |
 * | 3| 7|11|15|19|23| 4|10|16| 3| 7|11|15|19|23|
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |  |  |  |  |  |  | 3| 9|15|  |  |  |  |  |  |
 * |  |  |  |  |  |  +--+--+--+  |  |  |  |  |  |
 * | 2| 6|10|14|18|22| 2| 8|14| 2| 6|10|14|18|22|
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * | 1| 5| 9|13|17|21| 1| 7|13| 1| 5| 9|13|17|21|   x
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ -->
 * @endcode
 *
 * TTB sensor ids/detRegion: 74 entries needed for a TTB layer
 * @code
 *  11111111111111111 22222222222222 33333333333333333
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ ^ y
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | |
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * | 4| 8|12|16|20|24| 4|10|16|22|26| 4| 8|12|16|20|24|
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |  |  |  |  |  |  |  | 9|15|21|  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  +--+--+--+  |  |  |  |  |  |  |
 * | 3| 7|11|15|19|23| 3| 8|14|20|25| 3| 7|11|15|19|23|
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |  |  |  |  |  |  |  | 7|13|19|  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  +--+--+--+  |  |  |  |  |  |  |
 * | 2| 6|10|14|18|22| 2| 6|12|18|24| 2| 6|10|14|18|22|
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 * | 1| 5| 9|13|17|21| 1| 5|11|17|23| 1| 5| 9|13|17|21|   x
 * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ -->
 * @endcode
 *
 * <h3>ST numbering scheme</h3>
 *
 * for details, see
 * http://cdsweb.cern.ch/record/1067495/files/lhcb-2007-137.pdf
 *
 * @code
 *                                   ^ y
 *           region 4                |
 *           +-+-+-+-+-+-+-+ Top     +--->
 *           |1|2|3|4|5|6|7|             x
 * region 1  +-+-+-+-+-+-+-+  region 2
 * +-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+
 * | | | | | | | |     |1|2|3|4|5|6|7|
 * |1|2|3|4|5|6|7|     | | | | | | | |
 * +-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+
 * C         +-+-+-+-+-+-+-+         A
 *           |1|2|3|4|5|6|7|  
 *           +-+-+-+-+-+-+-+ Bottom
 *           region 3
 * @endcode
 */
class ZooSTChannelID : public ZooLHCbID
{
    public:
	/// constructor
	ZooSTChannelID(unsigned type, unsigned station, unsigned layer,
		unsigned region, unsigned sector, unsigned strip);

	/// conversion from ZooLHCbID
	ZooSTChannelID(const ZooLHCbID other);

	/// assignment from ZooLHCbID
	ZooSTChannelID& operator=(const ZooLHCbID other);

	/// return strip number
	unsigned strip() const { return m_id.stid.get<_STChannelID::strip>(); }
	/// return sector number
	unsigned sector() const { return m_id.stid.get<_STChannelID::sector>(); }
	/// return region number
	unsigned region() const { return m_id.stid.get<_STChannelID::region>(); }
	/// return layer number
	unsigned layer() const { return m_id.stid.get<_STChannelID::layer>(); }
	/// return station
	unsigned station() const { return m_id.stid.get<_STChannelID::station>(); }
	/// return type (0: TT, 1: IT)
	unsigned type() const { return m_id.stid.get<_STChannelID::type>(); }
	/// true if channel is in TT
	bool isTT() const { return 0 == type(); }
	/// true if channel is in IT
	bool isIT() const { return 1 == type(); }

	/// return unique strip number (within either IT or TT)
	unsigned uniqueStrip() const
	{ return m_id.stid.get<_STChannelID::uniquestrip>(); }
	/// return unique sector number (within either IT or TT)
	unsigned uniqueSector() const
	{ return m_id.stid.get<_STChannelID::uniquesector>(); }
	/// return unique region number (within either IT or TT)
	unsigned uniqueRegion() const
	{ return m_id.stid.get<_STChannelID::uniqueregion>(); }
	/// return unique layer number (within either IT or TT)
	unsigned uniqueLayer() const
	{ return m_id.stid.get<_STChannelID::uniquelayer>(); }

	/// true if channel is in x layer
	bool isX() const
	{ return isIT()?(0 == layer() || 3 == layer()):(layer() == station()); }
	/// true if channel is in u layer
	bool isU() const
	{ return isIT()?(1 == layer()):(0 == station() && 1 == layer()); }
	/// true if channel is in v layer
	bool isV() const
	{ return isIT()?(2 == layer()):(1 == station() && 0 == layer()); }
	/// true if channel is in stereo layer
	bool isStereo() const
	{ return !isX(); }

	/// is top part of detector
	/** In the IT, this is true for Top boxes.
	 *
	 * In the TT, this is true for channels above the xz plane.
	 */
	bool isTop() const;
	/// is channel in lower part of detector
	/** In the IT, this is true for Bottom boxes.
	 *
	 * In the TT, this is true for channels below the xz plane.
	 */
	bool isBottom() const { return isIT()?(3 == region()):(!isTop()); }
	/// is channel on A side?
	/** For the IT, this just corresponds to A side box. In TT, this
	 * corresponds to sensors which are on the A side (positive x). For
	 * the sensors in the middle (those which touch x = 0), see the note
	 * mentioned above.
	 */
	bool isASide() const;
	/// is channel on C side?
	/** For the IT, this just corresponds to C side box. In TT, this
	 * corresponds to sensors which are on the C side (negative x). For
	 * the sensors in the middle (those which touch x = 0), see the note
	 * mentioned above.
	 */
	bool isCSide() const { return isIT()?(2 == region()):(!isASide()); }
};

/// VertexLocator Channel ID
/**
 * p.c. from Kazu Akiba:
 * - A side sensors have even numbers, C side odd
 * - Phi sensors have numbers with bit 6 set, R with bit 6 cleared
 * - pile-up sensors have numbers with bit 7 set
 * - sensors are in order of increasing z
 * - pile-up sensors are at lower z than the ordinary ones
 */
class ZooVeloChannelID : public ZooLHCbID
{
    public:
	/// sensor types in Velo
	enum SensorType { RType = 1, PhiType = 2, PileUpType = 3 };

	/// constructor
	ZooVeloChannelID(unsigned sensor, unsigned strip);

	/// conversion from ZooLHCbID
	ZooVeloChannelID(const ZooLHCbID other);

	/// assignment from ZooLHCbID
	ZooVeloChannelID& operator=(const ZooLHCbID other);

	/// return strip number
	unsigned strip() const { return m_id.veloid.get<_VeloChannelID::strip>(); }
	/// return sector number
	unsigned sensor() const { return m_id.veloid.get<_VeloChannelID::sensor>(); }
	/// return type
	SensorType type() const
	{ return static_cast<SensorType>(m_id.veloid.get<_VeloChannelID::type>()); }
	/// return unique strip number
	unsigned uniqueStrip() const
	{ return m_id.veloid.get<_VeloChannelID::uniquestrip>(); }
	/// true if C side sensor
	bool isCSide() const { return sensor() & 1; }
	/// true if A side sensor
	bool isASide() const { return !isCSide(); }
	/// true if Phi sensor
	bool isPhiType() const { return sensor() & 0x40; }
	/// true if R sensor
	bool isRType() const { return !isPhiType(); }
	/// true if pile-up sensor
	bool isPileUp() const { return sensor() & 0x80; }
};

/// class which packs together multiple LHCbIDs
class ZooLHCbIDBlock : public TObject
{
    private:
	std::vector<UInt_t> m_ids;
    public:
	/// default constructor
	ZooLHCbIDBlock() { }
	/// assignment from e.g. vector of LHCbIDs or ZooLHCbIDs
	template<class T> ZooLHCbIDBlock& operator=(const std::vector<T>& other)
	{
	    // pack using subtraction - saves about 1/4 of disk space
	    m_ids.clear();
	    m_ids.reserve(other.size());
	    UInt_t last = 0;
	    for (unsigned i = 0; i < other.size(); ++i) {
		const UInt_t tmp = other[i].lhcbID();
		m_ids.push_back(tmp - last);
		last = tmp;
	    }
	    return *this;
	}
	/// constructor from e.g. vector of LHCbIDs or ZooLHCbIDs
	template<class T> ZooLHCbIDBlock(const std::vector<T>& other)
	{ *this = other; }

	/// return number of ZooLHCbIDs contained
	std::size_t size() const { return m_ids.size(); }

	/// return a vector of ZooLHCbIDs
	operator std::vector<ZooLHCbID>() const;

	ClassDef(ZooLHCbIDBlock, 1)
};

#endif // ZOOLHCBID_H

// vim: tw=78:sw=4:ft=cpp
