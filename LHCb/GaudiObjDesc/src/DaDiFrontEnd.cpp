// $Id: DaDiFrontEnd.cpp,v 1.34 2002-03-25 10:20:34 mato Exp $

//#include "GaudiKernel/Kernel.h"
#include "DaDiTools.h"
#include "DaDiFrontEnd.h"
#include "DaDiPackage.h"

#include "util/PlatformUtils.hpp"
#include "util/XMLUniDefs.hpp"
#include "parsers/DOMParser.hpp"
#include "dom/DOM_NodeList.hpp"
#include "dom/DOM_Element.hpp"
#include "dom/DOM_DOMException.hpp"
#include "dom/DOM_NamedNodeMap.hpp"
#include "dom/DOMString.hpp"

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

extern std::string argV0;

DaDiClass* gddClass;
std::string xmlVersion;


//-----------------------------------------------------------------------------
DOMString remSpaces(DOMString value)
//-----------------------------------------------------------------------------
{
  while (value.substringData(0,1).equals(" "))
  {
    value.deleteData(0,1);
  }
  while (value.substringData(value.length()-1,1).equals(" "))
  {
    value.deleteData(value.length()-1,1);
  }
  return value;
}


//-----------------------------------------------------------------------------
std::vector<DOMString> findWords(DOMString value, 
                                 DOMString delim)
//-----------------------------------------------------------------------------
{
  std::vector<DOMString> words;
  unsigned int i = 0, j = 0;

  for (i=0; i<value.length(); ++i)
  {
    if (value.substringData(i,1).equals(delim))
    {
      words.push_back(remSpaces(value.substringData(j, i-j)));
      j = i+1;
    }
  }
  words.push_back(remSpaces(value.substringData(j,i-j)));
  return words;
}


//-----------------------------------------------------------------------------
bool isPointer(DOMString value)
//-----------------------------------------------------------------------------
{
  value = remSpaces(value);
  return value.substringData(value.length()-1,1).equals("*");
}


//-----------------------------------------------------------------------------
template<class T> void parseImport(DOM_Node node, 
                                   T* element)
//-----------------------------------------------------------------------------
{  
  std::string import_name, import_soft, import_std, import_ignore;
  
  import_name = node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue().transcode();
  import_soft = node.getAttributes().
    getNamedItem(DOMString::transcode("soft")).getNodeValue().transcode();
  import_std = node.getAttributes().
    getNamedItem(DOMString::transcode("std")).getNodeValue().transcode();
  import_ignore = node.getAttributes().
    getNamedItem(DOMString::transcode("ignore")).getNodeValue().transcode();

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
        element->pushImpSoftList(import_name.substr(import_name.find_last_of("/")+1, std::string::npos));
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
        element->pushImportList(import_name.substr(import_name.find_last_of("/")+1, std::string::npos));
      }
    }
  }
}


//-----------------------------------------------------------------------------
void parseBaseClass(DOM_Node node,
                    DaDiBaseClass* gddBaseClass)
//-----------------------------------------------------------------------------
{
  gddBaseClass->setName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue().
    transcode());

  gddClass->pushImportList(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue().
    transcode());

  if (node.getAttributes().
      getNamedItem(DOMString::transcode("virtual")).getNodeValue().
      equals("TRUE"))
  {
    gddBaseClass->setVirtual_(true);
  }
  else
  {
    gddBaseClass->setVirtual_(false);
  }

  gddBaseClass->setAccess(node.getAttributes().
    getNamedItem(DOMString::transcode("access")).getNodeValue().
    transcode());
}


//-----------------------------------------------------------------------------
template<class T> void parseEnum(DOM_Node node,
                                 DaDiEnum* gddEnum,
                                 T* gdd)
//-----------------------------------------------------------------------------
{
  DOMString gddName;

  gddName = node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue();

  gddEnum->setName(gddName);
  gdd->pushNoImports(gddName.transcode());

  gddEnum->setDesc(node.getAttributes().
    getNamedItem(DOMString::transcode("desc")).getNodeValue().
    transcode());

  gddEnum->setValue(node.getAttributes().
    getNamedItem(DOMString::transcode("value")).getNodeValue().
    transcode());

  gddEnum->setAccess(node.getAttributes().
    getNamedItem(DOMString::transcode("access")).getNodeValue().
    transcode());
}


//-----------------------------------------------------------------------------
template<class T> void parseTypeDef(DOM_Node node,
                                    DaDiTypeDef* gddTypeDef,
                                    T* gdd)
//-----------------------------------------------------------------------------
{
  DOMString gddDef;

  gddTypeDef->setDesc(node.getAttributes().
    getNamedItem(DOMString::transcode("desc")).getNodeValue().
    transcode());

  gddTypeDef->setType(node.getAttributes().
    getNamedItem(DOMString::transcode("type")).getNodeValue().
    transcode());

  gddDef = node.getAttributes().
    getNamedItem(DOMString::transcode("def")).getNodeValue();

  gddTypeDef->setDef(gddDef);
  gdd->pushNoImports(gddDef.transcode());

  gddTypeDef->setAccess(node.getAttributes().
    getNamedItem(DOMString::transcode("access")).getNodeValue().
    transcode());

}


//-----------------------------------------------------------------------------
void parseLocation(DOM_Node node,
                   DaDiLocation* gddLocation)
//-----------------------------------------------------------------------------
{

  gddLocation->setName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue().
    transcode());

  gddLocation->setPlace(node.getAttributes().
    getNamedItem(DOMString::transcode("place")).getNodeValue().
    transcode());

  if (node.getAttributes().
    getNamedItem(DOMString::transcode("noQuote")).getNodeValue().
    equals("TRUE"))
  {
    gddLocation->setNoQuote(true);
  }
  else
  {
    gddLocation->setNoQuote(false);
  }
}


//-----------------------------------------------------------------------------
void parseArg(DOM_Node node,
              DaDiMethArgument* gddArg)
//-----------------------------------------------------------------------------
{
  DOMString argType = node.getAttributes().
    getNamedItem(DOMString::transcode("type")).getNodeValue();
        
  if (isPointer(argType))
  {
    gddArg->setIsPointer(true);
    gddArg->setType(argType.substringData(0,argType.length()-1));      
  }
  else
  {
    gddArg->setIsPointer(false);
    gddArg->setType(argType);
  }
                
  if (!node.getAttributes().getNamedItem(DOMString::transcode("name")).isNull())
  {
    gddArg->setName(node.getAttributes().
      getNamedItem(DOMString::transcode("name")).getNodeValue());
  }
  else
  {
    gddArg->setName(0);
  }

  if (node.getAttributes().getNamedItem(DOMString::transcode("const")).
      getNodeValue().equals("TRUE"))
  {
    gddArg->setConst_(true);
  }
  else
  {
    gddArg->setConst_(false);
  }

  gddArg->setInout(node.getAttributes().
    getNamedItem(DOMString::transcode("inout")).getNodeValue());
}


//-----------------------------------------------------------------------------
template <class T> void parseArgList(DOM_Node node,
                                     T* gddElement,
                                     std::string methName)
//-----------------------------------------------------------------------------
{
  std::vector<DOMString> argList, argInOut;
  if (!node.getAttributes().
      getNamedItem(DOMString::transcode("argList")).isNull())
  {
    argList = findWords(node.getAttributes().
      getNamedItem(DOMString::transcode("argList")).
      getNodeValue(), ",");

    for (std::vector<DOMString>::iterator iterL = argList.begin();
         iterL != argList.end(); ++iterL)     
    {
      DOMString argType = "", argEType = "";
      std::vector<DOMString> argWords = findWords(*iterL," ");

      if (argWords.size() < 2)
      {
        std::cerr << argV0 
          << ": Error in 'argList'-description of method "
          << methName << " (Class: " 
          << gddClass->name().transcode()
          << "), you have to provide at least a type-name-pair" 
          << std::endl;
        exit(1);
      }

      DaDiMethArgument* gddArg = new DaDiMethArgument();
      gddElement->pushDaDiMethArgument(gddArg);

      gddArg->setName(argWords[argWords.size()-1]);
      argWords.pop_back();

      argType = argWords[argWords.size()-1];
      argWords.pop_back();  

      if (isPointer(argType))
      {
        argType.deleteData(argType.length()-1,1);
        gddArg->setIsPointer(true);
      }
      else
      {
        gddArg->setIsPointer(false);
      }
			  
      gddArg->setConst_(false);
      for (std::vector<DOMString>::iterator iterW = argWords.begin();
           iterW != argWords.end(); ++iterW)
      {
        if (iterW->equals("const"))
        {
          gddArg->setConst_(true);
        }
        else
        {
          argEType += *iterW;
          argEType += " ";
        }
      }
      argEType += argType;
      gddArg->setType(argEType);


      if (!DaDiTools::isSimple(argType.transcode()) && !gddArg->isPointer())
			{
			  gddArg->setConst_(true);
			}
//
// handling of Input/Output arguments here !!!!
//
      gddArg->setInout("INPUT");

    }
  }
}


//-----------------------------------------------------------------------------
void parseMethod(DOM_Node node,
                 DaDiMethod* gddMethod)
//-----------------------------------------------------------------------------
{

  gddMethod->setName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).
    getNodeValue());
          
  gddMethod->setDesc(node.getAttributes().
    getNamedItem(DOMString::transcode("desc")).
    getNodeValue());
            
  gddMethod->setAccess(node.getAttributes().
    getNamedItem(DOMString::transcode("access")).
    getNodeValue());
            
  if (node.getAttributes().
      getNamedItem(DOMString::transcode("const")).
      getNodeValue().equals("TRUE"))
  {
    gddMethod->setConst_(true);
  }
  else
  {
    gddMethod->setConst_(false);
  }
            
  gddMethod->setVirtual_(node.getAttributes().
      getNamedItem(DOMString::transcode("virtual")).
      getNodeValue());
            
  if (node.getAttributes().
      getNamedItem(DOMString::transcode("static")).
      getNodeValue().equals("TRUE"))
  {
    gddMethod->setStatic_(true);
  }
  else
  {
    gddMethod->setStatic_(false);
  }

  if (node.getAttributes().
      getNamedItem(DOMString::transcode("inline")).
      getNodeValue().equals("TRUE"))
  {
    gddMethod->setInline_(true);
  }
  else
  {
    gddMethod->setInline_(false);
  }

  if (node.getAttributes().
      getNamedItem(DOMString::transcode("friend")).
      getNodeValue().equals("TRUE"))
  {
    gddMethod->setFriend_(true);
  }
  else
  {
    gddMethod->setFriend_(false);
  }

  DaDiMethReturn* gddMethReturn = new DaDiMethReturn();
  gddMethReturn->setConst_(false);
  if (!node.getAttributes().
      getNamedItem(DOMString::transcode("type")).isNull())
  {
    gddMethod->setDaDiMethReturn(gddMethReturn);

    std::string typeStr = node.getAttributes().
      getNamedItem(DOMString::transcode("type")).
      getNodeValue().transcode();
            
    if (typeStr.find("const ") == std::string::npos)
    {
      gddMethReturn->setType(node.getAttributes().
        getNamedItem(DOMString::transcode("type")).
        getNodeValue());
    }
    else
    {
      gddMethReturn->setType(DOMString::transcode(typeStr.substr(
        typeStr.find(' ')+1, std::string::npos).data()));
        gddMethReturn->setConst_(true);
    }
  }
  else
  {
    gddMethReturn->setType(0);
  }


  parseArgList(node, gddMethod, gddMethod->name().transcode());

  //
  // Child Elements of Method
  //
  gddMethod->setCode(0);
          
  DOM_Node met_child;
  met_child = node.getFirstChild();
          
  while(!met_child.isNull())
  {                
    switch(met_child.getNodeType())    
    {      
    case DOM_Node::ELEMENT_NODE:
    {
      if(met_child.getNodeName().equals("return"))
      {
        DaDiMethReturn* gddMethReturn = new DaDiMethReturn();
             
        gddMethod->setDaDiMethReturn(gddMethReturn);

        gddMethReturn->setType(met_child.getAttributes().
          getNamedItem(DOMString::transcode("type")).
          getNodeValue());
                
        if (met_child.getAttributes().
            getNamedItem(DOMString::transcode("const")).
            getNodeValue().equals("TRUE"))
        {
          gddMethReturn->setConst_(true);
        }
        else
        {
          gddMethReturn->setConst_(false);
        }         
      }
      if(met_child.getNodeName().equals("code"))
      {
        if (!met_child.getFirstChild().isNull())
        {
          gddMethod->setCode(met_child.getFirstChild().
            getNodeValue());
        }
        else
        {
          gddMethod->setCode(" ");
        }
      }
      if(met_child.getNodeName().equals("arg"))
      {        
        DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
        gddMethod->pushDaDiMethArgument(gddMethArgument);
        parseArg(met_child, gddMethArgument);

      }    
    }
    default:
    {
      met_child = met_child.getNextSibling();
    }
    }
  }
}


//-----------------------------------------------------------------------------
void parseConstructor(DOM_Node node, 
                      DaDiConstructor *gddConstructor)
//-----------------------------------------------------------------------------
{
  gddConstructor->setDesc(node.getAttributes().
    getNamedItem(DOMString::transcode("desc")).
    getNodeValue());

  if (!node.getAttributes().
      getNamedItem(DOMString::transcode("initList")).isNull())
  {
    gddConstructor->setInitList(node.getAttributes().
      getNamedItem(DOMString::transcode("initList")).
      getNodeValue());
    gddConstructor->setCode(DOMString::transcode(" "));
  }
  else
  {
    gddConstructor->setInitList(0);
  }

  parseArgList(node, gddConstructor, "Constructor");
  
  //
  // Child Elements of Constructor
  //
  gddConstructor->setCode(0);
         
  DOM_Node met_child;
  met_child = node.getFirstChild();
          
  while(!met_child.isNull())
  {                
    switch(met_child.getNodeType())    
    {      
    case DOM_Node::ELEMENT_NODE:
    {
      if(met_child.getNodeName().equals("code"))
      {
        if (!met_child.getFirstChild().isNull())
        {
          gddConstructor->setCode(met_child.getFirstChild().
            getNodeValue());
        }
        else
        {
          gddConstructor->setCode(" ");
        }
      }
      if(met_child.getNodeName().equals("arg"))
      {
        DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
        gddConstructor->pushDaDiMethArgument(gddMethArgument);
        parseArg(met_child, gddMethArgument);
      }
    }
    default:
    {
      met_child = met_child.getNextSibling();
    }
    }
  }
}


//-----------------------------------------------------------------------------
void parseDestructor(DOM_Node node, 
                     DaDiDestructor* gddDestructor)
//-----------------------------------------------------------------------------
{
  if (!node.getAttributes().
      getNamedItem(DOMString::transcode("desc")).isNull())
  {
    gddDestructor->setDesc(node.getAttributes().
      getNamedItem(DOMString::transcode("desc")).
      getNodeValue());
  }
  else
  {
    gddDestructor->setDesc(0);
  }

  parseArgList(node, gddDestructor, "Destructor"); 

  //
  // Child Elements of Destructor
  //
  gddDestructor->setCode(0);
         
  DOM_Node met_child;
  met_child = node.getFirstChild();
         
  while(!met_child.isNull())
  {                
    switch(met_child.getNodeType())    
    {      
    case DOM_Node::ELEMENT_NODE:
    {
      if(met_child.getNodeName().equals("code"))
      {
        if (!met_child.getFirstChild().isNull())
        {
          gddDestructor->setCode(met_child.getFirstChild().
            getNodeValue());
        }
        else
        {
          gddDestructor->setCode(" ");
        }
      }
      if(met_child.getNodeName().equals("arg"))
      {
        DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
        gddDestructor->pushDaDiMethArgument(gddMethArgument);
        parseArg(met_child, gddMethArgument);
      }
    }
    default:
    {
      met_child = met_child.getNextSibling();
    }
    }
  }
}


//-----------------------------------------------------------------------------
template<class T> void parseAttribute(DOM_Node node, 
                                      DaDiAttribute* gddAttribute,
                                      T* gdd)
//-----------------------------------------------------------------------------
{
  std::vector<DOMString> typeWords;
  std::vector<DOMString>::iterator iter;
  gddAttribute->setStatic_(false);

  gddAttribute->setName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).
    getNodeValue());

  gddAttribute->setArray(node.getAttributes().
    getNamedItem(DOMString::transcode("array")).
    getNodeValue());

  typeWords = findWords(node.getAttributes().
    getNamedItem(DOMString::transcode("type")).
    getNodeValue(), " ");

  for (iter = typeWords.begin(); iter != typeWords.end(); ++iter)
  {
    if ((*iter).equals("static"))
    {
      gddAttribute->setStatic_(true);
    }
  }

  gddAttribute->setType(node.getAttributes().
    getNamedItem(DOMString::transcode("type")).
    getNodeValue());

  gdd->pushImportList(node.getAttributes().
    getNamedItem(DOMString::transcode("type")).
    getNodeValue().transcode());
          
  if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).
      isNull())
  {
    gddAttribute->setDesc(node.getAttributes().
      getNamedItem(DOMString::transcode("desc")).
      getNodeValue());
  }
  else
  {
    gddAttribute->setDesc(0);
  }
           
  gddAttribute->setAccess(node.getAttributes().
    getNamedItem(DOMString::transcode("access")).
    getNodeValue());
    
  gddAttribute->setSetMeth(node.getAttributes().
    getNamedItem(DOMString::transcode("setMeth")).
    getNodeValue());
          
  gddAttribute->setGetMeth(node.getAttributes().
    getNamedItem(DOMString::transcode("getMeth")).
    getNodeValue());

  if(!node.getAttributes().getNamedItem(DOMString::transcode("init")).
     isNull())
  {
    gddAttribute->setInit(node.getAttributes().
    getNamedItem(DOMString::transcode("init")).
    getNodeValue());
  }
  else
  {
    gddAttribute->setInit(0);
  }            
}


//-----------------------------------------------------------------------------
void parseRelation(DOM_Node node, 
                   DaDiRelation* gddRelation)
//-----------------------------------------------------------------------------
{
  DOMString gddRelationRatio;

  gddRelation->setName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).
    getNodeValue());
            
  gddRelation->setType(node.getAttributes().
    getNamedItem(DOMString::transcode("type")).
    getNodeValue());    

  gddClass->pushImpSoftList(node.getAttributes().            
    getNamedItem(DOMString::transcode("type")).            
    getNodeValue().transcode());
          
  if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).          
      isNull())
  {        
    gddRelation->setDesc(node.getAttributes().
      getNamedItem(DOMString::transcode("desc")).            
      getNodeValue());          
  }          
  else   
  {      
    gddRelation->setDesc(0);   
  }
            
  gddRelation->setAccess(node.getAttributes().            
    getNamedItem(DOMString::transcode("access")).            
    getNodeValue());
          
  if (!node.getAttributes().          
      getNamedItem(DOMString::transcode("multiplicity")).isNull())
  {
    gddRelationRatio = node.getAttributes().        
      getNamedItem(DOMString::transcode("multiplicity")).
      getNodeValue();
    if (gddRelationRatio.equals("n") || 
        gddRelationRatio.equals("N") ||
        gddRelationRatio.equals("m") || 
        gddRelationRatio.equals("M"))
    {
      gddRelationRatio = "*";            
    }
    else if (gddRelationRatio.equals("1")) {}     
    else            
    {        
      std::cerr << argV0 << ": \"" << gddRelationRatio.transcode()       
        << "\" is not a valid value for RELATION-Attribute "
        << "\"multiplicity\"" << std::endl << "This error should never"               
        << " happen!!!! Maybe you've changed the DTD?" << std::endl;              
      exit(1);            
    }                
  }          
  else   
  {      
    gddRelationRatio = "1";   
  }

  gddRelation->setRatio(gddRelationRatio);
            
  gddRelation->setSetMeth(node.getAttributes().            
    getNamedItem(DOMString::transcode("setMeth")).            
    getNodeValue());   
            
  gddRelation->setGetMeth(node.getAttributes().            
    getNamedItem(DOMString::transcode("getMeth")).           
    getNodeValue());   
          		  
  gddRelation->setAddMeth(false);		 
  gddRelation->setRemMeth(false);

  if (gddRelationRatio.equals("*"))		  
  {
    gddRelation->setAddMeth(node.getAttributes().   
      getNamedItem(DOMString::transcode("addMeth")).             
      getNodeValue());
    
    DOMString relRemMeth = node.getAttributes().
      getNamedItem(DOMString::transcode("remMeth")).
      getNodeValue();
    gddRelation->setRemMeth(relRemMeth);
    if (!relRemMeth.equals("FALSE"))
    {
      gddClass->pushImpStdList("algorithm");
    }
	}    

  gddRelation->setClrMeth(node.getAttributes().
    getNamedItem(DOMString::transcode("clrMeth")).
    getNodeValue());
            
  if (gddRelationRatio.equals("1"))
  {
    gddClass->pushImportList("SmartRef");
  }
  else if (gddRelationRatio.equals("*"))                           
  {
    gddClass->pushImportList("SmartRef");
    gddClass->pushImportList("SmartRefVector");
  }
}


//-----------------------------------------------------------------------------
void parseClass(DOM_Node node, 
                DaDiClass* gddClass)
//-----------------------------------------------------------------------------
{
  bool classHasDesc = false;

  gddClass->setName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).
        getNodeValue());

  if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).isNull())
  {
    gddClass->setDesc(node.getAttributes().
      getNamedItem(DOMString::transcode("desc")).
      getNodeValue());
    classHasDesc = true;
  }
  else
  {
    gddClass->setDesc(0);
  }
  
  if (!node.getAttributes().getNamedItem(DOMString::transcode("location")).isNull())
  {
    gddClass->setLocation(node.getAttributes().
      getNamedItem(DOMString::transcode("location")).
      getNodeValue());
  }
  else
  {
    gddClass->setLocation(0);
  }

  gddClass->setAuthor(node.getAttributes().
    getNamedItem(DOMString::transcode("author")).
    getNodeValue());

  if (node.getAttributes().
    getNamedItem(DOMString::transcode("templateVector")).
    getNodeValue().equals("TRUE"))
  {
    gddClass->setClassTemplateVector(true);
  }
  else
  {
    gddClass->setClassTemplateVector(false);
  }

  if (gddClass->classTemplateVector())
  {
    gddClass->pushImportList("ObjectVector");
  }
  
  if (node.getAttributes().
    getNamedItem(DOMString::transcode("templateList")).
    getNodeValue().equals("TRUE"))
  {
    gddClass->setClassTemplateList(true);
  }
  else
  {
    gddClass->setClassTemplateList(false);
  }

  if (node.getAttributes().
    getNamedItem(DOMString::transcode("serializers")).
    getNodeValue().equals("TRUE"))
  {
    gddClass->setSerializers(true);
  }
  else
  {
    gddClass->setSerializers(false);
  }

  if (node.getAttributes().
    getNamedItem(DOMString::transcode("stdVectorTypeDef")).
    getNodeValue().equals("TRUE"))
  {
    gddClass->setStdVectorTypeDef(true);
  }
  else
  {
    gddClass->setStdVectorTypeDef(false);
  }

  if (gddClass->classTemplateList())
  {
    gddClass->pushImportList("ObjectList");
  }
  
  if(!node.getAttributes().getNamedItem(DOMString::transcode("id")).isNull())
  {
    gddClass->setID(node.getAttributes().
      getNamedItem(DOMString::transcode("id")).
      getNodeValue());
  }
  else
  {
    gddClass->setID(0);
  }

  gddClass->setLongDesc(0);

  node = node.getFirstChild();

  while(!node.isNull())
  {
    switch(node.getNodeType())
    {    
    case DOM_Node::ELEMENT_NODE:
      {
//
// Parse description
//
        if(node.getNodeName().equals("desc"))
        {
          DOMString description;
          if (!node.getFirstChild().isNull())
          {
            description = node.getFirstChild().getNodeValue();
            classHasDesc = true;
          }
          else
          {
            description = "";
          }
          if (gddClass->longDesc() != NULL)
          {
            description = description + gddClass->longDesc();
          }
          gddClass->setLongDesc(description);
        }
//
// ParseImport
//
        if(node.getNodeName().equals("import"))
        {
          parseImport(node,gddClass);
        }
//
// Parse baseclasses
//
        else if(node.getNodeName().equals("base"))
        {
          DaDiBaseClass* gddBaseClass = new DaDiBaseClass();
          gddClass->pushDaDiBaseClass(gddBaseClass);
          parseBaseClass(node, gddBaseClass);
        }

//
// Parse inner classes
//
        else if(node.getNodeName().equals("class"))
        {
          DaDiClass* gddInnerClass = new DaDiClass(true);
          gddClass->pushDaDiInnerClass(gddInnerClass);
          parseClass(node, gddInnerClass);
        }

//
// Parse locations (of classes in the TES)
//
        else if(node.getNodeName().equals("location"))
        {
          DaDiLocation* gddLocation = new DaDiLocation();
          gddClass->pushDaDiLocation(gddLocation);
          parseLocation(node, gddLocation);
        }

//
// Parse enums
//
        else if(node.getNodeName().equals("enum"))
        {
          DaDiEnum* gddEnum = new DaDiEnum();
          gddClass->pushDaDiEnum(gddEnum);
          parseEnum(node, gddEnum, gddClass);
        }

//
// Parse typedef
//
        else if(node.getNodeName().equals("typedef"))
        {
          DaDiTypeDef* gddTypeDef = new DaDiTypeDef();
          gddClass->pushDaDiTypeDef(gddTypeDef);
          parseTypeDef(node, gddTypeDef, gddClass);
        }

//
// Parse methods
//
        else if(node.getNodeName().equals("method"))
        {
          DaDiMethod* gddMethod = new DaDiMethod();
          gddClass->pushDaDiMethod(gddMethod);
          parseMethod(node, gddMethod);
        }

//
// Parse constructors
//
        else if(node.getNodeName().equals("constructor"))
        {
          DaDiConstructor* gddConstructor = new DaDiConstructor();
          gddClass->pushDaDiConstructor(gddConstructor);
          parseConstructor(node, gddConstructor);
        }

//
// Parse destructors
//
        else if(node.getNodeName().equals("destructor"))
        {
          DaDiDestructor* gddDestructor = new DaDiDestructor();
          gddClass->pushDaDiDestructor(gddDestructor);
          parseDestructor(node, gddDestructor);
        }

//
// Parse attributes
//
        else if(node.getNodeName().equals("attribute"))
        {
          DaDiAttribute* gddAttribute = new DaDiAttribute();
          gddClass->pushDaDiAttribute(gddAttribute);
          parseAttribute(node, gddAttribute, gddClass);
        }

//
// Parse relations
//
        else if(node.getNodeName().equals("relation"))
        {
          DaDiRelation* gddRelation = new DaDiRelation();
          gddClass->pushDaDiRelation(gddRelation);
          parseRelation(node, gddRelation);
        }
        else
        {    
          node = node.getNextSibling();
        } 
      }
      default:
      {
        node = node.getNextSibling();
      }
    }
  }
  if (!classHasDesc)
  {
    std::cerr << "Class " <<  gddClass->name().transcode() 
      << " has no description, please add one to the xml-file" << std::endl;
    exit(1);
  }
}


//-----------------------------------------------------------------------------
void parseNamespace(DOM_Node node, 
                    DaDiNamespace* gddNamespace)
//-----------------------------------------------------------------------------
{

  gddNamespace->setName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).
    getNodeValue());

  if (!node.getAttributes().
    getNamedItem(DOMString::transcode("author")).isNull())
  {
    gddNamespace->setAuthor(node.getAttributes().
      getNamedItem(DOMString::transcode("author")).
      getNodeValue());
  }
  else
  {
    gddNamespace->setAuthor("");
  }
  
  gddNamespace->setDesc(node.getAttributes().
    getNamedItem(DOMString::transcode("desc")).
    getNodeValue());

  gddNamespace->setLongDesc(0);
  
  node = node.getFirstChild();

  while(!node.isNull())
  {
    switch(node.getNodeType())
    {    
    case DOM_Node::ELEMENT_NODE:
      {
//
// Parse description
//
        if(node.getNodeName().equals("desc"))
        {
          DOMString description;
          if (!node.getFirstChild().isNull())
          {
            description = node.getFirstChild().getNodeValue();
          }
          else
          {
            description = "";
          }
          if (gddNamespace->longDesc() != NULL)
          {
            description = description + gddNamespace->longDesc();
          }
          gddNamespace->setLongDesc(description);
        }

//
// Parse classes
//
        if(node.getNodeName().equals("class"))
        {
          gddClass = new DaDiClass();
          gddNamespace->pushDaDiClass(gddClass);
          parseClass(node, gddClass);
        }

//
// ParseImport
//
        if(node.getNodeName().equals("import"))
        {
          parseImport(node,gddNamespace);
        }

//
// Parse attributes
//
        else if(node.getNodeName().equals("attribute"))
        {
          DaDiAttribute* gddAttribute = new DaDiAttribute();
          gddNamespace->pushDaDiAttribute(gddAttribute);
          parseAttribute(node, gddAttribute, gddNamespace);
        }

//
// Parse methods
//
        else if(node.getNodeName().equals("method"))
        {
          DaDiMethod* gddMethod = new DaDiMethod();
          gddNamespace->pushDaDiMethod(gddMethod);
          parseMethod(node, gddMethod);
        }

//
// Parse enums
//
        else if(node.getNodeName().equals("enum"))
        {
          DaDiEnum* gddEnum = new DaDiEnum();
          gddNamespace->pushDaDiEnum(gddEnum);
          parseEnum(node, gddEnum, gddNamespace);
        }

//
// Parse typedefs
//
        else if(node.getNodeName().equals("typedef"))
        {
          DaDiTypeDef* gddTypeDef = new DaDiTypeDef();
          gddNamespace->pushDaDiTypeDef(gddTypeDef);
          parseTypeDef(node, gddTypeDef, gddNamespace);
        }

        else
        {    
          node = node.getNextSibling();
        } 
      }
      default:
      {
        node = node.getNextSibling();
      }
    }
  }
}


//-----------------------------------------------------------------------------
void parsePackage(DOM_Node node, 
                        DaDiPackage* gddPackage)
//-----------------------------------------------------------------------------
{
  gddPackage->setPackageName(node.getAttributes().    
    getNamedItem(DOMString::transcode("name")).
    getNodeValue());
  
  node = node.getFirstChild();
  
  while(!node.isNull())
  {
    switch(node.getNodeType())
    {
    case DOM_Node::ELEMENT_NODE:
    {
      if (node.getNodeName().equals("import"))
      {
        parseImport(node,gddPackage);
      }
      else if (node.getNodeName().equals("class"))
      {
        gddClass = new DaDiClass();
        gddPackage->pushDaDiClass(gddClass);
        parseClass(node,gddClass);
      }
      else if (node.getNodeName().equals("namespace"))
      {
        DaDiNamespace*  gddNamespace = new DaDiNamespace();
        gddPackage->pushDaDiNamespace(gddNamespace);
        parseNamespace(node, gddNamespace);
      }
      else 
      {
        node = node.getNextSibling();
      }
    }
    default:
    {
      node = node.getNextSibling();
    }
    }
  }
}


//-----------------------------------------------------------------------------
DaDiPackage* DDFE::DaDiFrontEnd(char* filename)
//-----------------------------------------------------------------------------
{

  DaDiPackage* gddPackage = new DaDiPackage();

  static char* gXmlFile = 0;

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


  gXmlFile = filename;

  DOMParser *parser = new DOMParser;
  parser->setValidationScheme(DOMParser::Val_Auto);
  parser->setDoNamespaces(false);
  ErrorHandler* errReporter = new DaDiTools();
  parser->setErrorHandler(errReporter);
  parser->setCreateEntityReferenceNodes(false);
  parser->setToCreateXMLDeclTypeNode(true);

  bool errorsOccured = false;

//
// parse file, catch errors
//

  try
  {
    parser->parse(gXmlFile);
  }

  catch(const XMLException& e)
    {
    std::cerr << "An error occured during parsing file " << gXmlFile 
      << "\n Message: " << e.getMessage() << std::endl;
    errorsOccured = true;
    }

  catch(const DOM_DOMException& e)
    {
    std::cerr << "An error occured during parsing file " << gXmlFile 
      << "\n Message: " << e.msg.transcode() << std::endl;
      errorsOccured = true;
    }

  catch(...)
    {
    std::cerr << "An error occured during parsing file " << gXmlFile 
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
    DOM_Node doc = parser->getDocument();
    DOM_Node top = doc.getFirstChild();

    while (!top.getNodeName().equals("gdd"))
    {
      top = top.getNextSibling();
    }
    
    DOM_Node gdd_node = top.getNextSibling();

    xmlVersion = gdd_node.getAttributes().
      getNamedItem(DOMString::transcode("version")).
      getNodeValue().transcode();
      
    gdd_node = gdd_node.getFirstChild();

    while (!gdd_node.isNull())
    {
      if (gdd_node.getNodeName().equals("import"))
      {
        parseImport(gdd_node,gddPackage);
      }
      if (gdd_node.getNodeName().equals("package"))
      {
        parsePackage(gdd_node,gddPackage);
      } 
      if (gdd_node.getNodeName().equals("class"))
      {
        gddPackage->setPackageName(DOMString::transcode("__NO_PACKAGE__"));
        gddClass = new DaDiClass();
        gddPackage->pushDaDiClass(gddClass);
        parseClass(gdd_node,gddClass);
      }
      gdd_node = gdd_node.getNextSibling();
    }
//    std::cout << xmlVersion << std::endl;
  }
  return gddPackage;
}


