# just a small svg plotter
# to make nice test results
#
# Author: Stephan Nies 


from string import Template

xoffset = 100
yoffset = 100  
xwidth  = 600
yheight = 400


svg_template = Template( """<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">

<svg width="100%" height="100%" version="1.1"
xmlns="http://www.w3.org/2000/svg">

<polyline points="$xmin,$ymin $xmin,$ymax"
style="fill:white;stroke:black;stroke-width:2"/>

<polyline points="$xmin,$ymax $xmax,$ymax"
style="fill:white;stroke:black;stroke-width:2"/>

<text id="TextElement" x="$xlabel_x" y="$xlabel_y" style="font-family:Verdana;font-size:24"> $xlabel </text>
<text id="TextElement" x="$xmin" y="$ylabel_y" style="font-family:Verdana;font-size:24"> $ylabel </text>
<text id="TextElement" x="$xmin" y="$xlabel_y" style="font-family:Verdana;font-size:24"> $xmin_label </text>
<text id="TextElement" x="$xmax" y="$xlabel_y" style="font-family:Verdana;font-size:24"> $xmax_label </text>
<text id="TextElement" x="$ylabel_x" y="$ymax" style="font-family:Verdana;font-size:24"> $ymin_label </text>
<text id="TextElement" x="$ylabel_x" y="$ymin" style="font-family:Verdana;font-size:24"> $ymax_label </text>


<polyline points="$points"
style="fill:white;stroke:red;stroke-width:2"/>

</svg>
""")


def render(data, xlabel, ylabel):
  xmin = data[0][0] 
  xmax = data[0][0]
  ymin = data[0][1]
  ymax = data[0][1]
  for (x,y) in data:
    xmin = min(xmin, x)
    xmax = max(xmax, x)
    ymin = min(ymin, y)
    ymax = max(ymax, y)
  xrange = xmax - xmin
  yrange = ymax - ymin
  xscale = float(xwidth)  / xrange
  yscale = float(yheight) / yrange
  points_string = ""
  for (x,y) in data:
    xdiff = x-xmin
    ydiff = y-ymin  
    x_scaled = xscale * xdiff
    y_scaled = yscale * ydiff
    x_svg = int(x_scaled) + xoffset
    y_svg = int(yheight - y_scaled) + yoffset 
    points_string = points_string + ' ' + str(x_svg) + ',' +str(y_svg)
  tokens = {'points': points_string, 
            'xmin':   str(xoffset), 
            'xmax':   str(xoffset + xwidth),
            'ymin':   str(yoffset),
            'ymax':   str(yoffset + yheight),
            'xmin_label': str(xmin),
            'xmax_label': str(xmax),
            'ymin_label': str(ymin),
            'ymax_label': str(ymax),
            'xlabel_x': str(xoffset + int(xwidth * 0.5)),
            'xlabel_y': str(yoffset + yheight + 20),
            'ylabel_x': str(20),
            'ylabel_y': str(int(xoffset * 0.5)),
            'xlabel' : xlabel,
            'ylabel' : ylabel 
     }
  return svg_template.substitute(tokens)

if __name__ == '__main__':
  testdata = [(-100,10), (-20,50), (40,40), (120,30)]
  print render(testdata,'test A',' 123 TEST TEWS') 
