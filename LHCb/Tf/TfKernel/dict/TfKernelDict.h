// $Id: TfKernelDict.h,v 1.1 2007-08-21 22:44:51 jonrob Exp $
#ifndef DICT_TFKERNELDICT_H 
#define DICT_TFKERNELDICT_H 1

#include "TfKernel/TfIDTypes.h"

#include "TfKernel/OTLiteTime.h"

#include "TfKernel/OTHit.h"
#include "TfKernel/STHit.h"
#include "TfKernel/VeloRHit.h"
#include "TfKernel/VeloPhiHit.h"

#include "TfKernel/Region.h"
#include "TfKernel/RegionID.h"

#include "TfKernel/ISTHitCreator.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/ISTHitCleaner.h"
#include "TfKernel/IOTHitCleaner.h"

#include "TfKernel/IOTRawBankDecoder.h"

// instantiate some templated classes, to get them into the dictionary
namespace 
{
  struct _Instantiations 
  {
    Tf::Envelope<Tf::STHit::DetectorElementType> tf_envelope_sthit;
    Tf::Envelope<Tf::OTHit::DetectorElementType> tf_envelope_othit;
  };
}

#endif // DICT_TFKERNELDICT_H
