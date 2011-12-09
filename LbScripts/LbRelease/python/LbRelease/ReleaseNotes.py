'''
Created on 9 déc. 2011
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
    _html_style = r'(?:.*<H1>.*?%s.*?\n(.+?)</PRE>)' % version
    ch_style = re.compile(_html_style,flags=re.DOTALL)

    try :
        m = ch_style.match(release_notes)
        if m :
            note = m.group(0)
    except :
        pass


    return note