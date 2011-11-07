/** @file ZooOccupancy.cpp
 *
 * @author Manuel Schiller
 * @date 2010-01-22
 */

#include <vector>
#include <algorithm>

#include "ZooOccupancy.h"

void ZooOccupancy::clear()
{
    std::fill(m_occVelo, m_occVelo + 23 * 4, 0);
    std::fill(m_occTT, m_occTT + 2 * (66 + 74), 0);
    std::fill(m_occIT, m_occIT + 12 * 4 * 7, 0);
    std::fill(m_occOT, m_occOT + 12 * 4 * 9, 0);
}

void ZooOccupancy::getOccVelo(
	std::vector<std::vector<std::vector<int> > >& occVelo) const
{
    occVelo.clear();
    occVelo.resize(23,
	    std::vector<std::vector<int> >(2, std::vector<int>(2, 0)));

    UShort_t last = 0;
    for (unsigned sta = 0; sta < 23; ++sta) {
	for (unsigned type = 0; type < 4; ++type) {
	    last += m_occVelo[4 * sta + type];
	    occVelo[sta][type & 1][type / 2] = last;
	}
    }
    // unpack
    for (unsigned sta = 1; sta < 23; ++sta) {
	for (unsigned type = 0; type < 4; ++type) {
	    UShort_t tmp = occVelo[sta][type & 1][type / 2] +
		occVelo[sta - 1][type & 1][type / 2];
	    occVelo[sta][type & 1][type / 2] = tmp;
	}
    }
}

void ZooOccupancy::getOccOT(
	std::vector<std::vector<std::vector<std::vector<int> > > >& occOT) const
{
    occOT.clear();
    occOT.resize(3, std::vector<std::vector<std::vector<int> > >(4,
		std::vector<std::vector<int> >(4,
		    std::vector<int>(9, 0))));
    // unpack
    UChar_t last = 0;
    for (unsigned sta = 0; sta < 3; ++sta) {
	for (unsigned lay = 0; lay < 4; ++lay) {
	    for (unsigned qua = 0; qua < 4; ++qua) {
		for (unsigned mod = 0; mod < 9; ++mod) {
		    last += m_occOT[((sta * 4 + lay) * 4 + qua) * 9 + mod];
		    occOT[sta][qua][lay][mod] = last;
		}
	    }
	}
    }
    for (unsigned sta = 0; sta < 3; ++sta) {
	for (unsigned lay = 0; lay < 4; ++lay) {
	    for (unsigned qua = 0; qua < 4; ++qua) {
		if (0 == sta && 0 == qua && 0 == lay) continue;
		for (unsigned mod = 0; mod < 9; ++mod) {
		    unsigned idx = (sta * 4 + lay) * 4 + qua - 1;
		    unsigned oqua = idx & 3; idx >>= 2;
		    unsigned olay = idx & 3; idx >>= 2;
		    UChar_t tmp = occOT[sta][qua][lay][mod] +
			occOT[idx][oqua][olay][mod];
		    occOT[sta][qua][lay][mod] = tmp;
		}
	    }
	}
    }
}

void ZooOccupancy::getOccIT(
	std::vector<std::vector<std::vector<std::vector<int> > > >& occIT) const
{
    occIT.clear();
    occIT.resize(3, std::vector<std::vector<std::vector<int> > >(4,
		std::vector<std::vector<int> >(4,
		    std::vector<int>(7, 0))));

    UShort_t last = 0;
    for (unsigned lay = 0; lay < 12; ++ lay) {
	for (unsigned box = 0; box < 4; ++box) {
	    for (unsigned sen = 0; sen < 7; ++sen) {
		unsigned tmpsen = (box == 0)?(6 - sen):sen;
		last += m_occIT[(lay * 4 + box) * 7 + sen];
		occIT[lay / 4][lay % 4][box][tmpsen] = last;
	    }
	}
    }
    for (unsigned lay = 0; lay < 12; ++ lay) {
	for (unsigned box = 0; box < 4; ++box) {
	    if (0 == lay && 0 == box) continue;
	    for (unsigned sen = 0; sen < 7; ++sen) {
		unsigned prevlay = lay * 4 + box - 1;
		unsigned prevbox = prevlay % 4;
		prevlay /= 4;
		unsigned tmpsen = (box == 0)?(6 - sen):sen;
		unsigned prevsen = (prevbox == 0)?(6 - sen):sen;
		UShort_t tmp = occIT[lay / 4][lay % 4][box][tmpsen] +
		    occIT[prevlay / 4][prevlay % 4][prevbox][prevsen];
		occIT[lay / 4][lay % 4][box][tmpsen] = tmp;
	    }
	}
    }
}

void ZooOccupancy::getOccTT(
	std::vector<std::vector<std::vector<std::vector<int> > > >& occTT) const
{
    occTT.clear();
    occTT.resize(2, std::vector<std::vector<std::vector<int> > >(2,
		std::vector<std::vector<int> >(3,
		    std::vector<int>(26, 0))));
    // TT is fiddly, so use a temp to simplify life a bit
    UShort_t temp[2 * (66 + 74)];
    UShort_t last = 0;
    // unpack: delta sensor
    for (unsigned i = 0; i < 2 * (66 + 74); ++i) {
	last += m_occTT[i];
	temp[i] = last;
    }
    // unpack: delta "scanline"
    for (unsigned lay = 0; lay < 4; ++lay) {
	unsigned layoffs = (lay / 2)?(132 + 74 * (lay - 2)):(lay * 66);
	// first scanline in a station's 2nd layer
	for (unsigned i = (lay / 2)?17:15; (lay & 1) && (i-- > 0); ) {
	    temp[layoffs + i] += temp[layoffs + i - ((lay / 2)?74:66)];
	}
	// scanlines within a layer
	for (unsigned i = ((lay / 2)?17:15); i < ((lay / 2)?68:60); ++i)
	    temp[layoffs + i] += temp[layoffs + i - ((lay / 2)?17:15)];
	// middle six sensors
	for (unsigned i = (lay / 2)?74:66; i-- > ((lay / 2)?71:63); )
	    temp[layoffs + i] += temp[layoffs + i - 3];
    }
    // reshuffle to put unpacked stuff in temp into the right spots in
    // occTT
    for (unsigned lay = 0; lay < 4; ++lay) {
	unsigned layoffs = (lay / 2)?(132 + 74 * (lay - 2)):(lay * 66);
	for (unsigned reg = 0; reg < 3; ++reg) {
	    for (unsigned sen = 0; sen < ((1 != reg)?24:((lay & 2)?26:18)); ++sen) {
		occTT[lay / 2][lay % 2][reg][sen] =
		    temp[layoffs + ZooOccupancyHelper::TTIndex[lay / 2][reg][sen]];
	    }
	}
    }
}

ClassImp(ZooOccupancy);

// vim: sw=4:tw=78:ft=cpp
