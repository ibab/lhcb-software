// $Id: DaDiFrontEnd.cpp,v 1.2 2001-10-09 17:01:04 mato Exp $

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

DaDiPackage* DDFE::DaDiFrontEnd(char* filename)
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
    }

	gXmlFile = filename;

	DOMParser *parser = new DOMParser;
	parser->setValidationScheme(DOMParser::Val_Auto);
	parser->setDoNamespaces(false);
	ErrorHandler* errReporter = new DaDiTools();
	//  ErrorHandler *errReporter = new DaDiTools();
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
		std::cerr << "An error occured during parsing \n Message: "
			<< e.getMessage() << std::endl;
		errorsOccured = true;
    }

	catch(const DOM_DOMException& e)
    {
		std::cerr << "An error occured during parsing \n Message: "
			<< e.msg.transcode() << std::endl;
	    errorsOccured = true;
    }

	catch(...)
    {
		std::cerr << "An error occured during parsing \n " << std::endl;
		errorsOccured=true;
    }

//
// If no errors occured start walking DOMtree
//
	if (!errorsOccured)
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
  
	gddClass->setClassDesc(node.getAttributes().
		getNamedItem(DOMString::transcode("desc")).
        getNodeValue());
  
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
            
					gddMethod->setDesc(node.getAttributes().
						getNamedItem(DOMString::transcode("desc")).
                        getNodeValue().transcode());
            
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
///////////////
					gddAttribute->setDesc(node.getAttributes().
						getNamedItem(DOMString::transcode("desc")).
						getNodeValue().transcode());
            
					gddAttribute->setAccess(node.getAttributes().
						getNamedItem(DOMString::transcode("access")).
						getNodeValue().transcode());
            
					gddAttribute->setSetMeth(node.getAttributes().
						getNamedItem(DOMString::transcode("set_meth")).
		                getNodeValue().transcode());
            
					gddAttribute->setGetMeth(node.getAttributes().
						getNamedItem(DOMString::transcode("get_meth")).
					    getNodeValue().transcode());
            
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
            
					gddRelation->setDesc(node.getAttributes().
						getNamedItem(DOMString::transcode("desc")).
						getNodeValue().transcode());
            
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
						getNamedItem(DOMString::transcode("set_meth")).
						getNodeValue().transcode());
            
					gddRelation->setGetMeth(node.getAttributes().
						getNamedItem(DOMString::transcode("get_meth")).
						getNodeValue().transcode());
            
					gddRelation->setAddMeth(node.getAttributes().
						getNamedItem(DOMString::transcode("add_meth")).
						getNodeValue().transcode());
            
					gddRelation->setRemMeth(node.getAttributes().
						getNamedItem(DOMString::transcode("rem_meth")).
						getNodeValue().transcode());
            
					gddRelation->setClrMeth(node.getAttributes().
						getNamedItem(DOMString::transcode("clr_meth")).
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
