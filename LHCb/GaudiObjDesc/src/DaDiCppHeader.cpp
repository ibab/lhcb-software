// $Id: DaDiCppHeader.cpp,v 1.63 2002-05-14 08:27:48 mato Exp $

//#include "GaudiKernel/Kernel.h"

#include "DaDiTools.h"
#include "DaDiCppHeader.h"
#include "DaDiPackage.h"
#include "parsers/DOMParser.hpp"
#include "dom/DOM_DOMException.hpp"
#include "dom/DOM_NodeList.hpp"
#include "dom/DOM_Element.hpp"
#include "dom/DOM_NamedNodeMap.hpp"
#include "dom/DOMString.hpp"

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

std::vector<std::string> ContainedObjectClasses, KeyedObjectClasses, 
                         gddDefEnums, gddDefTypeDefs;

extern std::string xmlVersion;

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
    << "Usage: " << argV0 << " [-h] [-v] [-i] [-o [path]] [-x [path]] xml-file(s)"        << std::endl
    << "Produce .h-files out of xml-files"                                   << std::endl << std::endl
    << "   -h            display this help and exit"                                      << std::endl
    << "   -v            display version information and exit"                            << std::endl
    << "   -i            add additional file-package-information from './AddImports.txt'" << std::endl
    << "   -o [path]     define possible outputdestination with following precedence"     << std::endl
    << "                    -o path     use 'path'"                                       << std::endl
    << "                    -o          use environment-variable 'GODDOTHOUT'"            << std::endl
    << "                     default    use local directory"                              << std::endl
    << "   -x [path]     define location of 'GaudiCppExport.xml' which holds information" << std::endl
    << "                 about include-file<->package dependencies, with this precedence" << std::endl
    << "                    -x path     use 'path'"                                       << std::endl
    << "                    -x          use environment-variable 'GODXMLDB'"              << std::endl
    << "                     default    use '$(GAUDIOBJDESCROOT)/xml_files'"              << std::endl
    << "   xml-file(s)   xml-file(s) to be parsed (must have extension '.xml')"           << std::endl
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
    std::string gddArgType = gddArg->type().transcode(),
                gddArgInout = gddArg->inout().transcode(),
                gddArgName = gddArg->name().transcode();

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

    if (gddArgInout == "BOTH" || 
        (!DaDiTools::isFundamental(gddArgType) && !gddArgIsPointer) )
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
    std::string gddMethodAccess = gddMethod->access().transcode(),
                gddMethodTemplate = gddMethod->template_().transcode(),
                gddMethodDesc = gddMethod->desc().transcode(),
                gddMethodVirtual = gddMethod->virtual_().transcode(),
                gddMethodName = gddMethod->name().transcode(),
                gddMethodCode = gddMethod->code().transcode(),
                gddMethReturnType = gddMethod->daDiMethReturn()->type().transcode();
    bool gddMethodIsStatic = gddMethod->static_(),
         gddMethodIsFriend = gddMethod->friend_(),
         gddMethodIsConst = gddMethod->const_(),
         gddMethReturnIsConst = gddMethod->daDiMethReturn()->const_();
      
    if (gddMethodAccess == accessor || accessor == "")
    {
      xmlOut << "  /// " << gddMethodDesc << std::endl << "  ";
      if (!DaDiTools::isEmpty(gddMethodTemplate))
      {
        xmlOut << "template<" << gddMethodTemplate << "> ";
      }
      if ((accessor == "" ) && (gddMethodCode != ""))
      {
        xmlOut << "inline ";
      }
      if (gddMethodIsFriend)
      {
        xmlOut << "friend ";
      }
      if (gddMethodVirtual == "TRUE" ||
          gddMethodVirtual == "PURE")
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
    
      printArguments(xmlOut, gddMethod, gddElement->name().transcode());    
    
      xmlOut << ")";

      if (gddMethodIsConst)
      {
        xmlOut << " const";
      }
      if (gddMethodVirtual == "PURE")
      {
        xmlOut << " = 0";
      }
/*      if (gddMethodIsFriend)
      {
        xmlOut << std::endl 
          << "  {" << std::endl 
          << gddMethodCode << std::endl 
          << "  }" << std::endl 
          << std::endl;
      }
      else
      { */
        xmlOut << ";" << std::endl 
          << std::endl;
//      }
    }
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
    std::string gddMethodName = gddMethod->name().transcode(),
                gddMethodAccess = gddMethod->access().transcode(),
                gddMethodArgList = gddMethod->argList().transcode(),
                gddMethodCode = gddMethod->code().transcode(),
                gddMethodVirtual = gddMethod->virtual_().transcode(),
                gddMethReturnType = gddMethod->daDiMethReturn()->type().transcode(),
                gddClassName = gddElement->name().transcode();

    bool gddMethodIsConst = gddMethod->const_(),
         gddMethodIsStatic = gddMethod->static_(),
         gddMethodIsFriend = gddMethod->friend_(),
         gddMethReturnIsConst = gddMethod->daDiMethReturn()->const_(),
         gddMethodIsPureVirtual = (gddMethodVirtual == "PURE") ? true : false,
         gddMethodIsTemplated = (gddMethod->template_().length() == 0) ? false : true;


    if ((gddMethodAccess == accessor || accessor == "")
        && gddMethodCode != "" && !gddMethodIsPureVirtual
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
    std::string gddAttGetMeth = gddAttribute->getMeth().transcode(),
                gddAttSetMeth = gddAttribute->setMeth().transcode(),
                gddAttDesc = gddAttribute->desc().transcode(),
                gddAttType = gddAttribute->type().transcode(),
                gddAttName = gddAttribute->name().transcode();
    bool gddAttIsBitset = gddAttribute->bitset();

    if(gddAttGetMeth == accessor)
    {
      xmlOut << "  /// Retrieve " << gddAttDesc << std::endl << "  ";
      if (!DaDiTools::isFundamental(gddAttType) || DaDiTools::isPointer(gddAttType))
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
      
    if(gddAttSetMeth == accessor)
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
        std::string gddBfName = gddBitfield->name().transcode(),
                    gddBfDesc = gddBitfield->desc().transcode(),
                    gddBfSetMeth = gddBitfield->setMeth().transcode(),
                    gddBfGetMeth = gddBitfield->getMeth().transcode();
         
        if (gddBfSetMeth == accessor)
        {
          xmlOut << "  /// Update " << gddBfDesc << std::endl
              << "  void set" << DaDiTools::firstUp(gddBfName) 
              << "(unsigned long value);" << std::endl
            << std::endl;
        }
        if (gddBfGetMeth == accessor)
        {
          xmlOut << "  /// Retrieve " << gddBfDesc << std::endl
              << "  unsigned long " << gddBfName << "() const;" << std::endl
            << std::endl;
        }
      }
    }
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
    std::string gddAttGetMeth = gddAttribute->getMeth().transcode(),
                gddAttSetMeth = gddAttribute->setMeth().transcode(),
                gddAttType = gddAttribute->type().transcode(),
                gddAttName = gddAttribute->name().transcode(),
                gddClassName = gddClass->name().transcode();
    bool gddAttIsBitset = gddAttribute->bitset();
      
    if(gddAttGetMeth == accessor)
    {
      xmlOut << "inline ";
      if (!DaDiTools::isFundamental(gddAttType) || DaDiTools::isPointer(gddAttType))
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

      if (!DaDiTools::isFundamental(gddAttType) || DaDiTools::isPointer(gddAttType))
      {
        xmlOut << "inline " << checkClassPrefix(gddAttType, gddClassName);
        if (!DaDiTools::isPointer(gddAttType))
        {
          xmlOut << "&";
        }
        xmlOut << " " << gddClassName << "::" << DaDiTools::retGetName(gddAttName) << "()" 
            << std::endl
          << "{" << std::endl
          << "  return m_" << gddAttName << ";" << std::endl
          << "}" << std::endl
          << std::endl;
      }
    }
      
    if(gddAttSetMeth == accessor)
    {
      xmlOut << "inline void " << gddClassName << "::set" << DaDiTools::firstUp(gddAttName) << "(";
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

    if (gddAttIsBitset)
    {
      std::vector<std::string> bFields;
      std::vector<std::string>::iterator bfIter;
      for (j=0; j<gddAttribute->sizeDaDiBitfield(); ++j)
      {
        DaDiBitfield* gddBitfield = gddAttribute->popDaDiBitfield();
        std::string gddBfName = gddBitfield->name().transcode(),
                    gddBfSetMeth = gddBitfield->setMeth().transcode(),
                    gddBfGetMeth = gddBitfield->getMeth().transcode(),
                    gddShiftBits = "";
        int gddBfLength = gddBitfield->length();
        bool gddBfStartAtOne = gddBitfield->startAtOne();

        std::vector<std::string> gddBfMasks = gddBitfield->mask();
        bFields.push_back(gddBfName);

        if (gddBfLength == 0)
        {
          for (bfIter = bFields.begin(); bfIter != bFields.end(); ++bfIter)
          {
            if (std::find(gddBfMasks.begin(), gddBfMasks.end(), *bfIter) != gddBfMasks.end())
            {
              gddShiftBits = *bfIter;
              break;
            }
          }
          if (gddShiftBits == "")
          {
            std::cerr << "GOD says: Could not find a sub-mask (tbd)" << std::endl;
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
            << DaDiTools::firstUp(gddBfName) << "(unsigned long value)" << std::endl
            << "{" << std::endl
            << "  m_" << gddAttName << " &= ~" << gddBfName << "Mask;" << std::endl;
/*            << "  m_" << gddAttName << " &= (";

          bool firstElem = false;
          for (k=0; k<gddAttribute->sizeDaDiBitfield(); ++k)
          {
            DaDiBitfield* tmpBitfield = gddAttribute->popDaDiBitfield();
            std::string tmpBfName = tmpBitfield->name().transcode();
            int tmpBfLength = tmpBitfield->length();
            if ( tmpBfLength && (
                 (gddBfLength && tmpBfName != gddBfName) ||
                 (!gddBfLength && 
                 std::find(gddBfMasks.begin(), gddBfMasks.end(), tmpBfName) == gddBfMasks.end())))
            {
              if (firstElem) { xmlOut << " | "; }
              firstElem = true;
              xmlOut << tmpBfName << "Mask";
            }
          }          
          xmlOut << ");" << std::endl;*/

          xmlOut << "  m_" << gddAttName << " |= ((";
          if (gddBfStartAtOne)
          {
            xmlOut << "(";
          }
          xmlOut << "value";
          if (gddBfStartAtOne)
          {
            xmlOut << " - 1)";
          }
          xmlOut << " << " << gddShiftBits 
            << "Bits) & " << gddBfName << "Mask);" << std::endl; 
          
          xmlOut << "}" << std::endl
            << std::endl;
        }
        if (gddBfGetMeth == accessor)
        {
          xmlOut << "inline unsigned long " << gddClassName << "::" << gddBfName 
              << "() const" << std::endl
            << "{" << std::endl
            << "  return ((m_" << gddAttName << " & " << gddBfName << "Mask) >> "
              << gddShiftBits << "Bits)";
          if (gddBfStartAtOne)
          {
            xmlOut << " + 1";
          }   
          xmlOut << ";" << std::endl
            << "}" << std::endl
            << std::endl;
        }
      }
    }
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
    const char *get_ret, *set_arg, *add_arg;      
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    std::string gddRelRatio = gddRelation->ratio().transcode(),
                gddRelGetMeth = gddRelation->getMeth().transcode(),
                gddRelSetMeth = gddRelation->setMeth().transcode(),
                gddRelAddMeth = gddRelation->addMeth().transcode(),
                gddRelRemMeth = gddRelation->remMeth().transcode(),
                gddRelClrMeth = gddRelation->clrMeth().transcode(),
                gddRelType = gddRelation->type().transcode(),
                gddRelName = gddRelation->name().transcode(),
                gddRelDesc = gddRelation->desc().transcode();
      
    if (gddRelRatio == "1")
    {
      get_ret = "SmartRef<";
      set_arg = "SmartRef<";
      add_arg = "";
    }
    else if (gddRelRatio == "*")
    {
      get_ret = "SmartRefVector<";
      set_arg = "SmartRefVector<";
      add_arg = "SmartRef<";
    }
    else
    {
      get_ret = "";
      set_arg = "";
      add_arg = "";
    }

    if (gddRelGetMeth == accessor)
    {
      if( gddRelRatio == "1")
      {
        xmlOut << "  /// Retrieve " << gddRelDesc << " (const)" << std::endl
          << "  const " << gddRelType << "* " << DaDiTools::retGetName(gddRelName) 
            << "() const;" << std::endl
          << std::endl
          << "  /// Retrieve " << gddRelDesc << " (non-const)" << std::endl
          << "  " << gddRelType << "* " << DaDiTools::retGetName(gddRelName) 
            << "();" << std::endl 
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
        xmlOut << "  void " << "set" << DaDiTools::firstUp(gddRelName) << "(const " 
          << gddRelType << "* value); " << std::endl;
      }*/
      xmlOut  << "  void " << "set" << DaDiTools::firstUp(gddRelName) << "(const " 
          << set_arg << gddRelType << ">& value);" << std::endl 
        << std::endl;
    }
    if (strcmp("", add_arg) != 0)
    {
      if (gddRelAddMeth == accessor)
      {
        xmlOut << "  /// Add " << gddRelDesc << std::endl
          /*<< "  void " << "addTo" << DaDiTools::firstUp(gddRelName) << "(" << gddRelType 
            << "* value);" << std::endl */ 
          << "  void " << "addTo" << DaDiTools::firstUp(gddRelName) << "(const " << add_arg 
          << gddRelType << ">& value); " << std::endl << std::endl;
      }
      if (gddRelRemMeth == accessor)
      {
        xmlOut << "  /// Remove " << gddRelDesc << std::endl
          /*<< "  void " << "removeFrom" << DaDiTools::firstUp(gddRelName) << "("
            << gddRelType << "* value);" << std::endl */
          << "  void " << "removeFrom" << DaDiTools::firstUp(gddRelName)
            << "(const SmartRef<" << gddRelType << ">& value); " << std::endl 
          << std::endl;
      }
      if (gddRelClrMeth == accessor)
      {
        xmlOut << "  /// Clear " << gddRelDesc << std::endl
          << "  void " << "clear" << DaDiTools::firstUp(gddRelName) << "();"  << std::endl 
          << std::endl;
      }
    }      
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
    const char *get_ret, *set_arg, *add_arg;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    std::string gddRelGetMeth = gddRelation->getMeth().transcode(),
                gddRelSetMeth = gddRelation->setMeth().transcode(),
                gddRelAddMeth = gddRelation->addMeth().transcode(),
                gddRelRemMeth = gddRelation->remMeth().transcode(),
                gddRelClrMeth = gddRelation->clrMeth().transcode(),
                gddClassName = gddClass->name().transcode(),
                gddRelName = gddRelation->name().transcode(),
                gddRelType = gddRelation->type().transcode(),
                gddRelDesc = gddRelation->desc().transcode(),
                gddRelRatio = gddRelation->ratio().transcode();
      
    if (gddRelRatio == "1")
    {
      get_ret = "SmartRef<";
      set_arg = "SmartRef<";
      add_arg = "";
    }
    else if (gddRelRatio == "*")
    {
      get_ret = "SmartRefVector<";
      set_arg = "SmartRefVector<";
      add_arg = "SmartRef<";
    }
    else
    {
      get_ret = "";
      set_arg = "";
      add_arg = "";
    }

    if (gddRelGetMeth == accessor)
    {
      if (gddRelRatio == "1")
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
      /*if (gddRelRatio == "1") 
      { 
        xmlOut << "inline void " << gddClassName << "::" << "set" 
            << DaDiTools::firstUp(gddRelName) << "(const " << gddRelType << "* value)" << std::endl 
          << "{" << std::endl 
          << "   m_" << gddRelName << " = value;" << std::endl 
          << "}" << std::endl 
          << std::endl;
      }*/
      xmlOut  << "inline void " << gddClassName << "::" << "set" 
          << DaDiTools::firstUp(gddRelName) << "(const " << set_arg << gddRelType 
          << ">& value)" << std::endl 
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
		      << "  SmartRefVector<" << gddRelType << ">::iterator iter;" << std::endl
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
          << "  SmartRefVector<" << gddRelType << ">::iterator iter =" << std::endl
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
          << "  SmartRefVector<" << gddRelType << ">::iterator iter =" << std::endl
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
    std::string gddEnumName = gddEnum->name().transcode(),
                gddEnumValue = gddEnum->value().transcode(),
                gddEnumDesc = gddEnum->desc().transcode(),
                gddEnumAccess = gddEnum->access().transcode();

    if (accessor == "" || gddEnumAccess == accessor) 
    {
      xmlOut << "  enum " << gddEnumName << " {" << gddEnumValue << "};   ///<" 
          << gddEnumDesc << std::endl;
      newline = true;
    }
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
    std::string gddTypeDefDesc   = gddTypeDef->desc().transcode(),
                gddTypeDefType   = gddTypeDef->type().transcode(),
                gddTypeDefDef    = gddTypeDef->def().transcode(),
                gddTypeDefAccess = gddTypeDef->access().transcode();

    if (accessor == "" || gddTypeDefAccess == accessor) 
    {
      newline = true;
      xmlOut << "  typedef " << gddTypeDefType << " " << gddTypeDefDef << ";   ///<" 
          << gddTypeDefDesc << std::endl;
    }
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
    std::string gddAttName = gddAttribute->name().transcode(),
                gddAttType = gddAttribute->type().transcode(),
                gddAttArray = gddAttribute->array().transcode(); 
    
    int gddAttNameLength = gddAttName.length();
    if (gddAttArray != "FALSE")
    {
      gddAttNameLength = gddAttNameLength + 2 + gddAttArray.length();
    }

    if (gddAttName.length() > maxLengthName)
    { 
      maxLengthName = gddAttName.length(); 
    }

    if (gddAttType.length() > maxLengthType)
    { 
      maxLengthType = gddAttType.length(); 
    }
  }
  
  for(i=0; i<gddClass->sizeDaDiRelation(); ++i)
  {
    unsigned int lengthType = 0;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    std::string gddRelRatio = gddRelation->ratio().transcode(),
                gddRelName = gddRelation->name().transcode(),
                gddRelType = gddRelation->type().transcode();

    if (gddRelRatio == "1")
    { 
      lengthType = 10 + gddRelType.length(); 
    }
    else 
    { 
      lengthType = 16 + gddRelType.length(); 
    }

    if (gddRelName.length() > maxLengthName)
    { 
      maxLengthName = gddRelName.length(); 
    }

    if (lengthType > maxLengthType)
    { 
      maxLengthType = lengthType; 
    }
  }

  maxLengthName = maxLengthName + 4;

  //  xmlOut.setf(std::ios::left, std::ios::adjustfield);


  //
  // Enums for bitfields (lengths & masks)
  //
  for (i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    std::string gddAttAccess = gddAttribute->access().transcode(),
                gddAttName = gddAttribute->name().transcode(),
                gddAttType = gddAttribute->type().transcode(),
                gddAttDesc = gddAttribute->desc().transcode();
    bool gddAttIsBitset = gddAttribute->bitset();
    
    if (gddAttIsBitset && (gddAttAccess == accessor))
    {
      xmlOut << "  enum " << gddAttName << "Bits {";
      int offset = 0;
      for (j=0; j<gddAttribute->sizeDaDiBitfield(); ++j)
      {
        DaDiBitfield* gddBitfield = gddAttribute->popDaDiBitfield();
        int gddBfLength = gddBitfield->length();
        std::string gddBfName = gddBitfield->name().transcode();
        if (gddBfLength != 0)
        {
          if (j!=0) { xmlOut << ", "; }
          xmlOut << gddBfName << "Bits = " << offset;
          offset = offset + gddBfLength;
        }
      }
      xmlOut << "}; ///< Offsets of bitfield " << gddAttName << std::endl;
      if (offset > (signed)(sizeof(unsigned int)*8))
      {
        std::cerr << std::endl << "GOD says: WARNING: Bitset " 
          << gddAttName << " has more than " << (sizeof(unsigned long)*8) 
          << " bits";
      }

      xmlOut << "  enum " << gddAttName << "Masks {";
      offset = 0;
      for (j=0; j<gddAttribute->sizeDaDiBitfield(); ++j)
      {
        DaDiBitfield* gddBitfield = gddAttribute->popDaDiBitfield();
        int gddBfLength = gddBitfield->length();
        std::string gddBfName = gddBitfield->name().transcode();

        if (j!=0) { xmlOut << ", "; }
        xmlOut << gddBfName << "Mask = ";

        if (gddBfLength != 0)
        {
          int mask = (int)pow(2,gddBfLength)-1;
          mask = mask << offset;
          xmlOut << "0x";
          xmlOut.setf(std::ios::hex, std::ios::basefield);
          xmlOut.width(8);
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
      }
      xmlOut << "}; ///< Bitmasks for bitfield " << gddAttName << std::endl
        << std::endl;
    }
  }


  //
  // Private members (attributes)
  //
  for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    std::string gddAttAccess = gddAttribute->access().transcode(),
                gddAttName = gddAttribute->name().transcode(),
                gddAttType = gddAttribute->type().transcode(),
                gddAttDesc = gddAttribute->desc().transcode(),
                gddAttInit = gddAttribute->init().transcode(),
                gddAttArray = gddAttribute->array().transcode();
//    bool gddAttIsStatic = gddAttribute->static_();

    if (gddAttAccess == accessor)
    {
      std::string fullAttName = " m_" + gddAttName;
      if (gddAttArray != "FALSE")
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
  }

  //
  // Private members (relations)
  //
  for(i=0; i<gddClass->sizeDaDiRelation(); i++)
  {
    std::string rel_type;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    std::string gddRelAccess = gddRelation->access().transcode(),
                gddRelRatio = gddRelation->ratio().transcode(),
                gddRelType = gddRelation->type().transcode(),
                gddRelName = gddRelation->name().transcode(),
                gddRelDesc = gddRelation->desc().transcode();

    if (gddRelAccess == accessor)
    {
      if (gddRelRatio == "1")
      {
        rel_type = "SmartRef<";
      }
      else if (gddRelRatio == "*")
      {
        rel_type = "SmartRefVector<";
      }
      std::string relType = rel_type + gddRelType + '>';
      std::string relName = " m_" + gddRelName + ";";
      xmlOut.setf(std::ios::left, std::ios::adjustfield);
      xmlOut << "  ";
      xmlOut.width(maxLengthType);
      xmlOut << relType; 
      xmlOut.width(maxLengthName);
      xmlOut << relName;
      xmlOut.width(0);
      xmlOut << " ///< " << gddRelDesc << std::endl;
    }
  }
  xmlOut.unsetf(std::ios::adjustfield);                  
}


//-----------------------------------------------------------------------------
void printClass(std::ofstream& xmlOut,
                DaDiClass* gddClass,
                DaDiPackage* gddPackage,
                std::map<std::string,std::string>& dbExportClass)
//-----------------------------------------------------------------------------
{
  time_t ltime;
  int i, j;
  bool classTemplate = gddClass->keyedContTypeDef(), 
       isEventClass = false, 
       classTemplateVector = false,
       classTemplateList = false;

  std::vector<std::string>::iterator coIter, koIter;


              
  std::string gddClassName = gddClass->name().transcode(),
              gddClassID = gddClass->ID().transcode(),
              gddClassLocation = gddClass->location().transcode(),
              gddPackName = gddPackage->packageName().transcode(),
              gddClassDesc = gddClass->desc().transcode(),
              gddClassLongDesc = gddClass->longDesc().transcode(),
              gddClassAuthor = gddClass->author().transcode();
  
  bool gddClassSerializers = gddClass->serializers(),
       gddClassStdVectorTypeDef = gddClass->stdVectorTypeDef();
  
  
  gddDefEnums.clear();
  gddDefTypeDefs.clear();

  for (i=0; i<gddClass->sizeDaDiEnum(); ++i)
  {
    gddDefEnums.push_back(gddClass->popDaDiEnum()->name().transcode());
  }
  for (i=0; i<gddClass->sizeDaDiTypeDef(); ++i)
  {
    gddDefTypeDefs.push_back(gddClass->popDaDiTypeDef()->def().transcode());
  }
    
  ///
  /// check if Class is an 'Eventclass'
  ///
  /*for (i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
  {
    std::string bClassName = gddClass->popDaDiBaseClass()->name().transcode();
    if ((bClassName == "DataObject") || (bClassName == "ContainedObject"))
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
      << "  static const CLID& CLID_" << gddClassName << " = " << gddClassID 
        << ";" << std::endl 
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
      std::string gddLocName = gddLocation->name().transcode(),
                  gddLocPlace = gddLocation->place().transcode();
      bool gddNoQuote = gddLocation->noQuote();

      xmlOut << "  static const std::string& " << gddLocName << " = ";
      if (!gddNoQuote) { xmlOut << "\""; }
      xmlOut << gddLocPlace;
      if (!gddNoQuote) { xmlOut << "\""; }      
      xmlOut << ";" << std::endl;
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
      << "namespace " << gddClass->className().transcode() << "Location {"
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
      xmlOut << gddLocation->name().transcode();
    }
    xmlOut << "};" << std::endl << "}" << std::endl << std::endl << std::endl;
  }*/

  time(&ltime);

//
// class description
//
  xmlOut << "/** @class " << gddClassName << " " << gddClassName << ".h ";

  if (gddPackName == "__NO_PACKAGE__")
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
  
  if (gddClass->sizeDaDiBaseClass() > 0) 
  {
    xmlOut << ": ";
    for (i = 0; i < gddClass->sizeDaDiBaseClass(); ++i)
    {
      DaDiBaseClass* gddBaseClass = gddClass->popDaDiBaseClass();
      std::string fullBaseClName = gddBaseClass->name().transcode(),
                  baseClName = fullBaseClName.substr(0,fullBaseClName.find_first_of("<")),
                  gddBaseClassAccess = gddBaseClass->access().transcode();
      bool gddBaseClassIsVirtual = gddBaseClass->virtual_();

      koIter = std::find(KeyedObjectClasses.begin(), KeyedObjectClasses.end(), baseClName);
      if (koIter != KeyedObjectClasses.end())
      {
        KeyedObjectClasses.push_back(gddClassName);
        classTemplate = true;
      }
      //////////////////////  tbd
      coIter = std::find(ContainedObjectClasses.begin(), ContainedObjectClasses.end(), baseClName);
      if (coIter != ContainedObjectClasses.end())
      {
        ContainedObjectClasses.push_back(gddClass->name().transcode());
        if (gddClass->classTemplateVector())
        {
          classTemplateVector = true;
        }
        if (gddClass->classTemplateList())
        {
          classTemplateList = true;
        }
      }
      //////////////////////////
      if (i>0) {xmlOut << ", ";}
      if (gddBaseClassIsVirtual)
      {
        xmlOut << "virtual ";
      }
      xmlOut << DaDiTools::chooseAccess(gddBaseClassAccess) << " " << fullBaseClName;
    }
  }
  
  
  xmlOut << std::endl << "{" << std::endl 
    << std::endl;
  
  
  // 
  //   PUBLIC AREA
  //
  xmlOut << "public: " << std::endl
    << std::endl;

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
    std::string gddConsDesc = gddConstructor->desc().transcode(),
                gddConsCode = gddConstructor->code().transcode(),
                gddConsInitList = gddConstructor->initList().transcode();

    
    if (!DaDiTools::isEmpty(gddConsDesc))
    {
      xmlOut << "  /// " << gddConsDesc << std::endl;
    }
    xmlOut << "  " << gddClassName << "(";

    if (gddConstructor->sizeDaDiMethArgument() == 0) {constWithZeroArgs = true;}

    printArguments(xmlOut, gddConstructor);
    
    xmlOut << ")";


    if (gddConstructor->initList() != NULL)
    {
      xmlOut << std::endl << "    : " << gddConstructor->initList().transcode();
      if (gddConsCode == "")
      {
        gddConsCode == " ";
      }
    }
    
    if (gddConsCode != "")
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
      std::string initValue,
                  gddAttName = gddAttribute->name().transcode(),
                  gddAttType = gddAttribute->type().transcode(),
                  gddAttInit = gddAttribute->init().transcode();
      bool gddAttIsStatic = gddAttribute->static_();
      int lastspace = gddAttType.find_last_of(" ");
      gddAttType = gddAttType.substr(lastspace+1, gddAttType.size()-lastspace);
      if (!gddAttIsStatic)
      {
        if (!DaDiTools::isEmpty(gddAttInit))
        {
          initValue = gddAttInit;
        }
        else if ((gddAttType == "int") || (gddAttType == "unsigned") ||
                 (gddAttType == "short") || (gddAttType == "signed") ||
                 (gddAttType == "long"))
        {
          initValue = "0";
        }
        else if ((gddAttType == "float") || (gddAttType == "double"))
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
      std::string gddDestDesc = gddDestructor->desc().transcode(),
                  gddDestCode = gddDestructor->code().transcode();


      if (!DaDiTools::isEmpty(gddDestDesc))
      {
        xmlOut << "  /// " << gddDestDesc << std::endl;
      }
      xmlOut << "  ~" << gddClassName << "(";

      printArguments(xmlOut, gddDestructor);

      xmlOut << ")";

      if (gddDestCode != "")
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
    }
  }

//
// Functions clID() & classID()
//
  if (isEventClass)
  {
    xmlOut << "  /// Retrieve pointer to class definition structure" << std::endl 
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
	  xmlOut << "  /// Operator overloading for serializing (writing)"  << std::endl 
	    << "  friend StreamBuffer& operator<< (StreamBuffer& s, const "
	      << gddClassName << "& obj)" << std::endl 
      << "  {" << std::endl 
      << "    return obj.serialize(s);" << std::endl 
      << "  }" << std::endl 
      << std::endl;

	  xmlOut << "  /// Operator overloading for serializing (reading)" << std::endl
	    << "  friend StreamBuffer& operator>> (StreamBuffer& s, "
      << gddClassName << "& obj)" << std::endl 
      << "  {" << std::endl
	    << "    return obj.serialize(s);" << std::endl 
      << "  }" << std::endl
	    << std::endl;

	  xmlOut << "  /// Operator overloading for stringoutput" << std::endl
	    << "  friend std::ostream& operator<< (std::ostream& s, const "
	    << gddClass->name().transcode() << "& obj)" << std::endl 
      << "  {" << std::endl 
      << "    return obj.fillStream(s);" << std::endl 
      << "  }" << std::endl
      << std::endl;

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
      std::string gddMethodName = gddMethod->name().transcode(),
                  gddMethRetType = gddMethod->daDiMethReturn()->type().transcode();
      bool gddMethodIsConst = gddMethod->const_();

      if (gddMethodName == "serialize" && gddMethRetType == "StreamBuffer&" 
          && !gddMethodIsConst)
      {
        streamIn = true;
      }
      if (gddMethodName == "serialize" && gddMethRetType == "StreamBuffer&" 
          && gddMethodIsConst)
      {
        streamOut = true;
      }
      if (gddMethodName == "fillStream" && gddMethRetType == "std::ostream&")
      {
        ostreamOut = true;
      }
    }


//
//  Serializers, fillstream
//

    if (!streamOut)
    {
      xmlOut << "  /// Serialize the object for writing" << std::endl
	      << "  virtual StreamBuffer& serialize(StreamBuffer& s) const;" << std::endl 
        << std::endl;
    }
    if (!streamIn)
    {
      xmlOut << "  /// Serialize the object for reading" << std::endl
	      << "  virtual StreamBuffer& serialize(StreamBuffer& s);" << std::endl 
        << std::endl;
    }
    if (!ostreamOut)
    {
      xmlOut << "  /// Fill the ASCII output stream" << std::endl
	      << "  virtual std::ostream& fillStream(std::ostream& s) const;" << std::endl 
        << std::endl;
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
    << "// -----------------------------------------------------------------------------"
    << std::endl 
    << "//   end of class" << std::endl
    << "// -----------------------------------------------------------------------------"
    << std::endl;

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
          << argV0 << ": No information found for type: " << impName << std::endl 
          << argV0 << ": Line written: #include \"" << impName << ".h\"" << std::endl;
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
          << argV0 << ": No information found for type: " << impName << std::endl 
          << argV0 << ": Line written: #include \"" << impName << ".h\"" << std::endl;
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
    xmlOut << "inline const CLID& " << gddClassName << "::clID() const " << std::endl 
      << "{" << std::endl 
      << "  return " << gddClassName << "::classID();" << std::endl 
      << "}" << std::endl 
      << std::endl;

  //
  // Function classID()
  //
    xmlOut << "inline const CLID& " << gddClassName << "::classID()" << std::endl 
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
    std::string namesp = gddClass->className().transcode();
    namesp += "Location::";
    xmlOut << "inline static const std::string location(int loc)" << std::endl
      << "{" << std::endl << "  switch(loc) {";
    if (gddClass->location() != NULL)
    {
      xmlOut << std::endl << "  case " << namesp << "Default: " << std::endl 
        << "    return \"" << gddClass->location().transcode() << "\";";
    }
    for (i=0; i<gddClass->sizeDaDiLocation(); ++i)
    {
      DaDiLocation* gddLocation = gddClass->popDaDiLocation();
      xmlOut << std::endl << "  case " << namesp << gddLocation->name().transcode()
        << ":" << std::endl << "    return \"" << gddLocation->place().transcode()
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
	    << gddClass->name().transcode() << "& obj)" << std::endl 
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
        std::string gddAttName = gddAttribute->name().transcode(),
                    gddAttType = gddAttribute->type().transcode();
        bool gddAttSerialize = gddAttribute->serialize();

        if (gddAttType == "bool" && gddAttSerialize)
        {
          xmlOut << "  unsigned char " << "l_" << gddAttName << " = (m_"
              << gddAttName << ") ? 1 : 0;" << std::endl;
        } 
      }

      bool seriAtt = false;
      for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
      {
        std::string gddBaseName = gddClass->popDaDiBaseClass()->name().transcode();
        if (gddBaseName != "ContainedObject" &&
            gddBaseName != "DataObject")
        {
          xmlOut << "  " << gddBaseName << "::serialize(s);" << std::endl;
        }
      }
      for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        std::string gddAttType = gddAttribute->type().transcode(),
                    gddAttName = gddAttribute->name().transcode();
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
          if (gddAttType == "bool")
          {
            xmlOut << "<< l_";
          }
          else if (gddAttType == "double" && gddAttCompression)
          {
            xmlOut << "<< (float)m_";
          }
          else
          {
            xmlOut << "<< m_";
          }
          xmlOut << gddAttName;    
        }
      }

      for(i=0; i<gddClass->sizeDaDiRelation(); i++)
      {
        DaDiRelation* gddRelation = gddClass->popDaDiRelation();
        std::string gddRelName = gddRelation->name().transcode();
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

      bool attBool=false, attFloat=false;
  
      /// treating boolean values
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        std::string gddAttType = gddAttribute->type().transcode(),
                    gddAttName = gddAttribute->name().transcode();
        bool gddAttSerialize = gddAttribute->serialize();

        if (gddAttType == "bool" && gddAttSerialize)
        {
          if (!attBool)
          {
            xmlOut << "  unsigned char ";
            attBool = true;
          }
          else
          {
            xmlOut << ", ";
          }
          xmlOut << "l_" << gddAttName;
        }
      }
      if (attBool) 
      { 
        xmlOut << ";" << std::endl; 
      }

      /// treating float values
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        std::string gddAttType = gddAttribute->type().transcode(),
                    gddAttName = gddAttribute->name().transcode();
        bool gddAttSerialize = gddAttribute->serialize(),
             gddAttCompression = gddAttribute->compression();

        if (gddAttType == "double" && gddAttSerialize && gddAttCompression)
        {
          if (!attFloat)
          {
            xmlOut << "  float ";
            attFloat = true;
          }
          else
          {
            xmlOut << ", ";
          }
          xmlOut << "l_" << gddAttName;
        }
      }
      if (attFloat) 
      {  
        xmlOut << ";" << std::endl; 
      }

      bool seriAtt = false;
      for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
      {
        std::string gddBaseName = gddClass->popDaDiBaseClass()->name().transcode();
        if (gddBaseName != "ContainedObject" && gddBaseName != "DataObject")
        {
		    xmlOut << "  " << gddBaseName << "::serialize(s);" << std::endl;
        }
      }

      for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        std::string gddAttType = gddAttribute->type().transcode(),
                    gddAttName = gddAttribute->name().transcode();
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
          if (gddAttType == "bool" || (gddAttType == "double" && gddAttCompression))
          {
            xmlOut << ">> l_";
          }
          else
          {
            xmlOut << ">> m_";
          }
          xmlOut << gddAttName;    
        }
      }

      for(i=0; i<gddClass->sizeDaDiRelation(); i++)
      {
        DaDiRelation* gddRelation = gddClass->popDaDiRelation();
        std::string gddRelName = gddRelation->name().transcode();
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
      }
  	  if (seriAtt)
      {
        xmlOut << ";" << std::endl;
      }
     
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        std::string gddAttType = gddAttribute->type().transcode(),
                    gddAttName = gddAttribute->name().transcode();
        bool gddAttSerialize = gddAttribute->serialize();

        if (gddAttType == "bool" && gddAttSerialize)
        {
          xmlOut << "  m_" << gddAttName << " = (l_" << gddAttName
            << ") ? true : false;" << std::endl;
        }
      }
      for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        std::string gddAttType = gddAttribute->type().transcode(),
                    gddAttName = gddAttribute->name().transcode();
        bool gddAttSerialize = gddAttribute->serialize(),
             gddAttCompression = gddAttribute->compression();

        if (gddAttType == "double" && gddAttSerialize && gddAttCompression)
        {
          xmlOut << "  m_" << gddAttName << " = l_" << gddAttName << ";" << std::endl;
        }
      }
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
        std::string gddAttType = gddAttribute->type().transcode(),
                    gddAttName = gddAttribute->name().transcode();

        if (gddAttType == "bool")
        {
          xmlOut << "  char " << "l_" << gddAttName << " = (m_"
            << gddAttName << ") ? 'T' : 'F';" << std::endl;
        }     
      }

      bool seriAtt = false;
      for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
      {
        DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
        std::string gddAttName = gddAttribute->name().transcode(),
                    gddAttType = gddAttribute->type().transcode();
        bool gddAttCompression = gddAttribute->compression(),
             gddAttIsBitset = gddAttribute->bitset();

        if(i==0)
        {
          /*xmlOut << "  s << \"class " << gddClass->className().transcode()
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
        if (gddAttType == "bool")
        {  
          xmlOut << "<< l_";
        }
        else if (gddAttType == "double" && gddAttCompression)
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
            std::string gddBfName = gddBitfield->name().transcode();
            bool gddBfGetMeth = gddBitfield->getMeth().equals("TRUE");

            if (gddBfGetMeth)
            {
              xmlOut << " << std::endl" << std::endl 
                << "    << \"   " << gddBfName << ":\\t\" << " << gddBfName << "()";
            }
          }
        }
        
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
      xmlOut << "inline StreamBuffer& " << gddClass->className().transcode() 
        << "::serialize( StreamBuffer& s) const" << std::endl << "{" << std::endl
        << "  return s << *this;" << std::endl << "}" << std::endl << std::endl;

      xmlOut << "inline StreamBuffer& " << gddClass->className().transcode()
        << "::serialize(StreamBuffer& s)" << std::endl << "{" << std::endl
        << " return s >> *this;" << std::endl << "}" << std::endl << std::endl;

      xmlOut << "inline std::ostream& " << gddClass->className().transcode() 
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
//      << "template <class TYPE, class MAPPING> class KeyedContainer;" << std::endl
      << "typedef KeyedContainer<" << gddClassName << ", Containers::HashMap> " 
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
    xmlOut << "// Defintion of vector container type for " << gddClassName << std::endl
      << "template <class TYPE> class ObjectVector;" << std::endl
      << "typedef ObjectVector<" << gddClassName << "> " << gddClassName 
        << "Vector;" << std::endl;
  }
  
  if (classTemplateList)
  {
    xmlOut << "// Defintion of all list container types for " << gddClassName << std::endl
      << "template <class TYPE> class ObjectList;" << std::endl
      << "typedef ObjectList<" << gddClassName << "> " << gddClassName 
        << "List;" << std::endl;
  }
/////////////////////////////////////  
  gddDefEnums.clear();
  gddDefTypeDefs.clear();
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
  std::string gddNamespName = gddNamespace->name().transcode(),
              gddNamespAuthor = gddNamespace->author().transcode(),
              gddNamespDesc = gddNamespace->desc().transcode(),
              gddNamespLongDesc = gddNamespace->longDesc().transcode();

  gddDefEnums.clear();
  gddDefTypeDefs.clear();

  for (i=0; i<gddNamespace->sizeDaDiEnum(); ++i)
  {
    gddDefEnums.push_back(gddNamespace->popDaDiEnum()->name().transcode());
  }
  for (i=0; i<gddNamespace->sizeDaDiTypeDef(); ++i)
  {
    gddDefTypeDefs.push_back(gddNamespace->popDaDiTypeDef()->def().transcode());
  }

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
    std::string gddAttType = gddAttribute->type().transcode(),
                gddAttName = gddAttribute->name().transcode(),
                gddAttDesc = gddAttribute->desc().transcode(),
                gddAttInit = gddAttribute->init().transcode();

    xmlOut << "  /// " << gddAttDesc << std::endl
      << "  " << gddAttType << " " << gddAttName;
    if (!DaDiTools::isEmpty(gddAttInit))
    {
      xmlOut << " = " << gddAttInit;
    }
    xmlOut << ";" << std::endl
      << std::endl;
  }

  printMethodDecl(xmlOut, gddNamespace, "");

  for (i=0; i<gddNamespace->sizeDaDiClass(); ++i)
  {
    xmlOut << std::endl;
    DaDiClass* gddClass = gddNamespace->popDaDiClass();
    printClass(xmlOut,gddClass,gddPackage,dbExportClass);
  }

  xmlOut << "};" << std::endl
    << std::endl;

  printMethodImpl(xmlOut, gddNamespace, "");
  
  gddDefEnums.clear();
  gddDefTypeDefs.clear();
   
}


//-----------------------------------------------------------------------------
void printCppHeader(DaDiPackage* gddPackage, 
                    char* envXmlDB, 
                    const char* envOut, 
                    bool additionalImports)
//-----------------------------------------------------------------------------
{
  int i=0, j=0, k=0;
  std::list<std::string>::iterator iIter;
  std::map<std::string,std::string> dbExportClass;
  
  ContainedObjectClasses.push_back("ContainedObject");
  KeyedObjectClasses.push_back("KeyedObject");
  
  std::string gddPackName = gddPackage->packageName().transcode();

//
// Parser initialization
//
  DOMParser *dbParser = new DOMParser;
  dbParser->setValidationScheme(DOMParser::Val_Auto);
  dbParser->setDoNamespaces(false);
  ErrorHandler *dbErrReporter = new DaDiTools();
  dbParser->setErrorHandler(dbErrReporter);
  dbParser->setCreateEntityReferenceNodes(false);
  dbParser->setToCreateXMLDeclTypeNode(true);
  
  bool dbErrorsOccured = false;
  
//
// Parse the 'Headerdatabase'
//
  char* xmlDbFileName = strcat(envXmlDB,"GaudiCppExport.xml");
  try
    {
    dbParser->parse(xmlDbFileName);
    }
  
  catch(const XMLException& e)
    {
    std::cerr << argV0 << ": An error occured during parsing \n Message: "
      << e.getMessage() << std::endl;
      dbErrorsOccured = true;
    }
  
  catch(const DOM_DOMException& e)
    {
    std::cerr << argV0 << ": An error occured during parsing \n Message: "
            << e.msg.transcode() << std::endl;
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
    DOM_Node dbDoc = dbParser->getDocument();
    DOM_Node dbTop = dbDoc.getFirstChild();
      
    while(!dbTop.getNodeName().equals("gdd"))
    {
      dbTop = dbTop.getNextSibling();
    }
      
    DOM_Node dbPackNode = dbTop.getNextSibling().getFirstChild();
      
    while (!dbPackNode.isNull())
    {
      if (dbPackNode.getNodeName().equals("package"))
      {
        DOM_Node dbClassNode = dbPackNode.getFirstChild();
        std::string dbPackName;
        dbPackName = dbPackNode.getAttributes().
          getNamedItem(DOMString::transcode("name")).
          getNodeValue().transcode();
        while(!dbClassNode.isNull())
        {
          if (dbClassNode.getNodeName().equals("class")) 
          {
            std::string dbClassName, dbFileName;
            dbClassName = dbClassNode.getAttributes().
              getNamedItem(DOMString::transcode("name")).
              getNodeValue().transcode();
              if (dbClassNode.getAttributes().
                  getNamedItem(DOMString::transcode("filename")) != 0)
              {
                dbFileName = dbClassNode.getAttributes().
                  getNamedItem(DOMString::transcode("filename")).
                  getNodeValue().transcode();
                  dbExportClass[dbClassName] = dbPackName+"/"+dbFileName;
              }
              else
              {
                dbExportClass[dbClassName] = dbPackName+"/"+dbClassName;
              }
            }
            dbClassNode = dbClassNode.getNextSibling();
          }
        }
        dbPackNode = dbPackNode.getNextSibling();
      }
    }

  delete dbParser;
  delete dbErrReporter;

///
/// additional Imports
///
  if (additionalImports)
  {
    const char* addImportsFile = "AddImports.txt";
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
    std::string gddNamespName = gddNamespace->name().transcode();

    dbExportClass[gddNamespName] = "Event/" + gddNamespName;

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
    strcat(fileName, gddNamespace->name().transcode());
    strcat(fileName, ".h");
    std::cout << "Writing " << fileName;
    std::ofstream xmlOut(fileName);  

    xmlOut << std::endl 
      << std::endl
      << "//   **************************************************************************" << std::endl
      << "//   *                                                                        *" << std::endl
      << "//   *                      ! ! ! A T T E N T I O N ! ! !                     *" << std::endl
      << "//   *                                                                        *" << std::endl
      << "//   *  This file was created automatically by GaudiObjDesc, please do not    *" << std::endl
      << "//   *  delete it or edit it by hand.                                         *" << std::endl
      << "//   *                                                                        *" << std::endl
      << "//   *  If you want to change this file, first change the corresponding       *" << std::endl
      << "//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *" << std::endl
      << "//   *  are using it from inside a Gaudi-package).                            *" << std::endl
      << "//   *                                                                        *" << std::endl
      << "//   **************************************************************************" << std::endl
      << std::endl 
      << std::endl 
      << std::endl;


    //
    // IFNDEF
    //
    xmlOut << "#ifndef " << gddPackName << "_" << gddNamespName << "_H" << std::endl 
      << "#define " << gddPackName << "_" << gddNamespName << "_H 1" << std::endl 
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
      if (std::find(noImports.begin(), noImports.end(), impName) == noImports.end())
      {
        if(dbExportClass[impName] != "")
        {
          impName = dbExportClass[impName];
        }
        else
        {
          std::cerr << std::endl 
            << argV0 << ": No information found for type: " << impName << std::endl 
            << argV0 << ": Line written: #include \"" << impName << ".h\"" << std::endl;
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
  
    xmlOut << "#endif   ///" << gddPackName << "_" << gddNamespName << "_H" << std::endl;

    xmlOut.close();
    delete [] fileName;

    std::cout << " - OK" << std::endl;
  }





  for (k=0; k<gddPackage->sizeDaDiClass(); ++k)
  {  

    DaDiClass* gddClass = gddPackage->popDaDiClass();
    std::string gddClassName = gddClass->name().transcode();

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

/*    dbExportClass[std::string(gddClass->className().transcode())] =
      std::string(gddPackage->packageName().transcode()) + "/" 
      + std::string(gddClass->className().transcode()); */

    dbExportClass[gddClassName] = "Event/" + gddClassName;

    // ------------------------------------------------------------------------
    // Here starts the writing of the .h-file


    char* fileName = new char[256];
    strcpy(fileName, envOut);
    strcat(fileName, gddClass->name().transcode());
    strcat(fileName, ".h");
    std::cout << "Writing " << fileName;
    std::ofstream xmlOut(fileName);  

  
    //
    // 'donotedit'-message
    //

    xmlOut << std::endl 
      << std::endl
      << "//   **************************************************************************" << std::endl
      << "//   *                                                                        *" << std::endl
      << "//   *                      ! ! ! A T T E N T I O N ! ! !                     *" << std::endl
      << "//   *                                                                        *" << std::endl
      << "//   *  This file was created automatically by GaudiObjDesc, please do not    *" << std::endl
      << "//   *  delete it or edit it by hand.                                         *" << std::endl
      << "//   *                                                                        *" << std::endl
      << "//   *  If you want to change this file, first change the corresponding       *" << std::endl
      << "//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *" << std::endl
      << "//   *  are using it from inside a Gaudi-package).                            *" << std::endl
      << "//   *                                                                        *" << std::endl
      << "//   **************************************************************************" << std::endl
      << std::endl 
      << std::endl 
      << std::endl;


    //
    // IFNDEF
    //
    xmlOut << "#ifndef " << gddPackName << "_" << gddClassName << "_H" << std::endl 
      << "#define " << gddPackName << "_" << gddClassName << "_H 1" << std::endl 
      << std::endl;

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
      if (std::find(noImports.begin(), noImports.end(), impName) == noImports.end())
      {
        if(dbExportClass[impName] != "")
        {
          impName = dbExportClass[impName];
        }
        else
        {
          std::cerr << std::endl 
            << argV0 << ": No information found for type: " << impName << std::endl 
            << argV0 << ": Line written: #include \"" << impName << ".h\"" << std::endl;
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

    printClass(xmlOut, gddClass, gddPackage, dbExportClass);


    //
    // End of .h-File
    //
    xmlOut << std::endl;
  
    xmlOut << "#endif   ///" << gddPackName << "_" << gddClassName << "_H" << std::endl;

    xmlOut.close();
    delete [] fileName;

    std::cout << " - OK" << std::endl;  
  
  }
}


//-----------------------------------------------------------------------------
int main(int argC, 
         char* argV[])
//-----------------------------------------------------------------------------
{

  #ifdef WIN32
    const char* sep = "\\";
  #elif defined(__linux)
    const char* sep = "/";
  #endif

  std::vector<char*> files;
  const char* envOut = "";
  char* envXmlDB;
  std::string nextArg;
  bool additionalImports = false;

  argV0 = std::string(argV[0]);
  argV0.erase(0,argV0.find_last_of("\\")+1);
  argV0.erase(0,argV0.find_last_of("/")+1);
  
  if (getenv("GAUDIOBJDESCROOT") == NULL)
  {
    std::cerr << argV0 << ": Please set Environmentvariable 'GAUDIOBJDESCROOT'"
      << std::endl;
    exit(1);
  }
  else
  {
    envXmlDB = getenv("GAUDIOBJDESCROOT");
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
            envOut = getenv("GODDOTHOUT");
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
          envOut = argV[i+1];
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
            envXmlDB = getenv("GODXMLDB");
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
          envXmlDB = argV[i+1];
          ++i;
        }
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
  
  for (std::vector<char*>::iterator iter = files.begin(); 
       iter != files.end(); ++iter)
  {
    DaDiPackage* gddPackage = DDFE::DaDiFrontEnd(*iter);

    printCppHeader(gddPackage,envXmlDB,envOut,additionalImports);
  }
  return 0;
}


