// $Id: DaDiFrontEnd.cpp,v 1.4 2001-10-17 08:32:11 mato Exp $

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



std::vector<std::string> findWords(std::string value)
{
  std::vector<std::string> words;
  int i,j = 0;

  while ((i = value.find(" ")) != std::string::npos)
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



DaDiPackage* DDFE::DaDiFrontEnd(char* filename)
{

  DaDiPackage* gddPackage = new DaDiPackage();

  static char* gXmlFile = 0;

  std::vector<std::string> abc = findWords("ab cd  ef");

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
    std::cerr << "An error occured during parsing file " << gXmlFile << "\n Message: "
      << e.getMessage() << std::endl;
    errorsOccured = true;
    }

  catch(const DOM_DOMException& e)
    {
    std::cerr << "An error occured during parsing file " << gXmlFile << "\n Message: "
      << e.msg.transcode() << std::endl;
      errorsOccured = true;
    }

  catch(...)
    {
    std::cerr << "An error occured during parsing file " << gXmlFile << std::endl;
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

    while (strcmp(top.getNodeName().transcode(), "gdd") != 0)
        {
      top = top.getNextSibling();
        }
      
    DOM_Node gdd_node = top.getNextSibling().getFirstChild();
    while (!gdd_node.isNull())
    {
      if (strcmp(gdd_node.getNodeName().transcode(), "import")==0)
      {
        DDFE::parseImport(gdd_node,gddPackage);
      }
      if (strcmp(gdd_node.getNodeName().transcode(), "package")==0)
      {
        DDFE::parsePackage(gdd_node,gddPackage);
      } 
      if (strcmp(gdd_node.getNodeName().transcode(), "class")==0)
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

//
// ParseImport (package)
//
void DDFE::parseImport(DOM_Node node, DaDiPackage* gddPackage)
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

//
// ParseImport (class)
//
void DDFE::parseImport(DOM_Node node, DaDiClass* gddClass)
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


//
// ParsePackage
//
void DDFE::parsePackage(DOM_Node node, DaDiPackage* gddPackage)
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
        if (strcmp(node.getNodeName().transcode(), "import") == 0)
              {
          DDFE::parseImport(node,gddPackage);
        }
        else if (strcmp(node.getNodeName().transcode(), "class") == 0)
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


//
// ParseClass
//
void DDFE::parseClass(DOM_Node node, DaDiClass* gddClass)
{
  gddClass->setClassName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).
        getNodeValue());

  if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).isNull())
  {
    gddClass->setClassDesc(node.getAttributes().
      getNamedItem(DOMString::transcode("desc")).
      getNodeValue());
  }
  else
  {
    gddClass->setClassDesc(NULL);
  }
  
  gddClass->setClassAuthor(node.getAttributes().
    getNamedItem(DOMString::transcode("author")).
    getNodeValue());

  gddClass->setClassTemplateVector(node.getAttributes().
    getNamedItem(DOMString::transcode("templateVector")).
    getNodeValue());
  if (strcmp(gddClass->classTemplateVector().transcode(), "TRUE") == 0)
  {
    gddClass->pushImportList("ObjectVector");
  }
  
  gddClass->setClassTemplateList(node.getAttributes().
    getNamedItem(DOMString::transcode("templateList")).
    getNodeValue());
  if (strcmp(gddClass->classTemplateList().transcode(), "TRUE") == 0)
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
    gddClass->setClassID(NULL);
  }

  node = node.getFirstChild();

  while(!node.isNull())
  {
    switch(node.getNodeType())
       {    
    case DOM_Node::ELEMENT_NODE:
      {
//
// ParseImport
//
        if(strcmp(node.getNodeName().transcode(), "import") == 0)
        {
          DDFE::parseImport(node,gddClass);
            }
//
// Parse baseclasses
//
        else if(strcmp(node.getNodeName().transcode(), "base") == 0)
        {
          DaDiBaseClass* gddBaseClass = new DaDiBaseClass();

          gddClass->pushDaDiBaseClass(gddBaseClass);

          gddBaseClass->setName(node.getAttributes().getNamedItem(DOMString::transcode("name")).getNodeValue().transcode());

          gddClass->pushImportList(node.getAttributes().getNamedItem(DOMString::transcode("name")).getNodeValue().transcode());

          gddBaseClass->setVirtual_(node.getAttributes().getNamedItem(DOMString::transcode("virtual")).getNodeValue().transcode());

          gddBaseClass->setAccess(node.getAttributes().getNamedItem(DOMString::transcode("access")).getNodeValue().transcode());
        }
///
/// Parse friends
///
        



//
// Parse methods
//
        else if(strcmp(node.getNodeName().transcode(), "method") == 0)
        {
          DaDiMethod* gddMethod = new DaDiMethod();

          gddClass->pushDaDiMethod(gddMethod);

          gddMethod->setName(node.getAttributes().
            getNamedItem(DOMString::transcode("name")).
                        getNodeValue().transcode());
          
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("desc")).isNull())
          {
            gddMethod->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddMethod->setDesc(NULL);
          }
            
          gddMethod->setAccess(node.getAttributes().
            getNamedItem(DOMString::transcode("access")).
                        getNodeValue().transcode());
            
          gddMethod->setConst_(node.getAttributes().
            getNamedItem(DOMString::transcode("const")).
                        getNodeValue().transcode());
            
          gddMethod->setVirtual_(node.getAttributes().
            getNamedItem(DOMString::transcode("virtual")).
                        getNodeValue().transcode());
            
          gddMethod->setStatic_(node.getAttributes().
            getNamedItem(DOMString::transcode("static")).
                        getNodeValue().transcode());            

          gddMethod->setInline_(node.getAttributes().
            getNamedItem(DOMString::transcode("inline")).
                        getNodeValue().transcode());            

          DaDiMethReturn* gddMethReturn = new DaDiMethReturn();
          gddMethReturn->setConst_("FALSE");
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
                getNodeValue().transcode());
            }
            else
            {
              gddMethReturn->setType(DOMString::transcode(typeStr.substr(
                typeStr.find(' ')+1, std::string::npos).data()));
              gddMethReturn->setConst_("TRUE");
            }
          }
          else
          {
            gddMethReturn->setType(NULL);
          }

          std::string argList, argInOut;
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argList")).isNull())
          {
            argList = node.getAttributes().
              getNamedItem(DOMString::transcode("argList")).
              getNodeValue().transcode();

            bool lastElem = false;
            while(!lastElem)
            {
              std::vector<std::string> args;
              int i = argList.find(",");
              if (i != std::string::npos)
              {
                args = findWords(argList.substr(0,i));
                argList = argList.substr(i+1,argList.size()-i);
              }
              else
              {
                args = findWords(argList);
                lastElem = true;
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddMethod->pushDaDiMethArgument(gddMethArgument);
              
              gddMethArgument->setName(DOMString::transcode(args[args.size()-1].data()));
              gddMethArgument->setType(DOMString::transcode(args[args.size()-2].data()));
              
              for (int j = (args.size()-3); j>=0; --j)
              {
                if (args[j] == "const")
                {
                  gddMethArgument->setConst_("TRUE");
                }
                else
                {
                std::cerr << argV0 << ": Don't know how to handle modifier: " << args[j] << "." << std::endl;
                }
              }
            }
          }

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argInOut")).isNull())
          {
            argInOut = node.getAttributes().
              getNamedItem(DOMString::transcode("argInOut")).
              getNodeValue().transcode();           
          }

          
          gddMethod->setCode(NULL);
          
          DOM_Node met_child;
          met_child = node.getFirstChild();
          
          while(!met_child.isNull())
          {                
            switch(met_child.getNodeType())    
            {      
            case DOM_Node::ELEMENT_NODE:
              {
              if(strcmp(met_child.getNodeName().transcode(), "return") == 0)
              {
                DaDiMethReturn* gddMethReturn = new DaDiMethReturn();
              
                gddMethod->setDaDiMethReturn(gddMethReturn);

                gddMethReturn->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue().transcode());
                gddMethReturn->setConst_(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().transcode());
                          
              }
              if(strcmp(met_child.getNodeName().transcode(), "code") == 0)
              {
                gddMethod->setCode(met_child.getFirstChild().
                  getNodeValue().transcode());
              }
              if(strcmp(met_child.getNodeName().transcode(), "arg") == 0)
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddMethod->pushDaDiMethArgument(gddMethArgument);
                gddMethArgument->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue().transcode());
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue().transcode());
                }
                else
                {
                  gddMethArgument->setName(NULL);
                }
                gddMethArgument->setConst_(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().transcode());
                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue().transcode());
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
        else if(strcmp(node.getNodeName().transcode(), "constructor") == 0)
        {
          DaDiConstructor* gddConstructor = new DaDiConstructor();

          gddClass->pushDaDiConstructor(gddConstructor);

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("desc")).isNull())
          {
            gddConstructor->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddConstructor->setDesc(NULL);
          }
            
          std::string argList, argInOut;
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argList")).isNull())
          {
            argList = node.getAttributes().
              getNamedItem(DOMString::transcode("argList")).
              getNodeValue().transcode();

            bool lastElem = false;
            while(!lastElem)
            {
              std::vector<std::string> args;
              int i = argList.find(",");
              if (i != std::string::npos)
              {
                args = findWords(argList.substr(0,i));
                argList = argList.substr(i+1,argList.size()-i);
              }
              else
              {
                args = findWords(argList);
                lastElem = true;
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddConstructor->pushDaDiMethArgument(gddMethArgument);
              
              gddMethArgument->setName(DOMString::transcode(args[args.size()-1].data()));
              gddMethArgument->setType(DOMString::transcode(args[args.size()-2].data()));
              
              for (int j = (args.size()-3); j>=0; --j)
              {
                if (args[j] == "const")
                {
                  gddMethArgument->setConst_("TRUE");
                }
                else
                {
                std::cerr << argV0 << ": Don't know how to handle modifier: " << args[j] << "." << std::endl;
                }
              }
            }
          }

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argInOut")).isNull())
          {
            argInOut = node.getAttributes().
              getNamedItem(DOMString::transcode("argInOut")).
              getNodeValue().transcode();           
          }

          
          gddConstructor->setCode(NULL);
          
          DOM_Node met_child;
          met_child = node.getFirstChild();
          
          while(!met_child.isNull())
          {                
            switch(met_child.getNodeType())    
            {      
            case DOM_Node::ELEMENT_NODE:
              {
              if(strcmp(met_child.getNodeName().transcode(), "code") == 0)
              {
                gddConstructor->setCode(met_child.getFirstChild().
                  getNodeValue().transcode());
              }
              if(strcmp(met_child.getNodeName().transcode(), "arg") == 0)
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddConstructor->pushDaDiMethArgument(gddMethArgument);
                gddMethArgument->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue().transcode());
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue().transcode());
                }
                else
                {
                  gddMethArgument->setName(NULL);
                }
                gddMethArgument->setConst_(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().transcode());
                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue().transcode());
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
        else if(strcmp(node.getNodeName().transcode(), "destructor") == 0)
        {
          DaDiDestructor* gddDestructor = new DaDiDestructor();

          gddClass->pushDaDiDestructor(gddDestructor);

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("desc")).isNull())
          {
            gddDestructor->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddDestructor->setDesc(NULL);
          }
            
          std::string argList, argInOut;
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argList")).isNull())
          {
            argList = node.getAttributes().
              getNamedItem(DOMString::transcode("argList")).
              getNodeValue().transcode();

            bool lastElem = false;
            while(!lastElem)
            {
              std::vector<std::string> args;
              int i = argList.find(",");
              if (i != std::string::npos)
              {
                args = findWords(argList.substr(0,i));
                argList = argList.substr(i+1,argList.size()-i);
              }
              else
              {
                args = findWords(argList);
                lastElem = true;
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddDestructor->pushDaDiMethArgument(gddMethArgument);
              
              gddMethArgument->setName(DOMString::transcode(args[args.size()-1].data()));
              gddMethArgument->setType(DOMString::transcode(args[args.size()-2].data()));
              
              for (int j = (args.size()-3); j>=0; --j)
              {
                if (args[j] == "const")
                {
                  gddMethArgument->setConst_("TRUE");
                }
                else
                {
                std::cerr << argV0 << ": Don't know how to handle modifier: " << args[j] << "." << std::endl;
                }
              }
            }
          }

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argInOut")).isNull())
          {
            argInOut = node.getAttributes().
              getNamedItem(DOMString::transcode("argInOut")).
              getNodeValue().transcode();           
          }

          
          gddDestructor->setCode(NULL);
          
          DOM_Node met_child;
          met_child = node.getFirstChild();
          
          while(!met_child.isNull())
          {                
            switch(met_child.getNodeType())    
            {      
            case DOM_Node::ELEMENT_NODE:
              {
              if(strcmp(met_child.getNodeName().transcode(), "code") == 0)
              {
                gddDestructor->setCode(met_child.getFirstChild().
                  getNodeValue().transcode());
              }
              if(strcmp(met_child.getNodeName().transcode(), "arg") == 0)
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddDestructor->pushDaDiMethArgument(gddMethArgument);
                gddMethArgument->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue().transcode());
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue().transcode());
                }
                else
                {
                  gddMethArgument->setName(NULL);
                }
                gddMethArgument->setConst_(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().transcode());
                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue().transcode());
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
        else if(strcmp(node.getNodeName().transcode(), "attribute") == 0)
        {
          DaDiAttribute* gddAttribute = new DaDiAttribute();
          gddClass->pushDaDiAttribute(gddAttribute);
          gddAttribute->setName(node.getAttributes().
            getNamedItem(DOMString::transcode("name")).
            getNodeValue().transcode());
            
          gddAttribute->setType(node.getAttributes().
            getNamedItem(DOMString::transcode("type")).
            getNodeValue().transcode());
          gddClass->pushImportList(node.getAttributes().
              getNamedItem(DOMString::transcode("type")).
            getNodeValue().transcode());
          
          if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).
            isNull())
          {
            gddAttribute->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddAttribute->setDesc(NULL);
          }
            
          gddAttribute->setAccess(node.getAttributes().
            getNamedItem(DOMString::transcode("access")).
            getNodeValue().transcode());
            
          gddAttribute->setSetMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("setMeth")).
                    getNodeValue().transcode());
            
          gddAttribute->setGetMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("getMeth")).
              getNodeValue().transcode());

          if(!node.getAttributes().getNamedItem(DOMString::transcode("init")).isNull())
          {
            gddAttribute->setInit(node.getAttributes().
              getNamedItem(DOMString::transcode("init")).
              getNodeValue());
          }
          else
          {
            gddAttribute->setInit(NULL);
          }

            
        }
//
// Parse relations
//
        else if(strcmp(node.getNodeName().transcode(), "relation") == 0)
        {

          char* gddRelationRatio;
            
          DaDiRelation* gddRelation = new DaDiRelation();
          gddClass->pushDaDiRelation(gddRelation);
          gddRelation->setName(node.getAttributes().
            getNamedItem(DOMString::transcode("name")).
            getNodeValue().transcode());
            
          gddRelation->setType(node.getAttributes().
            getNamedItem(DOMString::transcode("type")).
            getNodeValue().transcode());
          gddClass->pushImpSoftList(node.getAttributes().
            getNamedItem(DOMString::transcode("type")).
            getNodeValue().transcode());
          
          if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).
            isNull())
          {
            gddRelation->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddRelation->setDesc(NULL);
          }
            
          gddRelation->setAccess(node.getAttributes().
            getNamedItem(DOMString::transcode("access")).
            getNodeValue().transcode());
            
          gddRelationRatio = node.getAttributes().
            getNamedItem(DOMString::transcode("ratio")).
            getNodeValue().transcode();
          gddRelation->setRatio(node.getAttributes().
            getNamedItem(DOMString::transcode("ratio")).
            getNodeValue().transcode());
            
          gddRelation->setSetMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("setMeth")).
            getNodeValue().transcode());
            
          gddRelation->setGetMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("getMeth")).
            getNodeValue().transcode());
            
          gddRelation->setAddMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("addMeth")).
            getNodeValue().transcode());
            
          gddRelation->setRemMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("remMeth")).
            getNodeValue().transcode());
            
          gddRelation->setClrMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("clrMeth")).
            getNodeValue().transcode());
            
          if (strcmp(gddRelationRatio, "1-1") == 0)
          {
            gddClass->pushImportList("SmartRef");
          }
          else if ((strcmp(gddRelationRatio, "1-M") == 0) ||
            (strcmp(gddRelationRatio, "M-N") == 0))
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
}

