// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiTools.h,v 1.2 2001-11-28 15:56:21 mato Exp $
#ifndef DADITOOLS_H 
#define DADITOOLS_H 1

// Include files
#include <iostream>
#include <string>

#include <sax/SAXParseException.hpp>
#include <sax/ErrorHandler.hpp>
#include <dom/DOMString.hpp>
#include <util/XercesDefs.hpp>


/** @class DaDiTools DaDiTools.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   13/06/2001
 */


class DaDiTools : public ErrorHandler
{

public:
    DaDiTools() {}
    ~DaDiTools() {} 

    void warning(const SAXParseException& toCatch);
    void error(const SAXParseException& toCatch);
    void fatalError(const SAXParseException& toCatch);
    void resetErrors();

    
    static char* chooseAccess(const DOMString& s);

};





#endif // DADITOOLS_H
