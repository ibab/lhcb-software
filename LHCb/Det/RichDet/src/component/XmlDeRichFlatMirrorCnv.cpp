#ifndef XMLDERICHFLATMIRRORCNV_CPP
#define XMLDERICHFLATMIRRORCNV_CPP 1

// Include files
//#include "RichDet/DeRichFlatMirror.h"
#include "RichDet/DeRichFlatMirror.h"
#include "DetDesc/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichFlatMirror>  XmlDeRichFlatMirrorCnv;
static CnvFactory<XmlDeRichFlatMirrorCnv> s_DeRichFlatMirrorFactory;
const ICnvFactory& XmlDeRichFlatMirrorCnvFactory = s_DeRichFlatMirrorFactory;

#endif // XMLDERICHFLATMIRRORCNV_CPP
