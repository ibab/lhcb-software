#!/usr/bin/env python

import os

from collections import defaultdict
from xml.dom import minidom
from os.path import join, basename, splitext, exists
from optparse import OptionParser

def main():
    parser = OptionParser(usage='%prog <cmake file> <xml files...>')
    parser.add_option('-s', '--src-output', action='store', metavar="DIR",
                      help='define possible output destination of source code. '
                      'Use the special value "env" to use the content of the environment '
                      'variable GODDOTHOUT. [default: %default]')

    parser.set_defaults(src_output=os.curdir)

    opts, args = parser.parse_args()
    if len(args) < 2:
        parser.error('wrong number of arguments')

    cmake_info = args.pop(0)
    xmlfiles = args

    products = defaultdict(set)

    for xmlfile in xmlfiles:
        key = splitext(basename(xmlfile))[0]
        dom = minidom.parse(xmlfile)
        products[key].update(join(opts.src_output, elem.getAttribute('name') + '.h')
                             for elem in dom.getElementsByTagName('class') +
                                         dom.getElementsByTagName('namespace'))

    old_data = open(cmake_info).read() if exists(cmake_info) else ''
    data = ''
    for key in sorted(products):
        files = sorted(products[key])
        data += ('set({0}_generated_files\n'
                 '    {1})\n').format(key, '\n    '.join(files))
    if data != old_data:
        open(cmake_info, 'w').write(data)


if __name__ == '__main__':
    main()
