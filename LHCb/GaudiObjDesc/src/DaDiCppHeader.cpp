// $Id: DaDiCppHeader.cpp,v 1.12 2001-11-06 09:56:44 mato Exp $

#include "GaudiKernel/Kernel.h"

#include <vector>
#include <ctime>
#include <iostream>
#include <list>
#include <fstream>
#include <string>
#include <map>

#include "parsers/DOMParser.hpp"
#include "dom/DOM_DOMException.hpp"
#include <dom/DOM_NodeList.hpp>
#include <dom/DOM_Element.hpp>
#include <dom/DOM_NamedNodeMap.hpp>
#include <dom/DOMString.hpp>

#include "DaDiCppHeader.h"
#include "DaDiPackage.h"
#include "DaDiTools.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaDiCppHeader
//
// 12/06/2001 : 
//----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
DOMString firstUp(const DOMString s)
//-----------------------------------------------------------------------------
{
  char *p = s.transcode();
  p[0] = toupper(p[0]);
  return DOMString::transcode(p);
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



//-----------------------------------------------------------------------------
bool isSimple(std::string value)
//-----------------------------------------------------------------------------
{
  int i = value.find_last_of(" ");
  value = value.substr(i+1, value.size()-i);
  if ((value == "bool")   || (value == "short")  ||
      (value == "long")   || (value == "int")    || 
      (value == "float")  || (value == "double") || 
      (value == "char"))
  {
    return true;
  }
  else
  {
    return false;
  }
}


std::string argV0;



//-----------------------------------------------------------------------------
int main(int argC, 
         char* argV[])
//-----------------------------------------------------------------------------
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

    DDBEcpp::printCppHeader(gddPackage,envXmlDB,envOut,additionalImports);
  }
  return 0;
}



//-----------------------------------------------------------------------------
void DDBEcpp::printCppHeader(DaDiPackage* gddPackage, 
                             char* envXmlDB, 
                             char* envOut, 
                             bool additionalImports)
//-----------------------------------------------------------------------------
{
  int i=0, j=0;
  time_t ltime;
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
    char* addImportsFile = "AddImports.txt";
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
    delete addImportsFile;
  }

  gddPackage->pushImportList("CLHEPStreams");

  while(gddPackage->sizeDaDiClass())
  {  

  DaDiClass* gddClass = gddPackage->popDaDiClass();

  // ------------------------------------------------------------------------
  // Here starts the writing of the .h-file


  char* fileName = new char[256];
  strcpy(fileName, envOut);
  strcat(fileName, gddClass->className().transcode());
  strcat(fileName, ".h");
  std::cout << "Writing " << fileName;
  std::ofstream xmlOut(fileName);  
  bool isEventClass = false;


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
  }
*/
  if (gddClass->classID() != NULL)
  {
    isEventClass = true;
  }

//
// CVS-tag and 'donotedit'-message
//

  xmlOut << std::endl << std::endl
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
    << std::endl << std::endl << std::endl;



//
// IFNDEF
//
  xmlOut << "#ifndef " << gddPackage->packageName().transcode() << "_" 
    << gddClass->className().transcode() << "_H" << std::endl << "#define " 
    << gddPackage->packageName().transcode() << "_" 
    << gddClass->className().transcode() << "_H 1" << std::endl << std::endl;

//
// Includes
//
  xmlOut << "// Include files" << std::endl;

  for(i=0; i<gddPackage->sizeImpStdList(); i++)
    {
    xmlOut << "#include <" << gddPackage->popImpStdList() << ">"
      << std::endl;
    }

  for(i=0; i<gddClass->sizeImpStdList(); i++)
    {
    xmlOut << "#include <" << gddClass->popImpStdList() << ">"
      << std::endl;
    }
  
  gddPackage->remDblImportList();

  for(i=0; i<gddPackage->sizeImportList(); i++)
  {
    std::string impName = gddPackage->popImportList();
    if(dbExportClass[impName] != "")
    {
      impName = dbExportClass[impName];
    }
    else
    {
      std::cerr << std::endl << argV0 << ": No information found for type: "
        << impName << std::endl << argV0 << ": Line written: #include \""
        << impName << ".h\"" << std::endl;
    }
    xmlOut << "#include \"" << impName << ".h\"" << std::endl;
  }

  gddClass->remDblImportList();

  for(i=0; i<gddClass->sizeImportList(); i++)
  {
    std::string impName = gddClass->popImportList();
    if(dbExportClass[impName] != "")
    {
      impName = dbExportClass[impName];
    }
    else
    {
      std::cerr << std::endl << argV0 << ": No information found for type: " 
        << impName << std::endl << argV0 << ": Line written: #include \"" 
        << impName << ".h\"" << std::endl;
    }
    xmlOut << "#include \"" << impName << ".h\"" << std::endl;
  }


//
// Forward declarations
//  
  gddPackage->remDblImpSoftList();
  gddClass->remDblImpSoftList();

  if (gddPackage->sizeImpSoftList() || gddClass->sizeImpSoftList())
  {
    xmlOut << std::endl << std::endl
      << "// Forward declarations" << std::endl;
    for(i=0; i<gddPackage->sizeImpSoftList(); i++)
    {
      xmlOut << "class " << gddPackage->popImpSoftList() << ";" << std::endl;
    }
    for(i=0; i<gddClass->sizeImpSoftList(); i++)
    {
      xmlOut << "class " << gddClass->popImpSoftList() << ";" << std::endl;
    }
  }
  
  xmlOut << std::endl << std::endl;

//
// Externals
//
  if (isEventClass)
  {
    xmlOut << "// Class ID definition" << std::endl 
      << "static const CLID& CLID_" << gddClass->className().transcode()
      << " = " << gddClass->classID().transcode() << ";" << std::endl 
      << std::endl;
  }

  time(&ltime);

//
// class description
//
  xmlOut << "/** @class " << gddClass->className().transcode() << " "
    << gddClass->className().transcode() << ".h ";

  if (gddPackage->packageName().equals("__NO_PACKAGE__"))
  {
    xmlOut << gddPackage->packageName().transcode() << "/" 
      << gddClass->className().transcode() << ".h";
  }

  xmlOut << std::endl << std::endl;

  if (gddClass->classDesc() != NULL)
  {
    xmlOut << "    " << gddClass->classDesc().transcode();
  }

  if (gddClass->longDesc() != NULL)
  {
    xmlOut << gddClass->longDesc().transcode();
  }

  xmlOut << std::endl << std::endl
    << "    @author " << gddClass->classAuthor().transcode() << std::endl
    << "    @date   " << ctime(&ltime) << std::endl
    << "*/" << std::endl << std::endl;

//
// class definition
//
  xmlOut << "class " << gddClass->className().transcode();
  
  if (gddClass->sizeDaDiBaseClass() > 0) 
  {
    xmlOut << ": ";
    for (i = 0; i < gddClass->sizeDaDiBaseClass(); ++i)
    {
      if (i>0) {xmlOut << ", ";}
      DaDiBaseClass* gddBaseClass = gddClass->popDaDiBaseClass();
      if (gddBaseClass->virtual_().equals("TRUE"))
      {
        xmlOut << "virtual ";
      }
      xmlOut << DaDiTools::chooseAccess(gddBaseClass->access().transcode()) 
        << " " << gddBaseClass->name().transcode();
    }
  }
  
  
  xmlOut << std::endl << " {" << std::endl << std::endl << "public: " 
    << std::endl << std::endl;

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
    
    if (gddConstructor->desc() != NULL)
    {
      xmlOut << "/// " << gddConstructor->desc().transcode() << std::endl;
    }
    xmlOut << gddClass->className().transcode() << "(";

    if (gddConstructor->sizeDaDiMethArgument() == 0) {constWithZeroArgs = true;}
    for(j=0; j<gddConstructor->sizeDaDiMethArgument(); ++j)
    {
      DaDiMethArgument* gddMethArgument = gddConstructor->popDaDiMethArgument();

      if(j!=0)
      {
        xmlOut << ", ";
      }
      if (gddMethArgument->const_().equals("TRUE"))
      {
        xmlOut << "const ";
      }
      xmlOut << gddMethArgument->type().transcode();
      if (gddMethArgument->name() != NULL)
      {
        xmlOut << " " << gddMethArgument->name().transcode();
      }
      else 
      {
        xmlOut << " value" << j;
      }
    }
    xmlOut << ")";
    if (gddConstructor->code() != NULL)
    {
      xmlOut << std::endl << "{" << std::endl
        << gddConstructor->code().transcode() << std::endl
        << "}" << std::endl << std::endl;
    }
    else
    {
      xmlOut << ";" << std::endl << std::endl;
    }
  }

//
// Default Constructor
//
  if (!constWithZeroArgs)
  {
    xmlOut << "/// Default Constructor " << std::endl 
           << gddClass->className().transcode() << "() ";

    bool firstLine = true;
    for (i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      std::string attType = gddAttribute->type().transcode();
      std::string initValue;
      if (gddAttribute->init() != NULL)
      {
        initValue = gddAttribute->init().transcode();
      }
      else if ((attType == "int") || 
               (attType == "short") || 
               (attType == "long"))
      {
        initValue = "0";
      }
      else if ((attType == "float") || (attType == "double"))
      {
        initValue = "0.0";
      }

      if (initValue != "")
      {
        if (firstLine)
        {
          xmlOut << std::endl << "  : ";
          firstLine = false;
        }
        else
        {
          xmlOut << "," << std::endl << "    ";
        }
        xmlOut << "m_" << gddAttribute->name().transcode() << "(" << initValue
          << ")";
      }
    }
    xmlOut << " {}" << std::endl << std::endl;
  }

  

  if (gddClass->sizeDaDiDestructor() == 0)
  {
    xmlOut << "/// Destructor " << std::endl << "virtual " << "~"
      << gddClass->className().transcode() << "() " << "{}" << std::endl
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
    
      if (gddDestructor->desc() != NULL)
      {
        xmlOut << "/// " << gddDestructor->desc().transcode() << std::endl;
      }
      xmlOut << "~" << gddClass->className().transcode() << "(";
      for(j=0; j<gddDestructor->sizeDaDiMethArgument(); ++j)
      {
        DaDiMethArgument* gddMethArgument =gddDestructor->popDaDiMethArgument();

        if(j!=0)
        {
          xmlOut << ", ";
        }
        if (gddMethArgument->const_().equals("TRUE"))
        {
          xmlOut << "const ";
        }
        xmlOut << gddMethArgument->type().transcode();
        if (gddMethArgument->name() != NULL)
        {
          xmlOut << " " << gddMethArgument->name().transcode();
        }
        else 
        {
          xmlOut << " value" << j;
        }
      }
      xmlOut << ")";
      if (gddDestructor->code() != NULL)
      {
        xmlOut << std::endl << "{" << std::endl
          << gddDestructor->code().transcode() << std::endl
          << "}" << std::endl << std::endl;
      }
      else
      {
        xmlOut << ";" << std::endl << std::endl;
      }
    }
  }

//
// Functions clID() & classID()
//
  if (isEventClass)
  {
    xmlOut << "/// Retrieve pointer to class definition structure" 
      << std::endl << "virtual const CLID& clID() const; " << std::endl;

    xmlOut << "static const CLID& classID(); " << std::endl << std::endl;
  }

// 
// Declaration of selfdefined methods
//
  for(i=0; i < gddClass->sizeDaDiMethod(); i++)
  {
    DaDiMethod* gddMethod = gddClass->popDaDiMethod();
      
    xmlOut << "/// " << gddMethod->desc().transcode() << std::endl;
    if (gddMethod->virtual_().equals("TRUE"))
    {
      xmlOut << "virtual ";
    }
    if (gddMethod->static_().equals("TRUE"))
    {
      xmlOut << "static ";
    }
    if (gddMethod->daDiMethReturn()->const_().equals("TRUE"))
    {
      xmlOut << "const ";
    }
    xmlOut << gddMethod->daDiMethReturn()->type().transcode() << " " 
      << gddMethod->name().transcode() << "(";
    
    for(j=0; j<gddMethod->sizeDaDiMethArgument(); j++)
    {
      DaDiMethArgument* gddMethArgument = gddMethod->popDaDiMethArgument();
      if (j>0)
      {
        xmlOut << ", ";
      }
      if ( gddMethArgument->const_().equals("TRUE") ||
          !isSimple(gddMethArgument->type().transcode()) )
      {
        xmlOut << "const ";
      }
      xmlOut << gddMethArgument->type().transcode();
      if (gddMethArgument->isPointer()) 
      {
        xmlOut << "*";
      }
      if ( gddMethArgument->inout().equals("BOTH") ||
          !isSimple(gddMethArgument->type().transcode()) )
      {
        xmlOut << "&";
      }

      if (gddMethArgument->name() != NULL)
      {
        xmlOut << " " << gddMethArgument->name().transcode();
      }
      else
      {
        xmlOut << " value" << j;
      }
    }
    xmlOut << ");" << std::endl << std::endl;
  }


  
  
//
// Declarations of set() and get() methods for attributes
//
  for(i=0; i < gddClass->sizeDaDiAttribute(); i++) 
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      
    if(gddAttribute->getMeth().equals("TRUE"))
    {
      xmlOut << "/// Retrieve " << gddAttribute->desc().transcode() 
        << std::endl;
      if (!isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "const ";
      }
      xmlOut << gddAttribute->type().transcode();
      if (!isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "&";
      }
      xmlOut << " " << gddAttribute->name().transcode() << "() const; " 
        << std::endl;
    }
      
    if(gddAttribute->setMeth().equals("TRUE"))
    {
      xmlOut << "/// Update " << gddAttribute->desc().transcode() << std::endl 
        << "void set" << firstUp(gddAttribute->name()).transcode() 
        << "(";
      if (!isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "const ";
      }
      xmlOut << gddAttribute->type().transcode();
      if (!isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "&";
      }
      xmlOut << " value);" << std::endl;
    }
      xmlOut << std::endl;
  }

//
// Declaration of set(), get(), rem(), add(), clr() methods for relations
//
  for(i=0; i < gddClass->sizeDaDiRelation(); i++)
    {
    char *get_ret, *set_arg, *add_arg;      
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
      
    if (gddRelation->ratio().equals("1"))
    {
      get_ret = "SmartRef<";
      set_arg = "SmartRef<";
      add_arg = "";
    }
    else if (gddRelation->ratio().equals("*"))
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

    if (gddRelation->getMeth().equals("TRUE"))
    {
      xmlOut << "/// Retrieve " << gddRelation->desc().transcode() << std::endl
        << "const " << get_ret << gddRelation->type().transcode() << ">& " 
                << gddRelation->name().transcode() << "() const;" << std::endl;
    }
    if (gddRelation->setMeth().equals("TRUE"))
    {
      xmlOut << "/// Update " << gddRelation->desc().transcode() << std::endl;
      if (gddRelation->ratio().equals("1") ) {
        xmlOut << "void " << "set" << firstUp(gddRelation->name()).transcode()
          << "(const " << gddRelation->type().transcode() << "* value); "
          << std::endl;
      }
      xmlOut  << "void " << "set"
        << firstUp(gddRelation->name()).transcode() << "(const "
        << set_arg << gddRelation->type().transcode() << ">& value);"
        << std::endl;
    }
    if (strcmp("", add_arg) != 0)
    {
      if (gddRelation->addMeth().equals("TRUE"))
      {
        xmlOut << "/// Add " << gddRelation->desc().transcode() << std::endl
        << "void " << "addTo"
        << firstUp(gddRelation->name()).transcode() 
        << "(" << gddRelation->type().transcode() << "* value);"
        << std::endl << "void " << "addTo"
        << firstUp(gddRelation->name()).transcode()
        << "(" << add_arg << gddRelation->type().transcode()
        << ">& value); " << std::endl;
      }
      if (gddRelation->remMeth().equals("TRUE"))
      {
        xmlOut << "/// Remove " << gddRelation->desc().transcode() << std::endl
          << "void " << "removeFrom"
          << firstUp(gddRelation->name()).transcode() << "("
          << gddRelation->type().transcode() << "* value);" << std::endl
          << "void " << "removeFrom"
          << firstUp(gddRelation->name()).transcode()
          << "(SmartRef<" << gddRelation->type().transcode() << ">& value); "
          << std::endl;
      }
      if (gddRelation->clrMeth().equals("TRUE"))
      {
        xmlOut << "/// Clear " << gddRelation->desc().transcode() << std::endl
        << "void " << "clear" 
              << firstUp(gddRelation->name()).transcode() << "();" 
              << std::endl;
      }
    }      
    xmlOut << std::endl;
  }


//
// Serializers & fillStream
//
  if (isEventClass)
  {
    xmlOut << "/// Serialize the object for writing" << std::endl
      << "virtual StreamBuffer& serialize(StreamBuffer& s) const;" 
      << std::endl << std::endl;

    xmlOut << "/// Serialize the object for reading" << std::endl
      << "virtual StreamBuffer& serialize(StreamBuffer& s);"
      << std::endl << std::endl;

    xmlOut << "/// Fill the ASCII output stream" << std::endl
      << "virtual std::ostream& fillStream(std::ostream& s) const;"
      << std::endl << std::endl;
  }



//
// SteamBuffer<<
//

  /// function header
  xmlOut << "friend StreamBuffer& operator<< (StreamBuffer& s, const "
    << gddClass->className().transcode() << "& obj)" << std::endl
    << "{" << std::endl;

  /// treating boolean values
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

    if (gddAttribute->type().equals("bool"))
    {
      xmlOut << "  unsigned char " << "l_" 
        << gddAttribute->name().transcode() << " = (obj.m_"
        << gddAttribute->name().transcode() << ") ? 1 : 0;"
        << std::endl;
    } 
  }

  bool seriAtt = false;
  xmlOut << "  s ";
  for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
  {
    DOMString baseName = gddClass->popDaDiBaseClass()->name();
    if (!baseName.equals("ContainedObject") &&
        !baseName.equals("DataObject"))
    {
      if (i==0)
      {
        seriAtt = true;
      }
      else
      {
        xmlOut << std::endl << "    ";
      }
      xmlOut << "<< (" << baseName.transcode() << ") obj";
    }
  }
  for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    if(i==0 && !seriAtt)
    {
      seriAtt = true;
    }
    else
    {
      xmlOut << std::endl << "    ";
    }
    if (gddAttribute->type().equals("bool"))
    {
      xmlOut << "<< l_";
    }
    else if (gddAttribute->type().equals("double"))
    {
      xmlOut << "<< (float)obj.m_";
    }
    else
    {
      xmlOut << "<< obj.m_";
    }
    xmlOut << gddAttribute->name().transcode();    
  }
  for(i=0; i<gddClass->sizeDaDiRelation(); i++)
  {
    if (i==0 && !seriAtt)
    {
      xmlOut << "<< obj.m_" 
        << gddClass->popDaDiRelation()->name().transcode();
    }
    else
    {
      xmlOut << std::endl << "    << obj.m_" 
        << gddClass->popDaDiRelation()->name().transcode();
    }
  }
  xmlOut << ";" << std::endl;
    
  xmlOut << "  return s;" << std::endl << "}" << std::endl << std::endl;
  

  
//
// StreamBuffer>>
//

  /// function header
  xmlOut << "friend StreamBuffer& operator>> (StreamBuffer& s, "
    << gddClass->className().transcode() << "& obj)" << std::endl
    << "{" << std::endl;

  bool attBool=false, attFloat=false;

  /// treating boolean values
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

    if (gddAttribute->type().equals("bool"))
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
      xmlOut << "l_" << gddAttribute->name().transcode();
    }
  }
  if (attBool) { xmlOut << ";" << std::endl; }

  /// treating float values
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

    if (gddAttribute->type().equals("double"))
    {
      if (!attFloat)
      {
        xmlOut << "  double ";
        attFloat = true;
      }
      else
      {
        xmlOut << ", ";
      }
      xmlOut << "l_" << gddAttribute->name().transcode();
    }
  }
  if (attFloat) { xmlOut << ";" << std::endl; }

  seriAtt = false;
  xmlOut << "  s ";
  for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
  {
    DOMString baseName = gddClass->popDaDiBaseClass()->name();
    if (!baseName.equals("ContainedObject") &&
        !baseName.equals("DataObject"))
    {
      if (i==0)
      {
        seriAtt = true;
      }
      else
      {
        xmlOut << std::endl << "    ";
      }
      xmlOut << "<< (" << baseName.transcode() << ") obj";
    }
  }
  for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    if(i==0 && !seriAtt)
    {
      seriAtt = true;
    }
    else
    {
      xmlOut << std::endl << "    ";
    }
    if (gddAttribute->type().equals("bool") ||
        gddAttribute->type().equals("double"))
    {
      xmlOut << ">> l_";
    }
    else
    {
      xmlOut << ">> obj.m_";
    }
    xmlOut << gddAttribute->name().transcode();    
  }
  for(i=0; i<gddClass->sizeDaDiRelation(); i++)
  {
    if (i==0 && !seriAtt)
    {
      xmlOut << ">> obj.m_" 
        << gddClass->popDaDiRelation()->name().transcode();
    }
    else
    {
      xmlOut << std::endl << "    " << ">> obj.m_" 
        << gddClass->popDaDiRelation()->name().transcode();
    }
  }
  xmlOut << ";" << std::endl;
    
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    if (gddAttribute->type().equals("bool"))
    {
      xmlOut << "  obj.m_" << gddAttribute->name().transcode()
        << " = (l_" << gddAttribute->name().transcode()
        << ") ? true : false;" << std::endl;
    }
  }
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    if (gddAttribute->type().equals("double"))
    {
      xmlOut << "  obj.m_" << gddAttribute->name().transcode()
        << " = l_" << gddAttribute->name().transcode() << ";" << std::endl;
    }
  }
  xmlOut << "  return s;" << std::endl << "}" << std::endl << std::endl;
  
  

//
// std::ostream<<
//

  /// function header
  xmlOut << "friend std::ostream& operator<< (std::ostream& s, const "
    << gddClass->className().transcode() << "& obj)" << std::endl
    << "{" << std::endl;


  /// treating boolean values
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

    if (gddAttribute->type().equals("bool"))
    {
      xmlOut << "  unsigned char " << "l_" 
        << gddAttribute->name().transcode() << " = (obj.m_"
        << gddAttribute->name().transcode() << ") ? 1 : 0;"
        << std::endl;
    }     
  }

  xmlOut << "  s ";
  for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    if(i!=0)
    {
      xmlOut << std::endl << "    ";
    }
    if (gddAttribute->type().equals("bool"))
    {
      xmlOut << "<< l_";
    }
    else if (gddAttribute->type().equals("double"))
    {
      xmlOut << "<< (float)obj.m_";
    }
    else
    {
      xmlOut << "<< obj.m_";
    }
    xmlOut << gddAttribute->name().transcode();    
  }
  xmlOut << ";" << std::endl;
  xmlOut << "  return s;" << std::endl << "}" << std::endl << std::endl;
  
 
//
//  Private members (attributes)
//
  xmlOut << "private: " << std::endl << std::endl;

  unsigned int maxLengthName = 0, maxLengthType = 0;
  
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    
    if (strlen(gddAttribute->name().transcode()) > maxLengthName)
    { 
      maxLengthName = strlen(gddAttribute->name().transcode()); 
    }

    if (strlen(gddAttribute->type().transcode()) > maxLengthType)
    { 
      maxLengthType = strlen(gddAttribute->type().transcode()); 
    }
  }

  for(i=0; i<gddClass->sizeDaDiRelation(); ++i)
  {
    unsigned int lengthType = 0;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();

    if (gddRelation->ratio().equals("1"))
    { 
      lengthType = 10 + strlen(gddRelation->type().transcode()); 
    }
    else 
    { 
      lengthType = 16 + strlen(gddRelation->type().transcode()); 
    }

    if (strlen(gddRelation->name().transcode()) > maxLengthName)
    { 
      maxLengthName = strlen(gddRelation->name().transcode()); 
    }

    if (lengthType > maxLengthType)
    { 
      maxLengthType = lengthType; 
    }
  }

  maxLengthName = maxLengthName + 4;

  xmlOut.setf(std::ios::left, std::ios::adjustfield);

  for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
    {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    std::string tAttName = gddAttribute->name().transcode();
    std::string attName = " m_" + tAttName + ";";
    xmlOut.width(maxLengthType);
    xmlOut.setf(std::ios::left, std::ios::adjustfield);
    xmlOut << gddAttribute->type().transcode(); 
    xmlOut.width(maxLengthName);
    xmlOut.setf(std::ios::left, std::ios::adjustfield);
    xmlOut << attName << " ///< " << gddAttribute->desc().transcode() 
      << std::endl;
    }

//
// Private members (relations)
//
  for(i=0; i<gddClass->sizeDaDiRelation(); i++)
  {
    std::string rel_type;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();

    if (gddRelation->ratio().equals("1"))
    {
      rel_type = "SmartRef";
    }
    else if (gddRelation->ratio().equals("*"))
    {
      rel_type = "SmartRefVector";
    }
    std::string tRelType = gddRelation->type().transcode();
    std::string tRelName = gddRelation->name().transcode();
    std::string relType = rel_type + '<' + tRelType + '>';
    std::string relName = " m_" + tRelName + ";";
    xmlOut.width(maxLengthType);
    xmlOut << relType; 
    xmlOut.width(maxLengthName);
    xmlOut << relName << " ///< " << gddRelation->desc().transcode() 
      << std::endl;
  }

  xmlOut.unsetf(std::ios::adjustfield);

  xmlOut << std::endl << "};" << std::endl << std::endl;

// ---------------------------------------------------------------------------
// Function Definitions


//
// Including files which were forwarded before
//

  if (gddPackage->sizeImpSoftList() || gddClass->sizeImpSoftList())
    {
    xmlOut << std::endl << std::endl
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
        std::cerr << std::endl << argV0 << ": No information found for type: "
          << impName << std::endl << argV0 << ": Line written: #include \""
          << impName << ".h\"" << std::endl;
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
        std::cerr << std::endl << argV0 << ": No information found for type: "
          << impName << std::endl << argV0 << ": Line written: #include \""
          << impName << ".h\"" << std::endl;
      }
      xmlOut << "#include \"" << impName << ".h\"" << std::endl;
        }
    }
  
  xmlOut << std::endl << std::endl;


  if (isEventClass)
  {
  // 
  // Function clID()
  //
    xmlOut << "inline const CLID& " << gddClass->className().transcode() 
      << "::clID() const " << std::endl << "{" << std::endl << "  return "
      << gddClass->className().transcode() << "::classID();" << std::endl << "}"
      << std::endl << std::endl;

  //
  // Function classID()
  //
    xmlOut << "inline const CLID& " << gddClass->className().transcode() 
      << "::classID()" << std::endl << "{" << std::endl 
      << "  return CLID_" << gddClass->className().transcode() 
      << ";" << std::endl << "}" << std::endl << std::endl;
  }


//
// Writing of selfdefinded Methods
//
  for(i=0; i<gddClass->sizeDaDiMethod(); i++)
  {
    DaDiMethod* gddMethod = gddClass->popDaDiMethod();
    if (gddMethod->code() != NULL)
    {
      xmlOut << "inline ";
      if(gddMethod->virtual_().equals("TRUE"))
      {
        xmlOut << "virtual ";
      }
      if (gddMethod->static_().equals("TRUE"))
      {
        xmlOut << "static ";
      }
      if (gddMethod->daDiMethReturn()->const_().equals("TRUE"))
      {
        xmlOut << "const ";
      }
      xmlOut << gddMethod->daDiMethReturn()->type().transcode() << " " 
        << gddClass->className().transcode() << "::" 
        << gddMethod->name().transcode() << "(";
      for(j=0; j<gddMethod->sizeDaDiMethArgument(); j++)
      {
        DaDiMethArgument* gddMethArgument = gddMethod->popDaDiMethArgument();
        if (j>0)
        {
          xmlOut << ", ";
        }
        if (gddMethArgument->const_().equals("TRUE") ||
            !isSimple(gddMethArgument->type().transcode()) )
        {
          xmlOut << "const ";
        }
        xmlOut << gddMethArgument->type().transcode();
        if (gddMethArgument->isPointer()) 
        {
          xmlOut << "*";
        }
        if ( gddMethArgument->inout().equals("BOTH") ||
            !isSimple(gddMethArgument->type().transcode()) )
        {
          xmlOut << "&";
        }

        if(gddMethArgument->name() != NULL)
        {
          xmlOut << " " << gddMethArgument->name().transcode();
        }
        else
        {
          xmlOut << " value" << j;
        }
      }
      xmlOut << ")" << std::endl << "{" << std::endl << "   " 
        << gddMethod->code().transcode() << std::endl << "}" << std::endl 
        << std::endl;
    }
  }


//
// setX() and getX() for Attributes
//
  i=0;

  for(i=0; i<gddClass->sizeDaDiAttribute(); i++) 
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      
    if(gddAttribute->getMeth().equals("TRUE"))
    {
      xmlOut << "inline ";
      if (!isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "const ";
      }      
      xmlOut << gddAttribute->type().transcode();
      if (!isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "&";
      }
      xmlOut << " " << gddClass->className().transcode() << "::" 
        << gddAttribute->name().transcode() << "() const " << std::endl << "{" 
        << std::endl << "  return m_" << gddAttribute->name().transcode() 
        << ";" << std::endl << "}" << std::endl << std::endl ;
    }
      
    if(gddAttribute->setMeth().equals("TRUE"))
    {
      xmlOut << "inline void " << gddClass->className().transcode() << "::set" 
        << firstUp(gddAttribute->name()).transcode() << "(";
      if (!isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "const ";
      }      
      xmlOut << gddAttribute->type().transcode();
      if (!isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "&";
      }
      xmlOut << " value)" << std::endl << "{" << std::endl << "  m_" 
        << gddAttribute->name().transcode() << " = value; " << std::endl << "}" 
        << std::endl << std::endl;
    }
  }

  
//
// setX(), getX(), addX(), remX(), clrX() Functions for Relations
//
  for(i=0; i<gddClass->sizeDaDiRelation(); i++)
  {
    char *get_ret, *set_arg, *add_arg;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
      
    if (gddRelation->ratio().equals("1"))
    {
      get_ret = "SmartRef<";
      set_arg = "SmartRef<";
      add_arg = "";
    }
    else if (gddRelation->ratio().equals("*"))
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

    if (gddRelation->getMeth().equals("TRUE"))
    {
      xmlOut << "inline const " << get_ret << gddRelation->type().transcode()
        << ">& " << gddClass->className().transcode() << "::"
        << gddRelation->name().transcode()
        << "() const" << std::endl << "{" << std::endl
        << "   return m_" << gddRelation->name().transcode() << ";" 
        << std::endl << "}" << std::endl << std::endl;
    }
    if (gddRelation->setMeth().equals("TRUE"))
    { 
      if (gddRelation->ratio().equals("1") ) { 
        xmlOut << "inline void " << gddClass->className().transcode() << "::" 
          << "set" << firstUp(gddRelation->name()).transcode() << "(const " 
          << gddRelation->type().transcode() << "* value)" << std::endl << "{" 
          << std::endl << "   m_" << gddRelation->name().transcode() 
          << " = value;" << std::endl << "}" << std::endl << std::endl;
      }
      xmlOut  << "inline void " << gddClass->className().transcode() << "::" << "set"
        << firstUp(gddRelation->name()).transcode() << "(const "
        << set_arg << gddRelation->type().transcode() << ">& value)" 
        << std::endl << "{" << std::endl << "   m_"
        << gddRelation->name().transcode() << " = value;" << std::endl << "}" 
        << std::endl << std::endl;
    }
    if (strcmp("", add_arg) != 0)
    {
      if (gddRelation->addMeth().equals("TRUE"))
      {
        xmlOut << "inline void " << gddClass->className().transcode() << "::" 
          << "addTo" << firstUp(gddRelation->name()).transcode() 
          << "(" << gddRelation->type().transcode() << "* value)"  
          << std::endl << "{" << std::endl << "   m_"  
          << gddRelation->name().transcode() << ".push_back(value);"
          << std::endl << "}" << std::endl << std::endl 
          << "inline void " << gddClass->className().transcode() << "::" 
          << "addTo" << firstUp(gddRelation->name()).transcode() 
          << "(" << add_arg << gddRelation->type().transcode() 
          << ">& value)" << std::endl << "{" << std::endl 
          << "   m_" << gddRelation->name().transcode() 
          << ".push_back(value);" << std::endl << "}" 
          << std::endl << std::endl;          
      }
      if (gddRelation->remMeth().equals("TRUE"))
      {
        xmlOut << "inline void " << gddClass->className().transcode() << "::"
          << "removeFrom" << firstUp(gddRelation->name()).transcode()
          << "(" << gddRelation->type().transcode() << "* value)" 
          << std::endl << "{" << std::endl << "   m_" 
          << gddRelation->name().transcode() << ".pop_back();" 
          << std::endl << "}" << std::endl << std::endl << "inline void " 
          << gddClass->className().transcode() << "::" << "removeFrom" 
          << firstUp(gddRelation->name()).transcode() << "(SmartRef<"
          << gddRelation->type().transcode() << ">& value)" << std::endl 
          << "{" << std::endl << "   m_" << gddRelation->name().transcode()
          << ".pop_back();" << std::endl << "}" << std::endl 
          << std::endl;        
      }
      if (gddRelation->clrMeth().equals("TRUE"))
      {
        xmlOut << "inline void " << gddClass->className().transcode() << "::"
          << "clear" << firstUp(gddRelation->name()).transcode() << "()"
          << std::endl << "{" << std::endl << "   m_" 
          << gddRelation->name().transcode() << ".clear();" << std::endl 
          << "}" << std::endl << std::endl;        
      }
    }
  }


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


//
// Definition of container types
//
  if (gddClass->classTemplateVector().equals("TRUE"))
  {
    xmlOut << "// Defintion of vector container type for " 
      << gddClass->className().transcode() << std::endl
      << "template <class TYPE> class ObjectVector;" << std::endl
      << "typedef ObjectVector<" << gddClass->className().transcode()
      << "> " << gddClass->className().transcode() << "Vector;" << std::endl;
  }
  
  if (gddClass->classTemplateList().equals("TRUE"))
  {
    xmlOut << "// Defintion of all list container types for " 
      << gddClass->className().transcode() << std::endl
      << "template <class TYPE> class ObjectList;" << std::endl
      << "typedef ObjectList<" << gddClass->className().transcode() << "> "
      << gddClass->className().transcode() << "List;" << std::endl;
  }
  

//
// End of .h-File
//
  xmlOut << std::endl;
  
  xmlOut << "#endif   ///" << gddPackage->packageName().transcode() << "_" 
    << gddClass->className().transcode() << "_H" << std::endl;

  xmlOut.close();
  delete [] fileName;

  std::cout << " - OK" << std::endl;

  }
}


