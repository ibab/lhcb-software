// $Id: DaDiCppDict.cpp,v 1.18 2002-03-13 18:35:46 mato Exp $

//#include "GaudiKernel/Kernel.h"
#include "DaDiTools.h"
#include "DaDiCppDict.h"
#include "DaDiPackage.h"

#include "parsers/DOMParser.hpp"
#include "dom/DOM_DOMException.hpp"
#include "dom/DOM_NodeList.hpp"
#include "dom/DOM_Element.hpp"
#include "dom/DOM_NamedNodeMap.hpp"
#include "dom/DOMString.hpp"

#include <ctime>
#include <iostream>
#include <list>
#include <fstream>
#include <string>
#include <map>
#include <vector>

//-----------------------------------------------------------------------------
// Implementation file for class : DaDiCppHeader
//
// 12/06/2001 : 
//----------------------------------------------------------------------------

std::string argV0,
            indent(16, ' '),
            remLine = "//" + std::string(78, '-');

//-----------------------------------------------------------------------------
void usage(std::string argV0)
//-----------------------------------------------------------------------------
{
  std::cout << std::endl << std::endl
    << "Usage: " << argV0 << " [-h] [-v] [-o [path]] [-x [path]] xml-file(s)"             << std::endl 
    << "Produce .cpp-files for the Gaudi Dictionary"                         << std::endl << std::endl 
    << "   -h            display this help and exit"                                      << std::endl
    << "   -v            display version information and exit"                            << std::endl
    << "   -i            add additional file-package-information from './AddImports.txt'" << std::endl
    << "   -o [path]     define possible outputdestination with following precedence"     << std::endl
    << "                    -o path     use 'path'"                                       << std::endl
    << "                    -o          use environment-variable 'GODDICTOUT'"            << std::endl
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
std::string printAccessor(std::string access)
//-----------------------------------------------------------------------------
{
  if (access == "PRIVATE") { return "MetaModifier::setPrivate()"; }
  else if (access == "PROTECTED") { return "MetaModifier::setProtected()"; }
  else if (access == "PUBLIC") {return "MetaModifier::setPublic()"; }
  else { return ""; }
}

//-----------------------------------------------------------------------------
std::string checkSymb(std::string word)
//-----------------------------------------------------------------------------
{
  unsigned int i;
  std::string retStr;
  for (i=0; i<word.size(); ++i)
  {
    char c = word[i];
    if (c == '=') { retStr += "Eq"; }
    else if ((c == '(') || (c == ')')) { retStr += "Br"; }
    else if ((c == '{') || (c == '}')) { retStr += "CBr"; }
    else if ((c == '[') || (c == ']')) { retStr += "EBr"; }
    else if (c == '<') { retStr += "Lt"; }
    else if (c == '>') { retStr += "Gt"; }
    else { retStr += c; }
  }
  return retStr;
}

//-----------------------------------------------------------------------------
std::string constructTypes(std::string type)
//-----------------------------------------------------------------------------
{
  std::string constrType;

  if (DaDiTools::isArray(type))
  {
    int pos1, pos2;
    pos1 = type.find_last_of("[");
    pos2 = type.find_last_of("]");
    std::string arrSize = type.substr(pos1,pos2),
                rType = type.substr(0,pos1);

    constrType = "  MetaClass::condCreateClass(\"" + type + "\",\n"
      + indent + "\"array of " + arrSize + " " + rType + "s\",\n"
      + indent + "sizeof(" + type + "),\n"
      + indent + "0,\n"
      + indent + arrSize + ");\n";
  }
  else if (DaDiTools::isStdContainer(type))
  {
    std::string contType = DaDiTools::containerType(type);
    constrType = "  MetaClass::condCreateClass(\"" + type + "\",\n" 
      + indent + "\"standard container of " + contType + "\",\n"
      + indent + "sizeof(" + type + "),\n"
      + indent + "0xC1,\n"
      + indent + "0);\n";
    if (DaDiTools::isStdContainer(contType) || DaDiTools::isArray(contType))
    {
      constrType = constructTypes(contType) + constrType;
    }
  }
  return constrType;
}


//-----------------------------------------------------------------------------
int main(int argC, char* argV[])
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
  std::string dothDir = "Event";

  argV0 = std::string(argV[0]);
  argV0.erase(0,argV0.find_last_of("\\")+1);
  argV0.erase(0,argV0.find_last_of("/")+1);
  
  if (getenv("GAUDIOBJDESCROOT") == NULL)
  {
    std::cerr << "Please set Environmentvariable 'GAUDIOBJDESCROOT'" 
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
        if (getenv("GODDOTHOUT") != NULL)
        {
          dothDir = getenv("GODDOTHOUT");
          int pos1 = dothDir.find(sep),
              pos2 = dothDir.rfind(sep);
          dothDir = dothDir.substr(pos1+1,pos2-pos1-1);
        }
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
            std::cerr << "Environment variable 'GODDICTOUT' not set, "
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
            std::cerr << "Environment variable 'GODXMLDB' not set, " 
              << "using '$(GAUDIOBJDESCROOT)/xml_files'" << std::endl;
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

    DDBEdict::printCppDictionary(gddPackage,envXmlDB,envOut,additionalImports,dothDir);
  }

  return 0;
}


//-----------------------------------------------------------------------------
void DDBEdict::printCppDictionary(DaDiPackage* gddPackage, 
                                  char* envXmlDB, 
                                  char* envOut, 
                                  bool additionalImports,
                                  std::string dothDir)
//-----------------------------------------------------------------------------
{
  int i=0, j=0 ,k=0;
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
      
    while(dbTop.getNodeName().equals("gdd"))
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


  for(k=0; k<gddPackage->sizeDaDiClass(); ++k)
  {
  
  DaDiClass* gddClass = gddPackage->popDaDiClass();

  std::string gddClassName = gddClass->name().transcode(),
              gddClassDesc = gddClass->desc().transcode(),
              gddClassAuthor = gddClass->author().transcode(),
              gddClassID = gddClass->ID().transcode();


  dbExportClass[std::string(gddClass->name().transcode())] =
    std::string(gddPackage->packageName().transcode()) + "/" 
    + std::string(gddClass->name().transcode());


// ------------------------------------------------------------------------
// Start of writing the .cpp-file for filling the MetaModel


  char* cppFileName = new char[256];
  strcpy(cppFileName,envOut);
  strcat(cppFileName, gddClass->name().transcode());
  strcat(cppFileName, "_dict.cpp");
  std::cout << "Writing " << cppFileName;
  std::ofstream metaOut(cppFileName);

//
// CVS-tag and 'donotedit'-message
//

  metaOut << std::endl << std::endl
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
// Include files
//
  metaOut << "//Include files" << std::endl 
    << "#include \"GaudiKernel/Kernel.h\"" << std::endl 
    << "#include \"GaudiKernel/SmartRef.h\"" << std::endl 
    << "#include \"GaudiKernel/SmartRefVector.h\"" << std::endl 
    << std::endl 
    << "#include <string>" << std::endl 
    << std::endl 
    << "#define private public" << std::endl
    << "#define protected public" << std::endl;
  
    std::string impName = gddClassName;

  metaOut << "#include \"" << dothDir << "/" << impName << ".h\"" << std::endl;

  metaOut << "#undef protected" << std::endl
    << "#undef private" << std::endl 
    << std::endl 
    << "#include \"GaudiIntrospection/Introspection.h\"" << std::endl 
    << std::endl 
    << std::endl;

//
// class_dict with constructor
//
  metaOut << remLine << std::endl
    << "class " << gddClassName << "_dict" << std::endl 
    << remLine << std::endl
    << "{" << std::endl 
    << "public:" << std::endl 
    << "  " << gddClassName << "_dict();" << std::endl 
    << "};" << std::endl 
    << std::endl;


// 
// static methods for invocation
//
  for (i=0; i<gddClass->sizeDaDiMethod(); ++i)
  {
    DaDiMethod* gddMethod = gddClass->popDaDiMethod();
    std::string gddMethName = gddMethod->name().transcode(),
                gddMethRetType = gddMethod->daDiMethReturn()->type().transcode();
    bool toReturn = (gddMethRetType == "void") ? false : true;
    if (gddMethName != "serialize")
    {
      metaOut << remLine << std::endl
        << "static void";
      if (toReturn)
      {
        metaOut << "*";
      }
      metaOut << " " << gddClassName << "_" << checkSymb(gddMethName) << "(void* v, std::vector<void*> argList)" << std::endl
        << remLine << std::endl
        << "{" << std::endl;
      if (toReturn && !DaDiTools::isRef(gddMethRetType))
      {
        metaOut << "  static " << gddMethRetType << " ret;" << std::endl;
      }
      metaOut << "  ";
      if (toReturn)
      {
        if (DaDiTools::isRef(gddMethRetType))
        {
          metaOut << "return &(" << gddMethRetType << ") ";
        }
        else
        {
          metaOut << "ret = ";
        }
      }
      metaOut << "((" << gddClassName << "*)v)->" << gddMethName << "(";

      for (j=0; j<gddMethod->sizeDaDiMethArgument(); ++j)
      {
        DaDiMethArgument* gddMethArgument = gddMethod->popDaDiMethArgument();
        std::string gddMethArgType = gddMethArgument->type().transcode();

        if (j!=0) 
        {   
          metaOut << "," << std::endl 
            << "          "; 
        }
        metaOut << "*(" << gddMethArgType;
        if (gddMethArgument->isPointer())
        {
          metaOut << "*";
        }
        metaOut << "*)argList[" << j << "]";
      }

      metaOut << ");" << std::endl;
      if (toReturn && !DaDiTools::isRef(gddMethRetType))
      {
        metaOut << "  return &ret;" << std::endl;
      }
      metaOut << "}" << std::endl
        << std::endl;
    }
  }

//
// static get- and set-methods for attributes
//
  for (i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    std::string gddAttName = gddAttribute->name().transcode(),
                gddAttType = gddAttribute->type().transcode();
    bool setMeth = (gddAttribute->setMeth().equals("FALSE")) ? false : true,
         getMeth = (gddAttribute->getMeth().equals("FALSE")) ? false : true;

    if (getMeth)
    {
      metaOut << remLine << std::endl
        << "static void* " << gddClassName << "_" << gddAttName 
          << "(void* v, std::vector<void*> argList)" << std::endl
        << remLine << std::endl
        << "{" << std::endl
        << "  static " << gddAttType << " ret;" << std::endl
        << "  ret = ((" << gddClassName << "*)v)->" << gddAttName 
          << "();" << std::endl
        << "  return &ret;" << std::endl
        << "}" << std::endl
        << std::endl;
    }

    if (setMeth)
    {
      metaOut << remLine << std::endl
        << "static void " << gddClassName << "_set" 
          << DaDiTools::firstUp(gddAttName) 
          << "(void* v, std::vector<void*> argList)" << std::endl
        << remLine << std::endl
        << "{" << std::endl
        << "  ((" << gddClassName << "*)v)->set" 
          << DaDiTools::firstUp(gddAttName) << "(*(" << gddAttType 
          << "*)argList[0]);" << std::endl
        << "}" << std::endl
        << std::endl;
    }
  }

//
// static get-, set-, addTo-, removeFrom-, clear-methods for relations
//
  for (i=0; i<gddClass->sizeDaDiRelation(); ++i)
  {
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    bool setMeth = (gddRelation->setMeth().equals("FALSE")) ? false : true,
         getMeth = (gddRelation->getMeth().equals("FALSE")) ? false : true,
         addMeth = (gddRelation->addMeth().equals("FALSE")) ? false : true,
         remMeth = (gddRelation->remMeth().equals("FALSE")) ? false : true,
         clrMeth = (gddRelation->clrMeth().equals("FALSE")) ? false : true,
         multRel = (gddRelation->ratio().equals("1")) ? false : true;
    std::string gddRelName = gddRelation->name().transcode(),
                gddRelType = gddRelation->type().transcode(),
                gddRealRelType = (multRel) ? 
                                 "SmartRefVector<" + gddRelType + ">" : 
                                 "SmartRef<" + gddRelType + ">";

    if (getMeth)
    {
      metaOut << remLine << std::endl
        << "static void* " << gddClassName << "_" << gddRelName
          << "(void* v, std::vector<void*> argList)" << std::endl
        << remLine << std::endl 
        << "{" << std::endl
        << "  static " << gddRealRelType << " ret;" << std::endl
        << "  ret = ((" << gddClassName << "*)v)->" << gddRelName
          << "();" << std::endl
        << "  return &ret;" << std::endl
        << "}" << std::endl
        << std::endl;
    }

    if (setMeth)
    {
      metaOut << remLine << std::endl
        << "static void " << gddClassName << "_set"
          << DaDiTools::firstUp(gddRelName)
          << "(void* v, std::vector<void*> argList)" << std::endl
        << remLine << std::endl
        << "{" << std::endl
        << "  ((" << gddClassName << "*)v)->set"
          << DaDiTools::firstUp(gddRelName) << "(*(" << gddRealRelType
          << "*)argList[0]);" << std::endl
        << "}" << std::endl
        << std::endl;
    }

    if (multRel)
    {
      if (clrMeth)
      {
        metaOut << remLine << std::endl
          << "static void " << gddClassName << "_clear"
            << DaDiTools::firstUp(gddRelName)
            << "(void* v, std::vector<void*> argList)" << std::endl
          << remLine << std::endl
          << "{" << std::endl
          << "  ((" << gddClassName << "*)v)->clear"
            << DaDiTools::firstUp(gddRelName) << "();" << std::endl
          << "}" << std::endl
          << std::endl;
      }

      if (addMeth)
      {
        metaOut << remLine << std::endl
          << "static void " << gddClassName << "_addTo"
            << DaDiTools::firstUp(gddRelName)
            << "(void* v, std::vector<void*> argList)" << std::endl
          << remLine << std::endl
          << "{" << std::endl
          << "  ((" << gddClassName << "*)v)->addTo"
          << DaDiTools::firstUp(gddRelName) << "(*(SmartRef<" << gddRelType
            << ">*)argList[0]);" << std::endl
          << "}" << std::endl
          << std::endl;
      }

      if (remMeth)
      {
        metaOut << remLine << std::endl
          << "static void " << gddClassName << "_removeFrom"
            << DaDiTools::firstUp(gddRelName)
            << "(void* v, std::vector<void*> argList)" << std::endl
          << remLine << std::endl
          << "{" << std::endl
          << "  ((" << gddClassName << "*)v)->removeFrom"
          << DaDiTools::firstUp(gddRelName) << "(*(SmartRef<" << gddRelType
            << ">*)argList[0]);" << std::endl
          << "}" << std::endl
          << std::endl;
      }
    }
  }


//
// Instance of class
//
  metaOut << remLine << std::endl
    << "static " << gddClassName << "_dict instance;" << std::endl 
    << remLine << std::endl
    << std::endl;

//
// Start of constructor
//
  metaOut << remLine << std::endl
    << gddClassName << "_dict::" << gddClassName << "_dict()" << std::endl 
    << remLine << std::endl
    << "{" << std::endl;

  if (gddClass->sizeDaDiMethod() || gddClass->sizeDaDiAttribute() || gddClass->sizeDaDiRelation())
  {
    metaOut << "  std::vector<MetaClass*> argTypes = std::vector<MetaClass*>();" << std::endl
      << "  MetaClass* retType;" << std::endl;
  }

//
// Creation of Metaclass
//
  metaOut << "  MetaClass* metaC = new MetaClass(\"" << gddClassName << "\"," << std::endl
    << indent << "\"" << gddClassDesc << "\"," << std::endl
    << indent << "0);" << std::endl 
    << std::endl;

//
// Creation of Superclasses
//

  if (gddClass->sizeDaDiBaseClass())
  {
    metaOut << "  " << gddClassName << "* cl = new " << gddClassName << "();" << std::endl;

    for (i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
    {
      std::string gddBaseClass = gddClass->popDaDiBaseClass()->name().transcode();

      metaOut << "  metaC->addSuperClass(\"" << gddBaseClass << "\"," << std::endl
        << indent << "(((int)cl)-((int)((" << gddBaseClass << "*)cl))));" << std::endl;
    }

    metaOut << "  delete cl;" << std::endl
      << std::endl;
  }

//
// Creation of fields for attributes and relations
//
  for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
 
    std::string gddAttName = gddAttribute->name().transcode(),
                gddAttType = gddAttribute->type().transcode(),
                gddAttDesc = gddAttribute->desc().transcode(),
                gddAttAccess = gddAttribute->access().transcode();

    metaOut << constructTypes(gddAttType);

    metaOut << "  new MetaField(\"" << gddAttName << "\"," << std::endl
      << indent << "\"" << gddAttType << "\"," << std::endl
      << indent << "\"" << gddAttDesc << "\"," << std::endl
      << indent << "&((" << gddClassName << "*)0)->m_" << gddAttName << "," << std::endl
      << indent << "metaC," << std::endl
      << indent << printAccessor(gddAttAccess) << ");" << std::endl
      << std::endl;
  }

  for(i=0; i<gddClass->sizeDaDiRelation(); ++i)
  {
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    std::string gddRelType = gddRelation->type().transcode(),
                gddRelName = gddRelation->name().transcode(),
                gddRelDesc = gddRelation->desc().transcode(),
                gddRelAccess = gddRelation->access().transcode();

    if (gddRelation->ratio().equals("1"))
    {
        gddRelType = "SmartRef<" + gddRelType + ">";
    }
    else // if (gddRelation->ratio().equals("*"))
    {
      gddRelType = "SmartRefVector<" + gddRelType + ">";
    }

    metaOut << "  new MetaField(\"" << gddRelName << "\"," << std::endl
      << indent << "\"" << gddRelType << "\"," << std::endl
      << indent << "\"" << gddRelDesc << "\"," << std::endl
      << indent << "&((" << gddClassName << "*)0)->m_" << gddRelName << "," << std::endl
      << indent << "metaC," << std::endl
      << indent << printAccessor(gddRelAccess) << ");" << std::endl 
      << std::endl;
  }


  for (i=0; i<gddClass->sizeDaDiMethod(); ++i)
  {
    DaDiMethod* gddMethod = gddClass->popDaDiMethod();
    std::string gddMethName = gddMethod->name().transcode(),
                gddMethDesc = gddMethod->desc().transcode(),
                gddMethRetType = gddMethod->daDiMethReturn()->type().transcode();

    if (gddMethName != "serialize")
    {
      metaOut << constructTypes(gddMethRetType);

      metaOut << "  retType = MetaClass::forName(\"" << gddMethRetType << "\");" << std::endl
        << "  argTypes.clear();" << std::endl;
    
      for (j=0; j<gddMethod->sizeDaDiMethArgument(); ++j)
      { 
        DaDiMethArgument* gddMethArgument = gddMethod->popDaDiMethArgument();
        std::string gddMethArgType = gddMethArgument->type().transcode();

        metaOut << constructTypes(gddMethArgType);
        metaOut << "  argTypes.push_back(MetaClass::forName(\"" << gddMethArgType << "\"));" << std::endl;
      }

      metaOut << "  metaC->addMethod(\"" << gddMethName << "\"," << std::endl
        << indent << "\"" << gddMethDesc << "\"," << std::endl
        << indent << "retType," << std::endl
        << indent << "argTypes," << std::endl
        << indent << gddClassName << "_" << checkSymb(gddMethName) << ");" << std::endl
        << std::endl;
    }
  }

  for (i=0; i<gddClass->sizeDaDiAttribute(); ++i)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    std::string gddAttName = gddAttribute->name().transcode(),
                gddAttType = gddAttribute->type().transcode(),
                gddAttDesc = gddAttribute->desc().transcode();
    bool getMeth = (gddAttribute->getMeth().equals("FALSE")) ? false : true,
         setMeth = (gddAttribute->setMeth().equals("FALSE")) ? false : true;

    if (getMeth)
    {
      metaOut << constructTypes(gddAttType)
        << "  retType = MetaClass::forName(\"" << gddAttType << "\");" << std::endl
        << "  argTypes.clear();" << std::endl
        << "  metaC->addMethod(\"" << gddAttName << "\"," << std::endl
        << indent << "\"" << gddAttDesc << "\"," << std::endl
        << indent << "retType," << std::endl
        << indent << "argTypes," << std::endl
        << indent << gddClassName << "_" << gddAttName << ");" << std::endl
        << std::endl;
    }

    if (setMeth)
    {
      metaOut << "  retType = MetaClass::forName(\"void\");" << std::endl
        << "  argTypes.clear();" << std::endl
        << "  argTypes.push_back(MetaClass::forName(\"" << gddAttType << "\"));" << std::endl
        << "  metaC->addMethod(\"set" << DaDiTools::firstUp(gddAttName) << "\"," << std::endl
        << indent << "\"" << gddAttDesc << "\"," << std::endl
        << indent << "retType," << std::endl
        << indent << "argTypes," << std::endl
        << indent << gddClassName << "_set" << DaDiTools::firstUp(gddAttName) << ");" << std::endl
        << std::endl;
    }
  }

  for (i=0; i<gddClass->sizeDaDiRelation(); ++i)
  {
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();
    std::string gddRelName = gddRelation->name().transcode(),
                gddRelType = gddRelation->type().transcode(),
                gddRelDesc = gddRelation->desc().transcode();
    bool getMeth = (gddRelation->getMeth().equals("FALSE")) ? false : true,
         setMeth = (gddRelation->setMeth().equals("FALSE")) ? false : true,
         clrMeth = (gddRelation->clrMeth().equals("FALSE")) ? false : true,
         addMeth = (gddRelation->addMeth().equals("FALSE")) ? false : true,
         remMeth = (gddRelation->remMeth().equals("FALSE")) ? false : true,
         multRel = (gddRelation->ratio().equals("1")) ? false : true;

    if (getMeth)
    {
      metaOut << "  retType = MetaClass::forName(\"" << gddRelType << "\");" << std::endl
        << "  argTypes.clear();" << std::endl
        << "  metaC->addMethod(\"" << gddRelName << "\"," << std::endl
        << indent << "\"" << gddRelDesc << "\"," << std::endl
        << indent << "retType," << std::endl
        << indent << "argTypes," << std::endl
        << indent << gddClassName << "_" << gddRelName << ");" << std::endl
        << std::endl;
    }

    if (setMeth)
    {
      metaOut << "  retType = MetaClass::forName(\"void\");" << std::endl
        << "  argTypes.clear();" << std::endl
        << "  argTypes.push_back(MetaClass::forName(\"" << gddRelType << "\"));" << std::endl
        << "  metaC->addMethod(\"set" << DaDiTools::firstUp(gddRelName) << "\"," << std::endl
        << indent << "\"" << gddRelDesc << "\"," << std::endl
        << indent << "retType," << std::endl
        << indent << "argTypes," << std::endl
        << indent << gddClassName << "_set" << DaDiTools::firstUp(gddRelName) << ");" << std::endl
        << std::endl;
    }

    if (multRel)
    {
      if (clrMeth)
      {
        metaOut << "  retType = MetaClass::forName(\"void\");" << std::endl
          << "  argTypes.clear();" << std::endl
          << "  metaC->addMethod(\"clear" << DaDiTools::firstUp(gddRelName) << "\"," << std::endl
          << indent << "\"" << gddRelDesc << "\"," << std::endl
          << indent << "retType," << std::endl
          << indent << "argTypes," << std::endl
          << indent << gddClassName << "_clear" << DaDiTools::firstUp(gddRelName) << ");" << std::endl
          << std::endl;
      } 

      if (addMeth)
      {
        metaOut << "  retType = MetaClass::forName(\"void\");" << std::endl
          << "  argTypes.clear();" << std::endl
          << "  argTypes.push_back(MetaClass::forName(\"" << gddRelType << "\"));" << std::endl
          << "  metaC->addMethod(\"addTo" << DaDiTools::firstUp(gddRelName) << "\"," << std::endl
          << indent << "\"" << gddRelDesc << "\"," << std::endl
          << indent << "retType," << std::endl
          << indent << "argTypes," << std::endl
          << indent << gddClassName << "_addTo" << DaDiTools::firstUp(gddRelName) << ");" << std::endl
          << std::endl;
      }

      if (remMeth)
      {
        metaOut << "  retType = MetaClass::forName(\"void\");" << std::endl
          << "  argTypes.clear();" << std::endl
          << "  argTypes.push_back(MetaClass::forName(\"" << gddRelType << "\"));" << std::endl
          << "  metaC->addMethod(\"removeFrom" << DaDiTools::firstUp(gddRelName) << "\"," << std::endl
          << indent << "\"" << gddRelDesc << "\"," << std::endl
          << indent << "retType," << std::endl
          << indent << "argTypes," << std::endl
          << indent << gddClassName << "_removeFrom" << DaDiTools::firstUp(gddRelName) << ");" << std::endl
          << std::endl;
      }
    }
  }


//
// Properties
//
  metaOut << "  MetaPropertyList* pl = new MetaPropertyList();" << std::endl 
    << "  pl->setProperty(\"Author\", " << "\"" << gddClassAuthor << "\");" << std::endl
    << "  pl->setProperty(\"ClassID\", " << "\"" << gddClassID << "\");" << std::endl;

  metaOut << "  metaC->setPropertyList(pl);" << std::endl;


//
// End of constructor
//
  metaOut << "}" << std::endl << std::endl << std::endl;

  metaOut.close();
  delete [] cppFileName;
  std::cout << " - OK" << std::endl;

  }
}

