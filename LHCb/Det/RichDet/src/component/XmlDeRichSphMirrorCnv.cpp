#ifndef XMLDERICHSPHMIRRORCNV_H 
#define XMLDERICHSPHMIRRORCNV_H 1

// Include files
//#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichSphMirror.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichSphMirror>  XmlDeRichSphMirrorCnv;
static CnvFactory<XmlDeRichSphMirrorCnv> s_DeRichSphMirrorFactory;
const ICnvFactory& XmlDeRichSphMirrorCnvFactory = s_DeRichSphMirrorFactory;

#endif // XMLDERICHSPHMIRRORCNV_H
