// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "TrajProjector.h"

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
template <typename T>
TrajProjector<T>::TrajProjector( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : TrackProjector( type, name, parent )
{
  declareInterface<ITrackProjector>(this);
  m_tolerance = T::defaultTolerance() ;
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
template <typename T>
TrajProjector<T>::~TrajProjector() {}

/// declare and instantiate ST and Velo projectors...
struct ST {
   static double defaultTolerance() { return 0.002*Gaudi::Units::mm; }
};

typedef TrajProjector<ST> TrajSTProjector;
DECLARE_TOOL_FACTORY( TrajSTProjector )

struct Velo {
  static double defaultTolerance() { return 0.0005*Gaudi::Units::mm; }
};

typedef TrajProjector<Velo> TrajVeloProjector;
DECLARE_TOOL_FACTORY( TrajVeloProjector )

struct VP {
  static double defaultTolerance() { return 0.0005*Gaudi::Units::mm; }
};

typedef TrajProjector<VP> TrajVPProjector;
DECLARE_TOOL_FACTORY( TrajVPProjector )

struct FT {
  static double defaultTolerance() { return 0.002*Gaudi::Units::mm; }
};

typedef TrajProjector<FT> TrajFTProjector;
DECLARE_TOOL_FACTORY( TrajFTProjector )

struct Muon {
  static double defaultTolerance() { return 0.002*Gaudi::Units::mm; }
};

typedef TrajProjector<Muon> TrajMuonProjector;
DECLARE_TOOL_FACTORY( TrajMuonProjector )
