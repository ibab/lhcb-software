#!/usr/bin/env python
# =============================================================================
# $Id: test_parse_schema.py,v 1.1 2009-09-14 09:49:32 rlambert Exp $
# =============================================================================
"""
*******************************************************************************
*      test the schema parsing.                                               *
*      python  test_parse_schema.py                                           *
*******************************************************************************
"""
# =============================================================================
__author__ = 'Rob Lambert Rob.Lambert@cern.ch'
# =============================================================================

#test I can import it!
from   XMLSummaryBase import schema
from   XMLSummaryBase import summary

topdir='$XMLSUMMARYBASEROOT/'
xsdfile='xml/XMLSummary.xsd'
xmlfile1='xml/Summary_Minimal.xml'
xmlfile2='xml/Summary_Example.xml'


#test I can parse the schema
xsd=schema.Schema(topdir+xsdfile)
print 'schema parsed'

#test the schema can make the default
sum_default=xsd.create_default(xsd.root())
print 'default OK'

#test the schema can parse the examples
sum_minimal=xsd.parse(topdir+xmlfile1)
sum_example=xsd.parse(topdir+xmlfile2)
print 'test example xml (1) OK'

#test that the schema prevents me from doing something stupid
try:
    sum_default.value('this should not work!!')
    raise AttributeError, 'rubbish was NOT prevented by the schema'
except (ValueError, TypeError):
    print 'rubbish value was prevented by the schema'
try:
    sum_default.add('this should not work!!')
    raise AttributeError, 'rubbish child was NOT prevented by the schema'
except (ValueError, TypeError, NameError):
    print 'rubbish child was prevented by the schema'

#test I can make the helper classes
sum=summary.Summary(topdir+xsdfile)
print 'helper class successful'

#test I can parse into the helper classes
sum.parse(topdir+xmlfile1)
sum.parse(topdir+xmlfile2)
print 'test example xml (2) OK'

#test I can merge different files
merge1=summary.Merge([topdir+xmlfile1,topdir+xmlfile2])
merge2=summary.Merge([topdir+xmlfile2,topdir+xmlfile2])
merge2=summary.Merge([sum,sum])
print 'merge OK'
merge2.dump()

