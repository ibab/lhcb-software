// $Id: DaDiCppDict.cpp,v 1.37 2003-11-27 10:22:53 mato Exp $

#include "DaDiTools.h"
#include "DaDiCppDict.h"
#include "DaDiPackage.h"

#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOMException.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"
#include "xercesc/util/XMLString.hpp"

#include <ctime>
#include <iostream>
#include <list>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

//-----------------------------------------------------------------------------
// Implementation file for class : DaDiCppHeader
//
// 12/06/2001 :
//----------------------------------------------------------------------------

using namespace xercesc;

std::string argV0,
  indent(16, ' '),
  remLine = "//" + std::string(78, '-');

std::vector<std::string> classTypes;

//-----------------------------------------------------------------------------
void usage(std::string argV0)
//-----------------------------------------------------------------------------
{
  std::cout
    << std::endl << std::endl
    << "Usage: " << argV0 << " [-h] [-v] [-o [path]] [-x [path]] xml-file(s)"
    << std::endl
    << "Produce .cpp-files for the Gaudi Dictionary"
    << std::endl << std::endl
    << "   -h            "
    << "display this help and exit"
    << std::endl
    << "   -v            "
    << "display version information and exit"
    << std::endl
    << "   -i            "
    << "add additional file-package-information from './AddImports.txt'"
    << std::endl
    << "   -o [path]     "
    << "define possible outputdestination with following precedence"
    << std::endl
    << "                    -o path     use 'path'"
    << std::endl
    << "                    -o          use environment-variable 'GODDICTOUT'"
    << std::endl
    << "                     default    use local directory"
    << std::endl
    << "   -x [path]     "
    << "define location of 'GaudiCppExport.xml' which holds information"
    << std::endl
    << "                 "
    << "about include-file<->package dependencies, with this precedence"
    << std::endl
    << "                    -x path     use 'path'"
    << std::endl
    << "                    -x          use environment-variable 'GODXMLDB'"
    << std::endl
    << "                     default    use '$(GAUDIOBJDESCROOT)/xml_files'"
    << std::endl
    << "   xml-file(s)   xml-file(s) to be parsed (must have extension '.xml')"
    << std::endl
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
std::string checkType(std::string type, std::string prefix)
//-----------------------------------------------------------------------------
{
  std::vector<std::string>::iterator iter =
    std::find(classTypes.begin(), classTypes.end(), type);
  if (iter != classTypes.end())
  {
    return prefix + "::" + type;
  }
  else
  {
    return type;
  }
}

/*/-----------------------------------------------------------------------------
  std::string stripString(std::string value)
  //----------------------------------------------------------------------------
  {
  std::string::iterator iter =
  std::remove(value.begin(), value.end(), " ");
  return value.erase(iter, value.end());
  }*/

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
void printCppDictionary(DaDiPackage* gddPackage,
                        char* envXmlDB,
                        const char* envOut,
                        bool additionalImports)
//std::string dothDir)
//-----------------------------------------------------------------------------
{
  int i=0, j=0 ,k=0;
  std::map<std::string,std::string> dbExportClass;
  unsigned int methodCounter = 0;

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
    std::cerr << "An error occured during parsing \n Message: "
              << e.getMessage() << std::endl;
    dbErrorsOccured = true;
  }

  catch(const DOMException& e)
  {
    char *msg = XMLString::transcode(e.msg);
    std::cerr << "An error occured during parsing \n Message: "
              << msg << std::endl;
    delete msg;
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
        char *cDbPackName = XMLString::transcode(dbPackNode->getAttributes()->
                                                 getNamedItem(itemStr)->
                                                 getNodeValue());
        std::string dbPackName = cDbPackName;
        XMLString::release(&cDbPackName);
        XMLString::release(&itemStr);
        while(dbClassNode != 0)
        {
          XMLCh* cmpStr2 = XMLString::transcode("class");
          if (XMLString::compareString(dbClassNode->getNodeName(),cmpStr2) == 0)
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
            XMLCh* itemStr = XMLString::transcode("filename");
            if (dbClassNode->getAttributes()->getNamedItem(itemStr) != 0)
            {
              char* cDbFileName = XMLString::transcode(dbClassNode->
                                                       getAttributes()->
                                                       getNamedItem(itemStr)->
                                                       getNodeValue());
              dbFileName = std::string(cDbFileName);
              XMLString::release(&cDbFileName);
              dbExportClass[dbClassName] = dbPackName+"/"+dbFileName;
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
      }
      XMLString::release(&cmpStr);
      dbPackNode = dbPackNode->getNextSibling();
    }
  }

  delete dbParser;
  //XMLString::release(&dbErrReporter);

  if (additionalImports)
  {
    const char* addImportsFile = "AddImports.txt";
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
  }


  for(k=0; k<gddPackage->sizeDaDiClass(); ++k)
  {

    DaDiClass* gddClass = gddPackage->popDaDiClass();
    methodCounter = 0;

    char *gddClassName = XMLString::transcode(gddClass->name()),
      *gddClassDesc = XMLString::transcode(gddClass->desc()),
      *gddClassAuthor = XMLString::transcode(gddClass->author()),
      *gddClassID = XMLString::transcode(gddClass->ID()),
      *gddPackageName = XMLString::transcode(gddPackage->packageName());


    dbExportClass[std::string(gddClassName)] =
      std::string(gddPackageName) + "/"  + std::string(gddClassName);

    for (i=0; i<gddClass->sizeDaDiEnum(); ++i)
    {
      char *gddEnum = XMLString::transcode(gddClass->popDaDiEnum()->name());
      classTypes.push_back(gddEnum);
      XMLString::release(&gddEnum);
    }

    // ------------------------------------------------------------------------
    // Start of writing the .cpp-file for filling the MetaModel


    char* cppFileName = new char[256];
    strcpy(cppFileName,envOut);
    strcat(cppFileName, gddClassName);
    strcat(cppFileName, "_dict.cpp");
    std::cout << "Writing " << cppFileName;
    std::ofstream metaOut(cppFileName);

    //
    // CVS-tag and 'donotedit'-message
    //

    metaOut
      << std::endl << std::endl
      << "//   ****************************************************************"
      << "**********" << std::endl
      << "//   *                                                               "
      << "         *" << std::endl
      << "//   *                      ! ! ! A T T E N T I O N ! ! !            "
      << "         *" << std::endl
      << "//   *                                                               "
      << "         *" << std::endl
      << "//   *  This file was created automatically by GaudiObjDesc, please d"
      << "o not    *" << std::endl
      << "//   *  delete it or edit it by hand.                                "
      << "         *" << std::endl
      << "//   *                                                               "
      << "         *" << std::endl
      << "//   *  If you want to change this file, first change the correspondi"
      << "ng       *" << std::endl
      << "//   *  xml-file and rerun the tools from GaudiObjDesc (or run make i"
      << "f you    *" << std::endl
      << "//   *  are using it from inside a Gaudi-package).                   "
      << "         *" << std::endl
      << "//   *                                                               "
      << "         *" << std::endl
      << "//   ****************************************************************"
      << "**********" << std::endl
      << std::endl << std::endl << std::endl;

    //
    // Include files
    //
    metaOut << "//Include files" << std::endl
            << "#include \"GaudiKernel/Kernel.h\"" << std::endl
            << "#include \"GaudiKernel/SmartRef.h\"" << std::endl
            << "#include \"GaudiKernel/SmartRefVector.h\"" << std::endl;
    metaOut << std::endl
            << "#include <string>" << std::endl
            << std::endl;
    //    << "#define private public" << std::endl
    //    << "#define protected public" << std::endl;

    //    std::string impName = gddClassName;

    //  metaOut << "#include \"" << dothDir << "/" << impName << ".h\"
    //            << std::endl;
    metaOut << "#include \"" << gddClassName << "_dict.h\"" << std::endl;

    metaOut //<< "#undef protected" << std::endl
      //<< "#undef private" << std::endl
      << std::endl
      << "#include \"GaudiIntrospection/Introspection.h\"" << std::endl
      << std::endl
      << std::endl;


    if (gddClass->sizeDaDiTypeDef())
    {
      metaOut << "// Typedef-declarations " << std::endl;
      for (i=0; i<gddClass->sizeDaDiTypeDef(); ++i)
      {
        DaDiTypeDef* gddTypeDef= gddClass->popDaDiTypeDef();
        char *gddTypeDefType   = XMLString::transcode(gddTypeDef->type()),
          *gddTypeDefDef    = XMLString::transcode(gddTypeDef->def());

        metaOut << "typedef " << gddTypeDefType << " " << gddTypeDefDef << ";"
                << std::endl;
        XMLString::release(&gddTypeDefType);
        XMLString::release(&gddTypeDefDef);
      }
      metaOut << std::endl
              << std::endl;
    }


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

    bool classIsAbstract = false;

    //
    // static methods for invocation
    //
    for (i=0; i<gddClass->sizeDaDiMethod(); ++i)
    {
      DaDiMethod* gddMethod = gddClass->popDaDiMethod();
      char *gddMethName = XMLString::transcode(gddMethod->name()),
        *gddMethRetType = XMLString::transcode(gddMethod->daDiMethReturn()->
                                               type()),
        *gddMethVirtual = XMLString::transcode(gddMethod->virtual_());
      XMLCh *privateStr = XMLString::transcode("PRIVATE"),
        *protectedStr = XMLString::transcode("PROTECTED");
      bool toReturn = (strcmp(gddMethRetType,"void")==0) ? false : true,
        gddMethIsTemplated = (XMLString::stringLen
                              (gddMethod->template_()) == 0) ? false : true,
        gddMethIsConst = gddMethod->const_(),
        gddMethIsFriend = gddMethod->friend_(),
        gddMethIsPureVirtual = (strcmp
                                (gddMethVirtual,"PURE") == 0) ? true : false,
        gddMethIsAccessible =
        ((XMLString::compareString(gddMethod->access(), protectedStr) == 0) ||
         (XMLString::compareString
          (gddMethod->access(), privateStr) == 0)) ? false : true;
      XMLString::release(&privateStr);
      XMLString::release(&protectedStr);

      if (gddMethIsPureVirtual) { classIsAbstract = true; }

      if (strcmp(gddMethName,"serialize") != 0 &&
          !DaDiTools::isEmpty(gddMethRetType)
          && !gddMethIsTemplated && !gddMethIsFriend && !gddMethIsPureVirtual
          && gddMethIsAccessible)
      {
        metaOut << remLine << std::endl
                << "static void";
        if (toReturn)
        {
          metaOut << "*";
        }
        metaOut << " " << gddClassName << "_" << checkSymb(gddMethName)
                << "_" << methodCounter++;
        if (gddMethIsConst)
        {
          metaOut << "const";
        }
        if (gddMethod->sizeDaDiMethArgument())
        {
          metaOut << "(void* v, std::vector<void*> argList)" << std::endl;
        }
        else
        {
          metaOut << "(void* v)" << std::endl;
        }
        metaOut << remLine << std::endl
                << "{" << std::endl;
        if (toReturn && !DaDiTools::isRef(gddMethRetType)
            && !DaDiTools::isPointer(gddMethRetType))
        {
          metaOut << "  static ";
          if (gddMethod->daDiMethReturn()->const_())
          {
            metaOut << "const ";
          }
          metaOut << checkType(gddMethRetType,gddClassName) << " ret;"
                  << std::endl;
        }
        metaOut << "  ";
        if (toReturn)
        {
          if (DaDiTools::isRef(gddMethRetType))
          {
            metaOut << "return &(" << gddMethRetType << ") ";
          }
          else if (DaDiTools::isPointer(gddMethRetType))
          {
            metaOut << "return (" << gddMethRetType << ") ";
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
          char* gddMethArgType = XMLString::transcode(gddMethArgument->type());

          if (j!=0)
          {
            metaOut << "," << std::endl
                    << "          ";
          }
          metaOut << "*(" << checkType(gddMethArgType,gddClassName);
          if (gddMethArgument->isPointer())
          {
            metaOut << "*";
          }
          metaOut << "*)argList[" << j << "]";
          XMLString::release(&gddMethArgType);
        }

        metaOut << ");" << std::endl;
        if (toReturn && !DaDiTools::isRef(gddMethRetType)
            && !DaDiTools::isPointer(gddMethRetType))
        {
          metaOut << "  return &ret;" << std::endl;
        }
        metaOut << "}" << std::endl
                << std::endl;
      }
      XMLString::release(&gddMethName);
      XMLString::release(&gddMethRetType);
      XMLString::release(&gddMethVirtual);
    }


    //
    // static get- and set-methods for attributes
    //
    for (i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      char *gddAttName = XMLString::transcode(gddAttribute->name()),
        *gddAttType = XMLString::transcode(gddAttribute->type());
      XMLCh *falseStr = XMLString::transcode("FALSE");
      bool setMeth = (XMLString::compareString
                      (gddAttribute->setMeth(), falseStr) == 0) ? false : true,
        getMeth = (XMLString::compareString
                   (gddAttribute->getMeth(), falseStr) == 0) ? false : true;
      XMLString::release(&falseStr);

      if (getMeth)
      {
        metaOut << remLine << std::endl
                << "static void* " << gddClassName << "_" << gddAttName
                << "_" << methodCounter++ << "(void* v)" << std::endl
                << remLine << std::endl
                << "{" << std::endl;
        if ( DaDiTools::isSimple(gddAttType) )
        {
          metaOut << "  static ";
        }
        else if (!DaDiTools::isFundamental(gddAttType) ||
                 DaDiTools::isPointer(gddAttType))
        {
          metaOut << "  const ";
        }
        metaOut << gddAttType;
        if (!DaDiTools::isSimple(gddAttType))
        {
          metaOut << "& ret =";
        }
        else
        {
          metaOut << " ret;" << std::endl << "  ret = ";
        }
        metaOut << " ((" << gddClassName << "*)v)->"
                << DaDiTools::retGetName(gddAttName) << "();" << std::endl
                << "  return (void*)&ret;" << std::endl
                << "}" << std::endl
                << std::endl;
      }

      if (setMeth)
      {
        metaOut << remLine << std::endl
                << "static void " << gddClassName << "_set"
                << DaDiTools::firstUp(gddAttName) << "_" << methodCounter++
                << "(void* v, std::vector<void*> argList)" << std::endl
                << remLine << std::endl
                << "{" << std::endl
                << "  ((" << gddClassName << "*)v)->set"
                << DaDiTools::firstUp(gddAttName) << "(*("
                << checkType(gddAttType,gddClassName) << "*)argList[0]);"
                << std::endl
                << "}" << std::endl
                << std::endl;
      }
      XMLString::release(&gddAttName);
      XMLString::release(&gddAttType);
    }


    //
    // static get-, set-, addTo-, removeFrom-, clear-methods for relations
    //
    for (i=0; i<gddClass->sizeDaDiRelation(); ++i)
    {
      DaDiRelation* gddRelation = gddClass->popDaDiRelation();
      char *gddRelName = XMLString::transcode(gddRelation->name()),
        *gddRelType = XMLString::transcode(gddRelation->type());
      XMLCh *falseStr = XMLString::transcode("FALSE"),
        *oneStr = XMLString::transcode("1");
      bool setMeth = (XMLString::compareString
                      (gddRelation->setMeth(), falseStr) == 0) ? false : true,
        getMeth = (XMLString::compareString
                   (gddRelation->getMeth(), falseStr) == 0) ? false : true,
        addMeth = (XMLString::compareString
                   (gddRelation->addMeth(), falseStr) == 0) ? false : true,
        remMeth = (XMLString::compareString
                   (gddRelation->remMeth(), falseStr) == 0) ? false : true,
        clrMeth = (XMLString::compareString
                   (gddRelation->clrMeth(), falseStr) == 0) ? false : true,
        multRel = (XMLString::compareString
                   (gddRelation->ratio(), oneStr) == 0) ? false : true;
      std::string gddRealRelType = (multRel) ?
        "SmartRefVector<" + std::string(gddRelType) + ">" :
        std::string(gddRelType) + "*";
      XMLString::release(&falseStr);
      XMLString::release(&oneStr);

      if (getMeth)
      {
        metaOut << remLine << std::endl
                << "static void* " << gddClassName << "_" << gddRelName
                << "_" << methodCounter++ << "(void* v)" << std::endl
                << remLine << std::endl
                << "{" << std::endl
                << (multRel ? "  const " : "  ") << gddRealRelType
                << (multRel ? "&" : "") << " ret = ((" << gddClassName
                << "*)v)->" << DaDiTools::retGetName(gddRelName) << "();"
                << std::endl
                << "  return " <<(multRel ? "(void*)&ret" : " (void*)ret")
                << ";" << std::endl
                << "}" << std::endl
                << std::endl;
      }

      if (setMeth)
      {
        metaOut << remLine << std::endl
                << "static void " << gddClassName << "_set"
                << DaDiTools::firstUp(gddRelName) << "_" << methodCounter++
                << "(void* v, std::vector<void*> argList)" << std::endl
                << remLine << std::endl
                << "{" << std::endl
                << "  ((" << gddClassName << "*)v)->set"
                << DaDiTools::firstUp(gddRelName) << (multRel ? "(*(" : "((")
                << gddRealRelType << (multRel ? "*" : "" ) << ")argList[0]);"
                << std::endl
                << "}" << std::endl
                << std::endl;
      }

      if (multRel)
      {
        if (clrMeth)
        {
          metaOut << remLine << std::endl
                  << "static void " << gddClassName << "_clear"
                  << DaDiTools::firstUp(gddRelName) << "_" << methodCounter++
                  << "(void* v)" << std::endl
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
                  << DaDiTools::firstUp(gddRelName) << "_" << methodCounter++
                  << "(void* v, std::vector<void*> argList)" << std::endl
                  << remLine << std::endl
                  << "{" << std::endl
                  << "  ((" << gddClassName << "*)v)->addTo"
                  << DaDiTools::firstUp(gddRelName) << "((" << gddRelType
                  << "*)argList[0]);" << std::endl
                  << "}" << std::endl
                  << std::endl;
        }

        if (remMeth)
        {
          metaOut << remLine << std::endl
                  << "static void " << gddClassName << "_removeFrom"
                  << DaDiTools::firstUp(gddRelName) << "_" << methodCounter++
                  << "(void* v, std::vector<void*> argList)" << std::endl
                  << remLine << std::endl
                  << "{" << std::endl
                  << "  ((" << gddClassName << "*)v)->removeFrom"
                  << DaDiTools::firstUp(gddRelName) << "((" << gddRelType
                  << "*)argList[0]);" << std::endl
                  << "}" << std::endl
                  << std::endl;
        }
      }
      XMLString::release(&gddRelName);
      XMLString::release(&gddRelType);
    }


    //
    // static methods for constructor
    //
    bool constWithZeroArgs = false;
    if (!classIsAbstract)
    {
      for (i=0; i<gddClass->sizeDaDiConstructor(); ++i)
      {
        DaDiConstructor* gddConstructor = gddClass->popDaDiConstructor();

        metaOut << remLine << std::endl
                << "static void* " << gddClassName << "_constructor_" << i
                << "(";

        if (gddConstructor->sizeDaDiMethArgument())
        {
          metaOut << "std::vector<void*> argList";
        }
        else
        {
          constWithZeroArgs = true;
        }

        metaOut << ")" << std::endl
                << remLine << std::endl
                << "{" << std::endl
                << "  static " << gddClassName << "* ret = new " << gddClassName
                << "(";

        for (j=0; j<gddConstructor->sizeDaDiMethArgument(); ++j)
        {
          DaDiMethArgument* gddMethArgument =
            gddConstructor->popDaDiMethArgument();
          char *gddMethArgType = XMLString::transcode(gddMethArgument->type());

          if (j!=0)
          {
            metaOut << "," << std::endl
                    << "          ";
          }
          metaOut << "*(" << checkType(gddMethArgType,gddClassName);
          if (gddMethArgument->isPointer())
          {
            metaOut << "*";
          }
          metaOut << "*)argList[" << j << "]";
          XMLString::release(&gddMethArgType);
        }

        metaOut << ");" << std::endl
                << "  return ret;" << std::endl
                << "}" << std::endl
                << std::endl;
      }

      if (!constWithZeroArgs)
      {
        metaOut << remLine << std::endl
                << "static void* " << gddClassName << "_constructor_" << ++i
                << "()" << std::endl
                << remLine << std::endl
                << "{" << std::endl
                << "  static " << gddClassName << "* ret = new " << gddClassName
                << "();" << std::endl
                << "  return ret;" << std::endl
                << "}" << std::endl
                << std::endl;
      }
    }



    methodCounter = 0;

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
            << gddClassName << "_dict::" << gddClassName << "_dict()"
            << std::endl
            << remLine << std::endl
            << "{" << std::endl;

    if (gddClass->sizeDaDiMethod() || gddClass->sizeDaDiAttribute() ||
        gddClass->sizeDaDiRelation())
    {
      metaOut
        << "  std::vector<std::string> argTypes = std::vector<std::string>();"
        << std::endl;
    }


    //
    // Creation of Metaclass
    //
    metaOut << "  MetaClass* metaC = new MetaClass(\"" << gddClassName << "\","
            << std::endl
            << indent << "\"" << gddClassDesc << "\"," << std::endl
            << indent << "0);" << std::endl
            << std::endl;


    //
    // Creation of Superclasses
    //

    if (gddClass->sizeDaDiBaseClass())
    {
      if (gddClass->abstract())
      {
        for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
        {
          char *gddBaseClassName =
            XMLString::transcode(gddClass->popDaDiBaseClass()->name());

          //
          // This is a hack (problems when abstract interface is derived
          // from another-one)
          //
          metaOut << "metaC->addSuperClass(\"" << gddBaseClassName << "\",0);"
                  << "// this will not work !!!! " << std::endl;

          //
          //  metaOut << "  " << gddBaseClassName << "* bc = new "
          //  << gddBaseClassName << "();" << std::endl
          //  << "  " << gddClassName << "* cl = (" << gddClassName << "*)bc;"
          //  << std::endl
          //  << "  metaC->addSuperClass(\"" << gddBaseClassName << "\","
          //  << std::endl
          //  << indent << "(int)cl-(int)bc);" << std::endl
          //  << "  delete bc;" << std::endl
          //  << "  delete cl;" << std::endl;
          //
          XMLString::release(&gddBaseClassName);
        }
      }
      else
      {
        metaOut << "  " << gddClassName << "* cl = new " << gddClassName
                << "();" << std::endl;

        for (i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
        {
          char *gddBaseClass =
            XMLString::transcode(gddClass->popDaDiBaseClass()->name());

          metaOut << "  metaC->addSuperClass(\"" << gddBaseClass << "\","
                  << std::endl
                  << indent << "(((int)cl)-((int)((" << gddBaseClass
                  << "*)cl))));" << std::endl;
          XMLString::release(&gddBaseClass);
        }

        metaOut << "  delete cl;" << std::endl
                << std::endl;
      }
    }


    //
    // Creation of constructors
    //
    if (!classIsAbstract)
    {
      for (i=0; i<gddClass->sizeDaDiConstructor(); ++i)
      {
        DaDiConstructor* gddConstructor = gddClass->popDaDiConstructor();
        char *gddConstDesc = XMLString::transcode(gddConstructor->desc());

        if (gddConstructor->sizeDaDiMethArgument())
        {
          metaOut << "  argTypes.clear();" << std::endl;

          for (j=0; j<gddConstructor->sizeDaDiMethArgument(); ++j)
          {
            DaDiMethArgument* gddConstArgument =
              gddConstructor->popDaDiMethArgument();
            char *gddConstArgType =
              XMLString::transcode(gddConstArgument->type());

            metaOut << constructTypes(gddConstArgType);
            metaOut << "  argTypes.push_back(\"" << gddConstArgType << "\");"
                    << std::endl;

            XMLString::release(&gddConstArgType);
          }
        }

        metaOut << "  metaC->addConstructor(\"" << gddConstDesc << "\","
                << std::endl;

        if (gddConstructor->sizeDaDiMethArgument())
        {
          metaOut << indent << "argTypes," << std::endl;
        }

        metaOut << indent << gddClassName << "_constructor_" << i << ");"
                << std::endl << std::endl;
        XMLString::release(&gddConstDesc);
      }

      if (!constWithZeroArgs)
      {
        metaOut << "  metaC->addConstructor(\"default constructor\","
                << std::endl
                << indent << gddClassName << "_constructor_" << ++i << ");"
                << std::endl << std::endl;
      }
    }


    //
    // Creation of fields for attributes and relations
    //
    for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

      char *gddAttName = XMLString::transcode(gddAttribute->name()),
        *gddAttType = XMLString::transcode(gddAttribute->type()),
        *gddAttDesc = XMLString::transcode(gddAttribute->desc()),
        *gddAttAccess = XMLString::transcode(gddAttribute->access());

      metaOut << constructTypes(gddAttType);

      metaOut << "  metaC->addField(\"" << gddAttName << "\"," << std::endl
              << indent << "\"" << gddAttType << "\"," << std::endl
              << indent << "\"" << gddAttDesc << "\"," << std::endl
              << indent << "OffsetOf(" << gddClassName << ", m_"
              << gddAttName << ")," << std::endl
        //       << indent << "&((" << gddClassName << "*)0)->m_" << gddAttName
        //       << "," << std::endl
              << indent << printAccessor(gddAttAccess) << ");" << std::endl
              << std::endl;
      XMLString::release(&gddAttName);
      XMLString::release(&gddAttType);
      XMLString::release(&gddAttDesc);
      XMLString::release(&gddAttAccess);
    }

    for(i=0; i<gddClass->sizeDaDiRelation(); ++i)
    {
      DaDiRelation* gddRelation = gddClass->popDaDiRelation();
      char *gddPureRelType = XMLString::transcode(gddRelation->type()),
        *gddRelName = XMLString::transcode(gddRelation->name()),
        *gddRelDesc = XMLString::transcode(gddRelation->desc()),
        *gddRelAccess = XMLString::transcode(gddRelation->access()),
        *gddRelType = new char[256];
      XMLCh *oneStr = XMLString::transcode("1");


      if (XMLString::compareString(gddRelation->ratio(), oneStr)  == 0)
      {
        strcpy(gddRelType,"SmartRef<");
        strcat(gddRelType,gddPureRelType);
        strcat(gddRelType,">");
      }
      else
        // if (XMLString::compareString(gddRelation->ratio(),
        //                              XMLString::transcode("*"))  == 0)
      {
        strcpy(gddRelType,"SmartRefVector<");
        strcat(gddRelType,gddPureRelType);
        strcat(gddRelType,">");
      }

      metaOut << "  metaC->addField(\"" << gddRelName << "\"," << std::endl
              << indent << "\"" << gddRelType << "\"," << std::endl
              << indent << "\"" << gddRelDesc << "\"," << std::endl
              << indent << "OffsetOf(" << gddClassName << ", m_" << gddRelName
              << ")," << std::endl
        //           << indent << "&((" << gddClassName << "*)0)->m_"
        //           << gddRelName << "," << std::endl
              << indent << printAccessor(gddRelAccess) << ");" << std::endl
              << std::endl;
      delete [] gddRelType;
      XMLString::release(&gddPureRelType);
      XMLString::release(&gddRelName);
      XMLString::release(&gddRelDesc);
      XMLString::release(&gddRelAccess);
      XMLString::release(&oneStr);
    }


    //
    // Creation of selfdefined methods
    //
    for (i=0; i<gddClass->sizeDaDiMethod(); ++i)
    {
      DaDiMethod* gddMethod = gddClass->popDaDiMethod();
      char *gddMethName = XMLString::transcode(gddMethod->name()),
        *gddMethDesc = XMLString::transcode(gddMethod->desc()),
        *gddMethRetType = XMLString::transcode(gddMethod->daDiMethReturn()->
                                               type()),
        *gddMethVirtual = XMLString::transcode(gddMethod->virtual_());
      XMLCh *protectedStr = XMLString::transcode("PROTECTED"),
        *privateStr = XMLString::transcode("PRIVATE");
      bool gddMethIsConst = gddMethod->const_(),
        gddMethIsTemplated = (XMLString::stringLen
                              (gddMethod->template_()) == 0) ? false : true,
        gddMethIsFriend = gddMethod->friend_(),
        gddMethIsPureVirtual = (strcmp
                                (gddMethVirtual,"PURE")==0) ? true : false,
        gddMethIsAccessible =
        ((XMLString::compareString(gddMethod->access(), protectedStr) == 0) ||
         (XMLString::compareString(gddMethod->access(),
                                   privateStr) == 0)) ? false : true;
      XMLString::release(&protectedStr);
      XMLString::release(&privateStr);

      if (strcmp(gddMethName,"serialize") != 0
          && !DaDiTools::isEmpty(gddMethRetType)
          && !gddMethIsTemplated && !gddMethIsFriend && !gddMethIsPureVirtual
          && gddMethIsAccessible)
      {
        metaOut << constructTypes(gddMethRetType);

        metaOut << "  argTypes.clear();" << std::endl;

        for (j=0; j<gddMethod->sizeDaDiMethArgument(); ++j)
        {
          DaDiMethArgument* gddMethArgument = gddMethod->popDaDiMethArgument();
          char *gddMethArgType = XMLString::transcode(gddMethArgument->type());

          metaOut << constructTypes(gddMethArgType);
          metaOut << "  argTypes.push_back(\"" << gddMethArgType << "\");"
                  << std::endl;
          XMLString::release(&gddMethArgType);
        }

        metaOut << "  metaC->addMethod(\"" << gddMethName << "\"," << std::endl
                << indent << "\"" << gddMethDesc << "\"," << std::endl;
        if (strcmp(gddMethRetType,"void") != 0)
        {
          metaOut << indent << "\"" << gddMethRetType << "\"," << std::endl;
        }
        if (gddMethod->sizeDaDiMethArgument())
        {
          metaOut << indent << "argTypes," << std::endl;
        }
        metaOut << indent << gddClassName << "_" << checkSymb(gddMethName)
                << "_" << methodCounter++ ;
        if (gddMethIsConst)
        {
          metaOut << "const";
        }
        metaOut << ");" << std::endl
                << std::endl;
      }
      XMLString::release(&gddMethName);
      XMLString::release(&gddMethDesc);
      XMLString::release(&gddMethRetType);
      XMLString::release(&gddMethVirtual);
    }


    //
    // Creation of setters and getters for attributes
    //
    for (i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      char *gddAttName = XMLString::transcode(gddAttribute->name()),
        *gddAttType = XMLString::transcode(gddAttribute->type()),
        *gddAttDesc = XMLString::transcode(gddAttribute->desc());
      XMLCh *falseStr = XMLString::transcode("FALSE");
      bool getMeth = (XMLString::compareString
                      (gddAttribute->getMeth(), falseStr) == 0) ? false : true,
        setMeth = (XMLString::compareString
                   (gddAttribute->setMeth(), falseStr) == 0) ? false : true;
      XMLString::release(&falseStr);

      if (getMeth)
      {
        metaOut << constructTypes(gddAttType)
                << "  metaC->addMethod(\"" << gddAttName << "\"," << std::endl
                << indent << "\"" << gddAttDesc << "\"," << std::endl
                << indent << "\"" << gddAttType << "\"," << std::endl
                << indent << gddClassName << "_" << gddAttName <<  "_"
                << methodCounter++ << ");" << std::endl
                << std::endl;
      }

      if (setMeth)
      {
        metaOut << "  argTypes.clear();" << std::endl
                << "  argTypes.push_back(\"" << gddAttType << "\");"
                << std::endl
                << "  metaC->addMethod(\"set" << DaDiTools::firstUp(gddAttName)
                << "\"," << std::endl
                << indent << "\"" << gddAttDesc << "\"," << std::endl
                << indent << "argTypes," << std::endl
                << indent << gddClassName << "_set"
                << DaDiTools::firstUp(gddAttName) << "_" << methodCounter++
                << ");" << std::endl
                << std::endl;
      }
      XMLString::release(&gddAttName);
      XMLString::release(&gddAttType);
      XMLString::release(&gddAttDesc);
    }


    //
    // Creation of setters, getters, etc. for relations
    //
    for (i=0; i<gddClass->sizeDaDiRelation(); ++i)
    {
      DaDiRelation* gddRelation = gddClass->popDaDiRelation();
      char *gddRelName = XMLString::transcode(gddRelation->name()),
        *gddRelType = XMLString::transcode(gddRelation->type()),
        *gddRelDesc = XMLString::transcode(gddRelation->desc());
      XMLCh *falseStr = XMLString::transcode("FALSE"),
        *oneStr = XMLString::transcode("1");
      bool getMeth = (XMLString::compareString
                      (gddRelation->getMeth(), falseStr) == 0) ? false : true,
        setMeth = (XMLString::compareString
                   (gddRelation->setMeth(), falseStr) == 0) ? false : true,
        clrMeth = (XMLString::compareString
                   (gddRelation->clrMeth(), falseStr) == 0) ? false : true,
        addMeth = (XMLString::compareString
                   (gddRelation->addMeth(), falseStr) == 0) ? false : true,
        remMeth = (XMLString::compareString
                   (gddRelation->remMeth(), falseStr) == 0) ? false : true,
        multRel = (XMLString::compareString
                   (gddRelation->ratio(), oneStr) == 0) ? false : true;
      std::string gddRealRelType = (multRel) ?
        "SmartRefVector<" + std::string(gddRelType) + ">" :
        std::string(gddRelType);
      XMLString::release(&falseStr);
      XMLString::release(&oneStr);

      if (getMeth)
      {
        metaOut << "  metaC->addMethod(\"" << gddRelName << "\"," << std::endl
                << indent << "\"" << gddRelDesc << "\"," << std::endl
                << indent << "\"" << gddRealRelType << "\"," << std::endl
                << indent << gddClassName << "_" << gddRelName  << "_"
                << methodCounter++ << ");" << std::endl
                << std::endl;
      }

      if (setMeth)
      {
        metaOut << "  argTypes.clear();" << std::endl
                << "  argTypes.push_back(\"" << gddRelType << "\");"
                << std::endl
                << "  metaC->addMethod(\"set" << DaDiTools::firstUp(gddRelName)
                << "\"," << std::endl
                << indent << "\"" << gddRelDesc << "\"," << std::endl
                << indent << "argTypes," << std::endl
                << indent << gddClassName << "_set"
                << DaDiTools::firstUp(gddRelName) << "_" << methodCounter++
                << ");" << std::endl
                << std::endl;
      }

      if (multRel)
      {
        if (clrMeth)
        {
          metaOut << "  metaC->addMethod(\"clear"
                  << DaDiTools::firstUp(gddRelName) << "\"," << std::endl
                  << indent << "\"" << gddRelDesc << "\"," << std::endl
                  << indent << gddClassName << "_clear"
                  << DaDiTools::firstUp(gddRelName) << "_" << methodCounter++
                  << ");" << std::endl
                  << std::endl;
        }

        if (addMeth)
        {
          metaOut << "  argTypes.clear();" << std::endl
                  << "  argTypes.push_back(\"" << gddRelType << "\");"
                  << std::endl
                  << "  metaC->addMethod(\"addTo"
                  << DaDiTools::firstUp(gddRelName) << "\"," << std::endl
                  << indent << "\"" << gddRelDesc << "\"," << std::endl
                  << indent << "argTypes," << std::endl
                  << indent << gddClassName << "_addTo"
                  << DaDiTools::firstUp(gddRelName) << "_" << methodCounter++
                  << ");" << std::endl
                  << std::endl;
        }

        if (remMeth)
        {
          metaOut << "  argTypes.clear();" << std::endl
                  << "  argTypes.push_back(\"" << gddRelType << "\");"
                  << std::endl
                  << "  metaC->addMethod(\"removeFrom"
                  << DaDiTools::firstUp(gddRelName) << "\"," << std::endl
                  << indent << "\"" << gddRelDesc << "\"," << std::endl
                  << indent << "argTypes," << std::endl
                  << indent << gddClassName << "_removeFrom"
                  << DaDiTools::firstUp(gddRelName) << "_" << methodCounter++
                  << ");" << std::endl
                  << std::endl;
        }
      }
      XMLString::release(&gddRelName);
      XMLString::release(&gddRelType);
      XMLString::release(&gddRelDesc);
    }


    //
    // Properties
    //
    metaOut << "  MetaPropertyList* pl = new MetaPropertyList();" << std::endl
            << "  pl->setProperty(\"Author\", " << "\"" << gddClassAuthor
            << "\");" << std::endl
            << "  pl->setProperty(\"ClassID\", " << "\"" << gddClassID << "\");"
            << std::endl;

    metaOut << "  metaC->setPropertyList(pl);" << std::endl;


    //
    // End of constructor
    //
    metaOut << "}" << std::endl << std::endl << std::endl;

    metaOut.close();
    delete [] cppFileName;
    std::cout << " - OK" << std::endl;

    XMLString::release(&gddClassName);
    XMLString::release(&gddClassDesc);
    XMLString::release(&gddClassAuthor);
    XMLString::release(&gddClassID);
    XMLString::release(&gddPackageName);
  }
}

//-----------------------------------------------------------------------------
int main(int argC, char* argV[])
//-----------------------------------------------------------------------------
{

#ifdef _WIN32
  const char* sep = "\\";
#else
  const char* sep = "/";
#endif

  std::vector<char*> files;
  char *envOut = new char[256];
  char *envXmlDB = new char[256];
  std::string nextArg;
  bool additionalImports = false;
  std::string dothDir = "Event";

  strcpy(envOut, "");

  argV0 = std::string(argV[0]);
  argV0.erase(0,argV0.find_last_of("\\")+1);
  argV0.erase(0,argV0.find_last_of("/")+1);

  if (getenv("GAUDIOBJDESCROOT") == NULL)
  {
    //    std::cerr << "Please set Environmentvariable 'GAUDIOBJDESCROOT'"
    //      << std::endl;
    //    exit(1);
    strcpy(envXmlDB, "");
  }
  else
  {
    strcpy(envXmlDB,getenv("GAUDIOBJDESCROOT"));
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
            strcpy(envOut, getenv("GODDICTOUT"));
          }
          else
          {
            std::cerr << "Environment variable 'GODDICTOUT' not set, "
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
            std::cerr << "Environment variable 'GODXMLDB' not set, "
                      << "using '$(GAUDIOBJDESCROOT)/xml_files'" << std::endl;
          }
        }
        else
        {
          strcpy(envXmlDB,argV[i+1]);
          ++i;
        }
      }
      else if (strcmp(argV[i], "-l") == 0)
      {
        nextArg = std::string(argV[i+1]);
        if (((argC-1) == i) ||
            (nextArg.find_last_of(sep) != (nextArg.length()-1)))
        {
          std::cerr << "No path given for argument '-l' or not ended with "
                    << "(back)slash" << std::endl;
          exit(1);
        }
        else
        {
          dothDir = nextArg.substr(0, nextArg.length()-1);
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

  strcat(envXmlDB,"GaudiCppExport.xml");

  for (std::vector<char*>::iterator iter = files.begin();
       iter != files.end(); ++iter)
  {
    DaDiPackage* gddPackage = new DaDiPackage();

    DDFE::DaDiFrontEnd(gddPackage, *iter);

    printCppDictionary(gddPackage,
                       envXmlDB,
                       envOut,
                       additionalImports);
    //                       dothDir);

    delete gddPackage;
  }

  delete [] envXmlDB;
  delete [] envOut;

  return 0;
}

