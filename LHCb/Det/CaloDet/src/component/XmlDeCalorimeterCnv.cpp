// $Log: not supported by cvs2svn $

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDesc/XmlUserDetElemCnv.h"
/// CaloDet 
#include "CaloDet/DeCalorimeter.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeCalorimeter>   XmlDeCalorimeterCnv;
static 
CnvFactory<XmlDeCalorimeterCnv>          s_XmlDeCalorimeterCnvFactory ;
const ICnvFactory& 
XmlDeCalorimeterCnvFactory             = s_XmlDeCalorimeterCnvFactory ;
