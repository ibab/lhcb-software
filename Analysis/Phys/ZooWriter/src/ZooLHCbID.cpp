#include <ios>
#include <iomanip>

#include "ZooLHCbID.h"

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
