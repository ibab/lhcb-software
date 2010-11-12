"""Test suite for testing line builders
"""

__all__ = ('test_make_many_instances',
           'test_default_raises',
           'test_single_constructor_argument_raises',
           'test_lines',
           'test_line_locations',
           'test_line',
           'test_line_location')

from py.test import raises
from StrippingConf.Configuration import StrippingLine

def test_make_many_instances(builderType, conf_dict) :
    baseName = 'StrippingLineBuilderTest'
    for n in '0123456789' :
        builderType(baseName + n, conf_dict)

def test_default_raises(builderType) :
    raises(Exception, builderType)

def test_single_constructor_argument_raises(builderType, conf_dict) :
     raises(Exception, builderType, conf_dict)
     raises(Exception, builderType, 'SomeCrazyName')

def test_line(line) :
    assert type(line) == StrippingLine

def test_line_location(line) :
    assert 'Stripping'+line.outputLocation().split('/')[-1] == line.name()

def test_lines(builder) :
    lines = builder.lines()
    for line in lines :
        test_line(line)

def test_line_locations(builder) :
    lines = builder.lines()
    for line in lines :

        test_line_location(line)

