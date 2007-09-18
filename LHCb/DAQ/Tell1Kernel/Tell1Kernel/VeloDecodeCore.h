// $Id: VeloDecodeCore.h,v 1.1 2007-09-18 07:33:02 cattanem Exp $
#ifndef VELODECODECORE_H 
#define VELODECODECORE_H 1

// Include files

/** @namespace VeloDecodeCore VeloDecodeCore.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-04-11
 */
namespace VeloTELL1
{
  typedef std::vector<unsigned int*> rawVec;
  typedef std::vector<unsigned int> dataVec;
  typedef std::vector<unsigned int>::const_iterator cdatIt;
  typedef std::vector<unsigned int>::iterator datIt;
  typedef std::vector<signed int> sdataVec;
  typedef std::vector<signed int>::const_iterator scdatIt;
  typedef std::vector<signed int>::iterator sdatIt;
  typedef std::vector<unsigned int> evtInfo;
  typedef std::vector<unsigned int> errorInfo;
  typedef std::vector<evtInfo> allEvt;
  typedef std::vector<errorInfo> allError;
  //
  enum bitShifts{
    bitShift8=8,
    bitShift12=12,
    bitShift16=16,
    bitShift19=19,
    bitShift21=21,
    bitShift22=22,
    bitShift24=24,
    bitShift30=30,
    bitShift32=32
  };
  //
  enum bitMasks{
    bitMask1=0x00000001,
    bitMask2=0x00000003,
    bitMask3=0x00000007,
    bitMask4=0x0000000f,
    bitMask8=0x000000ff,
    bitMask12=0x00000fff,
    bitMask16=0x0000ffff,
    bitMask24=0x00ffffff
  };
  //
  enum errorBankMarkers{
    EBMarker0=0x00000000,
    EBMarker1=0x00000001,
    EBMarker2=0x00000002,
    EBMarker3=0x00000003,
    EBMarker4=0x00000004,
    EBMarker=0x0000008e
  };
  //

}
#endif // VELODECODECORE_H
