# =============================================================================
"""
@namespace addDict
@brief Low level module. Contains functions to add elements in counters used in HltPython modules. Used in frequencyAndBpercentage.py, hadHLTpies.py
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@date 2008-05-02
"""
# =============================================================================


def add_elto_dic(dict,entry):
    """add element to a counter. If dictionary has the element increases one, if not goes to one.
    """
    if dict.has_key(entry):
        dict[entry]+=1

    else: dict[entry]=1 

#---------------------------------------------------

def add_dicto_dic(dict1,dict2):
    """add the elements of two counters
    """
    for key2 in dict2:
        if dict1.has_key(key2):
            dict1[key2]+=dict2[key2]
        else: dict1[key2]=dict2[key2]
