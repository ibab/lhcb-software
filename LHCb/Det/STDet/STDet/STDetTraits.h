// $Id: STDetTraits.h,v 1.5 2008-03-14 18:26:39 mneedham Exp $

#ifndef _STDetTraits_H
#define _STDetTraits_H

/** @class STDetTraits STDetTraits.h "STDet/STDetTraits.h"
 *
 *  Traits info
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

class DeITDetector;
class DeITStation;
class DeITBox;
class DeITLayer;
class DeITLadder;
class DeITSector;
class DeITSensor;

class DeTTDetector;
class DeTTStation;
class DeTTLayer;
class DeTTHalfModule;
class DeTTSector;
class DeTTSensor;

class DeUTDetector;
class DeUTStation;
class DeUTLayer;
class DeUTModule;
class DeUTSector;
class DeUTSensor;

template <typename TYPE>
class STDetTraits {
 public:
  typedef TYPE parent;
  typedef TYPE child;
};

template<>
class STDetTraits<DeITStation>{
 public:
  typedef DeITDetector parent;
  typedef DeITBox child;
};

template<>
class STDetTraits<DeTTStation>{
 public:
  typedef DeTTDetector parent;
  typedef DeTTLayer child;
};

template<>
class STDetTraits<DeUTStation>{
 public:
  typedef DeUTDetector parent;
  typedef DeUTLayer child;
};

template<>
class STDetTraits<DeITBox>{
 public:
  typedef DeITStation parent;
  typedef DeITLayer child;
};

template<>
class STDetTraits<DeITLayer>{
 public:
  typedef DeITBox parent;
  typedef DeITLadder child;
};

template<>
class STDetTraits<DeTTLayer>{
 public:
  typedef DeTTStation parent;
  typedef DeTTHalfModule child;
};

template<>
class STDetTraits<DeUTLayer>{
 public:
  typedef DeUTStation parent;
  typedef DeUTModule child;
};

template<>
class STDetTraits<DeITLadder>{
 public:
  typedef DeITLayer parent;
  typedef DeITSector child;
};

template<>
class STDetTraits<DeTTHalfModule>{
 public:
  typedef DeTTLayer parent;
  typedef DeTTSector child;
};

template<>
class STDetTraits<DeUTModule>{
 public:
  typedef DeUTLayer parent;
  typedef DeUTSector child;
};

template<>
class STDetTraits<DeITSector>{
 public:
  typedef DeITLadder parent;
  typedef DeITSensor child;
};

template<>
class STDetTraits<DeTTSector>{
 public:
  typedef DeTTHalfModule parent;
  typedef DeTTSensor child;
};

template<>
class STDetTraits<DeUTSector>{
 public:
  typedef DeUTModule parent;
  typedef DeUTSensor child;
};

template<>
class STDetTraits<DeITSensor>{
 public:
  typedef DeITSector parent;
  typedef DeITSensor child;
};

template<>
class STDetTraits<DeTTSensor>{
 public:
  typedef DeTTSector parent;
  typedef DeTTSensor child;
};

template<>
class STDetTraits<DeUTSensor>{
 public:
  typedef DeUTSector parent;
  typedef DeUTSensor child;
};

#endif
