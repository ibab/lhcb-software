#include <ios>
#include <iomanip>

#include "ZooLHCbID.h"

ZooOTChannelID::ZooOTChannelID(unsigned station, unsigned layer, unsigned quarter,
	unsigned module, unsigned straw, unsigned tdc)
{
    BOOST_STATIC_ASSERT(sizeof(ZooOTChannelID) == sizeof(ZooLHCbID));
    m_id.otid.set<_OTChannelID::tdc>(tdc);
    m_id.otid.set<_OTChannelID::straw>(straw);
    m_id.otid.set<_OTChannelID::module>(module);
    m_id.otid.set<_OTChannelID::quarter>(quarter);
    m_id.otid.set<_OTChannelID::layer>(layer);
    m_id.otid.set<_OTChannelID::station>(station);
    m_id.otid.set<_OTChannelID::channeltype>(OT);
}

ZooOTChannelID::ZooOTChannelID(const ZooLHCbID& other) :
    ZooLHCbID(other)
{
    BOOST_STATIC_ASSERT(sizeof(ZooOTChannelID) == sizeof(ZooLHCbID));
    if (OT != channeltype())
	throw bad_cast("Constructor argument is no ZooOTChannelID");
}

ZooOTChannelID& ZooOTChannelID::operator=(const ZooLHCbID& other)
{
    BOOST_STATIC_ASSERT(sizeof(ZooOTChannelID) == sizeof(ZooLHCbID));
    if (OT != other.channeltype())
	throw bad_cast("Assignment must have ZooOTChannelID on right hand side");
    m_id.lhcbid = unsigned(other);
    return *this;
}

ZooSTChannelID::ZooSTChannelID(unsigned type, unsigned station, unsigned layer,
	unsigned region, unsigned sector, unsigned strip)
{
    BOOST_STATIC_ASSERT(sizeof(ZooSTChannelID) == sizeof(ZooLHCbID));
    m_id.stid.set<_STChannelID::channeltype>(type?IT:TT);
    m_id.stid.set<_STChannelID::type>(type);
    m_id.stid.set<_STChannelID::station>(station);
    m_id.stid.set<_STChannelID::layer>(layer);
    m_id.stid.set<_STChannelID::region>(region);
    m_id.stid.set<_STChannelID::sector>(sector);
    m_id.stid.set<_STChannelID::strip>(strip);
}

ZooSTChannelID::ZooSTChannelID(const ZooLHCbID other) :
    ZooLHCbID(other)
{
    BOOST_STATIC_ASSERT(sizeof(ZooSTChannelID) == sizeof(ZooLHCbID));
    if (TT != channeltype() && IT != channeltype())
	throw bad_cast("Constructor argument is no ZooSTChannelID");
}

ZooSTChannelID& ZooSTChannelID::operator=(const ZooLHCbID other)
{
    BOOST_STATIC_ASSERT(sizeof(ZooSTChannelID) == sizeof(ZooLHCbID));
    if (TT != channeltype() && IT != channeltype())
	throw bad_cast("Assignment must have ZooSTChannelID on right hand side");
    m_id.lhcbid = unsigned(other);
    return *this;
}

bool ZooSTChannelID::isTop() const
{
    if (isIT()) return 4 == region();
    // is TT
    if (1 == (region() & 1)) {
	// region 1 or 3
	return (sector() & 3) > 2;
    }
    // region 2
    if (1 == station()) return ((sector() - 1) % 6) > 2;
    // region 2 in TTB
    if (sector() < 12 || sector() > 22)
	return std::min(sector(), sector() - 22) > 2;
    // middle of region 2 in TTB
    return ((sector() - 5) % 6) > 2;
}

bool ZooSTChannelID::isASide() const
{
    if (isIT()) return 1 == region();
    // is TT
    if (1 == (region() & 1)) {
	// region 1 or 3
	return 3 == region();
    }
    if (1 == station())
	return sector() > 9;
    if (sector() > 10 && sector() < 14) return true;
    return sector() > 16;
}

ZooVeloChannelID::ZooVeloChannelID(unsigned sensor, unsigned strip)
{
    BOOST_STATIC_ASSERT(sizeof(ZooVeloChannelID) == sizeof(ZooLHCbID));
    m_id.veloid.set<_VeloChannelID::channeltype>(Velo);
    m_id.veloid.set<_VeloChannelID::sensor>(sensor);
    m_id.veloid.set<_VeloChannelID::strip>(strip);
    if (sensor & 0x80) {
	m_id.veloid.set<_VeloChannelID::type>(PileUpType);
    } else if (sensor & 0x40) {
	m_id.veloid.set<_VeloChannelID::type>(PhiType);
    } else {
	m_id.veloid.set<_VeloChannelID::type>(RType);
    }
}

ZooVeloChannelID::ZooVeloChannelID(const ZooLHCbID other) :
    ZooLHCbID(other)
{
    BOOST_STATIC_ASSERT(sizeof(ZooVeloChannelID) == sizeof(ZooLHCbID));
    if (Velo != channeltype())
	throw bad_cast("Constructor argument is no ZooVeloChannelID");
}

ZooVeloChannelID& ZooVeloChannelID::operator=(const ZooLHCbID other)
{
    BOOST_STATIC_ASSERT(sizeof(ZooVeloChannelID) == sizeof(ZooLHCbID));
    if (Velo != other.channeltype())
	throw bad_cast("Assignment must have ZooVeloChannelID on right hand side");
    m_id.lhcbid = unsigned(other);
    return *this;
	}

ZooLHCbIDBlock::operator std::vector<ZooLHCbID>() const
{
    std::vector<ZooLHCbID> ids;
    ids.reserve(m_ids.size());
    UInt_t last = 0;
    for (unsigned i = 0; i < m_ids.size(); ++i)
	ids.push_back(ZooLHCbID(last += m_ids[i]));
    return ids;
}

ClassImp(ZooLHCbIDBlock);

std::ostream& operator<<(std::ostream& os, const ZooLHCbID& id)
{
    // save format flags
    std::ios::fmtflags fmtflags = os.flags();
    // output ZooLHCbID
    os.setf(std::ios::right, std::ios::dec);
    switch (id.channeltype()) {
	case ZooLHCbID::Velo:
	    {
		ZooVeloChannelID vid(id);
		os << "ZooVeloChannelID sensor type " <<
		    std::setw(1) << vid.type() << " sensor " <<
		    std::setw(4) << vid.sensor() << "(" <<
		    (vid.isASide()?'A':'C') << " side, " <<
		    (vid.isRType()?"R ":"Phi") << " type";
		if (vid.isPileUp()) os << ", Pileup";
		os << "), strip " << std::setw(3) << vid.strip();
	    }
	    break;
	case ZooLHCbID::TT:
	    // fall-through intended
	case ZooLHCbID::IT:
	    {
		ZooSTChannelID stid(id);
		os << "ZooSTChannelID type " << (stid.isTT()?"TT":"IT") <<
		    " station " << std::setw(1) << stid.station() <<
		    " layer " << std::setw(1) << stid.layer() <<
		    " region " << std::setw(2) << stid.region() <<
		    " sector " << std::setw(1) << stid.sector() <<
		    " strip " << std::setw(3) << stid.strip();
	    }
	    break;
	case ZooLHCbID::OT:
	    {
		ZooOTChannelID otid(id);
		os << "ZooOTChannelID " << "station " <<
		    std::setw(1) << otid.station() << " layer " <<
		    std::setw(1) << otid.layer() << " quarter " <<
		    std::setw(1) << otid.quarter() << " module " <<
		    std::setw(1) << otid.module() << " straw " <<
		    std::setw(3) << otid.straw() << " TDC " <<
		    std::setw(3) << otid.tdc();
	    }
	    break;
	default:
	    // unknown type of LHCbID, output what we know about it
	    // (and have the user make sense of the hex dump ;)
	    os.setf(std::ios_base::hex); os << std::setfill('0');
	    os << "ZooLHCbID type 0x" << std::setw(1) <<
		id.channeltype() << std::setfill(' ') << " (";
	    switch (id.channeltype()) {
		case ZooLHCbID::Velo:    os << "Velo   "; break;
		case ZooLHCbID::TT:      os << "TT     "; break;
		case ZooLHCbID::IT:      os << "IT     "; break;
		case ZooLHCbID::OT:      os << "OT     "; break;
		case ZooLHCbID::Rich:    os << "Rich   "; break;
		case ZooLHCbID::Calo:    os << "Calo   "; break;
		case ZooLHCbID::Muon:    os << "Muon   "; break;
		case ZooLHCbID::VeloPix: os << "VeloPix"; break;
		default:                 os << "Unknown"; break;
	    }
	    os << ") opaque channel id 0x" << std::setw(7) <<
		std::setfill('0') << id.channel() << std::setfill(' ');
	    break;
    }
    // restore flags
    os.setf(fmtflags);
    return os;
}

// vim: tw=78:sw=4:ft=cpp
