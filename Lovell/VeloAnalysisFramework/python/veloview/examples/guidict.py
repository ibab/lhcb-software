from collections import OrderedDict

button_label = "label"
histpath_label = "path"
canvas_part = "canvas_nr"

GUI_CONF_DICT = OrderedDict([
            ["Sine", {histpath_label: "sinxdivx", button_label: "Draw Sin(x)/x", canvas_part: 1}],
            ["Cosine", {histpath_label: "cosxdivx", button_label: "Draw Cos(x)/x", canvas_part: 2}]
               ])