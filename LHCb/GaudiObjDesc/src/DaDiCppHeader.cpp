// $Id: DaDiCppHeader.cpp,v 1.71 2003-12-17 17:31:18 mato Exp $

//#include "GaudiKernel/Kernel.h"

#include "DaDiTools.h"
#include "DaDiCppHeader.h"
#include "DaDiPackage.h"

#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOMException.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/util/XMLString.hpp"

#include <vector>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <list>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>

//-----------------------------------------------------------------------------
// Implementation file for class : DaDiCppHeader
//
// 12/06/2001 :
//----------------------------------------------------------------------------

using namespace xercesc;

std::vector<std::string> ContainedObjectClasses, KeyedObjectClasses;
std::vector<std::string> gddDefEnums, gddDefTypeDefs;

//extern std::string xmlVersion;

//-----------------------------------------------------------------------------
std::string checkClassPrefix(const std::string& type,
                             const std::string& prefix)
//-----------------------------------------------------------------------------
{
  bool prefixFound = false;
  std::vector<std::string>::iterator iter;
  iter = std::find(gddDefEnums.begin(), gddDefEnums.end(), type);
  if (iter != gddDefEnums.end())
  {
    prefixFound = true;
  }
  iter = std::find(gddDefTypeDefs.begin(), gddDefTypeDefs.end(), type);
  if (iter != gddDefTypeDefs.end())
  {
    prefixFound = true;
  }
  if (prefixFound)
  {
    return prefix + "::" + type;
  }
  else
  {
    return type;
  }
}


//-----------------------------------------------------------------------------
std::string printPlural(const std::string& singular)
//-----------------------------------------------------------------------------
{
  std::map<std::string,std::string> exceptions;
  std::map<std::string,std::string>::iterator iter;
  exceptions["Vertex"] = "Vertices";
  int pos;

  for (iter = exceptions.begin(); iter != exceptions.end(); ++iter)
  {
    pos = singular.rfind((*iter).first);
    if ( pos != -1 && (pos + ((*iter).first).length())
         == singular.length())
    {
      return singular.substr(0,singular.rfind((*iter).first))
        + exceptions[(*iter).first];
    }
  }
  return singular + "s";
}


//-----------------------------------------------------------------------------
void usage(std::string argV0)
//-----------------------------------------------------------------------------
{

  std::cout << std::endl << std::endl
            << "Usage: " << argV0 << " [-h] [-v] [-i] [-o [path]] [-x [path]] x"
            << "ml-file(s)" << std::endl
            << "Produce .h-files out of xml-files"
            << std::endl << std::endl
            << "   -h            display this help and exit"
            << std::endl
            << "   -v            display version information and exit"
            << std::endl
            << "   -i            add additional file-package-information from '"
            << "./AddImports.txt'" << std::endl
            << "   -o [path]     define possible outputdestination with followi"
            << "ng precedence" << std::endl
            << "                    -o path     use 'path'"
            << std::endl
            << "                    -o          use environment-variable 'GODDO"
            << "THOUT'" << std::endl
            << "                     default    use local directory"
            << std::endl
            << "   -x [path]     define location of 'GaudiCppExport.xml' which "
            << "holds information" << std::endl
            << "                 about include-file<->package dependencies, wit"
            << "h this precedence" << std::endl
            << "                    -x path     use 'path'"
            << std::endl
            << "                    -x          use environment-variable 'GODXM"
            << "LDB'" << std::endl
            << "                     default    use '$(GAUDIOBJDESCROOT)/xml_fi"
            << "les'" << std::endl
            << "   xml-file(s)   xml-file(s) to be parsed (must have extension "
            << "'.xml')" << std::endl
            << std::endl << std::endl;
  exit(0);
}



//-----------------------------------------------------------------------------
void version(std::string argV0)
//-----------------------------------------------------------------------------
{
  std::cout << std::endl << std::endl
            << argV0 << " version 1.0" << std::endl
            << "written by Stefan Roiser" << std::endl << std::endl;
  exit(0);
}




std::string argV0;



//-----------------------------------------------------------------------------
template <class T> void printArguments(std::ofstream& xmlOut,
                                       T* gdd,
                                       std::string elemName = "")
//-----------------------------------------------------------------------------
{
  int j;
  for(j=0; j<gdd->sizeDaDiMethArgument(); j++)
  {
    DaDiMethArgument* gddArg = gdd->popDaDiMethArgument();
    char *gddArgType = XMLString::transcode(gddArg->type()),
      *gddArgInout = XMLString::transcode(gddArg->inout()),
      *gddArgName = XMLString::transcode(gddArg->name());

    bool gddArgIsConst = gddArg->const_(),
      gddArgIsPointer = gddArg->isPointer();


    if (j>0)
    {
      xmlOut << ", ";
    }
    if (!DaDiTools::isFundamental(gddArgType) &&
        gddArgIsConst && !gddArgIsPointer)
    {
      xmlOut << "const ";
    }
    else if (gddArgIsConst)
    {
      xmlOut << "const ";
    }
    if (!DaDiTools::isEmpty(elemName))
    {
      xmlOut << checkClassPrefix(gddArgType, elemName);
    }
    else
    {
      xmlOut << gddArgType;
    }
    if (gddArgIsPointer)
    {
      xmlOut << "*";
    }

    if ((strcmp(gddArgInout,"BYVALUE") != 0) &&
        (strcmp(gddArgInout,"BOTH") == 0 ||
         (!DaDiTools::isFundamental(gddArgType) && !gddArgIsPointer)))
    {
      xmlOut << "&";
    }

    if (!DaDiTools::isEmpty(gddArgName))
    {
      xmlOut << " " << gddArgName;
    }
    else
    {
      xmlOut << " value" << j;
    }
    XMLString::release(&gddArgType);
    XMLString::release(&gddArgInout);
    XMLString::release(&gddArgName);
  }

  /*       if (gddMethArgument->const_() ||
           (!DaDiTools::isSimple(gddMethArgument->type().transcode()) &&
           gddMethArgument->const_()) ) */
}


//-----------------------------------------------------------------------------
template <class T> void printMethodDecl(std::ofstream& xmlOut,
                                        T* gddElement,
                                        const std::string& accessor)
//-----------------------------------------------------------------------------
{
  int i;
  for(i=0; i < gddElement->sizeDaDiMethod(); i++)
  {
    DaDiMethod* gddMethod = gddElement->popDaDiMethod();
    char *gddMethodAccess = XMLString::transcode(gddMethod->access()),
      *gddMethodTemplate = XMLString::transcode(gddMethod->template_()),
      *gddMethodDesc = XMLString::transcode(gddMethod->desc()),
      *gddMethodVirtual = XMLString::transcode(gddMethod->virtual_()),
      *gddMethodName = XMLString::transcode(gddMethod->name()),
      *gddMethodCode = XMLString::transcode(gddMethod->code()),
      *gddMethReturnType = XMLString::transcode(gddMethod->daDiMethReturn()->
                                                type());
    bool gddMethodIsStatic = gddMethod->static_(),
      gddMethodIsFriend = gddMethod->friend_(),
      gddMethodIsConst = gddMethod->const_(),
      gddMethReturnIsConst = gddMethod->daDiMethReturn()->const_();

    if ((gddMethodAccess == accessor) ||
        (accessor == ""))
    {
      xmlOut << "  /// " << gddMethodDesc << std::endl << "  ";
      if (!DaDiTools::isEmpty(gddMethodTemplate))
      {
        xmlOut << "template<" << gddMethodTemplate << "> ";
      }
      if ((accessor == "") && (strcmp(gddMethodCode,"") != 0))
      {
        xmlOut << "inline ";
      }
      if (gddMethodIsFriend)
      {
        xmlOut << "friend ";
      }
      if (strcmp(gddMethodVirtual,"TRUE") == 0 ||
          strcmp(gddMethodVirtual,"PURE") == 0)
      {
        xmlOut << "virtual ";
      }
      if (gddMethodIsStatic)
      {
        xmlOut << "static ";
      }
      if (gddMethReturnIsConst)
      {
        xmlOut << "const ";
      }
      xmlOut << gddMethReturnType << " " << gddMethodName << "(";

      char *elemName = XMLString::transcode(gddElement->name());
      printArguments(xmlOut, gddMethod, elemName);
      XMLString::release(&elemName);

      xmlOut << ")";

      if (gddMethodIsConst)
      {
        xmlOut << " const";
      }
      if (strcmp(gddMethodVirtual,"PURE") == 0)
      {
        xmlOut << " = 0";
      }
      else if ((strcmp(gddMethodCode,"") != 0) &&
               !DaDiTools::isEmpty(gddMethodTemplate))
      {
        xmlOut << std::endl
               << "  {" << std::endl
               << "     " << gddMethodCode << std::endl
               << "  }";
      }
      /*      if (gddMethodIsFriend)
              {
              xmlOut << std::endl
              << "  {" << std::endl
              << gddMethodCode << std::endl
              << "  }" << std::endl
              << std::endl;
              } */
      xmlOut << ";" << std::endl
             << std::endl;
    }
    XMLString::release(&gddMethodAccess);
    XMLString::release(&gddMethodTemplate);
    XMLString::release(&gddMethodDesc);
    XMLString::release(&gddMethodVirtual);
    XMLString::release(&gddMethodName);
    XMLString::release(&gddMethodCode);
    XMLString::release(&gddMethReturnType);
  }
}



//-----------------------------------------------------------------------------
template <class T> void printMethodImpl(std::ofstream& xmlOut,
                                        T* gddElement,
                                        const std::string& accessor)
//-----------------------------------------------------------------------------
{
  int i;
  for(i=0; i<gddElement->sizeDaDiMethod(); i++)
  {
    DaDiMethod* gddMethod = gddElement->popDaDiMethod();
    char *gddMethodName = XMLString::transcode(gddMethod->name()),
      *gddMethodAccess = XMLString::transcode(gddMethod->access()),
      //*gddMethodArgList = XMLString::transcode(gddMethod->argList()),
      *gddMethodCode = XMLString::transcode(gddMethod->code()),
      *gddMethodVirtual = XMLString::transcode(gddMethod->virtual_()),
      *gddMethReturnType = XMLString::transcode(gddMethod->daDiMethReturn()->
                                                type()),
      *gddClassName = XMLString::transcode(gddElement->name());

    bool gddMethodIsConst = gddMethod->const_(),
      gddMethodIsStatic = gddMethod->static_(),
      gddMethodIsFriend = gddMethod->friend_(),
      gddMethReturnIsConst = gddMethod->daDiMethReturn()->const_(),
      gddMethodIsPureVirtual = (strcmp
                                (gddMethodVirtual,"PURE") == 0) ? true : false,
      gddMethodIsTemplated = (XMLString::stringLen
                              (gddMethod->template_()) == 0) ? false : true;


    if (((gddMethodAccess == accessor) || (accessor == "")) &&
        strcmp(gddMethodCode,"") != 0 && !gddMethodIsPureVirtual
        && !gddMethodIsTemplated) // && !gddMethodIsFriend)
    {
      xmlOut << "inline ";
      if (gddMethodIsStatic)
      {
        xmlOut << "static ";
      }
      if (gddMethReturnIsConst)
      {
        xmlOut << "const ";
      }
      xmlOut << checkClassPrefix(gddMethReturnType, gddClassName) << " ";
      if (!gddMethodIsFriend)
      {
        xmlOut << gddClassName << "::";
      }
      xmlOut << gddMethodName << "(";

      printArguments(xmlOut, gddMethod);

      xmlOut << ")";
      if (gddMethodIsConst)
      {
        xmlOut << " const";
      }
      xmlOut << std::endl
             << "{" << std::endl
             << "   " << gddMethodCode << std::endl
             << "}" << std::endl
             << std::endl;
    }
    XMLString::release(&gddMethodName);
    XMLString::release(&gddMethodAccess);
    //XMLString::release(&gddMethodArgList);
    XMLString::release(&gddMethodCode);
    XMLString::release(&gddMethodVirtual);
    XMLString::release(&gddMethReturnType);
    XMLString::release(&gddClassName);
  }
}

//-----------------------------------------------------------------------------
void printSetGetAttDecl(std::ofstream& xmlOut,
                        DaDiClass* gddClass,
                        const char* accessor)
//-----------------------------------------------------------------------------
{
  int i, j;
  for(i=0; i < gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    char *gddAttGetMeth = XMLString::transcode(gddAttribute->getMeth()),
      *gddAttSetMeth = XMLString::transcode(gddAttribute->setMeth()),
      *gddAttDesc = XMLString::transcode(gddAttribute->desc()),
      *gddAttType = XMLString::transcode(gddAttribute->type()),
      *gddAttName = XMLString::transcode(gddAttribute->name());
    bool gddAttIsBitset = gddAttribute->bitset();

    if(strcmp(gddAttGetMeth,accessor) == 0)
    {
      xmlOut << "  /// Retrieve " << gddAttDesc << std::endl << "  ";
      if (!DaDiTools::isFundamental(gddAttType) ||
          DaDiTools::isPointer(gddAttType))
      {
        xmlOut << "const ";
      }
      xmlOut << gddAttType;
      if (!DaDiTools::isSimple(gddAttType))
      {
        xmlOut << "&";
      }
      xmlOut << " " << DaDiTools::retGetName(gddAttName) << "() const; "
             << std::endl
             << std::endl;

      if (!DaDiTools::isFundamental(gddAttType))
      {
        xmlOut << "  /// Retrieve " << gddAttDesc << " (non-const)" << std::endl
               << "  " << gddAttType;
        if (!DaDiTools::isPointer(gddAttType))
        {
          xmlOut << "&";
        }
        xmlOut << " " << DaDiTools::retGetName(gddAttName)
               << "();" << std::endl
               << std::endl;
      }
    }

    if(strcmp(gddAttSetMeth,accessor) == 0)
    {
      xmlOut << "  /// Update " << gddAttDesc << std::endl
             << "  void set" << DaDiTools::firstUp(gddAttName) << "(";
      if (!DaDiTools::isSimple(gddAttType))
      {
        xmlOut << "const ";
      }
      xmlOut << gddAttType;
      if (!DaDiTools::isSimple(gddAttType))
      {
        xmlOut << "&";
      }
      xmlOut << " value);" << std::endl
             << std::endl;
    }

    if (gddAttIsBitset)
    {
      for (j=0; j<gddAttribute->sizeDaDiBitfield(); ++j)
      {
        DaDiBitfield* gddBitfield = gddAttribute->popDaDiBitfield();
        char *gddBfName = XMLString::transcode(gddBitfield->name()),
          *gddBfDesc = XMLString::transcode(gddBitfield->desc()),
          *gddBfSetMeth = XMLString::transcode(gddBitfield->setMeth()),
          *gddBfGetMeth = XMLString::transcode(gddBitfield->getMeth());
        std::string argType = "";
        if (gddBitfield->length() == 1)
        {
          argType = "bool";
        }
        else
        {
          argType = gddAttType;
        }

        if (strcmp(gddBfSetMeth,accessor) == 0)
        {
          xmlOut << "  /// Update " << gddBfDesc << std::endl
                 << "  void set" << DaDiTools::firstUp(gddBfName)
                 << "(" << argType << " value);" << std::endl
                 << std::endl;
        }
        if (strcmp(gddBfGetMeth,accessor) == 0)
        {
          xmlOut << "  /// Retrieve " << gddBfDesc << std::endl
                 << "  " << argType << " " << gddBfName << "() const;" 
                 << std::endl 
                 << std::endl;
        }
        XMLString::release(&gddBfName);
        XMLString::release(&gddBfDesc);
        XMLString::release(&gddBfSetMeth);
        XMLString::release(&gddBfGetMeth);
      }
    }
    XMLString::release(&gddAttGetMeth);
    XMLString::release(&gddAttSetMeth);
    XMLString::release(&gddAttDesc);
    XMLString::release(&gddAttType);
    XMLString::release(&gddAttName);
  }
}


//-----------------------------------------------------------------------------
void printSetGetAttImpl(std::ofstream& xmlOut,
                        DaDiClass* gddClass,
                        const std::string& accessor)
//-----------------------------------------------------------------------------
{
  int i,j;

  for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    char *gddAttGetMeth = XMLString::transcode(gddAttribute->getMeth()),
      *gddAttSetMeth = XMLString::transcode(gddAttribute->setMeth()),
      *gddAttType = XMLString::transcode(gddAttribute->type()),
      *gddAttName = XMLString::transcode(gddAttribute->name()),
      *gddClassName = XMLString::transcode(gddClass->name());
    int gddAttBitsetLength = gddAttribute->bitset();

    if(gddAttGetMeth == accessor)
    {
      xmlOut << "inline ";
      if (!DaDiTools::isFundamental(gddAttType) ||
          DaDiTools::isPointer(gddAttType))
      {
        xmlOut << "const ";
      }
      xmlOut << checkClassPrefix(gddAttType, gddClassName);
      if (!DaDiTools::isSimple(gddAttType))
      {
        xmlOut << "&";
      }
      xmlOut << " " << gddClassName << "::"
             << DaDiTools::retGetName(gddAttName) << "() const " << std::endl
             << "{" << std::endl
             << "  return m_" << gddAttName << ";" << std::endl
             << "}" << std::endl
             << std::endl ;

      if (!DaDiTools::isFundamental(gddAttType) ||
          DaDiTools::isPointer(gddAttType))
      {
        xmlOut << "inline " << checkClassPrefix(gddAttType, gddClassName);
        if (!DaDiTools::isPointer(gddAttType))
        {
          xmlOut << "&";
        }
        xmlOut << " " << gddClassName << "::"
               << DaDiTools::retGetName(gddAttName) << "()" << std::endl
               << "{" << std::endl
               << "  return m_" << gddAttName << ";" << std::endl
               << "}" << std::endl
               << std::endl;
      }
    }

    if(gddAttSetMeth == accessor)
    {
      xmlOut << "inline void " << gddClassName << "::set"
             << DaDiTools::firstUp(gddAttName) << "(";
      if (!DaDiTools::isSimple(gddAttType))
      {
        xmlOut << "const ";
      }
      xmlOut << checkClassPrefix(gddAttType, gddClassName);
      if (!DaDiTools::isSimple(gddAttType))
      {
        xmlOut << "&";
      }
      xmlOut << " value)" << std::endl
             << "{" << std::endl
             << "  m_" << gddAttName << " = value; " << std::endl
             << "}" << std::endl
             << std::endl;
    }

    if (gddAttBitsetLength > 0)
    {
      std::vector<std::string> bFields;
      std::vector<std::string>::iterator bfIter;
      for (j=0; j<gddAttribute->sizeDaDiBitfield(); ++j)
      {
        DaDiBitfield* gddBitfield = gddAttribute->popDaDiBitfield();
        char *gddBfName = XMLString::transcode(gddBitfield->name()),
          *gddBfSetMeth = XMLString::transcode(gddBitfield->setMeth()),
          *gddBfGetMeth = XMLString::transcode(gddBitfield->getMeth());
        std::string gddShiftBits = "";
        int gddBfLength = gddBitfield->length();

        std::string argType = "";
        std::string intArgName = "";
        if (gddBfLength == 1)
        {
          argType = "bool";
          intArgName = "val";
        }
        else 
        {
          argType = gddAttType;
          intArgName = "value";
        }

        std::vector<std::string> gddBfMasks = gddBitfield->mask();
        bFields.push_back(gddBfName);

        if (gddBfLength == 0)
        {
          for (bfIter = bFields.begin(); bfIter != bFields.end(); ++bfIter)
          {
            if (std::find(gddBfMasks.begin(), gddBfMasks.end(), *bfIter)
                != gddBfMasks.end())
            {
              gddShiftBits = *bfIter;
              break;
            }
          }
          if (gddShiftBits == "")
          {
            std::cerr << "GOD says: Could not find a sub-mask (tbd)"
                      << std::endl;
            exit(1);
          }
        }
        else
        {
          gddShiftBits = gddBfName;
        }

        if (gddBfSetMeth == accessor)
        {
          xmlOut << "inline void " << gddClassName << "::set"
                 << DaDiTools::firstUp(gddBfName) << "(" << argType << " value)"
                 << std::endl
                 << "{" << std::endl;
          if (gddBfLength == 1)
          {
            xmlOut << "  " << gddAttType << " " << intArgName << "  = ("
                   << gddAttType << ")value;" << std::endl;
          } 
          xmlOut << "  m_" << gddAttName << " &= ~" << gddBfName << "Mask;"
                 << std::endl;
          /*            << "  m_" << gddAttName << " &= (";

          bool firstElem = false;
          for (k=0; k<gddAttribute->sizeDaDiBitfield(); ++k)
          {
          DaDiBitfield* tmpBitfield = gddAttribute->popDaDiBitfield();
          std::string tmpBfName = XMLString::transcode(tmpBitfield->name());
          int tmpBfLength = tmpBitfield->length();
          if ( tmpBfLength && (
          (gddBfLength && tmpBfName != gddBfName) ||
          (!gddBfLength &&
          std::find(gddBfMasks.begin(), gddBfMasks.end(), tmpBfName) ==
          gddBfMasks.end())))
          {
          if (firstElem) { xmlOut << " | "; }
          firstElem = true;
          xmlOut << tmpBfName << "Mask";
          }
          }
          xmlOut << ");" << std::endl;*/

          xmlOut << "  m_" << gddAttName << " |= ((";
          if (gddBfLength == 1)
          {
            xmlOut << "((" << gddAttType << ")";
          }
          xmlOut << intArgName;
          if (gddBfLength == 1)
          {
            xmlOut << ")";
          }
          xmlOut << " << " << gddShiftBits
                 << "Bits) & " << gddBfName << "Mask);";
          xmlOut << std::endl
                 << "}" << std::endl
                 << std::endl;
        }
        if (gddBfGetMeth == accessor)
        {
          xmlOut << "inline " << argType << " " << gddClassName << "::" 
                 << gddBfName << "() const" << std::endl
                 << "{" << std::endl
                 << "  return ";
          if (gddBfLength == 1)
          {
            xmlOut << "(" << argType << ")";
          }
          xmlOut << "((m_" << gddAttName << " & " << gddBfName
                 << "Mask) >> " << gddShiftBits << "Bits);";
          xmlOut << std::endl
                 << "}" << std::endl
                 << std::endl;
        }
        XMLString::release(&gddBfName);
        XMLString::release(&gddBfSetMeth);
        XMLString::release(&gddBfGetMeth);
      }
    }
    XMLString::release(&gddAttGetMeth);
    XMLString::release(&gddAttSetMeth);
    XMLString::release(&gddAttType);
    XMLString::release(&gddAttName);
    XMLString::release(&gddClassName);
  }
}


//-----------------------------------------------------------------------------
void printSetGetRelDecl(std::ofstream& xmlOut,
                        DaDiClass* gddClass,
                        const std::string& accessor)
//-----------------------------------------------------------------------------
{
  int i;
  for(i=0; i < gddClass->sizeDaDiRelation(); i++)
  {
    char *get_ret = new char[16],
      *set_arg = new char[16],
      *add_arg = new char[16];
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    char *gddRelRatio = XMLString::transcode(gddRelation->ratio()),
      *gddRelGetMeth = XMLString::transcode(gddRelation->getMeth()),
      *gddRelSetMeth = XMLString::transcode(gddRelation->setMeth()),
      *gddRelAddMeth = XMLString::transcode(gddRelation->addMeth()),
      *gddRelRemMeth = XMLString::transcode(gddRelation->remMeth()),
      *gddRelClrMeth = XMLString::transcode(gddRelation->clrMeth()),
      *gddRelType = XMLString::transcode(gddRelation->type()),
      *gddRelName = XMLString::transcode(gddRelation->name()),
      *gddRelDesc = XMLString::transcode(gddRelation->desc());

    if (strcmp(gddRelRatio,"1") == 0)
    {
      strcpy(get_ret,"SmartRef<");
      strcpy(set_arg,"SmartRef<");
      strcpy(add_arg,"");
    }
    else if (strcmp(gddRelRatio,"*") == 0)
    {
      strcpy(get_ret,"SmartRefVector<");
      strcpy(set_arg,"SmartRefVector<");
      strcpy(add_arg,"SmartRef<");
    }
    else
    {
      strcpy(get_ret,"");
      strcpy(set_arg,"");
      strcpy(add_arg,"");
    }

    if (gddRelGetMeth == accessor)
    {
      if (strcmp(gddRelRatio,"1") == 0)
      {
        xmlOut << "  /// Retrieve " << gddRelDesc << " (const)" << std::endl
               << "  const " << gddRelType << "* "
               << DaDiTools::retGetName(gddRelName) << "() const;" << std::endl
               << std::endl
               << "  /// Retrieve " << gddRelDesc << " (non-const)" << std::endl
               << "  " << gddRelType << "* "
               << DaDiTools::retGetName(gddRelName) << "();" << std::endl
               << std::endl;
      }
      else
      {
        xmlOut << "  /// Retrieve " << gddRelDesc << " (const)" << std::endl
               << "  const " << get_ret << gddRelType << ">& "
               << DaDiTools::retGetName(gddRelName) << "() const;" << std::endl
               << std::endl
               << "  /// Retrieve " << gddRelDesc << " (non-const)" << std::endl
               << "  " << get_ret << gddRelType << ">& "
               << DaDiTools::retGetName(gddRelName) << "();" << std::endl
               << std::endl;
      }
    }
    if (gddRelSetMeth == accessor)
    {
      xmlOut << "  /// Update " << gddRelDesc << std::endl;
      /*if (gddRelRatio == "1")
        {
        xmlOut << "  void " << "set" << DaDiTools::firstUp(gddRelName)
        << "(const " << gddRelType << "* value); " << std::endl;
        }*/
      xmlOut  << "  void " << "set" << DaDiTools::firstUp(gddRelName)
              << "(const " << set_arg << gddRelType << ">& value);" << std::endl
              << std::endl;
    }
    if (strcmp("", add_arg) != 0)
    {
      if (gddRelAddMeth == accessor)
      {
        xmlOut << "  /// Add " << gddRelDesc << std::endl
          /*<< "  void " << "addTo" << DaDiTools::firstUp(gddRelName) << "("
            << gddRelType << "* value);" << std::endl */
               << "  void " << "addTo" << DaDiTools::firstUp(gddRelName)
               << "(const " << add_arg << gddRelType << ">& value); "
               << std::endl << std::endl;
      }
      if (gddRelRemMeth == accessor)
      {
        xmlOut << "  /// Remove " << gddRelDesc << std::endl
          /*<< "  void " << "removeFrom" << DaDiTools::firstUp(gddRelName)
            << "(" << gddRelType << "* value);" << std::endl */
               << "  void " << "removeFrom" << DaDiTools::firstUp(gddRelName)
               << "(const SmartRef<" << gddRelType << ">& value); " << std::endl
               << std::endl;
      }
      if (gddRelClrMeth == accessor)
      {
        xmlOut << "  /// Clear " << gddRelDesc << std::endl
               << "  void " << "clear" << DaDiTools::firstUp(gddRelName)
               << "();" << std::endl
               << std::endl;
      }
    }
    XMLString::release(&gddRelRatio);
    XMLString::release(&gddRelGetMeth);
    XMLString::release(&gddRelSetMeth);
    XMLString::release(&gddRelAddMeth);
    XMLString::release(&gddRelRemMeth);
    XMLString::release(&gddRelClrMeth);
    XMLString::release(&gddRelType);
    XMLString::release(&gddRelName);
    XMLString::release(&gddRelDesc);
    delete [] get_ret;
    delete [] set_arg;
    delete [] add_arg;
  }
}


//-----------------------------------------------------------------------------
void printSetGetRelImpl(std::ofstream& xmlOut,
                        DaDiClass* gddClass,
                        const std::string& accessor)
//-----------------------------------------------------------------------------
{
  int i;
  for(i=0; i<gddClass->sizeDaDiRelation(); i++)
  {
    char *get_ret = new char[256],
      *set_arg = new char[256],
      *add_arg = new char[256];
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    char *gddRelGetMeth = XMLString::transcode(gddRelation->getMeth()),
      *gddRelSetMeth = XMLString::transcode(gddRelation->setMeth()),
      *gddRelAddMeth = XMLString::transcode(gddRelation->addMeth()),
      *gddRelRemMeth = XMLString::transcode(gddRelation->remMeth()),
      *gddRelClrMeth = XMLString::transcode(gddRelation->clrMeth()),
      *gddClassName = XMLString::transcode(gddClass->name()),
      *gddRelName = XMLString::transcode(gddRelation->name()),
      *gddRelType = XMLString::transcode(gddRelation->type()),
      //*gddRelDesc = XMLString::transcode(gddRelation->desc()),
      *gddRelRatio = XMLString::transcode(gddRelation->ratio());

    if (strcmp(gddRelRatio,"1") == 0)
    {
      strcpy(get_ret,"SmartRef<");
      strcpy(set_arg,"SmartRef<");
      strcpy(add_arg,"");
    }
    else if (strcmp(gddRelRatio,"*") == 0)
    {
      strcpy(get_ret,"SmartRefVector<");
      strcpy(set_arg,"SmartRefVector<");
      strcpy(add_arg,"SmartRef<");
    }
    else
    {
      strcpy(get_ret,"");
      strcpy(set_arg,"");
      strcpy(add_arg,"");
    }

    if (gddRelGetMeth == accessor)
    {
      if (strcmp(gddRelRatio,"1") == 0)
      {
        xmlOut << "inline const " << gddRelType << "* "
               << gddClassName << "::"<< DaDiTools::retGetName(gddRelName)
               << "() const" << std::endl
               << "{" << std::endl
               << "   return m_" << gddRelName << ";" << std::endl
               << "}" << std::endl
               << std::endl;
        xmlOut << "inline " << gddRelType << "* "
               << gddClassName << "::" << DaDiTools::retGetName(gddRelName)
               << "() " << std::endl
               << "{" << std::endl
               << "   return m_" << gddRelName << ";" << std::endl
               << "}" << std::endl
               << std::endl;
      } else {
        xmlOut << "inline const " << get_ret << gddRelType << ">& "
               << gddClassName << "::" << DaDiTools::retGetName(gddRelName)
               << "() const" << std::endl
               << "{" << std::endl
               << "   return m_" << gddRelName << ";" << std::endl
               << "}" << std::endl
               << std::endl
               << "inline " << get_ret << gddRelType << ">& "
               << gddClassName << "::" << DaDiTools::retGetName(gddRelName)
               << "()" << std::endl
               << "{" << std::endl
               << "   return m_" << gddRelName << ";" << std::endl
               << "}" << std::endl
               << std::endl;
      }
    }
    if (gddRelSetMeth == accessor)
    {
      /*if (strcmp(gddRelRatio,"1") == 0)
        {
        xmlOut << "inline void " << gddClassName << "::" << "set"
        << DaDiTools::firstUp(gddRelName) << "(const " << gddRelType
        << "* value)" << std::endl
        << "{" << std::endl
        << "   m_" << gddRelName << " = value;" << std::endl
        << "}" << std::endl
        << std::endl;
        }*/
      xmlOut  << "inline void " << gddClassName << "::" << "set"
              << DaDiTools::firstUp(gddRelName) << "(const " << set_arg
              << gddRelType << ">& value)" << std::endl
              << "{" << std::endl
              << "   m_" << gddRelName << " = value;" << std::endl
              << "}" << std::endl
              << std::endl;
    }
    if (strcmp("", add_arg) != 0)
    {
      if (gddRelAddMeth == accessor)
      {
        xmlOut
          /*<< "inline void " << gddClassName << "::" << "addTo"
            << DaDiTools::firstUp(gddRelName) << "(" << gddRelType
            << "* value)" << std::endl
            << "{" << std::endl
            << "   m_"  << gddRelName << ".push_back(value);" << std::endl
            << "}" << std::endl
            << std::endl*/
          << "inline void " << gddClassName << "::"
          << "addTo" << DaDiTools::firstUp(gddRelName)
          << "(const " << add_arg << gddRelType << ">& value)" << std::endl
          << "{" << std::endl << "   m_" << gddRelName << ".push_back(value);"
          << std::endl << "}" << std::endl << std::endl;
      }

      if (gddRelRemMeth == accessor)
      {
        xmlOut
          /*<< "inline void " << gddClassName << "::"
            << "removeFrom" << DaDiTools::firstUp(gddRelName)
            << "(" << gddRelType << "* value)"  << std::endl
            << "{" << std::endl
            << "  SmartRefVector<" << gddRelType << ">::iterator iter;"
            << std::endl
            << "  for (iter = m_" << gddRelName << ".begin(); iter != m_"
            << gddRelName << ".end(); ++iter)" << std::endl
            << "  {" << std::endl
            << "    if (iter->target() == value)" << std::endl
            << "    {" << std::endl
            << "      m_" << gddRelName << ".erase(iter);" << std::endl
            << "    }" << std::endl
            << "  }" << std::endl
            << "}" << std::endl
            << std::endl */
          /*<< "inline void " << gddClassName << "::"
            << "removeFrom" << DaDiTools::firstUp(gddRelName)
            << "(const SmartRef<" << gddRelType << ">& value)" << std::endl
            << "{" << std::endl
            << "  SmartRefVector<" << gddRelType << ">::iterator iter ="
            << std::endl
            << "    std::find(m_" << gddRelName << ".begin(), m_"
            << gddRelName << ".end(), value);" << std::endl
            << "  if (iter != m_" << gddRelName << ".end() )" << std::endl
            << "  {" << std::endl
            << "    m_" << gddRelName << ".erase(iter);" << std::endl
            << "  }" << std::endl
            << "}" << std::endl
            << std::endl;*/

          << "inline void " << gddClassName << "::"
          << "removeFrom" << DaDiTools::firstUp(gddRelName)
          << "(const SmartRef<" << gddRelType << ">& value)" << std::endl
          << "{" << std::endl
          << "  SmartRefVector<" << gddRelType << ">::iterator iter ="
          << std::endl
          << "    std::remove(m_" << gddRelName << ".begin(), m_"
          << gddRelName << ".end(), value);" << std::endl
          << "  m_" << gddRelName << ".erase(iter, m_" << gddRelName
          << ".end());" << std::endl
          << "}" << std::endl
          << std::endl;
      }

      if (gddRelClrMeth == accessor)
      {
        xmlOut << "inline void " << gddClassName << "::"
               << "clear" << DaDiTools::firstUp(gddRelName) << "()" << std::endl
               << "{" << std::endl
               << "   m_" << gddRelName << ".clear();" << std::endl
               << "}" << std::endl
               << std::endl;
      }
    }
    XMLString::release(&gddRelGetMeth);
    XMLString::release(&gddRelSetMeth);
    XMLString::release(&gddRelAddMeth);
    XMLString::release(&gddRelRemMeth);
    XMLString::release(&gddRelClrMeth);
    XMLString::release(&gddClassName);
    XMLString::release(&gddRelName);
    XMLString::release(&gddRelType);
    //XMLString::release(&gddRelDesc);
    XMLString::release(&gddRelRatio);
    delete [] get_ret;
    delete [] set_arg;
    delete [] add_arg;
  }
}


//-----------------------------------------------------------------------------
template<class T> void printEnums(std::ofstream& xmlOut,
                                  T* gdd,
                                  const std::string& accessor)
//-----------------------------------------------------------------------------
{
  int i;
  bool newline = false;
  for (i=0; i<gdd->sizeDaDiEnum(); ++i)
  {
    DaDiEnum* gddEnum = gdd->popDaDiEnum();
    char *gddEnumName = XMLString::transcode(gddEnum->name()),
      *gddEnumValue = XMLString::transcode(gddEnum->value()),
      *gddEnumDesc = XMLString::transcode(gddEnum->desc()),
      *gddEnumAccess = XMLString::transcode(gddEnum->access());

    if ((accessor == "") || (gddEnumAccess == accessor))
    {
      xmlOut << "  enum " << gddEnumName << " {" << gddEnumValue << "};   ///<"
             << gddEnumDesc << std::endl;
      newline = true;
    }
    XMLString::release(&gddEnumName);
    XMLString::release(&gddEnumValue);
    XMLString::release(&gddEnumDesc);
    XMLString::release(&gddEnumAccess);
  }
  if (newline)  xmlOut << std::endl;
}


//-----------------------------------------------------------------------------
template<class T> void printTypeDefs(std::ofstream& xmlOut,
                                     T* gdd,
                                     const std::string& accessor)
//-----------------------------------------------------------------------------
{
  int i;
  bool newline = false;
  for (i=0; i<gdd->sizeDaDiTypeDef(); ++i)
  {
    DaDiTypeDef* gddTypeDef      = gdd->popDaDiTypeDef();
    char *gddTypeDefDesc   = XMLString::transcode(gddTypeDef->desc()),
      *gddTypeDefType   = XMLString::transcode(gddTypeDef->type()),
      *gddTypeDefDef    = XMLString::transcode(gddTypeDef->def()),
      *gddTypeDefAccess = XMLString::transcode(gddTypeDef->access());

    if ((accessor == "") || (gddTypeDefAccess == accessor))
    {
      newline = true;
      xmlOut << "  typedef " << gddTypeDefType << " " << gddTypeDefDef
             << ";   ///<" << gddTypeDefDesc << std::endl;
    }
    XMLString::release(&gddTypeDefDesc);
    XMLString::release(&gddTypeDefType);
    XMLString::release(&gddTypeDefDef);
    XMLString::release(&gddTypeDefAccess);
  }
  if (newline) xmlOut << std::endl;
}


//-----------------------------------------------------------------------------
void printMembers(std::ofstream& xmlOut,
                  DaDiClass* gddClass,
                  const std::string& accessor)
//-----------------------------------------------------------------------------
{
  int i,j;
  unsigned int maxLengthName = 0, maxLengthType = 0;

  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    char *gddAttName = XMLString::transcode(gddAttribute->name()),
      *gddAttType = XMLString::transcode(gddAttribute->type()),
      *gddAttArray = XMLString::transcode(gddAttribute->array());

    int gddAttNameLength = strlen(gddAttName);
    if (strcmp(gddAttArray,"FALSE") != 0)
    {
      gddAttNameLength = gddAttNameLength + 2 + strlen(gddAttArray);
    }

    if (strlen(gddAttName) > maxLengthName)
    {
      maxLengthName = strlen(gddAttName);
    }

    if (strlen(gddAttType) > maxLengthType)
    {
      maxLengthType = strlen(gddAttType);
    }

    XMLString::release(&gddAttName);
    XMLString::release(&gddAttType);
    XMLString::release(&gddAttArray);
  }

  for(i=0; i<gddClass->sizeDaDiRelation(); ++i)
  {
    unsigned int lengthType = 0;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    char *gddRelRatio = XMLString::transcode(gddRelation->ratio()),
      *gddRelName = XMLString::transcode(gddRelation->name()),
      *gddRelType = XMLString::transcode(gddRelation->type());

    if (strcmp(gddRelRatio,"1") == 0)
    {
      lengthType = 10 + strlen(gddRelType);
    }
    else
    {
      lengthType = 16 + strlen(gddRelType);
    }

    if (strlen(gddRelName) > maxLengthName)
    {
      maxLengthName = strlen(gddRelName);
    }

    if (lengthType > maxLengthType)
    {
      maxLengthType = lengthType;
    }

    XMLString::release(&gddRelRatio);
    XMLString::release(&gddRelName);
    XMLString::release(&gddRelType);
  }

  maxLengthName = maxLengthName + 4;

  //  xmlOut.setf(std::ios::left, std::ios::adjustfield);


  //
  // Enums for bitfields (lengths & masks)
  //
  for (i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    char *gddAttAccess = XMLString::transcode(gddAttribute->access()),
      *gddAttName = XMLString::transcode(gddAttribute->name()),
      *gddAttType = XMLString::transcode(gddAttribute->type()),
      *gddAttDesc = XMLString::transcode(gddAttribute->desc());
    int gddAttBitsetLength = gddAttribute->bitset();

    if ((gddAttBitsetLength > 0) && (gddAttAccess == accessor))
    {
      xmlOut << "  enum " << gddAttName << "Bits {";
      int offset = 0;
      for (j=0; j<gddAttribute->sizeDaDiBitfield(); ++j)
      {
        DaDiBitfield* gddBitfield = gddAttribute->popDaDiBitfield();
        int gddBfLength = gddBitfield->length();
        char *gddBfName = XMLString::transcode(gddBitfield->name());
        if (gddBfLength != 0)
        {
          if (j!=0) { xmlOut << ", "; }
          xmlOut << gddBfName << "Bits = " << offset;
          offset = offset + gddBfLength;
        }
        XMLString::release(&gddBfName);
      }
      xmlOut << "}; ///< Offsets of bitfield " << gddAttName << std::endl;
      if (offset > (gddAttBitsetLength*8))
      {
        std::cerr << std::endl << "GOD says: WARNING: Bitset "
                  << gddAttName << " has more than "
                  << (gddAttBitsetLength*8) << " bits";
      }

      xmlOut << "  enum " << gddAttName << "Masks {";
      offset = 0;
      for (j=0; j<gddAttribute->sizeDaDiBitfield(); ++j)
      {
        DaDiBitfield* gddBitfield = gddAttribute->popDaDiBitfield();
        int gddBfLength = gddBitfield->length();
        char *gddBfName = XMLString::transcode(gddBitfield->name());

        if (j!=0) { xmlOut << ", "; }
        xmlOut << gddBfName << "Mask = ";

        if (gddBfLength != 0)
        {
          int mask = (int)pow(2,gddBfLength)-1;
          mask = mask << offset;
          xmlOut << "0x";
          xmlOut.setf(std::ios::hex, std::ios::basefield);
          xmlOut.width(gddAttBitsetLength/4);
          xmlOut.fill('0');
          xmlOut << mask;
          xmlOut.fill(' ');
          xmlOut.unsetf(std::ios::adjustfield);
        }
        else
        {
          std::vector<std::string> masks = gddBitfield->mask();
          std::vector<std::string>::const_iterator iter;

          for (iter = masks.begin(); iter != masks.end(); ++iter)
          {
            if (iter != masks.begin())
            {
              xmlOut << " + ";
            }
            xmlOut << *iter << "Mask";
          }
        }
        offset = offset + gddBfLength;
        XMLString::release(&gddBfName);
      }
      xmlOut << "}; ///< Bitmasks for bitfield " << gddAttName << std::endl
             << std::endl;
    }

    XMLString::release(&gddAttAccess);
    XMLString::release(&gddAttName);
    XMLString::release(&gddAttType);
    XMLString::release(&gddAttDesc);
  }


  //
  // Private members (attributes)
  //
  for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    char *gddAttAccess = XMLString::transcode(gddAttribute->access()),
      *gddAttName = XMLString::transcode(gddAttribute->name()),
      *gddAttType = XMLString::transcode(gddAttribute->type()),
      *gddAttDesc = XMLString::transcode(gddAttribute->desc()),
      *gddAttInit = XMLString::transcode(gddAttribute->init()),
      *gddAttArray = XMLString::transcode(gddAttribute->array());
    //    bool gddAttIsStatic = gddAttribute->static_();

    if (gddAttAccess == accessor)
    {
      std::string fullAttName = " m_" + std::string(gddAttName);
      if (strcmp(gddAttArray,"FALSE") != 0)
      {
        fullAttName = fullAttName + "[" + gddAttArray + "]";
      }
      fullAttName += ";";
      xmlOut.setf(std::ios::left, std::ios::adjustfield);
      xmlOut << "  ";
      xmlOut.width(maxLengthType);
      xmlOut << gddAttType;
      xmlOut.width(maxLengthName);
      xmlOut << fullAttName;
      xmlOut.width(0);
      xmlOut << " ///< " << gddAttDesc << std::endl;
    }

    XMLString::release(&gddAttAccess);
    XMLString::release(&gddAttName);
    XMLString::release(&gddAttType);
    XMLString::release(&gddAttDesc);
    XMLString::release(&gddAttInit);
    XMLString::release(&gddAttArray);

  }

  //
  // Private members (relations)
  //
  for(i=0; i<gddClass->sizeDaDiRelation(); i++)
  {
    std::string rel_type;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    char *gddRelAccess = XMLString::transcode(gddRelation->access()),
      *gddRelRatio = XMLString::transcode(gddRelation->ratio()),
      *gddRelType = XMLString::transcode(gddRelation->type()),
      *gddRelName = XMLString::transcode(gddRelation->name()),
      *gddRelDesc = XMLString::transcode(gddRelation->desc());

    if (gddRelAccess == accessor)
    {
      if (strcmp(gddRelRatio,"1") == 0)
      {
        rel_type = "SmartRef<";
      }
      else if (strcmp(gddRelRatio,"*") == 0)
      {
        rel_type = "SmartRefVector<";
      }
      std::string relType = rel_type + gddRelType + '>';
      std::string relName = " m_" + std::string(gddRelName) + ";";
      xmlOut.setf(std::ios::left, std::ios::adjustfield);
      xmlOut << "  ";
      xmlOut.width(maxLengthType);
      xmlOut << relType;
      xmlOut.width(maxLengthName);
      xmlOut << relName;
      xmlOut.width(0);
      xmlOut << " ///< " << gddRelDesc << std::endl;
    }

    XMLString::release(&gddRelAccess);
    XMLString::release(&gddRelRatio);
    XMLString::release(&gddRelType);
    XMLString::release(&gddRelName);
    XMLString::release(&gddRelDesc);

  }
  xmlOut.unsetf(std::ios::adjustfield);
}


//-----------------------------------------------------------------------------
void printClass(std::ofstream& xmlOut,
                DaDiClass* gddClass,
                DaDiPackage* gddPackage,
                std::map<std::string,std::string>& dbExportClass,
                bool dictionaryHeader)
//-----------------------------------------------------------------------------
{
  time_t ltime;
  int i, j;
  bool classTemplate = gddClass->keyedContTypeDef(),
    isEventClass = false,
    classTemplateVector = false,
    classTemplateList = false;

  char *gddClassName = XMLString::transcode(gddClass->name()),
    *gddClassID = XMLString::transcode(gddClass->ID()),
    *gddClassLocation = XMLString::transcode(gddClass->location()),
    *gddPackName = XMLString::transcode(gddPackage->packageName()),
    *gddClassDesc = XMLString::transcode(gddClass->desc()),
    *gddClassLongDesc = XMLString::transcode(gddClass->longDesc()),
    *gddClassAuthor = XMLString::transcode(gddClass->author());

  bool gddClassSerializers = gddClass->serializers(),
    gddClassStdVectorTypeDef = gddClass->stdVectorTypeDef();


  gddDefEnums.clear();
  gddDefTypeDefs.clear();

  for (i=0; i<gddClass->sizeDaDiEnum(); ++i)
  {
    char *tmpEnum = XMLString::transcode(gddClass->popDaDiEnum()->name());
    gddDefEnums.push_back(tmpEnum);
    XMLString::release(&tmpEnum);
  }
  for (i=0; i<gddClass->sizeDaDiTypeDef(); ++i)
  {
    char *tmpTypeDef = XMLString::transcode(gddClass->popDaDiTypeDef()->def());
    gddDefTypeDefs.push_back(tmpTypeDef);
    XMLString::release(&tmpTypeDef);
  }

  ///
  /// check if Class is an 'Eventclass'
  ///
  /*for (i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
    {
    std::string bClassName = XMLString::transcode(gddClass->
    popDaDiBaseClass()->name());
    if ((strcmp(bClassName,"DataObject") == 0) ||
    (strcmp(bClassName,"ContainedObject") == 0))
    {
    isEventClass = true;
    }
    }*/

  if (!DaDiTools::isEmpty(gddClassID))
  {
    isEventClass = true;
  }


  //
  // Externals
  //
  if (isEventClass)
  {
    xmlOut << "// Class ID definition" << std::endl
           << "  static const CLID& CLID_" << gddClassName << " = "
           << gddClassID << ";" << std::endl
           << std::endl;
  }



  //
  // create namespace for locations
  //
  if ((!DaDiTools::isEmpty(gddClassLocation)) || gddClass->sizeDaDiLocation())
  {
    xmlOut << "// Namespace for locations in TDS" << std::endl
           << "namespace " << gddClassName << "Location {" << std::endl;
    if (!DaDiTools::isEmpty(gddClassLocation))
    {
      xmlOut << "  static const std::string& Default = \""
             << gddClassLocation << "\";" << std::endl;
    }
    for (i = 0; i < gddClass->sizeDaDiLocation(); ++i)
    {
      DaDiLocation* gddLocation = gddClass->popDaDiLocation();
      char *gddLocName = XMLString::transcode(gddLocation->name()),
        *gddLocPlace = XMLString::transcode(gddLocation->place());
      bool gddNoQuote = gddLocation->noQuote();

      xmlOut << "  static const std::string& " << gddLocName << " = ";
      if (!gddNoQuote) { xmlOut << "\""; }
      xmlOut << gddLocPlace;
      if (!gddNoQuote) { xmlOut << "\""; }
      xmlOut << ";" << std::endl;

      XMLString::release(&gddLocName);
      XMLString::release(&gddLocPlace);
    }
    xmlOut << "}" << std::endl
           << std::endl
           << std::endl;
  }


  /*
  //
  // create namespace for locations
  //
  if ((gddClass->location() != NULL) || gddClass->sizeDaDiLocation())
  {
  bool def = false;
  xmlOut << "// Namespace for locations in TDS" << std::endl
  << "namespace " << XMLString::transcode(gddClass->className()) << "Location {"
  << std::endl << "  enum {";
  if (gddClass->location() != NULL)
  {
  xmlOut << "Default";
  def = true;
  }
  for (i = 0; i < gddClass->sizeDaDiLocation(); ++i)
  {
  DaDiLocation* gddLocation = gddClass->popDaDiLocation();
  if (def || i!=0) { xmlOut << ", "; }
  xmlOut << XMLString::transcode(gddLocation->name());
  }
  xmlOut << "};" << std::endl << "}" << std::endl << std::endl << std::endl;
  }*/

  time(&ltime);

  //
  // class description
  //
  xmlOut << "/** @class " << gddClassName << " " << gddClassName << ".h ";

  if (strcmp(gddPackName,"__NO_PACKAGE__") == 0)
  {
    xmlOut << gddPackName << "/" << gddClassName << ".h";
  }

  xmlOut << std::endl << " *" << std::endl;

  if (!DaDiTools::isEmpty(gddClassDesc))
  {
    xmlOut << " *  " << gddClassDesc;
  }

  if (!DaDiTools::isEmpty(gddClassLongDesc))
  {
    xmlOut << std::endl
           << gddClassLongDesc;
  }

  xmlOut << std::endl
         << " *" << std::endl
         << " *  @author " << gddClassAuthor << std::endl
         << " *  created " << ctime(&ltime)
         << " *" << std::endl
         << " */" << std::endl
         << std::endl;

  //
  // class definition
  //
  xmlOut << "class " << gddClassName;

  std::vector<std::string>::iterator coIter, koIter;

  if (gddClass->sizeDaDiBaseClass() > 0)
  {
    xmlOut << ": ";
    for (i = 0; i < gddClass->sizeDaDiBaseClass(); ++i)
    {
      DaDiBaseClass* gddBaseClass = gddClass->popDaDiBaseClass();
      char *fullBaseClName = XMLString::transcode(gddBaseClass->name()),
        *gddBaseClassAccess = XMLString::transcode(gddBaseClass->access());
      bool gddBaseClassIsVirtual = gddBaseClass->virtual_();
      std::string sFullBaseClName = std::string(fullBaseClName),
        baseClName = sFullBaseClName.substr(0,
                                            sFullBaseClName.find_first_of("<"));

      koIter = std::find(KeyedObjectClasses.begin(),
                         KeyedObjectClasses.end(),
                         baseClName);
      if (koIter != KeyedObjectClasses.end())
      {
        KeyedObjectClasses.push_back(gddClassName);
        classTemplate = true;
      }
      //////////////////////  tbd -->
      coIter = std::find(ContainedObjectClasses.begin(),
                         ContainedObjectClasses.end(),
                         std::string(baseClName));
      if (coIter != ContainedObjectClasses.end())
      {
        char *baseCl = XMLString::transcode(gddClass->name());
        ContainedObjectClasses.push_back(baseCl);
        XMLString::release(&baseCl);

        if (gddClass->classTemplateVector())
        {
          classTemplateVector = true;
        }
        if (gddClass->classTemplateList())
        {
          classTemplateList = true;
        }
      }
      ////////////////////////// <--
      if (i>0) {xmlOut << ", ";}
      if (gddBaseClassIsVirtual)
      {
        xmlOut << "virtual ";
      }
      xmlOut << DaDiTools::chooseAccess(gddBaseClassAccess) << " "
             << fullBaseClName;

      XMLString::release(&fullBaseClName);
      XMLString::release(&gddBaseClassAccess);
    }
  }


  xmlOut << std::endl << "{" << std::endl
         << std::endl;


  //
  //   PUBLIC AREA
  //
  xmlOut << "public: " << std::endl
         << std::endl;

  if (dictionaryHeader)
  {
    xmlOut << "  friend class " << gddClassName << "_dict;" << std::endl
           << std::endl;
  }

  //
  //  print public typedefs and enums
  //
  printTypeDefs(xmlOut, gddClass, "PUBLIC");
  printEnums(xmlOut, gddClass, "PUBLIC");

  //
  // (Standard) constructor & destructor
  //

  bool constWithZeroArgs = false;
  //
  // NonStandard Constructors
  //
  for(int h=0; h<gddClass->sizeDaDiConstructor(); ++h)
  {
    DaDiConstructor* gddConstructor = gddClass->popDaDiConstructor();
    char *gddConsDesc = XMLString::transcode(gddConstructor->desc()),
      *gddConsCode = XMLString::transcode(gddConstructor->code()),
      *gddConsInitList = XMLString::transcode(gddConstructor->initList());


    if (!DaDiTools::isEmpty(gddConsDesc))
    {
      xmlOut << "  /// " << gddConsDesc << std::endl;
    }
    xmlOut << "  " << gddClassName << "(";

    if (gddConstructor->sizeDaDiMethArgument() == 0) {constWithZeroArgs = true;}

    printArguments(xmlOut, gddConstructor);

    xmlOut << ")";


    if (strcmp(gddConsInitList,"") != 0)
    {
      char *initL = XMLString::transcode(gddConstructor->initList());
      xmlOut << std::endl << "    : " << initL;
      XMLString::release(&initL);

      if (strcmp(gddConsCode,"") == 0)
      {
        XMLString::release(&gddConsCode);
        gddConsCode = new char[2];
        strcpy(gddConsCode," ");
      }

    }

    if (strcmp(gddConsCode,"") != 0)
    {
      xmlOut << std::endl
             << "  {" << std::endl
             << gddConsCode << std::endl
             << "  }" << std::endl
             << std::endl;
    }
    else
    {
      xmlOut << ";" << std::endl
             << std::endl;
    }
    XMLString::release(&gddConsDesc);
    XMLString::release(&gddConsCode);
    XMLString::release(&gddConsInitList);
  }

  //
  // Default Constructor
  //
  if (!constWithZeroArgs)
  {
    xmlOut << "  /// Default Constructor " << std::endl
           << "  " << gddClassName << "() ";

    bool firstLine = true;
    for (i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      std::string initValue;
      char *gddAttName = XMLString::transcode(gddAttribute->name()),
        *gddAttType = XMLString::transcode(gddAttribute->type()),
        *gddAttInit = XMLString::transcode(gddAttribute->init());
      bool gddAttIsStatic = gddAttribute->static_();
      std::string sGddAttType = std::string(gddAttType);
      int lastspace = sGddAttType.find_last_of(" ");
      sGddAttType = sGddAttType.substr(lastspace+1,
                                       sGddAttType.size()-lastspace);
      if (!gddAttIsStatic)
      {
        if (!DaDiTools::isEmpty(gddAttInit))
        {
          initValue = gddAttInit;
        }
        else if ((sGddAttType == "int") || (sGddAttType == "unsigned") ||
                 (sGddAttType == "short") || (sGddAttType == "signed") ||
                 (sGddAttType == "long"))
        {
          initValue = "0";
        }
        else if ((sGddAttType == "float") || (sGddAttType == "double"))
        {
          initValue = "0.0";
        }

        //        if (initValue != "")
        //        {
        if (firstLine)
        {
          xmlOut << std::endl
                 << "    : ";
          firstLine = false;
        }
        else
        {
          xmlOut << "," << std::endl
                 << "    ";
        }
        xmlOut << "m_" << gddAttName << "(" << initValue << ")";
        //        }
      }
      XMLString::release(&gddAttName);
      XMLString::release(&gddAttType);
      XMLString::release(&gddAttInit);
    }
    xmlOut << " {}" << std::endl
           << std::endl;
  }



  if (gddClass->sizeDaDiDestructor() == 0)
  {
    xmlOut << "  /// Destructor " << std::endl
           << "  virtual " << "~" << gddClassName << "() {}" << std::endl
           << std::endl;
  }
  else
  {
    //
    // NonStandard Destructors
    //
    for(int h=0; h<gddClass->sizeDaDiDestructor(); ++h)
    {
      DaDiDestructor* gddDestructor = gddClass->popDaDiDestructor();
      char *gddDestDesc = XMLString::transcode(gddDestructor->desc()),
        *gddDestCode = XMLString::transcode(gddDestructor->code());


      if (!DaDiTools::isEmpty(gddDestDesc))
      {
        xmlOut << "  /// " << gddDestDesc << std::endl;
      }
      xmlOut << "  ~" << gddClassName << "(";

      printArguments(xmlOut, gddDestructor);

      xmlOut << ")";

      if (strcmp(gddDestCode,"") != 0)
      {
        xmlOut << std::endl
               << "  {" << std::endl
               << gddDestCode << std::endl
               << "  }" << std::endl
               << std::endl;
      }
      else
      {
        xmlOut << ";" << std::endl
               << std::endl;
      }
      XMLString::release(&gddDestDesc);
      XMLString::release(&gddDestCode);
    }
  }

  //
  // Functions clID() & classID()
  //
  if (isEventClass)
  {
    xmlOut << "  /// Retrieve pointer to class definition structure"
           << std::endl
           << "  virtual const CLID& clID() const; " << std::endl;

    xmlOut << "  static const CLID& classID(); " << std::endl
           << std::endl;
  }


  /*
  //
  // Return locations of class in TDS
  //
  if ((gddClass->location() != NULL) || gddClass->sizeDaDiLocation())
  {
  xmlOut << "  /// Retrieve location of class in TES " << std::endl
  << "  static const std::string location(int loc);" << std::endl
  << std::endl;
  }*/


  //
  // Declaration of selfdefined methods
  //

  printMethodDecl(xmlOut, gddClass, "PUBLIC");


  //
  // Declarations of set() and get() methods for attributes
  //

  printSetGetAttDecl(xmlOut, gddClass, "TRUE");

  //
  // Declaration of set(), get(), rem(), add(), clr() methods for relations
  //

  printSetGetRelDecl(xmlOut, gddClass, "TRUE");


  //
  // Serializers & fillStream
  //
  bool streamIn=false, streamOut=false, ostreamOut=false;
  if (gddClassSerializers)
  {
    if (!isEventClass)
    {
      xmlOut << "  /// Operator overloading for serializing (writing)"
             << std::endl
             << "  friend StreamBuffer& operator<< (StreamBuffer& s, const "
             << gddClassName << "& obj)" << std::endl
             << "  {" << std::endl
             << "    return obj.serialize(s);" << std::endl
             << "  }" << std::endl
             << std::endl;

      xmlOut << "  /// Operator overloading for serializing (reading)"
             << std::endl
             << "  friend StreamBuffer& operator>> (StreamBuffer& s, "
             << gddClassName << "& obj)" << std::endl
             << "  {" << std::endl
             << "    return obj.serialize(s);" << std::endl
             << "  }" << std::endl
             << std::endl;

      char *clName = XMLString::transcode(gddClass->name());
      xmlOut << "  /// Operator overloading for stringoutput" << std::endl
             << "  friend std::ostream& operator<< (std::ostream& s, const "
             << clName << "& obj)" << std::endl
             << "  {" << std::endl
             << "    return obj.fillStream(s);" << std::endl
             << "  }" << std::endl
             << std::endl;
      XMLString::release(&clName);

      /*    xmlOut << "/// Serialize the object for writing" << std::endl
            << "virtual StreamBuffer& serialize(StreamBuffer& s) const;"
            << std::endl << std::endl;

            xmlOut << "/// Serialize the object for reading" << std::endl
            << "virtual StreamBuffer& serialize(StreamBuffer& s);"
            << std::endl << std::endl;

            xmlOut << "/// Fill the ASCII output stream" << std::endl
            << "virtual std::ostream& fillStream(std::ostream& s) const;"
            << std::endl << std::endl;*/
    }

    for (i=0; i<gddClass->sizeDaDiMethod(); ++i)
    {
      DaDiMethod* gddMethod = gddClass->popDaDiMethod();
      char *gddMethodName = XMLString::transcode(gddMethod->name()),
        *gddMethRetType = XMLString::transcode(gddMethod->daDiMethReturn()->
                                               type());
      bool gddMethodIsConst = gddMethod->const_();

      if (strcmp(gddMethodName,"serialize") == 0 &&
          strcmp(gddMethRetType,"StreamBuffer&") == 0 && !gddMethodIsConst)
      {
        streamIn = true;
      }
      if (strcmp(gddMethodName,"serialize") == 0 &&
          strcmp(gddMethRetType,"StreamBuffer&") == 0 && gddMethodIsConst)
      {
        streamOut = true;
      }
      if ((strcmp(gddMethodName,"fillStream")==0) &&
          (strcmp(gddMethRetType,"std::ostream&")==0))
      {
        ostreamOut = true;
      }
      XMLString::release(&gddMethodName);
      XMLString::release(&gddMethRetType);
    }


    //
    //  Serializers, fillstream
    //

    if (!streamOut)
    {
      xmlOut << "  /// Serialize the object for writing" << std::endl
             << "  virtual StreamBuffer& serialize(StreamBuffer& s) const;"
             << std::endl << std::endl;
    }
    if (!streamIn)
    {
      xmlOut << "  /// Serialize the object for reading" << std::endl
             << "  virtual StreamBuffer& serialize(StreamBuffer& s);"
             << std::endl << std::endl;
    }
    if (!ostreamOut)
    {
      xmlOut << "  /// Fill the ASCII output stream" << std::endl
             << "  virtual std::ostream& fillStream(std::ostream& s) const;"
             << std::endl << std::endl;
    }
  }


  // print public members
  printMembers(xmlOut, gddClass, "PUBLIC");


  //
  //   PROTECTED AREA
  //

  xmlOut << "protected: " << std::endl
         << std::endl;

  printTypeDefs(xmlOut, gddClass, "PROTECTED");
  printMethodDecl(xmlOut, gddClass, "PROTECTED");
  printEnums(xmlOut, gddClass, "PROTECTED");
  printMembers(xmlOut,gddClass, "PROTECTED");


  //
  //   PRIVATE AREA
  //

  xmlOut << "private: " << std::endl
         << std::endl;

  // print private typedefs
  printTypeDefs(xmlOut, gddClass, "PRIVATE");

  // print declarations of private functions
  printMethodDecl(xmlOut, gddClass, "PRIVATE");

  //
  //  Private members (attributes)
  //
  printEnums(xmlOut, gddClass, "PRIVATE");
  printMembers(xmlOut, gddClass, "PRIVATE");

  xmlOut << std::endl
         << "};" << std::endl
         << std::endl
         << "// ---------------------------------------------------------------"
         << "--------------" << std::endl
         << "//   end of class" << std::endl
         << "// ---------------------------------------------------------------"
         << "--------------" << std::endl;

  // ---------------------------------------------------------------------------
  // Function Definitions


  //
  // Including files which were forwarded before
  //

  if (gddPackage->sizeImpSoftList() || gddClass->sizeImpSoftList())
  {
    xmlOut << std::endl
           << std::endl
           << "// Including forward declarations" << std::endl;
    for(i=0; i<gddPackage->sizeImpSoftList(); i++)
    {
      std::string impName = gddPackage->popImpSoftList();
      if (dbExportClass[impName] != "")
      {
        impName = dbExportClass[impName];
      }
      else
      {
        std::cerr << std::endl
                  << argV0 << ": No information found for type: " << impName
                  << std::endl
                  << argV0 << ": Line written: #include \"" << impName << ".h\""
                  << std::endl;
      }
      xmlOut << "#include \"" << impName << ".h\"" << std::endl;
    }
    for(i=0; i<gddClass->sizeImpSoftList(); i++)
    {
      std::string impName = gddClass->popImpSoftList();
      if (dbExportClass[impName] != "")
      {
        impName = dbExportClass[impName];
      }
      else
      {
        std::cerr << std::endl
                  << argV0 << ": No information found for type: " << impName
                  << std::endl
                  << argV0 << ": Line written: #include \"" << impName << ".h\""
                  << std::endl;
      }
      xmlOut << "#include \"" << impName << ".h\"" << std::endl;
    }
  }

  xmlOut << std::endl
         << std::endl;


  if (isEventClass)
  {
    //
    // Function clID()
    //
    xmlOut << "inline const CLID& " << gddClassName << "::clID() const "
           << std::endl
           << "{" << std::endl
           << "  return " << gddClassName << "::classID();" << std::endl
           << "}" << std::endl
           << std::endl;

    //
    // Function classID()
    //
    xmlOut << "inline const CLID& " << gddClassName << "::classID()"
           << std::endl
           << "{" << std::endl
           << "  return CLID_" << gddClassName << ";" << std::endl
           << "}" << std::endl
           << std::endl;
  }


  /*  //
  // return location in TES
  //
  if ((gddClass->location() != NULL) || gddClass->sizeDaDiLocation())
  {
  std::string namesp = XMLString::transcode(gddClass->className());
  namesp += "Location::";
  xmlOut << "inline static const std::string location(int loc)" << std::endl
  << "{" << std::endl << "  switch(loc) {";
  if (gddClass->location() != NULL)
  {
  xmlOut << std::endl << "  case " << namesp << "Default: " << std::endl
  << "    return \"" << XMLString::transcode(gddClass->location()) << "\";";
  }
  for (i=0; i<gddClass->sizeDaDiLocation(); ++i)
  {
  DaDiLocation* gddLocation = gddClass->popDaDiLocation();
  xmlOut << std::endl << "  case " << namesp
  << XMLString::transcode(gddLocation->name())
  << ":" << std::endl << "    return \""
  << XMLString::transcode(gddLocation->place())
  << "\";";
  }
  xmlOut << std::endl << "  default: " << std::endl
  << "    return \"\";" << std::endl << "  }" << std::endl
  << "}" << std::endl << std::endl;
  }
  */




  //
  // print all implementations of methods and setters and getters etc.
  // in the right order (because of gcc)
  //
  // public area
  //
  // Writing of selfdefinded Methods
  //
  printMethodImpl(xmlOut, gddClass, "PUBLIC");

  //
  // setX() and getX() for Attributes
  //
  printSetGetAttImpl(xmlOut, gddClass, "TRUE");

  //
  // setX(), getX(), addX(), remX(), clrX() Functions for Relations
  //
  printSetGetRelImpl(xmlOut, gddClass, "TRUE");

  // same for protected
  printMethodImpl(xmlOut, gddClass, "PROTECTED");

  // same for private
  printMethodImpl(xmlOut, gddClass, "PRIVATE");



  if (gddClassSerializers)
  {

    /*if (!isEventClass)
      {
      xmlOut << "StreamBuffer& operator<< (StreamBuffer& s, const "
      << gddClassName << "& obj)" << std::endl
      << "{" << std::endl
      << "  return obj.serialize(s);" << std::endl
      << "}" << std::endl
      << std::endl;

      xmlOut << "StreamBuffer& operator>> (StreamBuffer& s, "
      << gddClassName << "& obj)" << std::endl
      << "{" << std::endl
      << "  return obj.serialize(s);" << std::endl
      << "}" << std::endl
      << std::endl;

      xmlOut << "std::ostream& operator<< (std::ostream& s, const "
      << XMLString::transcode(gddClass->name()) << "& obj)" << std::endl
      << "{"  << std::endl
      << "  return obj.fillStream(s);" << std::endl
      << "}" << std::endl
      << std::endl;
      }*/



    //
    // SteamBuffer<<
    //
    if (!streamOut)
    {
      /// function header
      xmlOut << "inline StreamBuffer& " << gddClassName
             << "::serialize(StreamBuffer& s) const " << std::endl
             << "{" << std::endl;

      /// treating boolean values
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttName = XMLString::transcode(gddAttribute->name()),
          *gddAttType = XMLString::transcode(gddAttribute->type());
        bool gddAttSerialize = gddAttribute->serialize();

        if ((strcmp(gddAttType,"bool") == 0) && gddAttSerialize)
        {
          xmlOut << "  unsigned char " << "l_" << gddAttName << " = (m_"
                 << gddAttName << ") ? 1 : 0;" << std::endl;
        }
        XMLString::release(&gddAttName);
        XMLString::release(&gddAttType);
      }

      bool seriAtt = false;
      for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
      {
        char *gddBaseName = XMLString::transcode(gddClass->popDaDiBaseClass()->
                                                 name());
        if ((strcmp(gddBaseName, "ContainedObject") != 0) &&
            (strcmp(gddBaseName, "DataObject") != 0))
        {
          xmlOut << "  " << gddBaseName << "::serialize(s);" << std::endl;
        }
        XMLString::release(&gddBaseName);
      }
      for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttType = XMLString::transcode(gddAttribute->type()),
          *gddAttName = XMLString::transcode(gddAttribute->name());
        bool gddAttSerialize = gddAttribute->serialize(),
          gddAttCompression = gddAttribute->compression();

        if (gddAttSerialize)
        {
          if(i==0 && !seriAtt)
          {
            xmlOut << "  s ";
            seriAtt = true;
          }
          else
          {
            xmlOut << std::endl << "    ";
          }
          if (strcmp(gddAttType,"bool") == 0)
          {
            xmlOut << "<< l_";
          }
          else if ((strcmp(gddAttType,"double") == 0) && gddAttCompression)
          {
            xmlOut << "<< (float)m_";
          }
          else
          {
            xmlOut << "<< m_";
          }
          xmlOut << gddAttName;
        }
        XMLString::release(&gddAttType);
        XMLString::release(&gddAttName);
      }

      for(i=0; i<gddClass->sizeDaDiRelation(); i++)
      {
        DaDiRelation* gddRelation = gddClass->popDaDiRelation();
        char *gddRelName = XMLString::transcode(gddRelation->name());
        bool gddRelSerialize = gddRelation->serialize();

        if (gddRelSerialize)
        {
          if (i==0 && !seriAtt)
          {
            xmlOut << "s << m_" << gddRelName << "(this)";
            seriAtt = true;
          }
          else
          {
            xmlOut << std::endl << "    << m_" << gddRelName << "(this)";
          }
        }
        XMLString::release(&gddRelName);
      }

      if (seriAtt)
      {
        xmlOut << ";" << std::endl;
      }

      xmlOut << "  return s;" << std::endl
             << "}" << std::endl
             << std::endl;
    }


    //
    // StreamBuffer>>
    //
    if(!streamIn)
    {
      /// function header
      xmlOut << "inline StreamBuffer& " << gddClassName
             << "::serialize(StreamBuffer& s)" << std::endl
             << "{" << std::endl;

      bool typePrinted = false;

      /// treating boolean values
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttType = XMLString::transcode(gddAttribute->type()),
          *gddAttName = XMLString::transcode(gddAttribute->name());
        bool gddAttSerialize = gddAttribute->serialize();

        if ((strcmp(gddAttType,"bool") == 0) && gddAttSerialize)
        {
          if (!typePrinted)
          {
            xmlOut << "  unsigned char ";
            typePrinted = true;
          }
          else
          {
            xmlOut << ", ";
          }
          xmlOut << "l_" << gddAttName;
        }
        XMLString::release(&gddAttType);
        XMLString::release(&gddAttName);
      }
      if (typePrinted)
      {
        xmlOut << ";" << std::endl;
        typePrinted = false;
      }

      /// treating enums
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttType = XMLString::transcode(gddAttribute->type()),
          *gddAttName = XMLString::transcode(gddAttribute->name());
        bool gddAttSerialize = gddAttribute->serialize();

        if (std::find(gddDefEnums.begin(), gddDefEnums.end(), gddAttType) !=
            gddDefEnums.end() && gddAttSerialize)
        {
          if (!typePrinted)
          {
            xmlOut << "  unsigned ";
            typePrinted = true;
          }
          else
          {
            xmlOut << ", ";
          }
          xmlOut << "l_" << gddAttName;
        }
        XMLString::release(&gddAttType);
        XMLString::release(&gddAttName);
      }
      if (typePrinted)
      {
        xmlOut << ";" << std::endl;
        typePrinted = false;
      }

      /// treating float values
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttType = XMLString::transcode(gddAttribute->type()),
          *gddAttName = XMLString::transcode(gddAttribute->name());
        bool gddAttSerialize = gddAttribute->serialize(),
          gddAttCompression = gddAttribute->compression();

        if ((strcmp(gddAttType,"double") == 0) &&
            gddAttSerialize && gddAttCompression)
        {
          if (!typePrinted)
          {
            xmlOut << "  float ";
            typePrinted = true;
          }
          else
          {
            xmlOut << ", ";
          }
          xmlOut << "l_" << gddAttName;
        }
        XMLString::release(&gddAttType);
        XMLString::release(&gddAttName);
      }
      if (typePrinted)
      {
        xmlOut << ";" << std::endl;
        typePrinted = false;
      }

      bool seriAtt = false;
      for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
      {
        char *gddBaseName = XMLString::transcode(gddClass->popDaDiBaseClass()->
                                                 name());
        if (strcmp(gddBaseName,"ContainedObject") != 0 &&
            strcmp(gddBaseName,"DataObject") != 0)
        {
          xmlOut << "  " << gddBaseName << "::serialize(s);" << std::endl;
        }
        XMLString::release(&gddBaseName);
      }

      for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttType = XMLString::transcode(gddAttribute->type()),
          *gddAttName = XMLString::transcode(gddAttribute->name());
        bool gddAttSerialize = gddAttribute->serialize(),
          gddAttCompression = gddAttribute->compression();

        if (gddAttSerialize)
        {
          if(i==0 && !seriAtt)
          {
            xmlOut << "  s ";
            seriAtt = true;
          }
          else
          {
            xmlOut << std::endl << "    ";
          }
          if ((strcmp(gddAttType,"bool") == 0) ||
              ((strcmp(gddAttType,"double") == 0) && gddAttCompression) ||
              (std::find(gddDefEnums.begin(), gddDefEnums.end(), gddAttType) !=
               gddDefEnums.end()))
          {
            xmlOut << ">> l_";
          }
          else
          {
            xmlOut << ">> m_";
          }
          xmlOut << gddAttName;
        }
        XMLString::release(&gddAttType);
        XMLString::release(&gddAttName);
      }

      for(i=0; i<gddClass->sizeDaDiRelation(); i++)
      {
        DaDiRelation* gddRelation = gddClass->popDaDiRelation();
        char *gddRelName = XMLString::transcode(gddRelation->name());
        bool gddRelSerialize = gddRelation->serialize();

        if (gddRelSerialize)
        {
          if (i==0 && !seriAtt)
          {
            xmlOut << "s >> m_" << gddRelName << "(this)";
            seriAtt = true;
          }
          else
          {
            xmlOut << std::endl << "    " << ">> m_" << gddRelName << "(this)";
          }
        }
        XMLString::release(&gddRelName);
      }
      if (seriAtt)
      {
        xmlOut << ";" << std::endl;
      }

      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttType = XMLString::transcode(gddAttribute->type()),
          *gddAttName = XMLString::transcode(gddAttribute->name());
        bool gddAttSerialize = gddAttribute->serialize(),
          gddAttCompression = gddAttribute->compression();

        if ((strcmp(gddAttType,"bool") == 0) && gddAttSerialize)
        {
          xmlOut << "  m_" << gddAttName << " = (l_" << gddAttName
                 << ") ? true : false;" << std::endl;
        }
        if (std::find(gddDefEnums.begin(), gddDefEnums.end(), gddAttType) !=
            gddDefEnums.end())
        {
          xmlOut << "  m_" << gddAttName << " = " << gddAttType << "(l_"
                 << gddAttName << ");" << std::endl;
        }
        if ((strcmp(gddAttType,"double") == 0) &&
            gddAttSerialize && gddAttCompression)
        {
          xmlOut << "  m_" << gddAttName << " = l_" << gddAttName << ";"
                 << std::endl;
        }
        XMLString::release(&gddAttType);
        XMLString::release(&gddAttName);
      }

      /*
        for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
        {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        std::string gddAttType = XMLString::transcode(gddAttribute->type()),
        gddAttName = XMLString::transcode(gddAttribute->name());
        bool gddAttSerialize = gddAttribute->serialize(),
        gddAttCompression = gddAttribute->compression();

        if ((strcmp(gddAttType,"double") == 0) &&
        gddAttSerialize && gddAttCompression)
        {
        xmlOut << "  m_" << gddAttName << " = l_" << gddAttName << ";"
        << std::endl;
        }
        }
      */

      xmlOut << "  return s;" << std::endl
             << "}" << std::endl
             << std::endl;
    }


    //
    // std::ostream<<
    //
    if (!ostreamOut)
    {
      /// function header
      xmlOut << "inline std::ostream& " << gddClassName
             << "::fillStream(std::ostream& s) const" << std::endl
             << "{" << std::endl;


      /// treating boolean values
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttType = XMLString::transcode(gddAttribute->type()),
          *gddAttName = XMLString::transcode(gddAttribute->name());

        if (strcmp(gddAttType,"bool") == 0)
        {
          xmlOut << "  char " << "l_" << gddAttName << " = (m_"
                 << gddAttName << ") ? 'T' : 'F';" << std::endl;
        }
        XMLString::release(&gddAttType);
        XMLString::release(&gddAttName);
      }

      for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
      {
        char *gddBaseName = XMLString::transcode(gddClass->popDaDiBaseClass()->
                                                 name());
        std::string sGddBaseName = std::string(gddBaseName);
        if (sGddBaseName != "ContainedObject" &&
            sGddBaseName != "DataObject" &&
            sGddBaseName.substr(0,sGddBaseName.find("<")) != "KeyedObject")
        {
          xmlOut << "  " << gddBaseName << "::fillStream(s);" << std::endl;
        }
        XMLString::release(&gddBaseName);
      }
      bool seriAtt = false;
      for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        char *gddAttName = XMLString::transcode(gddAttribute->name()),
          *gddAttType = XMLString::transcode(gddAttribute->type());
        bool gddAttCompression = gddAttribute->compression(),
          gddAttIsBitset = gddAttribute->bitset();

        if(i==0)
        {
          /*xmlOut << "  s << \"class "
            << XMLString::transcode(gddClass->className())
            << ":\""; */
          xmlOut << "  s << \"{ \"" << std::endl << "    << \" "
                 << gddAttName << ":\\t\" ";
          seriAtt = true;
        }
        else
        {
          xmlOut << " << std::endl" << std::endl << "    << \"   "
                 << gddAttName << ":\\t\" ";
        }
        if (strcmp(gddAttType,"bool") == 0)
        {
          xmlOut << "<< l_";
        }
        else if ((strcmp(gddAttType,"double") == 0) && gddAttCompression)
        {
          xmlOut << "<< (float)m_";
        }
        else
        {
          xmlOut << "<< m_";
        }
        xmlOut << gddAttName;

        if (gddAttIsBitset)
        {
          for (j=0; j<gddAttribute->sizeDaDiBitfield(); ++j)
          {
            DaDiBitfield* gddBitfield = gddAttribute->popDaDiBitfield();
            char *gddBfName = XMLString::transcode(gddBitfield->name());
            XMLCh* trueString = XMLString::transcode("TRUE");
            bool gddBfGetMeth = (XMLString::compareString
                                 (gddBitfield->getMeth(), trueString) == 0);
            XMLString::release(&trueString);

            if (gddBfGetMeth)
            {
              xmlOut << " << std::endl" << std::endl
                     << "    << \"   " << gddBfName << ":\\t\" << "
                     << gddBfName << "()";
            }
            XMLString::release(&gddBfName);
          }
        }
        XMLString::release(&gddAttName);
        XMLString::release(&gddAttType);
      }
      if (seriAtt)
      {
        xmlOut << " << \" } \";" << std::endl;
      }
      xmlOut << "  return s;" << std::endl
             << "}" << std::endl
             << std::endl;
    }


    /*
    //
    // Serializers & fillStream
    //
    if (isEventClass)
    {
    xmlOut << "inline StreamBuffer& "
    << XMLString::transcode(gddClass->className())
    << "::serialize( StreamBuffer& s) const" << std::endl << "{" << std::endl
    << "  return s << *this;" << std::endl << "}" << std::endl << std::endl;

    xmlOut << "inline StreamBuffer& "
    << XMLString::transcode(gddClass->className())
    << "::serialize(StreamBuffer& s)" << std::endl << "{" << std::endl
    << " return s >> *this;" << std::endl << "}" << std::endl << std::endl;

    xmlOut << "inline std::ostream& "
    << XMLString::transcode(gddClass->className())
    << "::fillStream(std::ostream& s) const" << std::endl << "{" << std::endl
    << "  return s << *this;" << std::endl << "}" << std::endl << std::endl;
    }
    */
  }


  //
  // Definition of container types
  //
  if (classTemplate)
  {
    xmlOut << "//Defintion of keyed container for " << gddClassName << std::endl
      //      << "namespace Containers { class HashMap; };" << std::endl
      //      << "template <class TYPE, class MAPPING> class KeyedContainer;"
      //      << std::endl
           << "typedef KeyedContainer<" << gddClassName
           << ", Containers::HashMap> "
           << printPlural(gddClassName) << ";" << std::endl;
  }

  if (gddClassStdVectorTypeDef)
  {
    xmlOut << "// typedef for std::vector of " << gddClassName << std::endl
           << "typedef std::vector<" << gddClassName << "*> " << gddClassName
           << "Vector;" << std::endl;
  }


  /////////////////////////////// tbd - to be deleted
  //
  // Definition of container types
  //
  if (classTemplateVector)
  {
    xmlOut << "// Defintion of vector container type for " << gddClassName
           << std::endl
           << "template <class TYPE> class ObjectVector;" << std::endl
           << "typedef ObjectVector<" << gddClassName << "> " << gddClassName
           << "Vector;" << std::endl;
  }

  if (classTemplateList)
  {
    xmlOut << "// Defintion of all list container types for " << gddClassName
           << std::endl
           << "template <class TYPE> class ObjectList;" << std::endl
           << "typedef ObjectList<" << gddClassName << "> " << gddClassName
           << "List;" << std::endl;
  }
  /////////////////////////////////////
  gddDefEnums.clear();
  gddDefTypeDefs.clear();

  XMLString::release(&gddClassName);
  XMLString::release(&gddClassID);
  XMLString::release(&gddClassLocation);
  XMLString::release(&gddPackName);
  XMLString::release(&gddClassDesc);
  XMLString::release(&gddClassLongDesc);
  XMLString::release(&gddClassAuthor);

}


//-----------------------------------------------------------------------------
void printNamespace(std::ofstream& xmlOut,
                    DaDiNamespace* gddNamespace,
                    DaDiPackage* gddPackage,
                    std::map<std::string,std::string> dbExportClass)
//-----------------------------------------------------------------------------
{
  time_t ltime;
  int i;
  char *gddNamespName = XMLString::transcode(gddNamespace->name()),
    *gddNamespAuthor = XMLString::transcode(gddNamespace->author()),
    *gddNamespDesc = XMLString::transcode(gddNamespace->desc()),
    *gddNamespLongDesc = XMLString::transcode(gddNamespace->longDesc());

  gddDefEnums.clear();
  gddDefTypeDefs.clear();

  for (i=0; i<gddNamespace->sizeDaDiEnum(); ++i)
  {
    char *tmpEnum = XMLString::transcode(gddNamespace->popDaDiEnum()->name());
    gddDefEnums.push_back(tmpEnum);
    XMLString::release(&tmpEnum);
  }
  for (i=0; i<gddNamespace->sizeDaDiTypeDef(); ++i)
  {
    char *tmpTypeDef = XMLString::transcode(gddNamespace->popDaDiTypeDef()->
                                            def());
    gddDefTypeDefs.push_back(tmpTypeDef);
    XMLString::release(&tmpTypeDef);
  }

  time(&ltime);

  xmlOut << std::endl
         << "/** @namespace " << gddNamespName << std::endl
         << " *" << std::endl
         << " *  " << gddNamespDesc << std::endl
         << " *" << std::endl;

  if (!DaDiTools::isEmpty(gddNamespLongDesc))
  {
    xmlOut << " *  " << gddNamespLongDesc << std::endl;
  }

  xmlOut << " *" << std::endl
         << " *" << std::endl
         << " * @author " << gddNamespAuthor << std::endl
         << " * created " << ctime(&ltime)
         << " *" << std::endl
         << " */" << std::endl
         << std::endl
         << "namespace " << gddNamespName << std::endl
         << "{" << std::endl;

  // print typedefs
  printTypeDefs(xmlOut, gddNamespace, "");

  // print enums
  printEnums(xmlOut, gddNamespace, "");

  xmlOut << std::endl;

  for (i=0; i<gddNamespace->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddNamespace->popDaDiAttribute();
    char *gddAttType = XMLString::transcode(gddAttribute->type()),
      *gddAttName = XMLString::transcode(gddAttribute->name()),
      *gddAttDesc = XMLString::transcode(gddAttribute->desc()),
      *gddAttInit = XMLString::transcode(gddAttribute->init());

    xmlOut << "  /// " << gddAttDesc << std::endl
           << "  " << gddAttType << " " << gddAttName;
    if (!DaDiTools::isEmpty(gddAttInit))
    {
      xmlOut << " = " << gddAttInit;
    }
    xmlOut << ";" << std::endl
           << std::endl;
    XMLString::release(&gddAttType);
    XMLString::release(&gddAttName);
    XMLString::release(&gddAttDesc);
    XMLString::release(&gddAttInit);
  }

  printMethodDecl(xmlOut, gddNamespace, "");

  for (i=0; i<gddNamespace->sizeDaDiClass(); ++i)
  {
    xmlOut << std::endl;
    DaDiClass* gddClass = gddNamespace->popDaDiClass();
    printClass(xmlOut,gddClass,gddPackage,dbExportClass,false);
  }

  xmlOut << "};" << std::endl
         << std::endl;

  printMethodImpl(xmlOut, gddNamespace, "");

  gddDefEnums.clear();
  gddDefTypeDefs.clear();

  XMLString::release(&gddNamespName);
  XMLString::release(&gddNamespAuthor);
  XMLString::release(&gddNamespDesc);
  XMLString::release(&gddNamespLongDesc);
}


//-----------------------------------------------------------------------------
void printCppHeader(DaDiPackage* gddPackage,
                    char* envXmlDB,
                    char* envOut,
                    bool additionalImports,
                    bool dictionaryHeader)
//-----------------------------------------------------------------------------
{
  int i=0, j=0, k=0;
  std::list<std::string>::iterator iIter;
  std::map<std::string,std::string> dbExportClass;

  ContainedObjectClasses.push_back("ContainedObject");
  KeyedObjectClasses.push_back("KeyedObject");

  char *gddPackName = XMLString::transcode(gddPackage->packageName());

  //
  // Parser initialization
  //

  XercesDOMParser *dbParser = new XercesDOMParser;
  dbParser->setValidationScheme(XercesDOMParser::Val_Auto);
  dbParser->setDoNamespaces(false);
  dbParser->setDoSchema(false);
  dbParser->setCreateEntityReferenceNodes(false);
  dbParser->setValidationSchemaFullChecking(false);
  //  dbParser->setToCreateXMLDeclTypeNode(true);

  //  ErrorHandler* errReporter = new DaDiTools();
  DaDiTools* daditools = new DaDiTools();
  ErrorHandler* errReporter = dynamic_cast<ErrorHandler*>(daditools);
  delete daditools;
  dbParser->setErrorHandler(errReporter);

  bool dbErrorsOccured = false;

  //
  // Parse the 'Headerdatabase'
  //

  try
  {
    dbParser->parse(envXmlDB);
  }

  catch(const XMLException& e)
  {
    std::cerr << argV0 << ": An error occured during parsing \n Message: "
              << e.getMessage() << std::endl;
    dbErrorsOccured = true;
  }

  catch(const DOMException& e)
  {
    char *msg = XMLString::transcode(e.msg);
    std::cerr << argV0 << ": An error occured during parsing \n Message: "
              << msg << std::endl;
    XMLString::release(&msg);
    dbErrorsOccured = true;
  }

  catch(...)
  {
    std::cerr << argV0 << ": An error occured during parsing " << std::endl;
    dbErrorsOccured = true;
  }


  if (dbErrorsOccured)
  {
    char questCont;
    std::cerr << argV0 << ": Continue with Headerfileproduction? (y/n) ";
    std::cin >> questCont;
    if (toupper(questCont) == 'N')
    {
      std::cerr << argV0 << ": Production of Cpp-Headerfiles aborted!!"
                << std::endl;
      exit(1);
    }
  }

  //
  // Produce map of classes and files (for later use in #include-statements)
  //
  else
  {
    DOMNode* dbDoc = dbParser->getDocument();
    DOMNode* dbTop = dbDoc->getFirstChild();

    XMLCh* cmpStr = XMLString::transcode("gdd");
    while(XMLString::compareString(dbTop->getNodeName(), cmpStr) != 0)
    {
      dbTop = dbTop->getNextSibling();
    }
    XMLString::release(&cmpStr);

    DOMNode* dbPackNode = dbTop->getNextSibling()->getFirstChild();

    while (dbPackNode != 0)
    {
      cmpStr = XMLString::transcode("package");
      if (XMLString::compareString(dbPackNode->getNodeName(), cmpStr) == 0)
      {
        DOMNode* dbClassNode = dbPackNode->getFirstChild();
        XMLCh* itemStr = XMLString::transcode("name");
        char *cdbPackName = XMLString::transcode(dbPackNode->getAttributes()->
                                                 getNamedItem(itemStr)->
                                                 getNodeValue());
        std::string dbPackName = cdbPackName;
        XMLString::release(&cdbPackName);
        XMLString::release(&itemStr);
        while(dbClassNode != 0)
        {
          XMLCh* cmpStr2 = XMLString::transcode("class");
          if (XMLString::compareString
              (dbClassNode->getNodeName(), cmpStr2) == 0)
          {
            std::string dbClassName, dbFileName;
            itemStr = XMLString::transcode("name");
            char* cDbClassName = XMLString::transcode(dbClassNode->
                                                      getAttributes()->
                                                      getNamedItem(itemStr)->
                                                      getNodeValue());
            dbClassName = std::string(cDbClassName);
            XMLString::release(&cDbClassName);
            XMLString::release(&itemStr);
            itemStr = XMLString::transcode("filename");
            if (dbClassNode->getAttributes()->getNamedItem(itemStr) != 0)
            {
              //XMLCh* itemStr2 = XMLString::transcode("filename");
              char* cDbFileName = XMLString::transcode(dbClassNode->
                                                       getAttributes()->
                                                       getNamedItem(itemStr)->
                                                       getNodeValue());
              dbFileName = std::string(cDbFileName);
              XMLString::release(&cDbFileName);
              dbExportClass[dbClassName] = dbPackName+"/"+dbFileName;
              //XMLString::release(&itemStr2);
            }
            else
            {
              dbExportClass[dbClassName] = dbPackName+"/"+dbClassName;
            }
            XMLString::release(&itemStr);
          }
          XMLString::release(&cmpStr2);
          dbClassNode = dbClassNode->getNextSibling();
        }
        //XMLString::release(&cmpStr);??
      }
      XMLString::release(&cmpStr);
      dbPackNode = dbPackNode->getNextSibling();
    }
  }

  delete dbParser;
  //XMLString::release(&dbErrReporter);

  ///
  /// additional Imports
  ///
  if (additionalImports)
  {
    char* addImportsFile = new char[255];
    strcpy(addImportsFile,"AddImports.txt");
    std::ifstream addXml(addImportsFile);
    std::string aline, first, second;

    if (!addXml)
    {
      std::cerr << argV0 << ": Cannot open " << addImportsFile << std::endl;
    }
    else
    {
      while(!addXml.eof())
      {
        addXml >> first;
        addXml >> second;
        std::getline(addXml,aline);
        dbExportClass[first] = second+"/"+first;
      }
    }

    addXml.close();
    delete [] addImportsFile;
  }


  std::vector<std::string> addImp = DaDiTools::addImport();
  std::vector<std::string>::iterator viter;

  for (viter = addImp.begin(); viter != addImp.end(); ++viter)
  {
    std::string::size_type posS = (*viter).find_last_of("/");
    dbExportClass[(*viter).substr(posS+1,std::string::npos)] = (*viter);
  }


  gddPackage->pushImportList("CLHEPStreams");


  for(k=0; k<gddPackage->sizeDaDiNamespace(); ++k)
  {
    DaDiNamespace* gddNamespace = gddPackage->popDaDiNamespace();
    char *gddNamespName = XMLString::transcode(gddNamespace->name());

    dbExportClass[gddNamespName] = "Event/" + std::string(gddNamespName);

    std::vector<std::string> noImports;
    noImports.clear();

    for (i=0; i<gddNamespace->sizeNoImports(); ++i)
    {
      noImports.push_back(gddNamespace->popNoImports());
    }
    for (i=0; i<gddPackage->sizeNoImports(); ++i)
    {
      noImports.push_back(gddNamespace->popNoImports());
    }

    char* fileName = new char[256];
    strcpy(fileName, envOut);
    char* tmpNamespName = XMLString::transcode(gddNamespace->name());
    strcat(fileName, tmpNamespName);
    XMLString::release(&tmpNamespName);
    strcat(fileName, ".h");
    std::cout << "Writing " << fileName;
    std::ofstream xmlOut(fileName);

    xmlOut << std::endl
           << std::endl
           << "//   ***********************************************************"
           << "***************" << std::endl
           << "//   *                                                          "
           << "              *" << std::endl
           << "//   *                      ! ! ! A T T E N T I O N ! ! !       "
           << "              *" << std::endl
           << "//   *                                                          "
           << "              *" << std::endl
           << "//   *  This file was created automatically by GaudiObjDesc, ple"
           << "ase do not    *" << std::endl
           << "//   *  delete it or edit it by hand.                           "
           << "              *" << std::endl
           << "//   *                                                          "
           << "              *" << std::endl
           << "//   *  If you want to change this file, first change the corres"
           << "ponding       *" << std::endl
           << "//   *  xml-file and rerun the tools from GaudiObjDesc (or run m"
           << "ake if you    *" << std::endl
           << "//   *  are using it from inside a Gaudi-package).              "
           << "              *" << std::endl
           << "//   *                                                          "
           << "              *" << std::endl
           << "//   ***********************************************************"
           << "***************" << std::endl
           << std::endl
           << std::endl
           << std::endl;


    //
    // IFNDEF
    //
    xmlOut << "#ifndef " << gddPackName << "_" << gddNamespName << "_H"
           << std::endl
           << "#define " << gddPackName << "_" << gddNamespName << "_H 1"
           << std::endl
           << std::endl;

    //
    // Includes
    //

    std::list<std::string> stdImports, normImports, softImports, tmpList;

    xmlOut << "// Include files" << std::endl;

    stdImports = gddPackage->impStdList();
    normImports = gddPackage->importList();
    softImports = gddPackage->impSoftList();
    tmpList = gddNamespace->impStdList();
    stdImports.merge(tmpList);
    tmpList = gddNamespace->importList();
    normImports.merge(tmpList);
    tmpList = gddNamespace->impSoftList();
    softImports.merge(tmpList);
    for (j=0; j<gddNamespace->sizeDaDiClass(); ++j)
    {
      DaDiClass* gddClass = gddNamespace->popDaDiClass();
      tmpList = gddClass->impStdList();
      stdImports.merge(tmpList);
      tmpList = gddClass->importList();
      normImports.merge(tmpList);
      tmpList = gddClass->impSoftList();
      softImports.merge(tmpList);
    }
    stdImports.sort();
    normImports.sort();
    softImports.sort();
    stdImports.unique();
    normImports.unique();
    softImports.unique();


    for(iIter = stdImports.begin(); iIter != stdImports.end(); ++iIter)
    {
      xmlOut << "#include <" << *iIter << ">" << std::endl;
    }


    for(iIter = normImports.begin(); iIter != normImports.end(); ++iIter)
    {
      std::string impName = *iIter;
      if (std::find(noImports.begin(), noImports.end(), impName)
          == noImports.end())
      {
        if(dbExportClass[impName] != "")
        {
          impName = dbExportClass[impName];
        }
        else
        {
          std::cerr << std::endl
                    << argV0 << ": No information found for type: " << impName
                    << std::endl
                    << argV0 << ": Line written: #include \"" << impName
                    << ".h\"" << std::endl;
        }
        xmlOut << "#include \"" << impName << ".h\"" << std::endl;
      }
    }

    //
    // Forward declarations
    //
    if (softImports.size())
    {
      xmlOut << std::endl
             << std::endl
             << "// Forward declarations" << std::endl;
      for(iIter = softImports.begin(); iIter != softImports.end(); ++iIter)
      {
        xmlOut << "class " << *iIter << ";" << std::endl;
      }
    }

    xmlOut << std::endl
           << std::endl;

    printNamespace(xmlOut,gddNamespace,gddPackage,dbExportClass);

    //
    // End of .h-File
    //
    xmlOut << std::endl;

    xmlOut << "#endif   ///" << gddPackName << "_" << gddNamespName << "_H"
           << std::endl;

    xmlOut.close();
    delete [] fileName;

    std::cout << " - OK" << std::endl;

    XMLString::release(&gddNamespName);
  }


  for (k=0; k<gddPackage->sizeDaDiClass(); ++k)
  {

    DaDiClass* gddClass = gddPackage->popDaDiClass();
    char *gddClassName = XMLString::transcode(gddClass->name());

    std::vector<std::string> noImports;
    noImports.clear();

    for (i=0; i<gddClass->sizeNoImports(); ++i)
    {
      noImports.push_back(gddClass->popNoImports());
    }
    for (i=0; i<gddPackage->sizeNoImports(); ++i)
    {
      noImports.push_back(gddClass->popNoImports());
    }

    /* XMLString::transcode(dbExportClass[std::string(gddClass->className())])=
       std::string(XMLString::transcode(gddPackage->packageName())) + "/"
       + std::string(XMLString::transcode(gddClass->className())); */

    dbExportClass[gddClassName] = "Event/" + std::string(gddClassName);

    // ------------------------------------------------------------------------
    // Here starts the writing of the .h-file


    char* fileName = new char[256];
    strcpy(fileName, envOut);
    char* tmpClName = XMLString::transcode(gddClass->name());
    strcat(fileName, tmpClName);
    XMLString::release(&tmpClName);
    if (dictionaryHeader)
    {
      strcat(fileName, "_dict.h");
    }
    else
    {
      strcat(fileName, ".h");
    }
    std::cout << "Writing " << fileName;
    std::ofstream xmlOut(fileName);


    //
    // 'donotedit'-message
    //

    xmlOut << std::endl
           << std::endl
           << "//   ***********************************************************"
           << "***************" << std::endl
           << "//   *                                                          "
           << "              *" << std::endl
           << "//   *                      ! ! ! A T T E N T I O N ! ! !       "
           << "              *" << std::endl
           << "//   *                                                          "
           << "              *" << std::endl
           << "//   *  This file was created automatically by GaudiObjDesc, ple"
           << "ase do not    *" << std::endl
           << "//   *  delete it or edit it by hand.                           "
           << "              *" << std::endl
           << "//   *                                                          "
           << "              *" << std::endl
           << "//   *  If you want to change this file, first change the corres"
           << "ponding       *" << std::endl
           << "//   *  xml-file and rerun the tools from GaudiObjDesc (or run m"
           << "ake if you    *" << std::endl
           << "//   *  are using it from inside a Gaudi-package).              "
           << "              *" << std::endl
           << "//   *                                                          "
           << "              *" << std::endl
           << "//   ***********************************************************"
           << "***************" << std::endl
           << std::endl
           << std::endl
           << std::endl;


    //
    // IFNDEF
    //
    xmlOut << "#ifndef " << gddPackName << "_" << gddClassName << "_H"
           << std::endl
           << "#define " << gddPackName << "_" << gddClassName << "_H 1" \
           << std::endl << std::endl;

    //
    // Includes
    //
    gddClass->pushImportList("StreamBuffer");


    std::list<std::string> stdImports, normImports, softImports, tmpList;

    xmlOut << "// Include files" << std::endl;

    stdImports = gddPackage->impStdList();
    tmpList = gddClass->impStdList();
    stdImports.merge(tmpList);
    stdImports.sort();
    stdImports.unique();

    for (iIter = stdImports.begin(); iIter != stdImports.end(); ++iIter)
    {
      xmlOut << "#include <" << *iIter << ">" << std::endl;
    }

    normImports = gddPackage->importList();
    tmpList = gddClass->importList();
    normImports.merge(tmpList);
    normImports.sort();
    normImports.unique();

    for (iIter = normImports.begin(); iIter != normImports.end(); ++iIter)
    {
      std::string impName = *iIter;
      if (std::find(noImports.begin(), noImports.end(), impName) ==
          noImports.end())
      {
        if(dbExportClass[impName] != "")
        {
          impName = dbExportClass[impName];
        }
        else
        {
          std::cerr << std::endl
                    << argV0 << ": No information found for type: " << impName
                    << std::endl
                    << argV0 << ": Line written: #include \"" << impName
                    << ".h\"" << std::endl;
        }
        xmlOut << "#include \"" << impName << ".h\"" << std::endl;
      }
    }

    //
    // Forward declarations
    //
    softImports = gddPackage->impSoftList();
    tmpList = gddClass->impSoftList();
    softImports.merge(tmpList);
    softImports.sort();
    softImports.unique();

    if (softImports.size())
    {
      xmlOut << std::endl
             << std::endl
             << "// Forward declarations" << std::endl;
      for(iIter = softImports.begin(); iIter != softImports.end(); ++iIter)
      {
        xmlOut << "class " << *iIter << ";" << std::endl;
      }
    }

    xmlOut << std::endl
           << std::endl;

    printClass(xmlOut, gddClass, gddPackage, dbExportClass, dictionaryHeader);


    //
    // End of .h-File
    //
    xmlOut << std::endl;

    xmlOut << "#endif   ///" << gddPackName << "_" << gddClassName << "_H"
           << std::endl;

    xmlOut.close();
    delete [] fileName;

    std::cout << " - OK" << std::endl;

    XMLString::release(&gddClassName);
  }

  XMLString::release(&gddPackName);
}


//-----------------------------------------------------------------------------
int main(int argC,
         char* argV[])
//-----------------------------------------------------------------------------
{

  char* sep = new char[2];
#ifdef _WIN32
  strcpy(sep,"\\");
#elif defined(__linux) || defined(__APPLE__)
  strcpy(sep,"/");
#endif

  std::vector<char*> files;
  char* envOut = new char[256];
  char* envXmlDB = new char[256];
  std::string nextArg;
  bool additionalImports = false, dictionaryHeader = false;

  strcpy(envOut, "");

  argV0 = std::string(argV[0]);
  argV0.erase(0,argV0.find_last_of("\\")+1);
  argV0.erase(0,argV0.find_last_of("/")+1);

  if (getenv("GAUDIOBJDESCROOT") == NULL)
  {
    // std::cerr << argV0
    // << ": Please set Environmentvariable 'GAUDIOBJDESCROOT'"
    // << std::endl;
    // exit(1);
    strcpy(envXmlDB,"");
  }
  else
  {
    strcpy(envXmlDB, getenv("GAUDIOBJDESCROOT"));
    strcat(envXmlDB, sep);
    strcat(envXmlDB, "xml_files");
    strcat(envXmlDB, sep);
  }


  if (argC==1) { usage(argV0); }
  else
  {
    for (int i=1; i<argC; ++i)
    {
      if (strcmp(argV[i],"-o") == 0)
      {
        nextArg = std::string(argV[i+1]);
        if (((argC-1) == i) || (strcmp(argV[i+1],"-x") == 0) ||
            (nextArg.find_last_of(sep) != (nextArg.length()-1)))
        {
          if (getenv("GODDOTHOUT") != NULL)
          {
            strcpy(envOut,getenv("GODDOTHOUT"));
          }
          else
          {
            std::cerr << argV0
                      << ": Environment variable 'GODDOTHOUT' not set, "
                      << "using local directory" << std::endl;
          }
        }
        else
        {
          strcpy(envOut,argV[i+1]);
          ++i;
        }
      }
      else if (strcmp(argV[i], "-x") == 0)
      {
        nextArg = std::string(argV[i+1]);
        if (((argC-1) == i) || (strcmp(argV[i+1], "-o") == 0) ||
            (nextArg.find_last_of(sep) != (nextArg.length()-1)))
        {
          if (getenv("GODXMLDB") != NULL)
          {
            strcpy(envXmlDB,getenv("GODXMLDB"));
          }
          else
          {
            std::cerr << argV0 << ": Environment variable 'GODXMLDB' not set, "
                      << "using '$(GAUDIOBJDESCROOT)/xml_files'"
                      << std::endl;
          }
        }
        else
        {
          strcpy(envXmlDB,argV[i+1]);
          ++i;
        }
      }
      else if (strcmp(argV[i], "-d") == 0)
      {
        dictionaryHeader = true;
      }
      else if (strcmp(argV[i], "-i") == 0)
      {
        additionalImports = true;
      }
      else if (strcmp(argV[i], "-h") == 0)
      {
        usage(argV0);
      }
      else if (strcmp(argV[i], "-v") == 0)
      {
        version(argV0);
      }
      else
      {
        files.push_back(argV[i]);
      }
    }
  }

  strcat(envXmlDB,"GaudiCppExport.xml");

  delete [] sep;

  for (std::vector<char*>::iterator iter = files.begin();
       iter != files.end(); ++iter)
  {
    DaDiPackage* gddPackage = new DaDiPackage();

    DDFE::DaDiFrontEnd(gddPackage, *iter);

    printCppHeader(gddPackage,
                   envXmlDB,
                   envOut,
                   additionalImports,
                   dictionaryHeader);

    delete gddPackage;
  }

  delete [] envXmlDB;
  delete [] envOut;

  return 0;
}


