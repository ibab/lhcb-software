// $Log: not supported by cvs2svn $

// GaudiKernel
#include  "GaudiKernel/CnvFactory.h"
// DetDesc 
#include  "DetDesc/XmlUserDetElemCnv.h"
// CaloDet 
#include  "CaloDet/DeSubCalorimeter.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
typedef  XmlUserDetElemCnv<DeSubCalorimeter>  XmlDeSubCalorimeterCnv;
static  CnvFactory<XmlDeSubCalorimeterCnv>  s_XmlDeSubCalorimeterCnvFactory ;
const ICnvFactory&  
XmlDeSubCalorimeterCnvFactory             = s_XmlDeSubCalorimeterCnvFactory ;














