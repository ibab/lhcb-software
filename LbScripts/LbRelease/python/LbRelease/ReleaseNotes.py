'''
Created on 9.12.2011
Module to manipulate release notes
@author: hmd
'''

import re



def extract(file_path, version):
    """
    function to extract release notes of a given version from a project or a package note file
    @param file_path: release notes file path
    @type file_path: string
    @param version: version to extract
    @type version: string
    """
    release_notes = open(file_path).read()
    note = ""
#    _html_style = r'(?:.*<H1>.*?%s.*?\n(.+?)</PRE>)' % version
    _html_style = r'^(?:\s*?</PRE>)?<H1>(?:\s*?<A\s+.+?>)?.+?%s\s*?(?:</A>\s*?)?</H1>(?:\s*?<PRE>)?$(.+?)(?=^(?:\s*?</PRE>)?(<H1>(?:\s*?<A\s+.+?>)?.+?(?:</A>\s*?)?</H1>(?:\s*?<PRE>)?$))' % version
    ch_style = re.compile(_html_style,flags=re.DOTALL|re.MULTILINE|re.IGNORECASE)

    try :
        m = ch_style.search(release_notes)
        if m :
            note = m.group(1)
    except :
        pass

    if not note:
        _txt_style = r"^\s*?!=+?.+?%s.+?=+?$(.+?)(?=^\s*?!=+?)" % version
        ct_style = re.compile(_txt_style,flags=re.DOTALL|re.MULTILINE|re.IGNORECASE)
        m = ct_style.search(release_notes)
        if m :
            note = m.group(1)


    return note