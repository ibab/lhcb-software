///	$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlAddress.h,v 1.3 2001-01-25 15:36:44 ibelyaev Exp $
#ifndef DETDESC_XMLCNVSVC_XMLADDRESS_H
#define DETDESC_XMLCNVSVC_XMLADDRESS_H 1

/// Include files
#include <iostream>
#include <string>

#include "GaudiKernel/GenericAddress.h"


// ClassName:    XmlAddress
// 
// Description:  XML IOpaqueAddress for XML based data persistency
//
// Author:       Radovan Chytracek
//
class XmlAddress : public GenericAddress   {
    
public:
    
    /// Standard Constructor
    XmlAddress(
                const CLID& clid
               ,const std::string& fileName
               ,int   seqInt
               ,const std::string& containerName
              ); 

    /// Standard Constructor
    XmlAddress(
                const CLID& clid
               ,const std::string& fileName
               ,const std::string& containerName
              ); 

    /// Standard Constructor
    /// GenericLink argument is composed of XML_StorageType + classID
    XmlAddress(
                const GenericLink& link
               ,const std::string& fileName
               ,const std::string& containerName
               ,const std::string& objName
              );
    
    /// Standard Constructor
    XmlAddress( const CLID& clid );

    /// Standard Constructor for non-initialized addresses
    XmlAddress( const GenericLink& link );
    
    /// Standard Constructor for non-initialized addresses
    XmlAddress( const GenericLink* link );
    
    /// Standard destructor
    ~XmlAddress();

};

#endif // DETDESC_XMLCNVSVC_XMLADDRESS_H
