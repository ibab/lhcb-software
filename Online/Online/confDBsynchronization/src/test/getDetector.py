from database.utilities import getDetector

test_data = ["DUMHUGIN01", "DUMTELL05", "ECHUGIN01", "ECTELL01", "HCHUGIN01",
             "HCTELL01", "ITHUGIN01", "ITTELL01", "MUHUGINA01", "MUHUGINC01",
             "MUTELLA01", "MUTELLC07", "OTHUGINA01", "OTHUGINC03", "OTTELLA01",
             "OTTELLC23", "PSHUGIN01", "PSTELL01", "PUHUGIN01", "R1HUGIN01",
             "R1UKL101", "R2HUGIN01", "R2UKL101", "TCATELL01", "TFCHUGIN01",
             "TMUTELLQ01", "TTHUGIN04", "TTTELL48", "VEHUGINA01", "VETELLA01",
             "VETELLC42"]
i= 1
for data in test_data:
    print str(i)+" ("+str(data)+"):"+getDetector(data)
    i += 1
