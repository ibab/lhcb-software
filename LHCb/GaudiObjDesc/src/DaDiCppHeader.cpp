// $Id: DaDiCppHeader.cpp,v 1.29 2002-01-29 11:26:55 mato Exp $

#include "GaudiKernel/Kernel.h"

#include <vector>
#include <ctime>
#include <iostream>
#include <list>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

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

std::vector<std::string> ContainedObjectClasses, KeyedObjectClasses;

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




std::string argV0;


//-----------------------------------------------------------------------------
void printMethodDecl(std::ofstream& xmlOut,
                     DaDiClass* gddClass, 
                     char* accessor)
//-----------------------------------------------------------------------------
{
  int i,j;
  for(i=0; i < gddClass->sizeDaDiMethod(); i++)
  {
    DaDiMethod* gddMethod = gddClass->popDaDiMethod();
      
    if (gddMethod->access().equals(accessor))
    {
    xmlOut << "  /// " << gddMethod->desc().transcode() << std::endl << "  ";
    if (gddMethod->friend_())
    {
      xmlOut << "friend ";
    }
    if (gddMethod->virtual_())
    {
      xmlOut << "virtual ";
    }
    if (gddMethod->static_())
    {
      xmlOut << "static ";
    }
    if (gddMethod->daDiMethReturn()->const_())
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
      if (!DaDiTools::isSimple(gddMethArgument->type().transcode()) && gddMethArgument->const_())
      {
        xmlOut << "const ";
      }
	  else if (gddMethArgument->const_())
	  {
		xmlOut << "const ";
	  }
      xmlOut << gddMethArgument->type().transcode();
      if (gddMethArgument->isPointer()) 
      {
        xmlOut << "*";
      }
      if ( gddMethArgument->inout().equals("BOTH") ||
        !DaDiTools::isSimple(gddMethArgument->type().transcode()) )
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
    xmlOut << ")";
    if (gddMethod->const_())
    {
      xmlOut << " const";
    }
    if (gddMethod->friend_())
    {
      xmlOut << std::endl << "  {" << std::endl << gddMethod->code().transcode()
        << std::endl << "  }" << std::endl << std::endl;
    }
    else
    {
      xmlOut << ";" << std::endl << std::endl;
    }
    }
  }
}


//-----------------------------------------------------------------------------
void printMethodImpl(std::ofstream& xmlOut,
                     DaDiClass* gddClass,
                     char* accessor)
//-----------------------------------------------------------------------------
{
  int i,j;
  for(i=0; i<gddClass->sizeDaDiMethod(); i++)
  {
    DaDiMethod* gddMethod = gddClass->popDaDiMethod();
    if (gddMethod->access().equals(accessor) && 
        gddMethod->code() != NULL && !gddMethod->friend_())
    {
      xmlOut << "inline ";
      if(gddMethod->virtual_())
      {
        xmlOut << "virtual ";
      }
      if (gddMethod->static_())
      {
        xmlOut << "static ";
      }
      if (gddMethod->daDiMethReturn()->const_())
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
        if (gddMethArgument->const_() ||
          (!DaDiTools::isSimple(gddMethArgument->type().transcode()) &&
			 gddMethArgument->const_()) )
        {
          xmlOut << "const ";
        }
        xmlOut << gddMethArgument->type().transcode();
        if (gddMethArgument->isPointer()) 
        {
          xmlOut << "*";
        }
        if ( gddMethArgument->inout().equals("BOTH") ||
          !DaDiTools::isSimple(gddMethArgument->type().transcode()) )
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
      xmlOut << ")";
      if (gddMethod->const_())
      {
        xmlOut << " const";
      }
      xmlOut << std::endl << "{" << std::endl << "   " 
        << gddMethod->code().transcode() << std::endl << "}" << std::endl 
        << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------
void printSetGetAttDecl(std::ofstream& xmlOut,
                     DaDiClass* gddClass,
                     char* accessor)
//-----------------------------------------------------------------------------
{
  int i;
  for(i=0; i < gddClass->sizeDaDiAttribute(); i++) 
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

    if(gddAttribute->getMeth().equals(accessor))
    {
      xmlOut << "  /// Retrieve " << gddAttribute->desc().transcode() 
        << std::endl << "  ";
      if (!DaDiTools::isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "const ";
      }
      xmlOut << gddAttribute->type().transcode();
      if (!DaDiTools::isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "&";
      }
      xmlOut << " " << gddAttribute->name().transcode() << "() const; " 
        << std::endl << std::endl;
    }
      
    if(gddAttribute->setMeth().equals(accessor))
    {
      xmlOut << "  /// Update " << gddAttribute->desc().transcode() << std::endl 
        << "  void set" << firstUp(gddAttribute->name()).transcode() 
        << "(";
      if (!DaDiTools::isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "const ";
      }
      xmlOut << gddAttribute->type().transcode();
      if (!DaDiTools::isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "&";
      }
      xmlOut << " value);" << std::endl << std::endl;
    }
  }
}


//-----------------------------------------------------------------------------
void printSetGetAttImpl(std::ofstream& xmlOut,
                        DaDiClass* gddClass,
                        char* accessor)
//-----------------------------------------------------------------------------
{
  int i=0;

  for(i=0; i<gddClass->sizeDaDiAttribute(); i++) 
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      
    if(gddAttribute->getMeth().equals(accessor))
    {
      xmlOut << "inline ";
      if (!DaDiTools::isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "const ";
      }      
      xmlOut << gddAttribute->type().transcode();
      if (!DaDiTools::isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "&";
      }
      xmlOut << " " << gddClass->className().transcode() << "::" 
        << gddAttribute->name().transcode() << "() const " << std::endl << "{" 
        << std::endl << "  return m_" << gddAttribute->name().transcode() 
        << ";" << std::endl << "}" << std::endl << std::endl ;
    }
      
    if(gddAttribute->setMeth().equals(accessor))
    {
      xmlOut << "inline void " << gddClass->className().transcode() << "::set" 
        << firstUp(gddAttribute->name()).transcode() << "(";
      if (!DaDiTools::isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "const ";
      }      
      xmlOut << gddAttribute->type().transcode();
      if (!DaDiTools::isSimple(gddAttribute->type().transcode()))
      {
        xmlOut << "&";
      }
      xmlOut << " value)" << std::endl << "{" << std::endl << "  m_" 
        << gddAttribute->name().transcode() << " = value; " << std::endl << "}" 
        << std::endl << std::endl;
    }
  }
}


//-----------------------------------------------------------------------------
void printSetGetRelDecl(std::ofstream& xmlOut,
                        DaDiClass* gddClass,
                        char* accessor)
//-----------------------------------------------------------------------------
{
  int i;
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

    if (gddRelation->getMeth().equals(accessor))
    {
      xmlOut << "  /// Retrieve " << gddRelation->desc().transcode() << std::endl;
      if( gddRelation->ratio().equals("1"))
      {
        xmlOut << "  const " << gddRelation->type().transcode() << "* " 
               << gddRelation->name().transcode() << "() const;" << std::endl;
        xmlOut << "  " << gddRelation->type().transcode() << "* "
          << gddRelation->name().transcode() << "();" << std::endl << std::endl;
      } else {
        xmlOut << "  const " << get_ret << gddRelation->type().transcode() << ">& " 
               << gddRelation->name().transcode() << "() const;" << std::endl 
               << std::endl;
      }
    }
    if (gddRelation->setMeth().equals(accessor))
    {
      xmlOut << "  /// Update " << gddRelation->desc().transcode() << std::endl;
      if (gddRelation->ratio().equals("1") ) {
        xmlOut << "  void " << "set" << firstUp(gddRelation->name()).transcode()
          << "(const " << gddRelation->type().transcode() << "* value); "
          << std::endl;
      }
      xmlOut  << "  void " << "set"
        << firstUp(gddRelation->name()).transcode() << "(const "
        << set_arg << gddRelation->type().transcode() << ">& value);"
        << std::endl << std::endl;
    }
    if (strcmp("", add_arg) != 0)
    {
      if (gddRelation->addMeth().equals(accessor))
      {
        xmlOut << "  /// Add " << gddRelation->desc().transcode() << std::endl
        << "  void " << "addTo"
        << firstUp(gddRelation->name()).transcode() 
        << "(" << gddRelation->type().transcode() << "* value);"
        << std::endl << "  void " << "addTo"
        << firstUp(gddRelation->name()).transcode()
        << "(const " << add_arg << gddRelation->type().transcode()
        << ">& value); " << std::endl << std::endl;
      }
      if (gddRelation->remMeth().equals(accessor))
      {
        xmlOut << "  /// Remove " << gddRelation->desc().transcode() << std::endl
          << "  void " << "removeFrom"
          << firstUp(gddRelation->name()).transcode() << "("
          << gddRelation->type().transcode() << "* value);" << std::endl
          << "  void " << "removeFrom"
          << firstUp(gddRelation->name()).transcode()
          << "(const SmartRef<" << gddRelation->type().transcode() << ">& value); "
          << std::endl << std::endl;
      }
      if (gddRelation->clrMeth().equals(accessor))
      {
        xmlOut << "  /// Clear " << gddRelation->desc().transcode() << std::endl
        << "  void " << "clear" 
              << firstUp(gddRelation->name()).transcode() << "();" 
              << std::endl << std::endl;
      }
    }      
  }
}


//-----------------------------------------------------------------------------
void printSetGetRelImpl(std::ofstream& xmlOut,
                        DaDiClass* gddClass,
                        char* accessor)
//-----------------------------------------------------------------------------
{
  int i;
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

    if (gddRelation->getMeth().equals(accessor))
    {
      if (gddRelation->ratio().equals("1"))
      {
        xmlOut << "inline const " << gddRelation->type().transcode() << "* "
          << gddClass->className().transcode() << "::"
          << gddRelation->name().transcode()
          << "() const" << std::endl << "{" << std::endl
          << "   return m_" << gddRelation->name().transcode() << ";" 
          << std::endl << "}" << std::endl << std::endl;
        xmlOut << "inline " << gddRelation->type().transcode() << "* "
          << gddClass->className().transcode() << "::"
          << gddRelation->name().transcode()
          << "() " << std::endl << "{" << std::endl
          << "   return m_" << gddRelation->name().transcode() << ";" 
          << std::endl << "}" << std::endl << std::endl;
      } else {
        xmlOut << "inline const " << get_ret << gddRelation->type().transcode() << ">& "
          << gddClass->className().transcode() << "::"
          << gddRelation->name().transcode()
          << "() const" << std::endl << "{" << std::endl
          << "   return m_" << gddRelation->name().transcode() << ";" 
          << std::endl << "}" << std::endl << std::endl;
      }
    }
    if (gddRelation->setMeth().equals(accessor))
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
      if (gddRelation->addMeth().equals(accessor))
      {
        xmlOut << "inline void " << gddClass->className().transcode() << "::" 
          << "addTo" << firstUp(gddRelation->name()).transcode() 
          << "(" << gddRelation->type().transcode() << "* value)"  
          << std::endl << "{" << std::endl << "   m_"  
          << gddRelation->name().transcode() << ".push_back(value);"
          << std::endl << "}" << std::endl << std::endl 
          << "inline void " << gddClass->className().transcode() << "::" 
          << "addTo" << firstUp(gddRelation->name()).transcode() 
          << "(const " << add_arg << gddRelation->type().transcode() 
          << ">& value)" << std::endl << "{" << std::endl 
          << "   m_" << gddRelation->name().transcode() 
          << ".push_back(value);" << std::endl << "}" 
          << std::endl << std::endl;          
      }
      if (gddRelation->remMeth().equals(accessor))
      {
        xmlOut << "inline void " << gddClass->className().transcode() << "::"
          << "removeFrom" << firstUp(gddRelation->name()).transcode()
          << "(" << gddRelation->type().transcode() << "* value)" 
          << std::endl << "{" << std::endl 

		  << "  SmartRefVector<" << gddRelation->type().transcode()
		  << ">::iterator iter;" << std::endl
		  << "  for (iter = m_" << gddRelation->name().transcode()
		  << ".begin(); iter != m_" << gddRelation->name().transcode()
		  << ".end(); ++iter)" << std::endl << "  {" << std::endl
		  << "    if (iter->target() == value)" << std::endl << "    {" 
		  << std::endl << "      m_" << gddRelation->name().transcode()
		  << ".erase(iter);" << std::endl << "    }" << std::endl << "  }"
		  
		  << std::endl << "}" << std::endl << std::endl << "inline void " 
          << gddClass->className().transcode() << "::" << "removeFrom" 
          << firstUp(gddRelation->name()).transcode() << "(const SmartRef<"
          << gddRelation->type().transcode() << ">& value)" << std::endl 
          << "{" << std::endl 
		  
		  << "  SmartRefVector<" << gddRelation->type().transcode()
		  << ">::iterator iter =" << std::endl
		  << "    std::find(m_" << gddRelation->name().transcode()
		  << ".begin(), m_" << gddRelation->name().transcode()
		  << ".end(), value);" << std::endl << "  if (iter != m_"
		  << gddRelation->name().transcode() << ".end() )" << std::endl 
		  << "  {" << std::endl << "    m_" << gddRelation->name().transcode()
		  << ".erase(iter);" << std::endl << "  }" 
		  
		  << std::endl << "}" << std::endl 
          << std::endl;        
      }
      if (gddRelation->clrMeth().equals(accessor))
      {
        xmlOut << "inline void " << gddClass->className().transcode() << "::"
          << "clear" << firstUp(gddRelation->name()).transcode() << "()"
          << std::endl << "{" << std::endl << "   m_" 
          << gddRelation->name().transcode() << ".clear();" << std::endl 
          << "}" << std::endl << std::endl;        
      }
    }
  }
}


//-----------------------------------------------------------------------------
template<class T> void printEnums(std::ofstream& xmlOut,
                                  T* gdd,
                                  char* accessor)
//-----------------------------------------------------------------------------
{
  int i;
  for (i=0; i<gdd->sizeDaDiEnum(); ++i)
  {
    DaDiEnum* gddEnum = gdd->popDaDiEnum();

    if ((strcmp(accessor,"") == 0) || (gddEnum->access().equals(accessor))) 
    {
      xmlOut << "  enum " << gddEnum->name().transcode() << " {" 
        << gddEnum->value().transcode() << "};   ///<" 
        << gddEnum->desc().transcode() << std::endl;
    }
  }
}


//-----------------------------------------------------------------------------
void printMembers(std::ofstream& xmlOut,
                  DaDiClass* gddClass,
                  char* accessor)
//-----------------------------------------------------------------------------
{
  int i;
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

  //
  // Private members (attributes)
  //
  for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
  {
    DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

    if (gddAttribute->access().equals(accessor))
    {
      std::string tAttName = gddAttribute->name().transcode();
      std::string attName = " m_" + tAttName + ";";
      xmlOut << "  ";
      xmlOut.width(maxLengthType);
      xmlOut.setf(std::ios::left, std::ios::adjustfield);
      xmlOut << gddAttribute->type().transcode(); 
      xmlOut.width(maxLengthName);
      xmlOut.setf(std::ios::left, std::ios::adjustfield);
      xmlOut << attName << " ///< " << gddAttribute->desc().transcode() 
        << std::endl;
    }
  }

  //
  // Private members (relations)
  //
  for(i=0; i<gddClass->sizeDaDiRelation(); i++)
  {
    std::string rel_type;
    DaDiRelation* gddRelation = gddClass->popDaDiRelation();

    if (gddRelation->access().equals(accessor))
    {
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
      xmlOut << "  ";
      xmlOut.width(maxLengthType);
      xmlOut << relType; 
      xmlOut.width(maxLengthName);
      xmlOut << relName << " ///< " << gddRelation->desc().transcode() 
        << std::endl;
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
  int i,j;
  bool isEventClass = false, classTemplate = false, classTemplateVector = false,
    classTemplateList = false;
  std::vector<std::string>::iterator coIter, koIter;

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
  if (gddClass->classID() != NULL)
  {
    isEventClass = true;
  }


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

  xmlOut << std::endl << " *" << std::endl;

  if (gddClass->classDesc() != NULL)
  {
    xmlOut << " *  " << gddClass->classDesc().transcode();
  }

  if (gddClass->longDesc() != NULL)
  {
    xmlOut << std::endl << gddClass->longDesc().transcode();
  }

  xmlOut << std::endl << " *" << std::endl
    << " *  @author " << gddClass->classAuthor().transcode() << std::endl
    << " *  created " << ctime(&ltime) << " *" << std::endl
    << " */" << std::endl << std::endl;

//
// class definition
//
  xmlOut << "class " << gddClass->className().transcode();
  
  if (gddClass->sizeDaDiBaseClass() > 0) 
  {
    xmlOut << ": ";
    for (i = 0; i < gddClass->sizeDaDiBaseClass(); ++i)
    {
      DaDiBaseClass* gddBaseClass = gddClass->popDaDiBaseClass();
      std::string fullBaseClName = gddBaseClass->name().transcode();
      std::string baseClName = fullBaseClName.substr(0,fullBaseClName.find_first_of("<"));
      koIter = std::find(KeyedObjectClasses.begin(), KeyedObjectClasses.end(), baseClName);
      if (koIter != KeyedObjectClasses.end())
      {
        KeyedObjectClasses.push_back(gddClass->className().transcode());
        classTemplate = true;
      }
      //////////////////////  tbd
      coIter = std::find(ContainedObjectClasses.begin(), ContainedObjectClasses.end(), baseClName);
      if (coIter != ContainedObjectClasses.end())
      {
        ContainedObjectClasses.push_back(gddClass->className().transcode());
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
      if (gddBaseClass->virtual_())
      {
        xmlOut << "virtual ";
      }
      xmlOut << DaDiTools::chooseAccess(gddBaseClass->access().transcode()) 
        << " " << fullBaseClName;
    }
  }
  
  
  xmlOut << std::endl << "{" << std::endl << std::endl;
  
  
  // 
  //   PUBLIC AREA
  //
  xmlOut << "public: " << std::endl << std::endl;

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
      xmlOut << "  /// " << gddConstructor->desc().transcode() << std::endl;
    }
    xmlOut << "  " << gddClass->className().transcode() << "(";

    if (gddConstructor->sizeDaDiMethArgument() == 0) {constWithZeroArgs = true;}
    for(j=0; j<gddConstructor->sizeDaDiMethArgument(); ++j)
    {
      DaDiMethArgument* gddMethArgument = gddConstructor->popDaDiMethArgument();

      if(j!=0)
      {
        xmlOut << ", ";
      }
      if (gddMethArgument->const_())
      {
        xmlOut << "const ";
      }
      xmlOut << gddMethArgument->type().transcode();
      if (gddMethArgument->isPointer())
      {
        xmlOut << "*";
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
    xmlOut << ")";

    if (gddConstructor->code() != NULL)
    {
      if (gddConstructor->initList() != NULL)
      {
        xmlOut << std::endl << "    : " << gddConstructor->initList().transcode();
      }

      xmlOut << std::endl << "  {" << std::endl
        << gddConstructor->code().transcode() << std::endl
        << "  }" << std::endl << std::endl;
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
    xmlOut << "  /// Default Constructor " << std::endl 
           << "  " << gddClass->className().transcode() << "() ";

    bool firstLine = true;
    for (i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      std::string attType = gddAttribute->type().transcode();
      int lastspace = attType.find_last_of(" ");
      attType = attType.substr(lastspace+1, attType.size()-lastspace);
      std::string initValue;
      if (gddAttribute->init() != NULL)
      {
        initValue = gddAttribute->init().transcode();
      }
      else if ((attType == "int") || (attType == "unsigned") ||
               (attType == "short") || (attType == "signed") ||
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
          xmlOut << std::endl << "    : ";
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
    xmlOut << "  /// Destructor " << std::endl << "  virtual " << "~"
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
        xmlOut << "  /// " << gddDestructor->desc().transcode() << std::endl;
      }
      xmlOut << "  ~" << gddClass->className().transcode() << "(";
      for(j=0; j<gddDestructor->sizeDaDiMethArgument(); ++j)
      {
        DaDiMethArgument* gddMethArgument =gddDestructor->popDaDiMethArgument();

        if(j!=0)
        {
          xmlOut << ", ";
        }
        if (gddMethArgument->const_())
        {
          xmlOut << "const ";
        }
        xmlOut << gddMethArgument->type().transcode();
        if (gddMethArgument->isPointer())
        {
          xmlOut << "*";
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
      xmlOut << ")";
      if (gddDestructor->code() != NULL)
      {
        xmlOut << std::endl << "  {" << std::endl
          << gddDestructor->code().transcode() << std::endl
          << "  }" << std::endl << std::endl;
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
    xmlOut << "  /// Retrieve pointer to class definition structure" 
      << std::endl << "  virtual const CLID& clID() const; " << std::endl;

    xmlOut << "  static const CLID& classID(); " << std::endl << std::endl;
  }

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
  if (!isEventClass)
  {
	xmlOut << "  /// Operator overloading for serializing (writing)" 
	  << std::endl 
	  << "  friend StreamBuffer& operator<< (StreamBuffer& s, const "
	  << gddClass->className().transcode() << "& obj)" << std::endl << "  {" 
	  << std::endl << "    return obj.serialize(s);" << std::endl << "  }" 
	  << std::endl << std::endl;

	xmlOut << "  /// Operator overloading for serializing (reading)"
	  << std::endl
	  << "  friend StreamBuffer& operator>> (StreamBuffer& s, "
	  << gddClass->className().transcode() << "& obj)" << "  {" << std::endl
	  << "    return obj.serialize(s);" << std::endl << "  }" << std::endl
	  << std::endl;

	xmlOut << "  /// Operator overloading for stringoutput" << std::endl
	  << "  friend std::ostream& operator<< (std::ostream& s, const "
	  << gddClass->className().transcode() << "& obj)" << std::endl << "  {" 
	  << std::endl << "    return obj.fillStream(s);" << std::endl << "  }" 
	  << std::endl << std::endl;

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

  bool streamIn=false, streamOut=false, ostreamOut=false;
  for (i=0; i<gddClass->sizeDaDiMethod(); ++i)
  {
    DaDiMethod* gddMethod = gddClass->popDaDiMethod();
    if (gddMethod->name().equals("serialize") &&
        gddMethod->daDiMethReturn()->type().equals("StreamBuffer&") &&
		!gddMethod->const_())
    {
      streamIn = true;
    }
    if (gddMethod->name().equals("serialize") &&
        gddMethod->daDiMethReturn()->type().equals("StreamBuffer&") &&
		gddMethod->const_())
    {
      streamOut = true;
    }
    if (gddMethod->name().equals("fillStream") &&
        gddMethod->daDiMethReturn()->type().equals("std::ostream&"))
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


  // print public members
  printEnums(xmlOut, gddClass, "PUBLIC");
  printMembers(xmlOut, gddClass, "PUBLIC");

  //
  //   PROTECTED AREA
  //

  xmlOut << std::endl << "protected: " << std::endl << std::endl;

  printMethodDecl(xmlOut, gddClass, "PROTECTED");
  printEnums(xmlOut, gddClass, "PUBLIC");
  printMembers(xmlOut,gddClass, "PROTECTED");


  //
  //   PRIVATE AREA
  //

  xmlOut << std::endl << "private: " << std::endl << std::endl;


  // print declarations of private functions
  printMethodDecl(xmlOut, gddClass, "PRIVATE");

//
//  Private members (attributes)
//
  printEnums(xmlOut, gddClass, "PRIVATE");
  printMembers(xmlOut, gddClass, "PRIVATE");
  
  xmlOut << std::endl << "};" << std::endl << std::endl
    << "// -----------------------------------------------------------------------------"
    << std::endl << "//   end of class" << std::endl
    << "// -----------------------------------------------------------------------------"
    << std::endl;

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



//
// SteamBuffer<<
//

  if (!streamOut)
  {
    /// function header
    xmlOut << "inline StreamBuffer& " << gddClass->className().transcode()
		<< "::serialize(StreamBuffer& s) const " << std::endl
      << "{" << std::endl;

    /// treating boolean values
    for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

      if (gddAttribute->type().equals("bool"))
      {
        xmlOut << "  unsigned char " << "l_" 
          << gddAttribute->name().transcode() << " = (m_"
          << gddAttribute->name().transcode() << ") ? 1 : 0;"
          << std::endl;
      } 
    }

    bool seriAtt = false;
    for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
    {
      DOMString baseName = gddClass->popDaDiBaseClass()->name();
      if (!baseName.equals("ContainedObject") &&
          !baseName.equals("DataObject"))
      {
        xmlOut << "  " << baseName.transcode() << "::serialize(s);";
      }
    }
    for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      if(i==0 && !seriAtt)
      {
        xmlOut << "  s ";
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
        xmlOut << "<< (float)m_";
      }
      else
      {
        xmlOut << "<< m_";
      }
      xmlOut << gddAttribute->name().transcode();    
    }
    for(i=0; i<gddClass->sizeDaDiRelation(); i++)
    {
      if (i==0 && !seriAtt)
      {
        xmlOut << "s << m_" 
          << gddClass->popDaDiRelation()->name().transcode() << "(this)";
      }
      else
      {
        xmlOut << std::endl << "    << m_" 
          << gddClass->popDaDiRelation()->name().transcode() << "(this)";
      }
    }
	if (seriAtt)
	{
      xmlOut << ";" << std::endl;
	}
    
    xmlOut << "  return s;" << std::endl << "}" << std::endl << std::endl;
  }

  
//
// StreamBuffer>>
//

  if(!streamIn)
  {
    /// function header
    xmlOut << "inline StreamBuffer& " << gddClass->className().transcode()
	  << "::serialize(StreamBuffer& s)" << std::endl
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
          xmlOut << "  float ";
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

    bool seriAtt = false;
    for(i=0; i<gddClass->sizeDaDiBaseClass(); ++i)
    {
      DOMString baseName = gddClass->popDaDiBaseClass()->name();
      if (!baseName.equals("ContainedObject") &&
          !baseName.equals("DataObject"))
      {
		  xmlOut << "  " << baseName.transcode() << "::serialize(s);" << std::endl;
      }
    }
    for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      if(i==0 && !seriAtt)
      {
        xmlOut << "  s ";
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
        xmlOut << ">> m_";
      }
      xmlOut << gddAttribute->name().transcode();    
    }
    for(i=0; i<gddClass->sizeDaDiRelation(); i++)
    {
      if (i==0 && !seriAtt)
      {
        xmlOut << "s >> m_" 
          << gddClass->popDaDiRelation()->name().transcode() << "(this)";
      }
      else
      {
        xmlOut << std::endl << "    " << ">> m_" 
          << gddClass->popDaDiRelation()->name().transcode() << "(this)";
      }
    }
	if (seriAtt)
	{
      xmlOut << ";" << std::endl;
	}
    
    for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      if (gddAttribute->type().equals("bool"))
      {
        xmlOut << "  m_" << gddAttribute->name().transcode()
          << " = (l_" << gddAttribute->name().transcode()
          << ") ? true : false;" << std::endl;
      }
    }
    for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
      if (gddAttribute->type().equals("double"))
      {
        xmlOut << "  m_" << gddAttribute->name().transcode()
          << " = l_" << gddAttribute->name().transcode() << ";" << std::endl;
      }
    }
    xmlOut << "  return s;" << std::endl << "}" << std::endl << std::endl;
  }
  

//
// std::ostream<<
//
  
  if (!ostreamOut)
  {
    /// function header
    xmlOut << "inline std::ostream& " << gddClass->className().transcode()
	  << "::fillStream(std::ostream& s) const" << std::endl
      << "{" << std::endl;


    /// treating boolean values
    for(i=0; i<gddClass->sizeDaDiAttribute(); ++i)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();

      if (gddAttribute->type().equals("bool"))
      {
        xmlOut << "  char " << "l_" 
          << gddAttribute->name().transcode() << " = (m_"
          << gddAttribute->name().transcode() << ") ? 'T' : 'F';"
          << std::endl;
      }     
    }

    bool seriAtt = false;
    for(i=0; i<gddClass->sizeDaDiAttribute(); i++)
    {
      DaDiAttribute* gddAttribute = gddClass->popDaDiAttribute();
    if(i==0)
	{
      /*xmlOut << "  s << \"class " << gddClass->className().transcode()
             << ":\""; */
      xmlOut << "  s << \"{ \"" << std::endl << "    << \" " 
           << gddAttribute->name().transcode() << ":\\t\" ";
		  seriAtt = true;
	}
	else
	{
		xmlOut << " << std::endl" << std::endl << "    << \"   " 
           << gddAttribute->name().transcode() << ":\\t\" ";
	}
    if (gddAttribute->type().equals("bool"))
    {  
      xmlOut << "<< l_";
    }
    else if (gddAttribute->type().equals("double"))
    {
      xmlOut << "<< (float)m_";
    }
    else
    {
      xmlOut << "<< m_";
    }
    xmlOut << gddAttribute->name().transcode();
  }
	if (seriAtt)
	{
      xmlOut << " << \" } \";" << std::endl;
	}
    xmlOut << "  return s;" << std::endl << "}" << std::endl << std::endl;
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

//
// Definition of container types
//
  if (classTemplate)
  {
    xmlOut << "//Defintion of keyed container for "
      << gddClass->className().transcode() << std::endl
//      << "namespace Containers { class HashMap; };" << std::endl
//      << "template <class TYPE, class MAPPING> class KeyedContainer;" << std::endl
      << "typedef KeyedContainer<" << gddClass->className().transcode()
      << ", Containers::HashMap> " << gddClass->className().transcode() << "s;"
      << std::endl;
  }

/////////////////////////////// tbd - to be deleted   
  //
  // Definition of container types
  //
  if (classTemplateVector)
  {
    xmlOut << "// Defintion of vector container type for " 
      << gddClass->className().transcode() << std::endl
      << "template <class TYPE> class ObjectVector;" << std::endl
      << "typedef ObjectVector<" << gddClass->className().transcode()
      << "> " << gddClass->className().transcode() << "Vector;" << std::endl;
  }
  
  if (classTemplateList)
  {
    xmlOut << "// Defintion of all list container types for " 
      << gddClass->className().transcode() << std::endl
      << "template <class TYPE> class ObjectList;" << std::endl
      << "typedef ObjectList<" << gddClass->className().transcode() << "> "
      << gddClass->className().transcode() << "List;" << std::endl;
  }
/////////////////////////////////////  
}


//-----------------------------------------------------------------------------
void printNamespace(std::ofstream& xmlOut,
                    DaDiNamespace* gddNamespace,
                    DaDiPackage* gddPackage,
                    std::map<std::string,std::string> dbExportClass)
//-----------------------------------------------------------------------------
{
  int i;
  xmlOut << std::endl << "/** @namespace " << gddNamespace->name().transcode()
    << std::endl << " *" << std::endl << " *  " << gddNamespace->desc().transcode()
    << std::endl << " */" << std::endl << "namespace " << gddNamespace->name().transcode()
    << std::endl << "{" << std::endl;

  printEnums(xmlOut, gddNamespace, "");

  for (i=0; i<gddNamespace->sizeDaDiClass(); ++i)
  {
    xmlOut << std::endl;
    DaDiClass* gddClass = gddNamespace->popDaDiClass();
    printClass(xmlOut,gddClass,gddPackage,dbExportClass);
  }

  xmlOut << "};" << std::endl;

}


//-----------------------------------------------------------------------------
void printCppHeader(DaDiPackage* gddPackage, 
                    char* envXmlDB, 
                    char* envOut, 
                    bool additionalImports)
//-----------------------------------------------------------------------------
{
  int i=0, j=0, k=0;
  std::map<std::string,std::string> dbExportClass;
  ContainedObjectClasses.push_back("ContainedObject");
  KeyedObjectClasses.push_back("KeyedObject");

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

    char* fileName = new char[256];
    strcpy(fileName, envOut);
    strcat(fileName, gddNamespace->name().transcode());
    strcat(fileName, ".h");
    std::cout << "Writing " << fileName;
    std::ofstream xmlOut(fileName);  

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
      << gddNamespace->name().transcode() << "_H" << std::endl << "#define " 
      << gddPackage->packageName().transcode() << "_" 
      << gddNamespace->name().transcode() << "_H 1" << std::endl << std::endl;

    //
    // Includes
    //
    xmlOut << "// Include files" << std::endl;

    for(i=0; i<gddPackage->sizeImpStdList(); i++)
    {
      xmlOut << "#include <" << gddPackage->popImpStdList() << ">"
        << std::endl;
    }

    for (j=0; j<gddNamespace->sizeDaDiClass(); ++j)
    {
      DaDiClass* gddClass = gddNamespace->popDaDiClass();
      for(i=0; i<gddClass->sizeImpStdList(); i++)
      {
        xmlOut << "#include <" << gddClass->popImpStdList() << ">"
          << std::endl;
      }
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


    for (j=0; j<gddNamespace->sizeDaDiClass(); ++j)
    {
      DaDiClass* gddClass = gddNamespace->popDaDiClass();
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
    }


    //
    // Forward declarations
    //  
    gddPackage->remDblImpSoftList();

    bool classForwardDecl = false;
    for (i=0; i<gddNamespace->sizeDaDiClass(); ++i)
    {
      if (gddNamespace->popDaDiClass()->sizeImpSoftList())
      {
        classForwardDecl = true;
        break;
      }
    }

    if (gddPackage->sizeImpSoftList() || classForwardDecl)
    {
      xmlOut << std::endl << std::endl
        << "// Forward declarations" << std::endl;
      for(i=0; i<gddPackage->sizeImpSoftList(); i++)
      {
        xmlOut << "class " << gddPackage->popImpSoftList() << ";" << std::endl;
      }
      for (j=0; j<gddNamespace->sizeDaDiClass(); ++j)
      {
        DaDiClass* gddClass = gddNamespace->popDaDiClass();
        gddClass->remDblImpSoftList();
        for(i=0; i<gddClass->sizeImpSoftList(); i++)
        {
          xmlOut << "class " << gddClass->popImpSoftList() << ";" << std::endl;
        }
      }
    }
  
    xmlOut << std::endl << std::endl;

    printNamespace(xmlOut,gddNamespace,gddPackage,dbExportClass);

    //
    // End of .h-File
    //
    xmlOut << std::endl;
  
    xmlOut << "#endif   ///" << gddPackage->packageName().transcode() << "_" 
      << gddNamespace->name().transcode() << "_H" << std::endl;

    xmlOut.close();
    delete [] fileName;

    std::cout << " - OK" << std::endl;
  }





  for (k=0; k<gddPackage->sizeDaDiClass(); ++k)
  {  

    DaDiClass* gddClass = gddPackage->popDaDiClass();


    dbExportClass[std::string(gddClass->className().transcode())] =
      std::string(gddPackage->packageName().transcode()) + "/" 
      + std::string(gddClass->className().transcode());

    // ------------------------------------------------------------------------
    // Here starts the writing of the .h-file


    char* fileName = new char[256];
    strcpy(fileName, envOut);
    strcat(fileName, gddClass->className().transcode());
    strcat(fileName, ".h");
    std::cout << "Writing " << fileName;
    std::ofstream xmlOut(fileName);  

  
    //
    // 'donotedit'-message
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


    printClass(xmlOut, gddClass, gddPackage, dbExportClass);


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

    printCppHeader(gddPackage,envXmlDB,envOut,additionalImports);
  }
  return 0;
}


