import os, sys, shutil
import re, pickle, time, datetime, copy
import codecs, cgi
import xml.dom as xmlDOM

def unique(seq, keepstr=True):
  print "uniquefy"
  if seq is None:
    return []
  t = type(seq)
  if t==str:
    t = (list, ''.join)[bool(keepstr)]
  seen = []
  return t(c for c in seq if not (c in seen or seen.append(c)))

class messageXML:
    def __init__(self,xmlTitle="LHCbNightlies"):
        '''Create the basic XML'''
        xmlImplementation = xmlDOM.getDOMImplementation()
        xmlDoc = xmlImplementation.createDocument(None,xmlTitle, None)
        self.xmlDoc = xmlDoc
        self.fileName = time.strftime("NightliesMessages_%Y%m%d_%H%M%S.xml", time.localtime())
        self.namespace = "LHCbNightlies"

        self.foundLineagesList = []
        self.foundLeafList = []
        self.lineageList = []
        self.lineageDict = {}
        self.foundNodesList = []
        self.keyList = []

    def getDoc(self):
        '''return the XML object'''
        return self.xmlDoc

    def getRootNode(self):
        '''return the document head node'''
        return self.xmlDoc.documentElement

    def printDoc(self):
        '''pretty print the XML document'''
        return self.xmlDoc.toprettyxml()
#    def setXMLNodeAttribute(self,childPar,motherList = ["root"],attributeMap = None,namespace = "LHCbNightlies"):
#        '''add an attribute to a node'''
#        child = str(childPar) 
#        mother = str(motherPar)
#        if motherPar is None or mother.lower() == "root":
#            
#        else:
#            childList = self.xmlDoc.getElementsByTagName(mother)    
#        
#            for attribute,value in attributeMap.iteritems():
#                xmlNode.setAttribute(str(attribute),str(value))



    def getLineageDict(self,nodeElement,localLineageDict = {}, namespace = "LHCbNightlies"):
        '''For a DOM element get its mothers as a dictionary with the node names and objetcs
        however since dictionaries are unsorted the lineage gets lost'''
        self.lineageDict = copy.copy(localLineageDict)
        localLineageDict = []
        rootDocumentName = str(self.xmlDoc.nodeName)
        rootNodeName = str(self.xmlDoc.documentElement.nodeName)
        nodeElementName = str(nodeElement.nodeName)
        motherNode = nodeElement.parentNode
        motherNodeName = str(nodeElement.parentNode.nodeName)
        if (motherNodeName == rootDocumentName):
            self.lineageDict[nodeElementName] = nodeElement
            return self.lineageDict
        else:
            self.lineageDict[nodeElementName] = nodeElement
            motherNodeName = str(motherNode.parentNode.nodeName)
            self.getLineageDict(motherNode,self.lineageDict)
            return self.lineageDict

    def getLineageList(self,nodeElement, localLineageList = [], namespace = "LHCbNightlies"):
        '''For a DOM element get its mothers as a list with each the node names and objetcs in a 2-list
        clumsy, but at least the lineage is preserved in a sorted list
        probably an own version of a sorted list would be more handy...'''
        # is there a better way to null the local/class internal variables before a recursive call of the same function?
        self.lineageList = copy.copy(localLineageList)
        localLineageList = []

        rootDocumentName = str(self.xmlDoc.nodeName)
        rootNodeName = str(self.xmlDoc.documentElement.nodeName)
        nodeElementName = str(nodeElement.nodeName)
        motherNode = nodeElement.parentNode
        motherNodeName = str(nodeElement.parentNode.nodeName)
        if (motherNodeName == rootDocumentName):
            self.lineageList.append([nodeElementName,nodeElement])
            return self.lineageList
        else:
            self.lineageList.append([nodeElementName,nodeElement])
            motherNodeName = str(motherNode.parentNode.nodeName)
            self.getLineageList(motherNode,self.lineageList)
            return self.lineageList


    def getLineage(self,nodeNamePar,boolDict = False):
        '''returns a list of lineages (standard as lists or when set to True as dictionaries)
        found for leafs with the given name'''
        nodeName = str(nodeNamePar)
        nodeList = self.findNode(nodeName)
        self.foundLineagesList = []
        for nodeIterator in nodeList:
            self.lineageDict = {}
            self.lineageList = []
            if boolDict:
                self.foundLineagesList.append(self.getLineageDict(nodeIterator,self.lineageDict))
            else:
                self.foundLineagesList.append(self.getLineageList(nodeIterator,self.lineageList))
        return self.foundLineagesList

    def getLeaf(self,branchList = ["LHCbNightlies"],listReverse = True):
        '''Returns a list of found XML objects at the end of a given branch
        by standard a list beginning with the root object is assumed
        else set the reverse to false'''
        if listReverse:
            branchList.reverse()
        if branchList[-1] != str(self.xmlDoc.documentElement.nodeName):
             branchList.append(str(self.xmlDoc.documentElement.nodeName))
        self.foundLeafList = []
        self.foundNodesList = self.getLineage(branchList[0])
        for foundNodesListIterator in self.foundNodesList:
            foundNodesListIteratorKeys = self._listKeys(foundNodesListIterator)
            foundNodesListIteratorValues = self._listValues(foundNodesListIterator)
            if foundNodesListIteratorKeys == branchList:
                self.foundLeafList.append(foundNodesListIteratorValues[0])
        return self.foundLeafList

    def _listKeys(self,listArray = []):
        self.keyList = []
        if len(listArray)==0:
            return []
        else:
            for arrayIterator in listArray:
                if isinstance(arrayIterator,list):
                  self.keyList.append(arrayIterator[0])
            return self.keyList

    def _listValues(self,listArray = []):
        self.keyList = []
        if len(listArray)==0:
            return []
        else:
            for arrayIterator in listArray:
                if isinstance(arrayIterator,list):
                    self.keyList.append(arrayIterator[1])
            return self.keyList

    def hasAttributes(self,node,attributeDict={}):
        '''Returns true, if a node has attributes with the given values'''
        nodeMap = node.attributes
        tagTrue = False
        for tagKey,tagValue  in attributeDict:
            if tagKey in nodeMap and nodeMap[tagKey]==tagValue:
                tagTrue = True
        return tagTrue
                

    def appendNode(self,childPar,motherList = [],attributeMap = {},textString = None,namespace = "LHCbNightlies"):
        '''Append a child node to the XML document
        if no list of mothers is given as node will directly attached to the root
        else the node is attached to the mother if the mother node is found, else it is attached directly to the root
        attributes are read from a dictionary of tags and corresponding values, i.e. {tag1:value1, tag2:value2,...}
        plus a text field for error/warning messages
        Attention: if more than one matching mother element are found, an own child object will be attached to each
        '''
        if isinstance(motherList,list) is False:
            if (isinstance(motherList,str) or isinstance(motherList,int) or isinstance(motherList,float) or isinstance(motherList,long)):
                motherList = [str(motherList)]
            else:
                raise TypeError('a list of mother nodes or a string was expected')
        if (len(motherList) == 0) or (motherList[-1] != str(self.xmlDoc.documentElement.nodeName)):
             motherList.insert(0,str(self.xmlDoc.documentElement.nodeName))
        child = str(childPar)
        motherObjectList = self.getLeaf(motherList)
        for motherObject in motherObjectList:
            #commentNode = self.xmlDoc.createComment(child)
            newXMLChild = self.xmlDoc.createElementNS(namespace,child)
            #newXMLChild.appendChild(commentNode)
            if namespace is None:
                namespace = self.namespace
            if attributeMap is None:
                attributeMap = {}
            else:
                for attribute,value in attributeMap.iteritems():
                    newXMLChild.setAttribute(str(attribute),str(value))
            if textString is not None:
                textElement = self.xmlDoc.createTextNode(str(textString))
                newXMLChild.appendChild(textElement)
            motherObject.appendChild(newXMLChild)
        return motherObjectList

    def appendSpecificNode(self,childPar,motherObject,attributeMap = {},textString = None,namespace = "LHCbNightlies"):
        '''Appends a node to a given node'''
        child = str(childPar)

        #commentNode = self.xmlDoc.createComment(child)
        newXMLChild = self.xmlDoc.createElementNS(namespace,child)
        #newXMLChild.appendChild(commentNode)

        if namespace is None:
          namespace = self.namespace
        if attributeMap is None:
          attributeMap = {}
        else:
          for attribute,value in attributeMap.iteritems():
            newXMLChild.setAttribute(str(attribute),str(value))
        if textString is not None:
          textElement = self.xmlDoc.createTextNode(str(textString))
          newXMLChild.appendChild(textElement)

        appendedChild = motherObject.appendChild(newXMLChild)
        return appendedChild

    def modifyNode(self,motherList = [],attributeMap = None,textString = None,namespace = "LHCbNightlies"):
        if isinstance(motherList,list) is False:
            if (isinstance(motherList,str) or isinstance(motherList,int) or isinstance(motherList,float) or isinstance(motherList,long)):
                motherList = [str(motherList)]
            else:
                raise TypeError('a list of mother nodes or a string was expected')
        if (len(motherList) == 0) or (motherList[-1] != str(self.xmlDoc.documentElement.nodeName)):
             motherList.insert(0,str(self.xmlDoc.documentElement.nodeName))
        motherObjectList = self.getLeaf(motherList)
        for motherObject in motherObjectList:
            for attributeKey in attributeMap.keys():
                try:
                    motherObject.removeAttribute(attributeKey)
                except:
                    pass
                motherObject.setAttribute(attributeKey,attributeMap[attributeKey])
                if textString is not None:
                    textElement = self.xmlDoc.createTextNode(str(textString))
                    motherObject.appendChild(textElement)
        return motherObjectList

    def modifySpecificNode(self,motherObject,attributeMap = {},textString = None,namespace = "LHCbNightlies"):
      for attributeKey in attributeMap.keys():
        try:
          motherObject.removeAttribute(attributeKey)
        except:
          pass
        motherObject.setAttribute(attributeKey,attributeMap[attributeKey])
        if textString is not None:
          textElement = self.xmlDoc.createTextNode(str(textString))
          motherObject.appendChild(textElement)
      return motherObject

    def findNode(self,nodeNamePar,namespace = "LHCbNightlies"):
        '''returns a list of found elements'''
        nodeName = str(nodeNamePar)
        return self.xmlDoc.getElementsByTagName(str(nodeName))

    def appendText(self,nodeElementPar = None,textString = None):
        '''append a text object to an element
        if the element does not exist the text 
        is appended to the document root'''
        if textString is None or nodeElementPar is None:
            pass
        else:
            nodeElement = str(nodeElementPar)
            textElement = self.xmlDoc.createTextNode(str(textString))
            childList = self.xmlDoc.getElementsByTagName(nodeElement)
            if nodeElement.lower() == "root" or len(childList) == 0:
                self.xmlDoc.documentElement.appendChild(textElement)
            else:
                for childElement in childList:
                    childElement.appendChild(textElement)


    def writeDoc(self,fileName = time.strftime("NightliesMessages_%Y%m%d_%H%M%S.xml", time.localtime())):
        file = open(fileName, "w")
        self.xmlDoc.writexml(file, "\n", "  ")
        file.close()
    
#     class sanitizeElement:
#         def __init__(self,element):
#             self.selement=element
#             self.typeStatus = False

#             if element is not None:
#                 self.notNone=True
#             else: 
#                 self.notNone=False
#         def getNotNone(self):
#             return self.notNone
#         def isInteger(self):
#             if self.notNone:
#                 self.typeStatus = isinstance(self.element, int )
#             else:
#                 self.typeStatus = False
#         def isString(self):
#             if self.notNone:
#                 self.typeStatus = isinstance(self.element, str )
#             else:
#                 self.typeStatus = False
#         def getTypeOK(self):
#             return self.typeStatus
#         def getInteger(self):
#             if self.notNone :
#                 isInteger()
#                 return int(self.element)
#             else:
#                 return 0
#         def getString(self):
#             if self.notNone:
#                 isString()
#                 return str(self.element)
#             else:
#                 return ""
