import re
from re import compile, match

vehugin_pattern = compile(r'vehugin0[2-9]{1}', re.IGNORECASE)
vehugins = ["vehugina00", "vehugina01", "vehugina02", "vehugina03", "vehugina04", "vehugina05"]


vetella_pattern1 = compile(r'vetella((1[0-6]){1}|(0[0-9]){1})', re.IGNORECASE)
vetells = ["vetella01", "vetella09", "vetella10", "vetella16", "vetella17"]
for  vetell in vetells:
    if vetella_pattern1.match(vetell):
        print vetell+" found"
    else:
        print vetell+" did not match"