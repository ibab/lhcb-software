// $Id: DaDiFrontEnd.cpp,v 1.12 2001-11-27 17:02:22 mato Exp $

#include "GaudiKernel/Kernel.h"

#include <iostream>
#include <string>
#include <list>
#include <ctime>
#include <vector>

#include <util/PlatformUtils.hpp>
#include <util/XMLUniDefs.hpp>
#include <parsers/DOMParser.hpp>
#include <dom/DOM_NodeList.hpp>
#include <dom/DOM_Element.hpp>
#include <dom/DOM_DOMException.hpp>
#include <dom/DOM_NamedNodeMap.hpp>
#include <dom/DOMString.hpp>

#include "DaDiTools.h"
#include "DaDiFrontEnd.h"
#include "DaDiPackage.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Dadi
//
// 18/05/2001 : 
//-----------------------------------------------------------------------------

extern std::string argV0;


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
std::vector<std::string> findWords(std::string value, 
                                   std::string delim)
//-----------------------------------------------------------------------------
{
  std::vector<std::string> words;
  unsigned int i = 0;

  while ((i = value.find(delim)) != std::string::npos)
  {
    while((i = value.find(" ")) == 0)
    {
      value = value.substr(1, value.size()-1);
    }
    words.push_back(value.substr(0,i));
    value = value.substr(i+1,value.size()-i);
  }
  if (value.size() != 0)
  {
    words.push_back(value.substr(0,value.size()));
  }
  return words;
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
      
    DOM_Node gdd_node = top.getNextSibling().getFirstChild();
    while (!gdd_node.isNull())
    {
      if (gdd_node.getNodeName().equals("import"))
      {
        DDFE::parseImport(gdd_node,gddPackage);
      }
      if (gdd_node.getNodeName().equals("package"))
      {
        DDFE::parsePackage(gdd_node,gddPackage);
      } 
      if (gdd_node.getNodeName().equals("class"))
      {
        gddPackage->setPackageName(DOMString::transcode("__NO_PACKAGE__"));
        DaDiClass* gddClass = new DaDiClass();
        gddPackage->pushDaDiClass(gddClass);
        DDFE::parseClass(gdd_node,gddClass);
      }
      gdd_node = gdd_node.getNextSibling();
    }
  }
  return gddPackage;
}



//-----------------------------------------------------------------------------
void DDFE::parseImport(DOM_Node node, 
                       DaDiPackage* gddPackage)
//-----------------------------------------------------------------------------
{  
  std::string import_name, import_soft, import_std;
  
  import_name = node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue().transcode();
  import_soft = node.getAttributes().
    getNamedItem(DOMString::transcode("soft")).getNodeValue().transcode();
  import_std = node.getAttributes().
    getNamedItem(DOMString::transcode("std")).getNodeValue().transcode();

  if (import_soft == "TRUE")
    {
    gddPackage->pushImpSoftList(import_name);
    }
  else if (import_std == "TRUE")
    {
    gddPackage->pushImpStdList(import_name);
    }
  else
    {
    gddPackage->pushImportList(import_name);
    }
}



//-----------------------------------------------------------------------------
void DDFE::parseImport(DOM_Node node, 
                       DaDiClass* gddClass)
//-----------------------------------------------------------------------------
{  
  std::string import_name, import_soft, import_std;
  
  import_name = node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue().transcode();
  import_soft = node.getAttributes().
    getNamedItem(DOMString::transcode("soft")).getNodeValue().transcode();
  import_std = node.getAttributes().
    getNamedItem(DOMString::transcode("std")).getNodeValue().transcode();
  
  if (import_soft == "TRUE")
    {
    gddClass->pushImpSoftList(import_name);
    }
  else if (import_std == "TRUE")
    {
    gddClass->pushImpStdList(import_name);
    }
  else
    {
    gddClass->pushImportList(import_name);
    }
}



//-----------------------------------------------------------------------------
void DDFE::parsePackage(DOM_Node node, 
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
          DDFE::parseImport(node,gddPackage);
        }
        else if (node.getNodeName().equals("class"))
        {
          DaDiClass* gddClass = new DaDiClass();
          gddPackage->pushDaDiClass(gddClass);
          DDFE::parseClass(node,gddClass);
        }
        else 
        {
          node = node.getNextSibling();
        }
    default:
        {
          node = node.getNextSibling();
        }
      }
    }
  }
}



//-----------------------------------------------------------------------------
void DDFE::parseClass(DOM_Node node, 
                      DaDiClass* gddClass)
//-----------------------------------------------------------------------------
{
  bool classHasDesc = false;

  gddClass->setClassName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).
        getNodeValue());

  if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).isNull())
  {
    gddClass->setClassDesc(node.getAttributes().
      getNamedItem(DOMString::transcode("desc")).
      getNodeValue());
    classHasDesc = true;
  }
  else
  {
    gddClass->setClassDesc(0);
  }
  
  gddClass->setClassAuthor(node.getAttributes().
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

  if (gddClass->classTemplateList())
  {
    gddClass->pushImportList("ObjectList");
  }
  
  if(!node.getAttributes().getNamedItem(DOMString::transcode("id")).isNull())
  {
    gddClass->setClassID(node.getAttributes().
      getNamedItem(DOMString::transcode("id")).
      getNodeValue());
  }
  else
  {
    gddClass->setClassID(0);
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
          gddClass->setLongDesc(node.getFirstChild().
                  getNodeValue());
          classHasDesc = true;
        }
//
// ParseImport
//
        if(node.getNodeName().equals("import"))
        {
          DDFE::parseImport(node,gddClass);
        }
//
// Parse baseclasses
//
        else if(node.getNodeName().equals("base"))
        {
          DaDiBaseClass* gddBaseClass = new DaDiBaseClass();

          gddClass->pushDaDiBaseClass(gddBaseClass);

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

//
// Parse methods
//
        else if(node.getNodeName().equals("method"))
        {
          DaDiMethod* gddMethod = new DaDiMethod();

          gddClass->pushDaDiMethod(gddMethod);

          gddMethod->setName(node.getAttributes().
            getNamedItem(DOMString::transcode("name")).
                        getNodeValue());
          
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("desc")).isNull())
          {
            gddMethod->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue());
          }
          else
          {
            gddMethod->setDesc(0);
          }
            
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
            
          if (node.getAttributes().
            getNamedItem(DOMString::transcode("virtual")).
                        getNodeValue().equals("TRUE"))
          {
            gddMethod->setVirtual_(true);
          }
          else
          {
            gddMethod->setVirtual_(false);
          }
            
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

          //
          // Handling of argList and argInOut
          //
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
                  << gddMethod->name().transcode() << " (Class: " 
                  << gddClass->className().transcode()
                  << "), you have to provide at least a type-name-pair" 
                  << std::endl;
                exit(1);
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddMethod->pushDaDiMethArgument(gddMethArgument);

              gddMethArgument->setName(argWords[argWords.size()-1]);
              argWords.pop_back();

              argType = argWords[argWords.size()-1];
              argWords.pop_back();  

              if (isPointer(argType))
              {
                argType.deleteData(argType.length()-1,1);
                gddMethArgument->setIsPointer(true);
              }
              else
              {
                gddMethArgument->setIsPointer(false);
              }
              
              for (std::vector<DOMString>::iterator iterW = argWords.begin();
              iterW != argWords.end(); ++iterW)
              {
                if (iterW->equals("const"))
                {
                  gddMethArgument->setConst_(true);
                }
                else
                {
                  argEType += *iterW;
                  argEType += " ";
                }
              }
              argEType += argType;
              gddMethArgument->setType(argEType);

//
// handling of Input/Output arguments here !!!!
//

              gddMethArgument->setInout("INPUT");

            }
          }

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
                gddMethod->setCode(met_child.getFirstChild().
                  getNodeValue());
              }
              if(met_child.getNodeName().equals("arg"))
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddMethod->pushDaDiMethArgument(gddMethArgument);
                DOMString argType = met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue();
                if (isPointer(argType))
                {
                  gddMethArgument->setIsPointer(true);
                  gddMethArgument->setType(argType.
                    substringData(0,argType.length()-1));
                }
                else
                {
                  gddMethArgument->setIsPointer(false);
                  gddMethArgument->setType(argType);
                }
                
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue());
                }
                else
                {
                  gddMethArgument->setName(0);
                }

                if (met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().equals("TRUE"))
                {
                  gddMethArgument->setConst_(true);
                }
                else
                {
                  gddMethArgument->setConst_(false);
                }

                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue());
              }
              }
            default:
              {
                met_child = met_child.getNextSibling();
              }
            }
          }
        }

//
// Parse constructors
//
        else if(node.getNodeName().equals("constructor"))
        {
          DaDiConstructor* gddConstructor = new DaDiConstructor();

          gddClass->pushDaDiConstructor(gddConstructor);

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("desc")).isNull())
          {
            gddConstructor->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue());
          }
          else
          {
            gddConstructor->setDesc(0);
          }

          
          //
          // Handling of argList and argInOut
          //
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
                  << ": Error in 'argList'-description of constructor in Class "
                  << gddClass->className().transcode()
                  << ", you have to provide at least a type-name-pair" 
                  << std::endl;
                exit(1);
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddConstructor->pushDaDiMethArgument(gddMethArgument);

              gddMethArgument->setName(argWords[argWords.size()-1]);
              argWords.pop_back();

              argType = argWords[argWords.size()-1];
              argWords.pop_back();  

              if (isPointer(argType))
              {
                argType.deleteData(argType.length()-1,1);
                gddMethArgument->setIsPointer(true);
              }
              else
              {
                gddMethArgument->setIsPointer(false);
              }
              
              for (std::vector<DOMString>::iterator iterW = argWords.begin();
              iterW != argWords.end(); ++iterW)
              {
                if (iterW->equals("const"))
                {
                  gddMethArgument->setConst_(true);
                }
                else
                {
                  argEType += *iterW;
                  argEType += " ";
                }
              }
              argEType += argType;
              gddMethArgument->setType(argEType);

//
// handling of Input/Output arguments here !!!!
//

              gddMethArgument->setInout("INPUT");

            }
          }


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
                gddConstructor->setCode(met_child.getFirstChild().
                  getNodeValue());
              }
              if(met_child.getNodeName().equals("arg"))
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddConstructor->pushDaDiMethArgument(gddMethArgument);
                gddMethArgument->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue());
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue());
                }
                else
                {
                  gddMethArgument->setName(0);
                }

                if (met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().equals("TRUE"))
                {
                  gddMethArgument->setConst_(true);
                }
                else
                {
                  gddMethArgument->setConst_(false);
                }

                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue());
              }
              }
            default:
              {
                met_child = met_child.getNextSibling();
              }
            }
          }
        }

//
// Parse destructors
//
        else if(node.getNodeName().equals("destructor"))
        {
          DaDiDestructor* gddDestructor = new DaDiDestructor();

          gddClass->pushDaDiDestructor(gddDestructor);

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

          
          //
          // Handling of argList and argInOut
          //
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
                  << ": Error in 'argList'-description of destructor in Class "
                  << gddClass->className().transcode()
                  << ", you have to provide at least a type-name-pair" 
                  << std::endl;
                exit(1);
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddDestructor->pushDaDiMethArgument(gddMethArgument);

              gddMethArgument->setName(argWords[argWords.size()-1]);
              argWords.pop_back();

              argType = argWords[argWords.size()-1];
              argWords.pop_back();  

              if (isPointer(argType))
              {
                argType.deleteData(argType.length()-1,1);
                gddMethArgument->setIsPointer(true);
              }
              else
              {
                gddMethArgument->setIsPointer(false);
              }
              
              for (std::vector<DOMString>::iterator iterW = argWords.begin();
              iterW != argWords.end(); ++iterW)
              {
                if (iterW->equals("const"))
                {
                  gddMethArgument->setConst_(true);
                }
                else
                {
                  argEType += *iterW;
                  argEType += " ";
                }
              }
              argEType += argType;
              gddMethArgument->setType(argEType);

//
// handling of Input/Output arguments here !!!!
//

              gddMethArgument->setInout("INPUT");

            }
          }


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
                gddDestructor->setCode(met_child.getFirstChild().
                  getNodeValue());
              }
              if(met_child.getNodeName().equals("arg"))
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddDestructor->pushDaDiMethArgument(gddMethArgument);
                gddMethArgument->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue());
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue());
                }
                else
                {
                  gddMethArgument->setName(0);
                }

                if (met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().equals("TRUE"))
                {
                  gddMethArgument->setConst_(true);
                }
                else
                {
                  gddMethArgument->setConst_(false);
                }

                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue());
              }
              }
            default:
              {
                met_child = met_child.getNextSibling();
              }
            }
          }
        }

//
// Parse attributes
//
        else if(node.getNodeName().equals("attribute"))
        {
          DaDiAttribute* gddAttribute = new DaDiAttribute();
          gddClass->pushDaDiAttribute(gddAttribute);
          gddAttribute->setName(node.getAttributes().
            getNamedItem(DOMString::transcode("name")).
            getNodeValue());
            
          gddAttribute->setType(node.getAttributes().
            getNamedItem(DOMString::transcode("type")).
            getNodeValue());
          gddClass->pushImportList(node.getAttributes().
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
          
          if (node.getAttributes().
            getNamedItem(DOMString::transcode("setMeth")).
            getNodeValue().equals("TRUE"))
          {
            gddAttribute->setSetMeth(true);
          }
          else
          {
            gddAttribute->setSetMeth(false);
          }
           
          if (node.getAttributes().
            getNamedItem(DOMString::transcode("getMeth")).
            getNodeValue().equals("TRUE"))
          {
            gddAttribute->setGetMeth(true);
          }
          else
          {
            gddAttribute->setGetMeth(false);
          }

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

//
// Parse relations
//
        else if(node.getNodeName().equals("relation"))
        {
          DOMString gddRelationRatio;
            
          DaDiRelation* gddRelation = new DaDiRelation();
          gddClass->pushDaDiRelation(gddRelation);
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
            
          if (node.getAttributes().
            getNamedItem(DOMString::transcode("setMeth")).
            getNodeValue().equals("TRUE"))
          {
            gddRelation->setSetMeth(true);
          }
          else
          {
            gddRelation->setSetMeth(false);
          }
            
          if (node.getAttributes().
            getNamedItem(DOMString::transcode("getMeth")).
            getNodeValue().equals("TRUE"))
          {
            gddRelation->setGetMeth(true);
          }
          else
          {
            gddRelation->setGetMeth(false);
          }
          
		  
		  gddRelation->setAddMeth(false);
		  gddRelation->setRemMeth(false);

		  if (gddRelationRatio.equals("*"))
		  {
            if (node.getAttributes().
              getNamedItem(DOMString::transcode("addMeth")).
              getNodeValue().equals("TRUE"))
			{
              gddRelation->setAddMeth(true);
			}
            
            if (node.getAttributes().
              getNamedItem(DOMString::transcode("remMeth")).
              getNodeValue().equals("TRUE"))
			{
              gddRelation->setRemMeth(true);
			  gddClass->pushImpStdList("algorithm");
			}
		  }    



          if (node.getAttributes().
            getNamedItem(DOMString::transcode("clrMeth")).
            getNodeValue().equals("TRUE"))
          {
            gddRelation->setClrMeth(true);
          }
          else
          {
            gddRelation->setClrMeth(false);
          }
            
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
    std::cerr << "Class " <<  gddClass->className().transcode() 
      << " has no description, please add one to the xml-file" << std::endl;
    exit(1);
  }
}

