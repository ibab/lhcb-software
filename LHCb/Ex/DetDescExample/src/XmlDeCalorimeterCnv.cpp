// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/XmlDeCalorimeterCnv.cpp,v 1.2 2003-04-23 14:23:18 sponce Exp $
#ifndef XMLDECALORIMETERCNV_H 
#define XMLDECALORIMETERCNV_H 1

// Include files
#include "DeCalorimeter.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeCalorimeter>  XmlDeCalorimeterCnv;
static CnvFactory<XmlDeCalorimeterCnv> DeCalorimeterFactory;
const ICnvFactory& XmlDeCalorimeterCnvFactory = DeCalorimeterFactory;

#endif // XMLDECALORIMETERCNV_H
