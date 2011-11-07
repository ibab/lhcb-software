/** @file ZooOccupancy.h
 * 
 * @author Manuel Schiller
 * @date 2010-01-20
 */
#ifndef ZOOOCCUPANCY_H
#define ZOOOCCUPANCY_H

#include <vector>

#include "TObject.h"
#include "TClass.h"

/// helpers for class ZooOccupancy
namespace ZooOccupancyHelper {
    /// mapping from station, region, sensor to storage order
    /**
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
     */
    const int TTIndex[2][3][26] = {
	{
      	    {    0,  30,  59,  29,   1,    31,  58,  28,   2,   32,
		57,  27,   3,  33,  56,    26,   4,  34,  55,   25,
		 5,  35,  54,  24,  -1,    -1 },
	    {    6,  36,  60,  65,  53,    23,   7,  37,  61,  64,
		52,  22,   8,  38,  62,    63,  51,  21,  -1,  -1,
		-1,  -1,  -1,  -1,  -1,    -1 },
	    {    9,  39,  50,  20,  10,   40,  49,  19,  11,  41,
		48,  18,  12,  42,  47,   17,  13,  43,  46,  16,
		14,  44,  45,  15,  -1,   -1 }
	}, {
	    {    0,  34,  67,  33,   1,    35,  66,  32,   2,   36,
		65,  31,   3,  37,  64,    30,   4,  38,  63,   29,
		 5,  39,  62,  28,  -1,    -1 },
	    {    6,  40,  61,  27,   7,    41,  68,  73,  60,  26,
		 8,  42,  69,  72,  59,    25,   9,  43,  70,  71,
		58,  24,  10,  44,  57,    23 },
	    {   11,  45,  56,  22,  12,    46,  55,  21,  13,  47,
		54,  20,  14,  48,  53,    19,  15,  49,  52,  18,
		16,  50,  51,  17,  -1,    -1 }
	}
    };

    /// OT numbering scheme (dummy class, only there for the comments)
    /**
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
    struct OTIndexDummy { enum { OT = 42 }; };

    /// IT numbering scheme (dummy class, only there for the comments)
    /**
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
    struct ITIndexDummy { enum { IT = 42 }; };

    /// Velo numbering scheme (dummy class, only there for the comments)
    /**
     * from p.c. from Kazu Akiba:
     * - A side sensors have even numbers, C side odd
     * - Phi sensors have numbers with bit 6 set, R with bit 6 cleared
     * - pile-up sensors have numbers with bit 7 set
     * - sensors are in order of increasing z
     * - pile-up sensors are at lower z than the ordinary ones
     */
    struct VeloIndexDummy { enum { Velo = 42 }; };
}

/// class to store occupancy per detector element
class ZooOccupancy : public TObject
{
    private:
	UShort_t m_occVelo[23 * 4]; // RA RC PHIA PHIC
	UShort_t m_occTT[2 * (66 + 74)]; // TTAx TTAu TTBv TTBx
	UShort_t m_occIT[12 * 4 * 7]; // TOP BOT A C
	UChar_t m_occOT[12 * 4 * 9]; // Q1 Q2 Q3 Q4
    public:
	// default constructor
	ZooOccupancy () { clear(); }

	/// clear ZooOccupancy for subsequent reuse
	void clear();

	/// assign from vectors with # hits / subdetector element
	/**
	 * @param velo	hits per velo sensor
	 * @param tt	hits per tt sensor
	 * @param it	hits per it sensor
	 * @param ot	hits per ot sensor
	 *
	 * indexing:
	 * - velo[station][A = 0, C = 1][R = 0, Phi = 1]
	 *
	 * - tt[station][layer][region][sensor]
	 *   regions: C-side 0, B-side (beam pipe) 1, A-side 2
	 *
	 * - it[station][layer][box][sensor]
	 *   box: C-side 0, A-side 1, Bottom 2, Top 3
	 *   sensor: same as in ZooOccupancyHelper::TTIndex, but starting at 0
	 *
	 * - ot[station][quarter][layer][module]
	 *   quarter: C bottom = 0, A bottom = 1, C top = 2, A top = 3
	 *   modules with higher numbers are closer to beam pipe
	 */
	template<class VO, class TTO, class ITO, class OTO>
	ZooOccupancy& assign(
		const VO& velo, const TTO& tt, const ITO& it, const OTO& ot)
	{
	    // fill Velo
	    for (unsigned sta = 0; sta < 23; ++sta) {
		for (unsigned type = 0; type < 4; ++type)
		    m_occVelo[4 * sta + type] = velo[sta][type & 1][type >> 1];
	    }
	    // pack Velo: delta station, delta sensor
	    for (unsigned i = 23 * 4; i-- > 4; )
		m_occVelo[i] -= m_occVelo[i - 4];
	    for (unsigned i = 23 * 4; i-- > 1; )
		m_occVelo[i] -= m_occVelo[i - 1];

	    // fill TT
	    for (unsigned lay = 0; lay < 4; ++lay) {
		unsigned layoffs = (lay / 2)?(132 + 74 * (lay - 2)):(lay * 66);
		for (unsigned reg = 0; reg < 3; ++reg) {
		    for (unsigned sen = 0; sen < ((1 != reg)?24:((lay & 2)?26:18)); ++sen) {
			m_occTT[layoffs +
			    ZooOccupancyHelper::TTIndex[lay / 2][reg][sen]] =
			    tt[lay / 2][lay % 2][reg][sen];
		    }
		}
	    }
	    // pack TT: per station, layer and region: delta "scanline"
	    for (unsigned lay = 4; lay-- > 0; ) {
		unsigned layoffs = (lay / 2)?(132 + 74 * (lay - 2)):(lay * 66);
		// middle six sensors
		for (unsigned i = (lay / 2)?74:66; i-- > ((lay / 2)?71:63); )
		    m_occTT[layoffs + i] -= m_occTT[layoffs + i - 3];
		// scanlines within a layer
		for (unsigned i = (lay / 2)?68:60; i-- > ((lay / 2)?17:15); )
		    m_occTT[layoffs + i] -= m_occTT[layoffs + i - ((lay / 2)?17:15)];
		// first scanline in a station's 2nd layer
		for (unsigned i = (lay / 2)?17:15; (lay & 1) && (i-- > 0); ) {
		    m_occTT[layoffs + i] -= m_occTT[layoffs + i - ((lay / 2)?74:66)];
		}
	    }
	    // pack TT: delta sensor
	    for (unsigned i = 2 * (66 + 74); i-- > 1; ) {
		m_occTT[i] -= m_occTT[i - 1];
	    }

	    // fill IT
	    for (unsigned lay = 0; lay < 12; ++ lay) {
		for (unsigned box = 0; box < 4; ++box) {
		    for (unsigned sen = 0; sen < 7; ++sen) {
			unsigned tmpsen = (box == 0)?(6 - sen):sen;
			m_occIT[(lay * 4 + box) * 7 + tmpsen] =
			    it[lay / 4][lay % 4][box][sen];
		    }
		}
	    }
	    // pack IT: delta (station, layer), delta sensor
	    for (unsigned i = 12 * 4 * 7; i-- > 7; )
		m_occIT[i] -= m_occIT[i - 7];
	    for (unsigned i = 12 * 4 * 7; i-- > 1; )
		m_occIT[i] -= m_occIT[i - 1];

	    // fill OT
	    for (unsigned sta = 0; sta < 3; ++sta) {
		for (unsigned qua = 0; qua < 4; ++qua) {
		    for (unsigned lay = 0; lay < 4; ++lay) {
			for (unsigned mod = 0; mod < 9; ++mod)
			    m_occOT[((sta * 4 + lay) * 4 + qua) * 9 + mod] =
				ot[sta][qua][lay][mod];
		    }
		}
	    }
	    // pack OT: delta(station, layer, quarter), delta module
	    for (unsigned i = 12 * 4 * 9; i-- > 9; )
		m_occOT[i] -= m_occOT[i - 9];
	    for (unsigned i = 12 * 4 * 9; i-- > 1; )
		m_occOT[i] -= m_occOT[i - 1];

	    return *this;
	}

	/// initialize from vectors with # hits / subdetector element
	/**
	 * @param velo	hits per velo sensor
	 * @param tt	hits per tt sensor
	 * @param it	hits per it sensor
	 * @param ot	hits per ot sensor
	 *
	 * indexing:
	 * - velo[station][A = 0, C = 1][R = 0, Phi = 1]
	 *
	 * - tt[station][layer][region][sensor]
	 *   regions: C-side 0, B-side (beam pipe) 1, A-side 2
	 *
	 * - it[station][layer][box][sensor]
	 *   box: C-side 0, A-side 1, Bottom 2, Top 3
	 *   sensor: same as in ZooOccupancyHelper::TTIndex, but starting at 0
	 *
	 * - ot[station][quarter][layer][module]
	 *   quarter: C bottom = 0, A bottom = 1, C top = 2, A top = 3
	 *   modules with higher numbers are closer to beam pipe
	 */
	template<class VO, class TTO, class ITO, class OTO>
	ZooOccupancy(const VO& velo, const TTO& tt, const ITO& it, const OTO& ot)
	{ assign(velo, tt, it, ot); }

	/// get Velo occupancy
	/**
	 * @param occVelo array to hold Velo occupancy per station and sensor
	 *
	 * occVelo[station][A = 0, C = 1][R = 0, Phi = 1]
	 */
	void getOccVelo(
		std::vector<std::vector<std::vector<int> > >& occVelo) const;

	/// get OT occupancy
	//
	/**
	 * @param occOT array to hold OT occupancy per station, quarter, layer, module
	 *
	 * - occOT[station][quarter][layer][module]
	 *   quarter: C bottom = 0, A bottom = 1, C top = 2, A top = 3
	 *   modules with higher numbers are closer to beam pipe
	 */
	void getOccOT(
		std::vector<std::vector<std::vector<std::vector<int> > > >& occOT) const;

	/// get IT occupancy
	/**
	 * @param occIT array to hold IT occupancy per station, quarter, layer, module
	 *
	 * - occIT[station][layer][box][sensor]
	 *   box: C-side 0, A-side 1, Bottom 2, Top 3
	 *   sensor: same as in ZooOccupancyHelper::TTIndex, but starting at 0
	 */
	void getOccIT(
		std::vector<std::vector<std::vector<std::vector<int> > > >& occIT) const;

	/// get TT occupancy
	/**
	 * @param occTT holds TT occupancy per station, layer, detRegion and sensor
	 *
         * - tt[station][layer][region][sensor]
         *   regions: C-side 0, B-side (beam pipe) 1, A-side 2
	 */
	void getOccTT(
		std::vector<std::vector<std::vector<std::vector<int> > > >& occTT) const;

	ClassDef(ZooOccupancy, 1)
};

#endif // ZOOOCUPANCY_H

// vim: sw=4:tw=78:ft=cpp
