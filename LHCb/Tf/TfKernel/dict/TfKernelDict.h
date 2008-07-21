// $Id: TfKernelDict.h,v 1.4 2008-07-21 13:06:04 gguerrer Exp $
#ifndef DICT_TFKERNELDICT_H 
#define DICT_TFKERNELDICT_H 1

#include "TfKernel/TfIDTypes.h"

#include "TfKernel/OTHit.h"
#include "TfKernel/STHit.h"
#include "TfKernel/VeloRHit.h"
#include "TfKernel/VeloPhiHit.h"

#include "TfKernel/Region.h"
#include "TfKernel/RegionID.h"

#include "TfKernel/ITTHitCreator.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/ISTHitCleaner.h"
#include "TfKernel/IOTHitCleaner.h"

#include "TfKernel/TStationHitManager.h"
#include "TfKernel/RegionSelectors.h"

// instantiate some templated classes, to get them into the dictionary
namespace 
{
  struct _Instantiations 
  {
    Tf::Envelope<Tf::STHit::DetectorElementType> tf_envelope_sthit;
    Tf::Envelope<Tf::OTHit::DetectorElementType> tf_envelope_othit;
	
	std::vector<Tf::IStationSelector*> tf_station_selectors;
  };
}

#endif // DICT_TFKERNELDICT_H
