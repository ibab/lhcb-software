// $Id: DaDiCppDict.cpp,v 1.3 2001-10-09 17:04:54 mato Exp $

#include "GaudiKernel/Kernel.h"

#include <ctime>
#include <iostream>
#include <list>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "parsers/DOMParser.hpp"
#include "dom/DOM_DOMException.hpp"
#include <dom/DOM_NodeList.hpp>
#include <dom/DOM_Element.hpp>
#include <dom/DOM_NamedNodeMap.hpp>
#include <dom/DOMString.hpp>

#include "DaDiCppDict.h"
#include "DaDiPackage.h"
#include "DaDiTools.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaDiCppHeader
//
// 12/06/2001 : 
//----------------------------------------------------------------------------

//
// Usage()
//
void usage(std::string argV0)
{
  std::cout << std::endl << std::endl;
  std::cout << "Usage: " << argV0 << " [-h] [-v] [-o [path]] [-x [path]] xml-file(s)"
    << std::endl << "Produce .cpp-files for the Gaudi Dictionary" << std::endl
    << std::endl 
    << "   -h            display this help and exit" << std::endl
    << "   -v            display version information and exit" << std::endl
    << "   -i            add additional file-package-information from './AddImports.txt'" << std::endl
    << "   -o [path]     define possible outputdestination with following precedence" << std::endl
    << "                    -o path     use 'path'" << std::endl
    << "                    -o          use environment-variable 'GODDICTOUT'" << std::endl
    << "                     default    use local directory" << std::endl
    << "   -x [path]     define location of 'GaudiCppExport.xml' which holds information" << std::endl
    << "                 about include-file<->package dependencies, with this precedence" << std::endl
    << "                    -x path     use 'path'" << std::endl
    << "                    -x          use environment-variable 'GODXMLDB'" << std::endl
    << "                     default    use '$(GAUDIOBJDESCROOT)/xml_files'" << std::endl
    << "   xml-file(s)   xml-file(s) to be parsed (must have extension '.xml')" << std::endl
    << std::endl << std::endl;
  exit(0);
}

//
// Versioninfo
//
void version(std::string argV0)
{
  std::cout << std::endl << std::endl
    << argV0 << " version 1.0" << std::endl
    << "written by Stefan Roiser" << std::endl << std::endl;
  exit(0);
}


//
// Main
//
int main(int argC, char* argV[])
{

  #ifdef WIN32
    char* sep = "\\";
  #elif defined(__linux)
    char* sep = "/";
  #endif

  std::vector<char*> files;
  char *envOut = "", *envXmlDB;
  std::string nextArg;
  bool additionalImports = false;

  std::string argV0 = std::string(argV[0]);
  argV0.erase(0,argV0.find_last_of("\\")+1);
  argV0.erase(0,argV0.find_last_of("/")+1);
  
  if (getenv("GAUDIOBJDESCROOT") == NULL)
  {
    std::cerr << "Please set Environmentvariable 'GAUDIOBJDESCROOT'" << std::endl;
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
          if (getenv("GODDICTOUT") != NULL)
          {
            envOut = getenv("GODDICTOUT");
          }
          else
          {
            std::cerr << "Environment variable 'GODDICTOUT' not set, using local directory"
              << std::endl;
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
            std::cerr << "Environment variable 'GODXMLDB' not set, using '$(GAUDIOBJDESCROOT)/xml_files'"
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

//  std::cout << "envOut: " << envOut << std::endl
//    << "envXmlDB: " << envXmlDB << std::endl;

  
  for (std::vector<char*>::iterator iter = files.begin(); iter != files.end(); ++iter)
  {
    DaDiPackage* gddPackage = DDFE::DaDiFrontEnd(*iter);

    DDBEdict::printCppDictionary(gddPackage,envXmlDB,envOut,additionalImports);
  }

  return 0;
}


//
// printCppDictionary
//
void DDBEdict::printCppDictionary(DaDiPackage* gddPackage, char* envXmlDB, char* envOut, bool additionalImports)
{
  int i=0, j=0;
  std::map<std::string,std::string> dbExportClass;

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
    std::cerr << "An error occured during parsing \n Message: "
      << e.getMessage() << std::endl;
      dbErrorsOccured = true;
    }
  
  catch(const DOM_DOMException& e)
    {
    std::cerr << "An error occured during parsing \n Message: "
            << e.msg.transcode() << std::endl;
    dbErrorsOccured = true;
    }
  
  catch(...)
    {
    std::cerr << "An error occured during parsing " << std::endl;
    dbErrorsOccured = true;
    }
  

  if (dbErrorsOccured)
    {
    char questCont;
    std::cerr << "Continue with Headerfileproduction? (y/n) ";
    std::cin >> questCont;
    if (toupper(questCont) == 'N')
        {
      std::cerr << "Production of Cpp-Headerfiles aborted!!" << std::endl;
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
      
    while(strcmp(dbTop.getNodeName().transcode(), "gdd") != 0)
        {
      dbTop = dbTop.getNextSibling();
        }
      
    DOM_Node dbPackNode = dbTop.getNextSibling().getFirstChild();
      
    while (!dbPackNode.isNull())
        {
      if (strcmp(dbPackNode.getNodeName().transcode(), "package") == 0)
            {
        DOM_Node dbClassNode = dbPackNode.getFirstChild();
        std::string dbPackName;
        dbPackName = dbPackNode.getAttributes().
                  getNamedItem(DOMString::transcode("name")).
                getNodeValue().transcode();
        while(!dbClassNode.isNull())
                {
          if (strcmp(dbClassNode.getNodeName().transcode(), "class") 
                      == 0)
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

  if (additionalImports)
  {
    char* addImportsFile = "AddImports.txt";
    std::ifstream addXml(addImportsFile);
    std::string aline, first, second;

    if (!addXml) 
    { 
      std::cerr << "Cannot open " << addImportsFile << std::endl; 
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
    delete addImportsFile;
  }


  while(gddPackage->sizeDaDiClass())
    {  

  DaDiClass* gddClass = gddPackage->popDaDiClass();


// ------------------------------------------------------------------------
// Start of writing the .cpp-file for filling the MetaModel


  char* cppFileName = new char[256];
  strcpy(cppFileName,envOut);
  strcat(cppFileName, gddClass->className().transcode());
  strcat(cppFileName, "_dict.cpp");
  std::cout << "Writing " << cppFileName;
  std::ofstream metaOut(cppFileName);

//
// Include files
//
  metaOut << "//Include files" << std::endl << "#include \"GaudiKernel/Kernel.h\"" 
    << std::endl << std::endl << "#include <string>" << std::endl << std::endl 
    << "#define private public" << std::endl;
  
    std::string impName = gddClass->className().transcode();
    if(dbExportClass[impName] != "")
        {
      impName = dbExportClass[impName];
        }
      else
        {
      std::cerr << std::endl << "   No information found for type: "
        << impName << std::endl << "   Line written: #include \""
        << impName << ".h\"" << std::endl;
        }
    metaOut << "#include \"" << impName << ".h\"" << std::endl;

  metaOut << "#undef private" << std::endl << std::endl << "#include \"GaudiIntrospection/Reflection.h\"" 
    << std::endl << std::endl << std::endl;

//
// class_dict with constructor
//
  metaOut << "class " << gddClass->className().transcode() << "_dict" << std::endl
    << "{" << std::endl << "public:" << std::endl << "    "
    << gddClass->className().transcode() << "_dict();" << std::endl << "};" << std::endl
    << std::endl;

//
// Instance of class
//
  metaOut << "static " << gddClass->className().transcode() << "_dict instance;" << std::endl
    << std::endl;

//
// Start of constructor
//
  metaOut << gddClass->className().transcode() << "_dict::" << gddClass->className().transcode()
    << "_dict()" << std::endl << "{" << std::endl;

//
// Creation of Metaclass
//
  metaOut << "    MetaClass* metaC = new MetaClass(\"" << gddClass->className().transcode() 
    << "\", \"" << gddClass->classDesc().transcode() << "\", 0);" << std::endl << std::endl;

//
// Creation of fields for attributes and relations
//
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    metaOut << "    metaC->addField(\""
      << gddAttribute->name().transcode() << "\", \"" << gddAttribute->type().transcode()
      << "\", \"" << gddAttribute->desc().transcode() << "\", 0, &(("
      << gddClass->className().transcode() << "*)0)->m_" << gddAttribute->name().transcode()
      << ");" << std::endl << std::endl;
  }

  for(i=0; i<gddClass->sizeDaDiRelation(); ++i)
  {
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    metaOut << "    metaC->addField(\""
      << gddRelation->name().transcode() << "\", \"" << gddRelation->type().transcode()
      << "\", \"" << gddRelation->desc().transcode() << "\", 0, &(("
      << gddClass->className().transcode() << "*)0)->m_" << gddRelation->name().transcode()
      << ");" << std::endl << std::endl;
  }


//
// End of constructor
//
  metaOut << "}" << std::endl << std::endl << std::endl;

  metaOut.close();
  delete [] cppFileName;
  std::cout << " - OK" << std::endl;

  }
}

