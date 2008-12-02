#ifndef OTDAQ_OTENUM_H_
#define OTDAQ_OTENUM_H_

namespace OTDAQ
{
  enum { OffsetStations=1, OffsetLayers=0, OffsetQuadrants=0, OffsetModules=1, OffsetStraws=1 } ;
  enum { NumStations=3, NumLayers=4, NumQuadrants=4, NumModules=9, NumStraws=128 } ;
  enum { NumChanPerOtis=32 } ;
} ;

#endif
