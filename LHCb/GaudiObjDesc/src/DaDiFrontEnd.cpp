// $Id: DaDiFrontEnd.cpp,v 1.43 2003-12-17 17:31:19 mato Exp $

//#include "GaudiKernel/Kernel.h"
#include "DaDiTools.h"
#include "DaDiFrontEnd.h"
#include "DaDiPackage.h"

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XMLUniDefs.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMException.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/util/XMLString.hpp"

#include <iostream>
#include <string>
#include <list>
#include <ctime>
#include <vector>

//-----------------------------------------------------------------------------
// Implementation file for class : Dadi
//
// 18/05/2001 :
//-----------------------------------------------------------------------------

using namespace xercesc;

extern std::string argV0;

DaDiClass* gddClass;
//std::string xmlVersion;


//-----------------------------------------------------------------------------
void trim(std::string value)
//-----------------------------------------------------------------------------
{
  while (value[0] == ' ')
  {
    value = value.substr(1, std::string::npos);
  }
  while (value[value.length()-1] == ' ')
  {
    value = value.substr(0, value.length()-1);
  }
}


//-----------------------------------------------------------------------------
std::vector<std::string> findWords(std::string value,
                                   std::string delim)
//-----------------------------------------------------------------------------
{
  std::vector<std::string> words;
  std::string tmpStr;
  unsigned int i = 0;

  while ((i = value.find(delim)) != std::string::npos)
  {
    tmpStr = value.substr(0,i-1);
    trim(tmpStr);
    words.push_back(tmpStr);
    value = value.substr(i, std::string::npos);
  }
  if (value.size() != 0)
  {
    trim(value);
    words.push_back(value);
  }
  return words;
}


//-----------------------------------------------------------------------------
std::vector<XMLCh*> findWords(const XMLCh* value,
                              XMLCh delim)
//-----------------------------------------------------------------------------
{
  std::vector<XMLCh*> words;
  unsigned int i = 0;
  int j = 0;

  while ((j = XMLString::indexOf(value,delim,i)) != -1)
  {
    XMLCh* tmpStr = new XMLCh [j-i+1];
    XMLString::subString(tmpStr, value, i, j);
    //std::cout << "." << XMLString::transcode(tmpStr) << "." << j-i+1 << std::endl;
    XMLString::trim(tmpStr);
    words.push_back(tmpStr);
    i = j+1;
  }

  if (i < XMLString::stringLen(value))
  {
    XMLCh* tmpStr = new XMLCh[XMLString::stringLen(value)-i+1];
    XMLString::subString(tmpStr, value, i, XMLString::stringLen(value));
    XMLString::trim(tmpStr);
    if (XMLString::stringLen(tmpStr) > 0)
    {
      words.push_back(tmpStr);
    }
  }
  return words;
}


//-----------------------------------------------------------------------------
bool isPointer(const XMLCh* value)
//-----------------------------------------------------------------------------
{
  //XMLCh* tmpStr = XMLString::transcode("");
  //XMLString::copyString(tmpStr, value);
  //XMLString::trim(tmpStr);
  XMLCh* pStr = XMLString::transcode("*");
  bool ret = XMLString::endsWith(value, pStr);
  //delete tmpStr;
  XMLString::release(&pStr);
  return ret;
}


//-----------------------------------------------------------------------------
template<class T> void parseImport(DOMNode* node,
                                   T* element)
//-----------------------------------------------------------------------------
{
  std::string import_name, import_soft, import_std, import_ignore;
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  char *cStr = XMLString::transcode(nodeMap->getNamedItem(itemStr)->
                                    getNodeValue());
  import_name = std::string(cStr);
  XMLString::release(&cStr);
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("soft");
  cStr = XMLString::transcode(nodeMap->getNamedItem(itemStr)->getNodeValue());
  import_soft = std::string(cStr);
  XMLString::release(&cStr);
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("std");
  cStr = XMLString::transcode(nodeMap->getNamedItem(itemStr)->getNodeValue());
  import_std = std::string(cStr);
  XMLString::release(&cStr);
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("ignore");
  cStr = XMLString::transcode(nodeMap->getNamedItem(itemStr)->getNodeValue());
  import_ignore = std::string(cStr);
  XMLString::release(&cStr);
  XMLString::release(&itemStr);

  if (import_ignore == "TRUE")
  {
    element->pushNoImports(import_name);
  }
  else
  {
    if (import_soft == "TRUE")
    {
      if (import_name.find("/") == std::string::npos)
      {
        element->pushImpSoftList(import_name);
      }
      else
      {
        DaDiTools::pushAddImport(import_name);
        element->pushImpSoftList(import_name.substr
                                 (import_name.find_last_of("/")+1,
                                  std::string::npos));
      }
    }
    else if (import_std == "TRUE")
    {
      element->pushImpStdList(import_name);
    }
    else
    {
      if (import_name.find("/") == std::string::npos)
      {
        element->pushImportList(import_name);
      }
      else
      {
        DaDiTools::pushAddImport(import_name);
        element->pushImportList(import_name.substr
                                (import_name.find_last_of("/")+1,
                                 std::string::npos));
      }
    }
  }
}


//-----------------------------------------------------------------------------
void parseBaseClass(DOMNode* node,
                    DaDiBaseClass* gddBaseClass)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  XMLCh* baseClassName = new XMLCh[XMLString::stringLen(nodeMap->
                                                        getNamedItem(itemStr)->
                                                        getNodeValue())+1];
  XMLString::copyString(baseClassName,nodeMap->getNamedItem(itemStr)->
                        getNodeValue());
  XMLString::release(&itemStr);
  gddBaseClass->setName(baseClassName);
  char* cBaseClassName = XMLString::transcode(baseClassName);
  gddClass->pushImportList(cBaseClassName);
  XMLString::release(&cBaseClassName);
  delete [] baseClassName;

  itemStr = XMLString::transcode("virtual");
  XMLCh* boolStr = XMLString::transcode("TRUE");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->
                               getNodeValue(), boolStr) == 0)
  {
    gddBaseClass->setVirtual_(true);
  }
  else
  {
    gddBaseClass->setVirtual_(false);
  }
  XMLString::release(&boolStr);
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("access");
  gddBaseClass->setAccess(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);
}


//-----------------------------------------------------------------------------
template<class T> void parseEnum(DOMNode* node,
                                 DaDiEnum* gddEnum,
                                 T* gdd)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  XMLCh* gddName = new XMLCh[XMLString::stringLen(nodeMap->
                                                  getNamedItem(itemStr)->
                                                  getNodeValue())+1];
  XMLString::copyString(gddName,nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  gddEnum->setName(gddName);
  char* cGddName = XMLString::transcode(gddName);
  gdd->pushNoImports(cGddName);
  XMLString::release(&cGddName);
  delete [] gddName;

  itemStr = XMLString::transcode("desc");
  gddEnum->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("value");
  gddEnum->setValue(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("access");
  gddEnum->setAccess(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);
}


//-----------------------------------------------------------------------------
void parseTemplate(DOMNode* node,
		   DaDiTemplate* gddTemplate)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  gddTemplate->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("t1");
  gddTemplate->setT1(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("t2");
  if (nodeMap->getNamedItem(itemStr))
  {
    gddTemplate->setT2(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddTemplate->setT2(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("t3");
  if (nodeMap->getNamedItem(itemStr))
  {
    gddTemplate->setT3(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddTemplate->setT3(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("t4");
  if (nodeMap->getNamedItem(itemStr))
  {
    gddTemplate->setT4(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddTemplate->setT4(0);
  }
  XMLString::release(&itemStr);

}


//-----------------------------------------------------------------------------
template<class T> void parseTypeDef(DOMNode* node,
                                    DaDiTypeDef* gddTypeDef,
                                    T* gdd)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("desc");
  gddTypeDef->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("type");
  gddTypeDef->setType(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("def");
  XMLCh* gddDef = new XMLCh[XMLString::stringLen(nodeMap->
                                                 getNamedItem(itemStr)->
                                                 getNodeValue())+1];
  XMLString::copyString(gddDef, nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);
  gddTypeDef->setDef(gddDef);
  char* cGddDef = XMLString::transcode(gddDef);
  gdd->pushNoImports(cGddDef);
  XMLString::release(&cGddDef);
  delete [] gddDef;

  itemStr = XMLString::transcode("access");
  gddTypeDef->setAccess(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);
}


//-----------------------------------------------------------------------------
void parseLocation(DOMNode* node,
                   DaDiLocation* gddLocation)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  gddLocation->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("place");
  gddLocation->setPlace(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("noQuote");
  XMLCh* boolStr = XMLString::transcode("TRUE");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->getNodeValue(),
                               boolStr) == 0)
  {
    gddLocation->setNoQuote(true);
  }
  else
  {
    gddLocation->setNoQuote(false);
  }
  XMLString::release(&boolStr);
  XMLString::release(&itemStr);
}


//-----------------------------------------------------------------------------
void parseArg(DOMNode* node,
              DaDiMethArgument* gddArg)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("type");
  XMLCh* argType = new XMLCh[XMLString::stringLen(nodeMap->
                                                  getNamedItem(itemStr)->
                                                  getNodeValue())+1];
  XMLString::copyString(argType, nodeMap->getNamedItem(itemStr)->
                        getNodeValue());
  XMLString::release(&itemStr);

  if (isPointer(argType))
  {
    gddArg->setIsPointer(true);
    XMLCh* tmpStr = new XMLCh[XMLString::stringLen(argType)+1];
    XMLString::subString(tmpStr, argType, 0, XMLString::stringLen(argType)-1);
    gddArg->setType(tmpStr);
    delete [] tmpStr;
    //    gddArg->setType(argType.substringData(0,argType.length()-1));
  }
  else
  {
    gddArg->setIsPointer(false);
    gddArg->setType(argType);
  }

  delete [] argType;

  itemStr = XMLString::transcode("name");
  if (nodeMap->getNamedItem(itemStr) != 0)
  {
    gddArg->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddArg->setName(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("const");
  XMLCh* boolStr = XMLString::transcode("TRUE");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->
                               getNodeValue(), boolStr) == 0)
  {
    gddArg->setConst_(true);
  }
  else
  {
    gddArg->setConst_(false);
  }
  XMLString::release(&itemStr);
  XMLString::release(&boolStr);

  itemStr = XMLString::transcode("inout");
  gddArg->setInout(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);
}


//-----------------------------------------------------------------------------
template <class T> void parseArgList(DOMNode* node,
                                     T* gddElement,
                                     std::string methName)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();
  std::vector<XMLCh*> argList, argInOut;
  std::vector<XMLCh*>::iterator xmlchIter;

  XMLCh* itemStr = XMLString::transcode("argList");
  if (nodeMap->getNamedItem(itemStr) != 0)
  {
    XMLCh* del = XMLString::transcode(",");
    argList = findWords(nodeMap->getNamedItem(itemStr)->getNodeValue(), *del);
    XMLString::release(&del);

    for (std::vector<XMLCh*>::iterator iterL = argList.begin();
         iterL != argList.end(); ++iterL)
    {
      XMLCh *argType = XMLString::transcode(""),
        *argEType = XMLString::transcode(""),
        *space = XMLString::transcode(" ");
      std::vector<XMLCh*> argWords = findWords(*iterL, *space);
      XMLString::release(&space);

      if (argWords.size() < 2)
      {
        char* cClassName = XMLString::transcode(gddClass->name());
        std::cerr << argV0
                  << ": Error in 'argList'-description of method "
                  << methName << " (Class: " << cClassName
                  << "), you have to provide at least a type-name-pair"
                  << std::endl;
        XMLString::release(&cClassName);
        exit(1);
      }

      DaDiMethArgument* gddArg = new DaDiMethArgument();
      gddElement->pushDaDiMethArgument(gddArg);

      XMLCh* tmpCh = argWords[argWords.size()-1];
      gddArg->setName(tmpCh);
      argWords.pop_back();
      delete [] tmpCh;

      tmpCh = argWords[argWords.size()-1];
      XMLString::release(&argType);
      argType = XMLString::replicate(tmpCh);
      argWords.pop_back();
      delete [] tmpCh;


      if (isPointer(argType))
      {
        XMLString::subString(argType,argType,0,XMLString::stringLen(argType)-1);
        //argType.deleteData(argType.length()-1,1);
        gddArg->setIsPointer(true);
      }
      else
      {
        gddArg->setIsPointer(false);
      }

      gddArg->setConst_(false);
      for (std::vector<XMLCh*>::iterator iterW = argWords.begin();
           iterW != argWords.end(); ++iterW)
      {
        XMLCh* cmpStr = XMLString::transcode("const");
        if (XMLString::compareString(*iterW, cmpStr) == 0)
        {
          gddArg->setConst_(true);
        }
        else
        {
          XMLCh* space2 = XMLString::transcode(" ");
          XMLString::release(&argEType);
		  argEType = XMLString::replicate(*iterW);
          //argEType = new XMLCh[XMLString::stringLen(*iterW)+2];
          //XMLString::copyString(argEType,*iterW);
          //    argEType = (XMLCh*)realloc(argEType,(
          //        xercesc::XMLString::stringLen(*iterW)+10)*sizeof(XMLCh));
          //   XMLString::catString(argEType,*iterW);
          XMLString::catString(argEType, space2);
          XMLString::release(&space2);
        }
        XMLString::release(&cmpStr);
      }
      //      argEType = (XMLCh*)realloc(argEType,(
      //         xercesc::XMLString::stringLen(argEType) +
      //     xercesc::XMLString::stringLen(argType) + 1)*sizeof(XMLCh));
      XMLCh* argSType = new XMLCh[XMLString::stringLen(argEType) +
                                  XMLString::stringLen(argType) + 1];
      XMLString::copyString(argSType, argEType);
      XMLString::catString(argSType, argType);
      gddArg->setType(argSType);
      delete [] argSType;
	  XMLString::release(&argEType);

      char* cArgType = XMLString::transcode(argType);
      if (!DaDiTools::isSimple(cArgType) && !gddArg->isPointer())
      {
        gddArg->setConst_(true);
      }
      XMLString::release(&cArgType);
      XMLString::release(&argType);
      //
      // handling of Input/Output arguments here !!!!
      //
      XMLCh* cStr = XMLString::transcode("INPUT");
      gddArg->setInout(cStr);
      XMLString::release(&cStr);
      for (xmlchIter=argWords.begin(); xmlchIter!=argWords.end(); ++xmlchIter)
      {
        delete [] *xmlchIter;
      }
    }
    for (xmlchIter = argList.begin(); xmlchIter != argList.end(); ++xmlchIter)
    {
      delete [] *xmlchIter;
    }

  }
  XMLString::release(&itemStr);
}


//-----------------------------------------------------------------------------
void parseMethod(DOMNode* node,
                 DaDiMethod* gddMethod,
                 DaDiClass* gddClass)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  gddMethod->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("desc");
  gddMethod->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("template");
  if(nodeMap->getNamedItem(itemStr) != 0)
  {
    gddMethod->setTemplate(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddMethod->setTemplate(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("access");
  gddMethod->setAccess(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("const");
  XMLCh* trueStr = XMLString::transcode("TRUE");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->getNodeValue(),
                               trueStr) == 0)
  {
    gddMethod->setConst_(true);
  }
  else
  {
    gddMethod->setConst_(false);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("virtual");
  gddMethod->setVirtual_(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  XMLCh* pureStr = XMLString::transcode("PURE");
  if (XMLString::compareString(gddMethod->virtual_(), pureStr) == 0 &&
      gddClass != 0)
  {
    gddClass->setAbstract(true);
  }
  XMLString::release(&pureStr);

  itemStr = XMLString::transcode("static");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->getNodeValue(),
                               trueStr) == 0)
  {
    gddMethod->setStatic_(true);
  }
  else
  {
    gddMethod->setStatic_(false);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("inline");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->getNodeValue(),
                               trueStr) == 0)
  {
    gddMethod->setInline_(true);
  }
  else
  {
    gddMethod->setInline_(false);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("friend");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->getNodeValue(),
                               trueStr) == 0)
  {
    gddMethod->setFriend_(true);
  }
  else
  {
    gddMethod->setFriend_(false);
  }
  XMLString::release(&itemStr);

  DaDiMethReturn* gddMethReturn = new DaDiMethReturn();
  gddMethReturn->setConst_(false);

  itemStr = XMLString::transcode("type");
  if (nodeMap->getNamedItem(itemStr) != 0)
  {
    gddMethod->setDaDiMethReturn(gddMethReturn);

    char* cTypeStr = XMLString::transcode(nodeMap->getNamedItem(itemStr)->
                                          getNodeValue());
    std::string typeStr = std::string(cTypeStr);
    XMLString::release(&cTypeStr);

    if (typeStr.find("const ") == std::string::npos)
    {
      gddMethReturn->setType(nodeMap->getNamedItem(itemStr)->getNodeValue());
    }
    else
    {
      XMLCh* xmlType = XMLString::transcode(typeStr.substr
                                            (typeStr.find(' ')+1,
                                             std::string::npos).c_str());
      gddMethReturn->setType(xmlType);
      XMLString::release(&xmlType);
      gddMethReturn->setConst_(true);
    }
  }
  else
  {
    gddMethReturn->setType(0);
  }
  XMLString::release(&itemStr);

  char* cMethodName = XMLString::transcode(gddMethod->name());
  parseArgList(node, gddMethod, cMethodName);
  XMLString::release(&cMethodName);

  //
  // Child Elements of Method
  //
  DOMNode* met_child;
  met_child = node->getFirstChild();

  bool codeSet = false;
  while(met_child)
  {
    switch(met_child->getNodeType())
    {
    case DOMNode::ELEMENT_NODE:
      {
        DOMNamedNodeMap* childMap = met_child->getAttributes();

        XMLCh* cmpStr = XMLString::transcode("return");
        if(XMLString::compareString(met_child->getNodeName(), cmpStr) == 0)
        {
          DaDiMethReturn* gddMethReturn = new DaDiMethReturn();

          gddMethod->setDaDiMethReturn(gddMethReturn);

          itemStr = XMLString::transcode("type");
          gddMethReturn->setType(childMap->getNamedItem(itemStr)->
                                 getNodeValue());
          XMLString::release(&itemStr);

          itemStr = XMLString::transcode("const");
          if (XMLString::compareString(childMap->getNamedItem(itemStr)->
                                       getNodeValue(), trueStr) == 0)
          {
            gddMethReturn->setConst_(true);
          }
          else
          {
            gddMethReturn->setConst_(false);
          }
          XMLString::release(&itemStr);
        }
        XMLString::release(&cmpStr);

        cmpStr = XMLString::transcode("code");
        if(XMLString::compareString(met_child->getNodeName(), cmpStr) == 0)
        {
          if (met_child->getFirstChild())
          {
            gddMethod->setCode(met_child->getFirstChild()->getNodeValue());
          }
          else
          {
            XMLCh* empty = XMLString::transcode(" ");
            gddMethod->setCode(empty);
            XMLString::release(&empty);
          }
          codeSet = true;
        }
        XMLString::release(&cmpStr);

        cmpStr = XMLString::transcode("arg");
        if(XMLString::compareString(met_child->getNodeName(), cmpStr) == 0)
        {
          DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
          gddMethod->pushDaDiMethArgument(gddMethArgument);
          parseArg(met_child, gddMethArgument);
        }
        XMLString::release(&cmpStr);
      }
    default:
      {
        met_child = met_child->getNextSibling();
      }
    }
  }
  if (!codeSet) gddMethod->setCode(0);

  XMLString::release(&trueStr);
}


//-----------------------------------------------------------------------------
void parseConstructor(DOMNode* node,
                      DaDiConstructor *gddConstructor)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("desc");
  gddConstructor->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("initList");
  if (nodeMap->getNamedItem(itemStr) != 0)
  {
    gddConstructor->setInitList(nodeMap->getNamedItem(itemStr)->getNodeValue());
    //    XMLCh* empty = XMLString::transcode(" ");
    //    gddConstructor->setCode(empty);
    //    XMLString::release(&empty);
  }
  else
  {
    gddConstructor->setInitList(0);
  }
  XMLString::release(&itemStr);

  parseArgList(node, gddConstructor, "Constructor");

  //
  // Child Elements of Constructor
  //

  DOMNode* met_child;
  met_child = node->getFirstChild();

  bool codeSet = false;
  while(met_child != 0)
  {
    switch(met_child->getNodeType())
    {
    case DOMNode::ELEMENT_NODE:
      {
        XMLCh* cmpStr = XMLString::transcode("code");
        if(XMLString::compareString(met_child->getNodeName(), cmpStr) == 0)
        {
          if (met_child->getFirstChild() != 0)
          {
            gddConstructor->setCode(met_child->getFirstChild()->getNodeValue());
          }
          else
          {
            XMLCh* empty = XMLString::transcode(" ");
            gddConstructor->setCode(empty);
            XMLString::release(&empty);
          }
          codeSet = true;
        }
        XMLString::release(&cmpStr);

        cmpStr = XMLString::transcode("arg");
        if(XMLString::compareString(met_child->getNodeName(), cmpStr) == 0)
        {
          DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
          gddConstructor->pushDaDiMethArgument(gddMethArgument);
          parseArg(met_child, gddMethArgument);
        }
        XMLString::release(&cmpStr);
      }
    default:
      {
        met_child = met_child->getNextSibling();
      }
    }
  }
  if (!codeSet) gddConstructor->setCode(0);
}


//-----------------------------------------------------------------------------
void parseDestructor(DOMNode* node,
                     DaDiDestructor* gddDestructor)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();
  XMLCh* empty = XMLString::transcode("");

  XMLCh* itemStr = XMLString::transcode("desc");
  if (nodeMap->getNamedItem(itemStr) != 0)
  {
    gddDestructor->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddDestructor->setDesc(empty);
  }
  XMLString::release(&itemStr);

  parseArgList(node, gddDestructor, "Destructor");

  //
  // Child Elements of Destructor
  //

  DOMNode* met_child;
  met_child = node->getFirstChild();

  bool codeSet = false;
  while(met_child != 0)
  {
    switch(met_child->getNodeType())
    {
    case DOMNode::ELEMENT_NODE:
      {
        XMLCh* cmpStr = XMLString::transcode("code");
        if(XMLString::compareString(met_child->getNodeName(), cmpStr) == 0)
        {
          if (met_child->getFirstChild() != 0)
          {
            gddDestructor->setCode(met_child->getFirstChild()->getNodeValue());
          }
          else
          {
            XMLCh* empty = XMLString::transcode(" ");
            gddDestructor->setCode(empty);
            XMLString::release(&empty);
          }
          codeSet = true;
        }
        XMLString::release(&cmpStr);

        cmpStr = XMLString::transcode("arg");
        if(XMLString::compareString(met_child->getNodeName(), cmpStr) == 0)
        {
          DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
          gddDestructor->pushDaDiMethArgument(gddMethArgument);
          parseArg(met_child, gddMethArgument);
        }
        XMLString::release(&cmpStr);
      }
    default:
      {
        met_child = met_child->getNextSibling();
      }
    }
  }
  if (!codeSet) gddDestructor->setCode(empty);
  XMLString::release(&empty);
}

//-----------------------------------------------------------------------------
void parseBitfield(DOMNode* node,
                   DaDiBitfield* gddBitfield)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  gddBitfield->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("desc");
  gddBitfield->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("length");
  gddBitfield->setLength(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("getMeth");
  gddBitfield->setGetMeth(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("setMeth");
  gddBitfield->setSetMeth(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);
}

//-----------------------------------------------------------------------------
void parseAssociation(DOMNode* node,
                      DaDiAssociation* gddAssociation)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("to");
  gddAssociation->setDestination(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("type");
  gddAssociation->setType(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("weight");
  gddAssociation->setWeight(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);
}

//-----------------------------------------------------------------------------
template<class T> void parseAttribute(DOMNode* node,
                                      DaDiAttribute* gddAttribute,
                                      T* gdd)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  std::vector<XMLCh*> typeWords;
  std::vector<XMLCh*>::iterator iter;
  gddAttribute->setStatic_(false);

  XMLCh* itemStr = XMLString::transcode("name");
  gddAttribute->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("array");
  gddAttribute->setArray(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("type");
  XMLCh* gddAttType = new XMLCh[XMLString::stringLen(nodeMap->
                                                     getNamedItem(itemStr)->
                                                     getNodeValue())+1];
  XMLString::copyString(gddAttType, nodeMap->getNamedItem(itemStr)->
                        getNodeValue());
  XMLString::release(&itemStr);

  XMLCh* space = XMLString::transcode(" ");
  typeWords = findWords(gddAttType, *space);
  XMLString::release(&space);

  XMLCh* cmpStr = XMLString::transcode("static");
  for (iter = typeWords.begin(); iter != typeWords.end(); ++iter)
  {
    if (XMLString::compareString(*iter, cmpStr) == 0)
    {
      gddAttribute->setStatic_(true);
    }
    delete [] *iter;
  }
  XMLString::release(&cmpStr);

  cmpStr = XMLString::transcode("bitfield");
  XMLCh* cmpStr8  = XMLString::transcode("bitfield8");
  XMLCh* cmpStr16 = XMLString::transcode("bitfield16");
  XMLCh* cmpStr32 = XMLString::transcode("bitfield32");
  XMLCh* cmpStr64 = XMLString::transcode("bitfield64");
  if (XMLString::compareString(gddAttType, cmpStr8) == 0)
  {
    XMLCh* typeStr = XMLString::transcode("unsigned char");
    gddAttribute->setType(typeStr);
    gddAttribute->setBitset(8);
    XMLString::release(&typeStr);
  }
  else if (XMLString::compareString(gddAttType, cmpStr16) == 0)
  {
    XMLCh* typeStr = XMLString::transcode("unsigned short");
    gddAttribute->setType(typeStr);
    gddAttribute->setBitset(16);
    XMLString::release(&typeStr);
  }
  else if ((XMLString::compareString(gddAttType, cmpStr32) == 0) ||
           (XMLString::compareString(gddAttType, cmpStr) == 0))
  {
    XMLCh* typeStr = XMLString::transcode("unsigned");
    gddAttribute->setType(typeStr);
    gddAttribute->setBitset(32);
    XMLString::release(&typeStr);
  }
  else if (XMLString::compareString(gddAttType, cmpStr64) == 0)
  {
    XMLCh* typeStr = XMLString::transcode("unsigned longlong");
    gddAttribute->setType(typeStr);
    gddAttribute->setBitset(64);
    XMLString::release(&typeStr);
  }
  else
  {
    gddAttribute->setType(gddAttType);
    char* cGddAttType = XMLString::transcode(gddAttType);
    gdd->pushImportList(cGddAttType);
    gddAttribute->setBitset(0);
    XMLString::release(&cGddAttType);
  }
  XMLString::release(&cmpStr);
  XMLString::release(&cmpStr8);
  XMLString::release(&cmpStr16);
  XMLString::release(&cmpStr32);
  XMLString::release(&cmpStr64);
  delete [] gddAttType;

  /*
    gddAttribute->setType(node.getAttributes().
    getNamedItem(XMLString::transcode("type")).
    getNodeValue());

    gdd->pushImportList(node.getAttributes().
    getNamedItem(XMLString::transcode("type")).
    getNodeValue().transcode());

    if (node.getAttributes().
    getNamedItem(XMLString::transcode("bitset")).
    getNodeValue().equals("TRUE"))
    {
    gddAttribute->setBitset(true);
    }
    else
    {
    gddAttribute->setBitset(false);
    }
  */

  itemStr = XMLString::transcode("desc");
  if (nodeMap->getNamedItem(itemStr))
  {
    gddAttribute->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddAttribute->setDesc(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("access");
  gddAttribute->setAccess(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("setMeth");
  gddAttribute->setSetMeth(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("getMeth");
  gddAttribute->setGetMeth(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("init");
  if(nodeMap->getNamedItem(itemStr) != 0)
  {
    gddAttribute->setInit(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddAttribute->setInit(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("dictalias");
  if(nodeMap->getNamedItem(itemStr) != 0)
  {
    gddAttribute->setDictalias(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddAttribute->setDictalias(0);
  }
  XMLString::release(&itemStr);

  cmpStr = XMLString::transcode("compression");
  XMLCh* trueStr = XMLString::transcode("TRUE");
  if (XMLString::compareString(nodeMap->getNamedItem(cmpStr)->getNodeValue(),
                               trueStr) == 0)
  {
    gddAttribute->setCompression(true);
  }
  else
  {
    gddAttribute->setCompression(false);
  }
  XMLString::release(&cmpStr);

  cmpStr = XMLString::transcode("serialize");
  if (XMLString::compareString(nodeMap->getNamedItem(cmpStr)->getNodeValue(),
                               trueStr) == 0)
  {
    gddAttribute->setSerialize(true);
  }
  else
  {
    gddAttribute->setSerialize(false);
  }
  XMLString::release(&cmpStr);

  cmpStr = XMLString::transcode("transient");
  if (XMLString::compareString(nodeMap->getNamedItem(cmpStr)->getNodeValue(),
                               trueStr) == 0)
  {
    gddAttribute->setTransient(true);
  }
  else
  {
    gddAttribute->setTransient(false);
  }
  XMLString::release(&cmpStr);

  node = node->getFirstChild();

  while(node != 0)
  {
    switch(node->getNodeType())
    {
    case DOMNode::ELEMENT_NODE:
      {
        //
        // parse Bitfield
        //
        cmpStr = XMLString::transcode("bitfield");
        if(XMLString::compareString(node->getNodeName(), cmpStr) == 0)
        {
          DaDiBitfield* gddBitfield = new DaDiBitfield();
          gddAttribute->pushDaDiBitfield(gddBitfield);
          parseBitfield(node,gddBitfield);
        }
        else
        {
          node = node->getNextSibling();
        }
        XMLString::release(&cmpStr);
      }
    default:
      {
        node = node->getNextSibling();
      }
    }
  }
  XMLString::release(&trueStr);
}


//-----------------------------------------------------------------------------
void parseRelation(DOMNode* node,
                   DaDiRelation* gddRelation)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* gddXMLRelationRatio = new XMLCh[2];
  XMLCh* gddRelationRatio = new XMLCh[2];

  XMLCh* itemStr = XMLString::transcode("name");
  gddRelation->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("type");
  XMLCh* type = new XMLCh[XMLString::stringLen(nodeMap->getNamedItem(itemStr)->
                                               getNodeValue())+1];
  XMLString::copyString(type,nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  gddRelation->setType(type);

  char* cType = XMLString::transcode(type);
  gddClass->pushImpSoftList(cType);
  XMLString::release(&cType);
  delete [] type;

  XMLCh* cmpStr = XMLString::transcode("serialize");
  XMLCh* trueStr = XMLString::transcode("TRUE");
  if (XMLString::compareString(nodeMap->getNamedItem(cmpStr)->getNodeValue(),
                               trueStr) == 0)
  {
    gddRelation->setSerialize(true);
  }
  else
  {
    gddRelation->setSerialize(false);
  }
  XMLString::release(&cmpStr);
  XMLString::release(&trueStr);

  itemStr = XMLString::transcode("desc");
  if (nodeMap->getNamedItem(itemStr) != 0)
  {
    gddRelation->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddRelation->setDesc(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("access");
  gddRelation->setAccess(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  XMLCh* one = XMLString::transcode("1");
  itemStr = XMLString::transcode("multiplicity");
  if (nodeMap->getNamedItem(itemStr) != 0)
  {
    XMLString::copyString(gddXMLRelationRatio,nodeMap->getNamedItem(itemStr)->
                          getNodeValue());
    XMLCh *smalln = XMLString::transcode("n"),
      *bigN = XMLString::transcode("N"),
      *smallm = XMLString::transcode("m"),
      *bigM = XMLString::transcode("M");
    if ((XMLString::compareString(gddXMLRelationRatio, smalln) == 0) ||
        (XMLString::compareString(gddXMLRelationRatio, bigN) == 0) ||
        (XMLString::compareString(gddXMLRelationRatio, smallm) == 0) ||
        (XMLString::compareString(gddXMLRelationRatio, bigM) == 0))
    {
      XMLCh* star = XMLString::transcode("*");
      XMLString::copyString(gddRelationRatio,star);
      XMLString::release(&star);
    }
    else if (XMLString::compareString(gddXMLRelationRatio, one) == 0)
    {
      XMLString::copyString(gddRelationRatio,one);
    }
    else
    {
      char* cRatio = XMLString::transcode(gddXMLRelationRatio);
      std::cerr << argV0 << ": \"" << cRatio
                << "\" is not a valid value for RELATION-Attribute "
                << "\"multiplicity\"" << std::endl << "This error should never"
                << " happen!!!! Maybe you've changed the DTD?" << std::endl;
      XMLString::release(&cRatio);
      exit(1);
    }
    XMLString::release(&smalln);
    XMLString::release(&bigN);
    XMLString::release(&smallm);
    XMLString::release(&bigM);
  }
  else
  {
    XMLString::copyString(gddRelationRatio,one);
  }
  XMLString::release(&itemStr);
  XMLString::release(&one);


  gddRelation->setRatio(gddRelationRatio);

  itemStr = XMLString::transcode("setMeth");
  gddRelation->setSetMeth(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("getMeth");
  gddRelation->setGetMeth(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  cmpStr = XMLString::transcode("*");
  if (XMLString::compareString(gddRelationRatio, cmpStr) == 0)
  {
    itemStr = XMLString::transcode("addMeth");
    gddRelation->setAddMeth(nodeMap->getNamedItem(itemStr)->getNodeValue());
    XMLString::release(&itemStr);

    itemStr = XMLString::transcode("remMeth");
    XMLCh* relRemMeth = new XMLCh[XMLString::stringLen
                                  (nodeMap->getNamedItem
                                   (itemStr)->getNodeValue())+1];
    XMLString::copyString(relRemMeth,nodeMap->getNamedItem(itemStr)->
                          getNodeValue());
    XMLString::release(&itemStr);
    gddRelation->setRemMeth(relRemMeth);

    XMLCh* cmpStr2 = XMLString::transcode("TRUE");
    if (XMLString::compareString(relRemMeth, cmpStr2) == 0)
    {
      gddClass->pushImpStdList("algorithm");
    }
    XMLString::release(&cmpStr2);
    delete [] relRemMeth;
  }
  else
  {
    XMLCh* falseStr = XMLString::transcode("FALSE");
    gddRelation->setAddMeth(falseStr);
    gddRelation->setRemMeth(falseStr);
    XMLString::release(&falseStr);
  }
  XMLString::release(&cmpStr);

  itemStr = XMLString::transcode("clrMeth");
  gddRelation->setClrMeth(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  cmpStr = XMLString::transcode("1");
  if (XMLString::compareString(gddRelationRatio, cmpStr) == 0)
  {
    gddClass->pushImportList("SmartRef");
  }
  XMLString::release(&cmpStr);

  cmpStr = XMLString::transcode("*");
  if (XMLString::compareString(gddRelationRatio, cmpStr) == 0)
  {
    gddClass->pushImportList("SmartRef");
    gddClass->pushImportList("SmartRefVector");
  }
  XMLString::release(&cmpStr);
  delete [] gddXMLRelationRatio;
  delete [] gddRelationRatio;
}


//-----------------------------------------------------------------------------
void parseClass(DOMNode* node,
                DaDiClass* gddClass)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  bool classHasDesc = false;

  XMLCh* itemStr = XMLString::transcode("name");
  gddClass->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("location");
  if (nodeMap->getNamedItem(itemStr))
  {
    gddClass->setLocation(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddClass->setLocation(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("desc");
  if (nodeMap->getNamedItem(itemStr))
  {
    gddClass->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
    classHasDesc = true;
  }
  else
  {
    gddClass->setDesc(0);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("author");
  gddClass->setAuthor(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("templateVector");
  XMLCh* boolStr = XMLString::transcode("TRUE");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->
                               getNodeValue(), boolStr) == 0)
  {
    gddClass->setClassTemplateVector(true);
  }
  else
  {
    gddClass->setClassTemplateVector(false);
  }
  XMLString::release(&itemStr);

  if (gddClass->classTemplateVector())
  {
    gddClass->pushImportList("ObjectVector");
  }

  itemStr = XMLString::transcode("templateList");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->
                               getNodeValue(), boolStr) == 0)
  {
    gddClass->setClassTemplateList(true);
  }
  else
  {
    gddClass->setClassTemplateList(false);
  }
  XMLString::release(&itemStr);

  if (gddClass->classTemplateList())
  {
    gddClass->pushImportList("ObjectList");
  }

  itemStr = XMLString::transcode("serializers");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->
                               getNodeValue(), boolStr) == 0)
  {
    gddClass->setSerializers(true);
  }
  else
  {
    gddClass->setSerializers(false);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("stdVectorTypeDef");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->
                               getNodeValue(), boolStr) == 0)
  {
    gddClass->setStdVectorTypeDef(true);
  }
  else
  {
    gddClass->setStdVectorTypeDef(false);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("keyedContTypeDef");
  if (XMLString::compareString(nodeMap->getNamedItem(itemStr)->
                               getNodeValue(), boolStr) == 0)
  {
    gddClass->setKeyedContTypeDef(true);
  }
  else
  {
    gddClass->setKeyedContTypeDef(false);
  }
  XMLString::release(&itemStr);
  XMLString::release(&boolStr);

  itemStr = XMLString::transcode("id");
  if(nodeMap->getNamedItem(itemStr) != 0)
  {
    gddClass->setID(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    gddClass->setID(0);
  }
  XMLString::release(&itemStr);

  node = node->getFirstChild();


  XMLCh *descStr = XMLString::transcode("desc"),
    *importStr = XMLString::transcode("import"),
    *baseStr = XMLString::transcode("base"),
    *classStr = XMLString::transcode("class"),
    *locationStr = XMLString::transcode("location"),
    *enumStr = XMLString::transcode("enum"),
    *typedefStr = XMLString::transcode("typedef"),
    *methodStr = XMLString::transcode("method"),
    *constructorStr = XMLString::transcode("constructor"),
    *destructorStr = XMLString::transcode("destructor"),
    *attributeStr = XMLString::transcode("attribute"),
    *relationStr = XMLString::transcode("relation"),
    *associationStr = XMLString::transcode("assoc"),
    *templateStr = XMLString::transcode("template");

  bool longDescSet = false;
  while(node != 0)
  {
    switch(node->getNodeType())
    {
    case DOMNode::ELEMENT_NODE:
      {
        //
        // Parse description
        //
        if(XMLString::compareString(node->getNodeName(), descStr) == 0)
        {
          XMLCh* description = XMLString::transcode("");
          if (node->getFirstChild() != 0)
          {
            XMLString::release(&description);
            description = XMLString::replicate(node->getFirstChild()->
                                               getNodeValue());
            //classHasDesc = true;
          }
          /* fixme
             if (gddClass->longDesc() != 0)
             {
             XMLString::catString(description, gddClass->longDesc());
             }
          */
          gddClass->setLongDesc(description);
          XMLString::release(&description);
          longDescSet = true;
        }
        //
        // ParseImport
        //
        if(XMLString::compareString(node->getNodeName(), importStr) == 0)
        {
          parseImport(node,gddClass);
        }
        //
        // Parse baseclasses
        //
        if(XMLString::compareString(node->getNodeName(), baseStr) == 0)
        {
          DaDiBaseClass* gddBaseClass = new DaDiBaseClass();
          gddClass->pushDaDiBaseClass(gddBaseClass);
          parseBaseClass(node, gddBaseClass);
        }

        //
        // Parse inner classes
        //
        /*
          if(XMLString::compareString(node->getNodeName(), classStr) == 0)
          {
          DaDiClass* gddInnerClass = new DaDiClass(true);
          gddClass->pushDaDiInnerClass(gddInnerClass);
          parseClass(node, gddInnerClass);
          }
        */
        //
        // Parse locations (of classes in the TES)
        //
        if(XMLString::compareString(node->getNodeName(), locationStr) == 0)
        {
          DaDiLocation* gddLocation = new DaDiLocation();
          gddClass->pushDaDiLocation(gddLocation);
          parseLocation(node, gddLocation);
        }

        //
        // Parse enums
        //
        if(XMLString::compareString(node->getNodeName(), enumStr) == 0)
        {
          DaDiEnum* gddEnum = new DaDiEnum();
          gddClass->pushDaDiEnum(gddEnum);
          parseEnum(node, gddEnum, gddClass);
        }

        //
        // Parse typedef
        //
        if(XMLString::compareString(node->getNodeName(), typedefStr) == 0)
        {
          DaDiTypeDef* gddTypeDef = new DaDiTypeDef();
          gddClass->pushDaDiTypeDef(gddTypeDef);
          parseTypeDef(node, gddTypeDef, gddClass);
        }

        //
        // Parse methods
        //
        if(XMLString::compareString(node->getNodeName(), methodStr) == 0)
        {
          DaDiMethod* gddMethod = new DaDiMethod();
          gddClass->pushDaDiMethod(gddMethod);
          parseMethod(node, gddMethod, gddClass);
        }

        //
        // Parse constructors
        //
        if(XMLString::compareString(node->getNodeName(), constructorStr) == 0)
        {
          DaDiConstructor* gddConstructor = new DaDiConstructor();
          gddClass->pushDaDiConstructor(gddConstructor);
          parseConstructor(node, gddConstructor);
        }

        //
        // Parse destructors
        //
        if(XMLString::compareString(node->getNodeName(), destructorStr) == 0)
        {
          DaDiDestructor* gddDestructor = new DaDiDestructor();
          gddClass->pushDaDiDestructor(gddDestructor);
          parseDestructor(node, gddDestructor);
        }

        //
        // Parse attributes
        //
        if(XMLString::compareString(node->getNodeName(), attributeStr) == 0)
        {
          DaDiAttribute* gddAttribute = new DaDiAttribute();
          gddClass->pushDaDiAttribute(gddAttribute);
          parseAttribute(node, gddAttribute, gddClass);
        }

        //
        // Parse relations
        //
        if(XMLString::compareString(node->getNodeName(), relationStr) == 0)
        {
          DaDiRelation* gddRelation = new DaDiRelation();
          gddClass->pushDaDiRelation(gddRelation);
          parseRelation(node, gddRelation);
        }

        //
        // Parse associations
        //
        if(XMLString::compareString(node->getNodeName(), associationStr) == 0)
        {
          DaDiAssociation* gddAssociation = new DaDiAssociation();
          gddClass->pushDaDiAssociation(gddAssociation);
          parseAssociation(node, gddAssociation);
        }

        //
        // Parse templates
        //
        if(XMLString::compareString(node->getNodeName(), templateStr) == 0)
        {
          DaDiTemplate* gddTemplate = new DaDiTemplate();
          gddClass->pushDaDiTemplate(gddTemplate);
          parseTemplate(node, gddTemplate);
        }
	
        else
        {
          node = node->getNextSibling();
        }
      }
    default:
      {
        node = node->getNextSibling();
      }
    }
  }
  if (!longDescSet) gddClass->setLongDesc(0);

  XMLString::release(&descStr);
  XMLString::release(&importStr);
  XMLString::release(&baseStr);
  XMLString::release(&classStr);
  XMLString::release(&locationStr);
  XMLString::release(&enumStr);
  XMLString::release(&typedefStr);
  XMLString::release(&methodStr);
  XMLString::release(&constructorStr);
  XMLString::release(&destructorStr);
  XMLString::release(&attributeStr);
  XMLString::release(&relationStr);
  XMLString::release(&associationStr);
  XMLString::release(&templateStr);

  if (!classHasDesc)
  {
    char* cClassName = XMLString::transcode(gddClass->name());
    std::cerr << "Class " << cClassName
              << " has no description, please add one to the xml-file"
              << std::endl;
    XMLString::release(&cClassName);
    exit(1);
  }
}


//-----------------------------------------------------------------------------
void parseNamespace(DOMNode* node,
                    DaDiNamespace* gddNamespace)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  gddNamespace->setName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("author");
  if (nodeMap->getNamedItem(itemStr) != 0)
  {
    gddNamespace->setAuthor(nodeMap->getNamedItem(itemStr)->getNodeValue());
  }
  else
  {
    XMLCh* emptyStr = XMLString::transcode("");
    gddNamespace->setAuthor(emptyStr);
    XMLString::release(&emptyStr);
  }
  XMLString::release(&itemStr);

  itemStr = XMLString::transcode("desc");
  gddNamespace->setDesc(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  gddNamespace->setLongDesc(0);

  node = node->getFirstChild();

  XMLCh *descStr = XMLString::transcode("desc"),
    *classStr = XMLString::transcode("class"),
    *importStr = XMLString::transcode("import"),
    *attributeStr = XMLString::transcode("attribute"),
    *methodStr = XMLString::transcode("method"),
    *enumStr = XMLString::transcode("enum"),
    *typedefStr = XMLString::transcode("typedef");
  while(node != 0)
  {
    switch(node->getNodeType())
    {
    case DOMNode::ELEMENT_NODE:
      {
        //
        // Parse description
        //
        if(XMLString::compareString(node->getNodeName(), descStr) == 0)
        {
          XMLCh* description = XMLString::transcode("");
          if (node->getFirstChild() != 0)
          {
            XMLString::copyString(description, node->getFirstChild()->
                                  getNodeValue());
          }
          if (gddNamespace->longDesc() != NULL)
          {
            XMLString::catString(description, gddNamespace->longDesc());
          }
          gddNamespace->setLongDesc(description);
          XMLString::release(&description);
        }

        //
        // Parse classes
        //
        if(XMLString::compareString(node->getNodeName(), classStr) == 0)
        {
          gddClass = new DaDiClass();
          gddNamespace->pushDaDiClass(gddClass);
          parseClass(node, gddClass);
        }

        //
        // ParseImport
        //
        if(XMLString::compareString(node->getNodeName(), importStr) == 0)
        {
          parseImport(node,gddNamespace);
        }

        //
        // Parse attributes
        //
        if(XMLString::compareString(node->getNodeName(), attributeStr) == 0)
        {
          DaDiAttribute* gddAttribute = new DaDiAttribute();
          gddNamespace->pushDaDiAttribute(gddAttribute);
          parseAttribute(node, gddAttribute, gddNamespace);
        }

        //
        // Parse methods
        //
        if(XMLString::compareString(node->getNodeName(), methodStr) == 0)
        {
          DaDiMethod* gddMethod = new DaDiMethod();
          gddNamespace->pushDaDiMethod(gddMethod);
          parseMethod(node, gddMethod, 0);
        }

        //
        // Parse enums
        //
        if(XMLString::compareString(node->getNodeName(), enumStr) == 0)
        {
          DaDiEnum* gddEnum = new DaDiEnum();
          gddNamespace->pushDaDiEnum(gddEnum);
          parseEnum(node, gddEnum, gddNamespace);
        }

        //
        // Parse typedefs
        //
        if(XMLString::compareString(node->getNodeName(), typedefStr) == 0)
        {
          DaDiTypeDef* gddTypeDef = new DaDiTypeDef();
          gddNamespace->pushDaDiTypeDef(gddTypeDef);
          parseTypeDef(node, gddTypeDef, gddNamespace);
        }
        else
        {
          node = node->getNextSibling();
        }

      }
    default:
      {
        node = node->getNextSibling();
      }
    }
  }
  XMLString::release(&descStr);
  XMLString::release(&classStr);
  XMLString::release(&importStr);
  XMLString::release(&attributeStr);
  XMLString::release(&methodStr);
  XMLString::release(&enumStr);
  XMLString::release(&typedefStr);

}


//-----------------------------------------------------------------------------
void parsePackage(DOMNode* node,
                  DaDiPackage* gddPackage)
//-----------------------------------------------------------------------------
{
  DOMNamedNodeMap* nodeMap = node->getAttributes();

  XMLCh* itemStr = XMLString::transcode("name");
  gddPackage->setPackageName(nodeMap->getNamedItem(itemStr)->getNodeValue());
  XMLString::release(&itemStr);

  node = node->getFirstChild();

  XMLCh *importStr = XMLString::transcode("import"),
    *classStr = XMLString::transcode("class"),
    *namespaceStr = XMLString::transcode("namespace");
  while(node != 0)
  {
    switch(node->getNodeType())
    {
    case DOMNode::ELEMENT_NODE:
      {
        if (XMLString::compareString(node->getNodeName(), importStr) == 0)
        {
          parseImport(node,gddPackage);
        }

        if (XMLString::compareString(node->getNodeName(), classStr) == 0)
        {
          gddClass = new DaDiClass();
          gddPackage->pushDaDiClass(gddClass);
          parseClass(node,gddClass);
        }

        if (XMLString::compareString(node->getNodeName(), namespaceStr) == 0)
        {
          DaDiNamespace*  gddNamespace = new DaDiNamespace();
          gddPackage->pushDaDiNamespace(gddNamespace);
          parseNamespace(node, gddNamespace);
        }
        else
        {
          node = node->getNextSibling();
        }
      }
    default:
      {
        node = node->getNextSibling();
      }
    }
  }
  XMLString::release(&importStr);
  XMLString::release(&classStr);
  XMLString::release(&namespaceStr);
}


//-----------------------------------------------------------------------------
void DDFE::DaDiFrontEnd(DaDiPackage* gddPackage, char* filename)
//-----------------------------------------------------------------------------
{

  //  DaDiPackage* gddPackage = new DaDiPackage();

  //  static char* gXmlFile = new char[256];

  //
  // Initialization of XML-Parser
  //
  try
  {
    XMLPlatformUtils::Initialize();
  }

  catch(const XMLException& toCatch)
  {
    std::cerr << "Error during Xerces-c Initialization.\n"
              << " Exception message: "
              << toCatch.getMessage() << std::endl;
    exit(1);
  }


  //  strcpy(gXmlFile,filename);

  XercesDOMParser *parser = new XercesDOMParser;
  parser->setValidationScheme(XercesDOMParser::Val_Auto);
  parser->setDoNamespaces(false);
  parser->setDoSchema(false);
  parser->setCreateEntityReferenceNodes(false);
  parser->setValidationSchemaFullChecking(false);
  //  parser->setToCreateXMLDeclTypeNode(true);

  //  ErrorHandler* errReporter = new DaDiTools();
  DaDiTools* daditools = new DaDiTools();
  ErrorHandler* errReporter = dynamic_cast<ErrorHandler*>(daditools);
  delete daditools;
  parser->setErrorHandler(errReporter);

  bool errorsOccured = false;

  //
  // parse file, catch errors
  //

  try
  {
    parser->parse(filename);
    //    parser->parse(gXmlFile);
  }

  catch(const XMLException& e)
  {
    std::cerr << "An error occured during parsing file " << filename //gXmlFile
              << "\n Message: " << e.getMessage() << std::endl;
    errorsOccured = true;
  }

  catch(const DOMException& e)
  {
    char* msg = XMLString::transcode(e.msg);
    std::cerr << "An error occured during parsing file " << filename //gXmlFile
              << "\n Message: " << msg << std::endl;
    errorsOccured = true;
    XMLString::release(&msg);
  }

  catch(...)
  {
    std::cerr << "An error occured during parsing file " << filename //gXmlFile
              << std::endl;
    errorsOccured=true;
  }

  if (errorsOccured)
  {
    std::cerr << argV0 << ": Errors occured, so exiting" << std::endl;
    exit(1);
  }
  //
  // If no errors occured start walking DOMtree
  //
  else
  {
    DOMNode* topelem = parser->getDocument();
    DOMNode* top = topelem->getFirstChild();

    XMLCh* cmpStr = XMLString::transcode("gdd");
    while (XMLString::compareString(top->getNodeName(), cmpStr) != 0)
    {
      //      std::cout << XMLString::transcode(top->getNodeName())
      //                << std::endl;
      top = top->getNextSibling();
    }
    XMLString::release(&cmpStr);

    DOMNode* gdd_node = top->getNextSibling();

    XMLCh* itemStr = XMLString::transcode("version");
    char *xmlVersion = XMLString::transcode(gdd_node->getAttributes()->
                                            getNamedItem(itemStr)->
                                            getNodeValue());
    XMLString::release(&itemStr);

    gdd_node = gdd_node->getFirstChild();



    XMLCh *importStr = XMLString::transcode("import"),
      *packageStr = XMLString::transcode("package"),
      *classStr = XMLString::transcode("class"),
      *nopackageStr = XMLString::transcode("__NO_PACKAGE__");
    while (gdd_node != 0)
    {
      if (XMLString::compareString(gdd_node->getNodeName(), importStr) == 0)
      {
        parseImport(gdd_node,gddPackage);
      }

      if (XMLString::compareString(gdd_node->getNodeName(), packageStr) == 0)
      {
        parsePackage(gdd_node,gddPackage);
      }

      if (XMLString::compareString(gdd_node->getNodeName(), classStr) == 0)
      {
        gddPackage->setPackageName(nopackageStr);
        gddClass = new DaDiClass();
        gddPackage->pushDaDiClass(gddClass);
        parseClass(gdd_node,gddClass);
      }
      gdd_node = gdd_node->getNextSibling();
    }
    XMLString::release(&importStr);
    XMLString::release(&packageStr);
    XMLString::release(&classStr);
    XMLString::release(&nopackageStr);
    XMLString::release(&xmlVersion);
  }
  //  delete [] gXmlFile;
  delete parser;
  //  return gddPackage;
}


