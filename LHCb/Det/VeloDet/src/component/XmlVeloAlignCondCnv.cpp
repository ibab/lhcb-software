// Include files
#include "VeloDet/VeloAlignCond.h"
#include "DetDescCnv/XmlUserConditionCnv.h"
#include "GaudiKernel/CnvFactory.h"

typedef  XmlUserConditionCnv<VeloAlignCond>  XmlVeloAlignCondCnv;
DECLARE_CONVERTER_FACTORY(XmlVeloAlignCondCnv)
