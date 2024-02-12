*******************************************************************************
* GENERAL INFORMATION
*******************************************************************************

Contact
-------
mail : yannick.faula at insa-lyon.fr
tel : 06 46 24 90 11

General description of the software
--------------------------------
App for detecting lines on images.

*******************************************************************************
* HELP WITH PARAMETERS *
*******************************************************************************

ANGLE_SEARCH: Search angle for points detected around a given point. 
                          The smaller the angle, the straighter the line detected.
                          The angle is in degrees.

BETA: Not necessary for historical maps because the process is carried out on black and white images

BLACK_ON_WHITE?         Boolean to determine whether processing is done normally (value 1) for
                         detection of a black line on a white background,
                         or in reverse video (value <=0 ) for the detection of a white line on a black background.

DIFFERENCE_LEVEL_SIDES: Not necessary for historical maps

MIN_SIZE_GRAPH: APPROXIMATE minimum size of an end of a line (a graph) to be displayed.
                         The size is in pixels.

MIN_SIZE_GRAPH: APPROXIMATE minimum size of a line to be displayed.
                         The size is the number of pixels detected in a graph.
                         Graphs smaller than this will not be taken into account in the merge.
                         If you are looking for very fine lines that are difficult to detect,
                         it is better to keep the size small, between 5-20 pixels.
                         
NB_FRIENDS_MAX: Number of friends (detected points) taken into account in the link between pixels. 
                         More points means better detection but slows down the process.
                         This is not an essential parameter for historical maps.
                         Between 5 and 12 friends max is acceptable.

THRESHOLD_LINE_CRITER: A threshold parameter to eliminate false detections. If you don't want to take it into account
                         set the value very high (at least 100).
                         Don't touch it too much.
                         Otherwise, test values of around 10, 20 or 30, depending on the type of wall in particular.


THRESHOLD_REL_EIGEN: A threshold parameter to eliminate false detections. This parameter takes floats between 0 and 1.
                         It determines the dispersion of a graph representing a line. If the points are widely scattered
                         this graph will have a value between 0 and 0.5. If a graph is a straight line, it will be very close to 1.
                         This gives a threshold for eliminating certain graphs that may be false detections.

X_FACTOR_X2ORIGIN: Floating value between 0.1 and 1. Used with the FACTOR_INTERVAL parameter.
                          Starting resolution factor for detection. If this parameter is set to 0.75,
                          processing will be at 75 percent of the original resolution 
                          then processing at all FACTOR_INTERVAL levels up to the original resolution.
                          !! REM: No more than 3 levels of resolution for processing historical maps !!

FACTOR_INTERVAL: float indicating the range of the resolution factor to be taken into account with X_FACTOR_X2ORIGIN.

ID_PALIER: 0, 1, 2 or 3. Four levels representing the perimeter and the minimum number of searches 
                          of pixels detected around a given point.
                          Not very useful for maps.



For line detection on historical maps, you can use the following settings:

ANGLE_SEARCH = 22.5
BETA = 8
BLACK_ON_WHITE? = 0
FACTOR_INTERVAL = 0.25
ID_PALIER = 0
MIN_SIZE_GRAPH = 30
NB_FRIENDS_MAX = 10
THRESHOLD_LINE_CRITER = 100
THRESHOLD_REL_EIGEN = 0.6
X_FACTOR_X2ORIGIN = 0.75