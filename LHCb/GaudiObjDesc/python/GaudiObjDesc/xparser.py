import os
from lxml import etree
import logging

# Required in a doctest
from StringIO import StringIO

class GODResolver(etree.Resolver):
    """
    Implementation of lxml.etree.Resolver to be able to locate the DTD in a
    fixed directory instead of the same directory of the XML file.
    """
    def __init__(self, dtdpath=None):
        """
        Initialize the instance with the full path to the DTD file (gdd.dtd).
        """
        self.dtdpath = dtdpath
    def resolve(self, system_url, public_id, context):
        """
        When asked for gdd.dtd, return the file declared in the constructor.
        """
        if self.dtdpath:
            if os.path.basename(system_url) == "gdd.dtd" or public_id == 'gdd.dtd':
                return self.resolve_filename(self.dtdpath, context)
        return None

class GODSrcParser(object):
    '''
    Parser of G.O.D. source XML files.
    '''
    def __init__(self, dtdPath=None, dtd_validation=True):
        '''
        Initialize the instance with an optional location for the G.O.D. DTD.
        '''
        self.log = logging.getLogger('GODSrcParser')
        self.dtdPath = dtdPath
        self.parser = etree.XMLParser(dtd_validation=dtd_validation, attribute_defaults=dtd_validation)
        if self.dtdPath:
            self.parser.resolvers.add(GODResolver(self.dtdPath))

    def parseSource(self, sourcefile):
        """
        Parse the file given as argument returning a hierarchy of dictionaries
        representing the XML file.
        For each element, there is one dictionary containing the keys:
         - 'attrs': a dictionary of the attributes
         - 'cont': the CDATA of the element
         - <tag>: a list of all the dictionaries corresponding to the element '<tag>'

        >>> xml = '''<root>
        ...   <element key='value'></element>
        ...   <element>text</element>
        ... </root>
        ... '''
        >>> parser = GODSrcParser(dtd_validation=False)
        >>> parser.parseSource(StringIO(xml))
        {'root': [{'attrs': {}, 'element': [{'attrs': {'key': 'value'}}, {'cont': 'text', 'attrs': {}}]}]}
        """
        self.log.info('Parsing Source File %s', sourcefile)
        root = etree.parse(sourcefile, parser=self.parser)
        def el2dict(element):
            '''
            Helper function to recursively convert the elements.
            '''
            # first get the attibutes
            attrs = dict(element.attrib)
            # Fix the namespace in attributes:
            NS = '{http://www.w3.org/XML/1998/namespace}'
            for k in attrs:
                if NS in k:
                    v = attrs[k]
                    del attrs[k]
                    attrs[k.replace(NS, 'xml:')] = v
            result = {'attrs': attrs}
            # then get the CDATA
            text = element.text
            for e in element.iterchildren():
                if e.tail:
                    text += e.tail
            # check if we need to keep blanks
            if attrs.get('xml:space') != 'preserve':
                if text:
                    text = text.strip()
            if text:
                result['cont'] = text
            # process the children (recursively)
            for child in element.iterchildren(tag=etree.Element):
                if child.tag not in result:
                    result[child.tag] = []
                result[child.tag].append(el2dict(child))
            return result
        element = root.getroot()
        result = {element.tag: [el2dict(element)]}
        self.log.debug('Done')
        return result

#================================================================================
class xparser:
#--------------------------------------------------------------------------------
    def __init__(self, dtdPath=None):
        self.parser = GODSrcParser(dtdPath)
#--------------------------------------------------------------------------------
    def parseSource(self, sourcefile):
        data = self.parser.parseSource(sourcefile)
        return data['gdd'][0]
