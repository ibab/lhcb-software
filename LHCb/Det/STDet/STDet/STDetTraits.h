// $Id: STDetTraits.h,v 1.4 2007-03-01 11:08:01 cattanem Exp $

#ifndef _STDetTraits_H
#define _STDetTraits_H

/** @class STDetTraits STDetTraits.h "STDet/STDetTraits.h"
 *
 *  Traits info
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

class DeTTDetector;
class DeITDetector;
class DeITStation;
class DeTTStation;
class DeITBox;
class DeITLayer;
class DeTTLayer;
class DeTTHalfModule;
class DeTTSector;
class DeITSector;
class DeITLadder;

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
class STDetTraits<DeITBox>{
 public:
  typedef DeITStation parent;
  typedef DeITLayer child;
};

template<>
class STDetTraits<DeTTLayer>{
 public:
  typedef DeTTStation parent;
  typedef DeTTHalfModule child;
};

template<>
class STDetTraits<DeITLayer>{
 public:
  typedef DeITBox parent;
  typedef DeITLadder child;
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
class STDetTraits<DeTTSector>{
 public:
  typedef DeTTHalfModule parent;
  typedef DeTTSector child;
};


template<>
class STDetTraits<DeITSector>{
 public:
  typedef DeITLadder parent;
  typedef DeITSector child;
};


#endif
