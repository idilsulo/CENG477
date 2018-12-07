

# author: Gorkem Ozer
# more info: https://github.com/arifgorkemozer/3dgeometricshapes/

import math
import sys

if len(sys.argv) < 8:
	print "Usage: python star_maker.py <star_center_x>\n\t\t\t<star_center_y>\n\t\t\t<star_z_coord>\n\t\t\t<star_back_distance>\n\t\t\t<start_x>\n\t\t\t<start_y>\n\t\t\t<last_vertex_id_in_3d_space>"

else:
	
	sides = 5
	angle = 360.0 / sides

	center_x = (float)(sys.argv[1])
	center_y = (float)(sys.argv[2])
	common_coord = (float)(sys.argv[3])
	star_back_distance = (float)(sys.argv[4])
	x_first = (float)(sys.argv[5])
	y_first = (float)(sys.argv[6])
	last_vertex_id = (int)(sys.argv[7])

	points = [ (x_first, y_first, common_coord) ]

	for i in range(1, sides):
		new_angle = (i*angle/360.0) * 2 * math.pi
		x_new = x_first * math.cos(new_angle) - y_first * math.sin(new_angle)
		y_new = y_first * math.cos(new_angle) + x_first * math.sin(new_angle)
		points.append( (x_new, y_new, common_coord) )

	# add center at last

	fc = 0
	sc = 1

	x1 = points[0][fc]
	y1 = points[0][sc]

	x2 = points[1][fc]
	y2 = points[1][sc]

	x3 = points[2][fc]
	y3 = points[2][sc]

	x4 = points[3][fc]
	y4 = points[3][sc]

	x5 = points[4][fc]
	y5 = points[4][sc]


	points.append( (((x1+x2)/2 + center_x)/2, ((y1+y2)/2 + center_y)/2 , common_coord ) )
	points.append( (((x2+x3)/2 + center_x)/2, ((y2+y3)/2 + center_y)/2 , common_coord ) )
	points.append( (((x3+x4)/2 + center_x)/2, ((y3+y4)/2 + center_y)/2 , common_coord ) )
	points.append( (((x4+x5)/2 + center_x)/2, ((y4+y5)/2 + center_y)/2 , common_coord ) )
	points.append( (((x5+x1)/2 + center_x)/2, ((y5+y1)/2 + center_y)/2 , common_coord ) )

	# front center
	points.append( (center_x, center_y, common_coord) )

	# back center
	points.append( (center_x, center_y, common_coord - star_back_distance) )

	print "num of points: ", len(points)
	print "star coordinates are:"

	for elem in points:

		if elem[0] < 0.00001 and elem[0] > -0.00001:
			a = 0
		else:
			a = elem[0]

		if elem[1] < 0.00001 and elem[1] > -0.00001:
			b = 0
		else:
			b = elem[1]

		print a, b, elem[2]


	print "triangles are:"

	# front triangles
	print 1 +last_vertex_id, 6 +last_vertex_id, 10 +last_vertex_id
	print 2 +last_vertex_id, 7 +last_vertex_id, 6 +last_vertex_id
	print 3 +last_vertex_id, 8 +last_vertex_id, 7 +last_vertex_id
	print 4 +last_vertex_id, 9 +last_vertex_id, 8 +last_vertex_id
	print 5 +last_vertex_id, 10 +last_vertex_id, 9 +last_vertex_id

	print 11 +last_vertex_id, 10 +last_vertex_id, 6 +last_vertex_id
	print 11 +last_vertex_id, 9 +last_vertex_id, 10 +last_vertex_id
	print 11 +last_vertex_id, 8 +last_vertex_id, 9 +last_vertex_id
	print 11 +last_vertex_id, 7 +last_vertex_id, 8 +last_vertex_id
	print 11 +last_vertex_id, 6 +last_vertex_id, 7 +last_vertex_id

	# back triangles

	print 1 +last_vertex_id, 10 +last_vertex_id, 12 +last_vertex_id
	print 1 +last_vertex_id, 12 +last_vertex_id, 6 +last_vertex_id
	print 5 +last_vertex_id, 12 +last_vertex_id, 10 +last_vertex_id
	print 5 +last_vertex_id, 9 +last_vertex_id, 12 +last_vertex_id
	print 4 +last_vertex_id, 12 +last_vertex_id, 9 +last_vertex_id
	print 4 +last_vertex_id, 8 +last_vertex_id, 12 +last_vertex_id
	print 3 +last_vertex_id, 12 +last_vertex_id, 8 +last_vertex_id
	print 3 +last_vertex_id, 7 +last_vertex_id, 12 +last_vertex_id
	print 2 +last_vertex_id, 12 +last_vertex_id, 7 +last_vertex_id
	print 2 +last_vertex_id, 6 +last_vertex_id, 12 +last_vertex_id
	print 1 +last_vertex_id, 12 +last_vertex_id, 6 +last_vertex_id
	

	print "colors are:"

	print "255 255 255"
	print "255 255 255"
	print "255 255 255"
	print "255 255 255"
	print "255 255 255"

	print "200 200 200"
	print "200 200 200"
	print "200 200 200"
	print "200 200 200"
	print "200 200 200"

	print "255 255 255"
	print "20 20 20"

	# write to a 3d scene file
	with open("star_scene.txt", 'w') as f:
		
		f.write("100 100 100")
		f.write("\n")
		f.write("1")
		f.write("\n")
		f.write("#Vertices")
		f.write("\n")
		f.write(str(len(points)))
		f.write("\n")
		f.write("#Colors")
		f.write("\n")
		f.write( "255 255 255" )
		f.write("\n")
		f.write( "255 255 255" )
		f.write("\n")
		f.write( "255 255 255" )
		f.write("\n")
		f.write( "255 255 255" )
		f.write("\n")
		f.write( "255 255 255" )
		f.write("\n")
		f.write( "200 200 200" )
		f.write("\n")
		f.write( "200 200 200" )
		f.write("\n")
		f.write( "200 200 200" )
		f.write("\n")
		f.write( "200 200 200" )
		f.write("\n")
		f.write( "200 200 200" )
		f.write("\n")
		f.write( "255 255 255" )
		f.write("\n")
		f.write( "20 20 20" )
		f.write("\n")
		f.write("#Positions")
		f.write("\n")

		for elem in points:

			if elem[0] < 0.00001 and elem[0] > -0.00001:
				a = 0
			else:
				a = elem[0]

			if elem[1] < 0.00001 and elem[1] > -0.00001:
				b = 0
			else:
				b = elem[1]

			f.write( str(a) + " " + str(b) + " " + str(elem[2]) )
			f.write("\n")

		f.write("#Translations")
		f.write("\n")
		f.write("0")
		f.write("\n")
		f.write("#Scalings")
		f.write("\n")
		f.write("0")
		f.write("\n")
		f.write("#Rotations")
		f.write("\n")
		f.write("0")
		f.write("\n")
		f.write("#Models")
		f.write("\n")
		f.write("1")
		f.write("\n")
		f.write("1")
		f.write("\n")
		f.write("1")
		f.write("\n")	
		f.write("0")
		f.write("\n")
		f.write("20")
		f.write("\n")
		
		# front triangles
		f.write( str(1 +last_vertex_id) + " " + str( 6 +last_vertex_id) + " " + str( 10 +last_vertex_id) )
		f.write("\n")
		f.write( str(2 +last_vertex_id) + " " + str( 7 +last_vertex_id) + " " + str( 6 +last_vertex_id) )
		f.write("\n")
		f.write( str(3 +last_vertex_id) + " " + str( 8 +last_vertex_id) + " " + str( 7 +last_vertex_id) )
		f.write("\n")
		f.write( str(4 +last_vertex_id) + " " + str( 9 +last_vertex_id) + " " + str( 8 +last_vertex_id) )
		f.write("\n")
		f.write( str(5 +last_vertex_id) + " " + str( 10 +last_vertex_id) + " " + str( 9 +last_vertex_id) )
		f.write("\n")

		f.write( str(11 +last_vertex_id) + " " + str( 10 +last_vertex_id) + " " + str( 6 +last_vertex_id) )
		f.write("\n")
		f.write( str(11 +last_vertex_id) + " " + str( 9 +last_vertex_id) + " " + str( 10 +last_vertex_id) )
		f.write("\n")
		f.write( str(11 +last_vertex_id) + " " + str( 8 +last_vertex_id) + " " + str( 9 +last_vertex_id) )
		f.write("\n")
		f.write( str(11 +last_vertex_id) + " " + str( 7 +last_vertex_id) + " " + str( 8 +last_vertex_id) )
		f.write("\n")
		f.write( str(11 +last_vertex_id) + " " + str( 6 +last_vertex_id) + " " + str( 7 +last_vertex_id) )
		f.write("\n")

		# back triangles

		f.write( str(1 +last_vertex_id) + " " + str( 10 +last_vertex_id) + " " + str( 12 +last_vertex_id) )
		f.write("\n")
		f.write( str(1 +last_vertex_id) + " " + str( 12 +last_vertex_id) + " " + str( 6 +last_vertex_id) )
		f.write("\n")
		f.write( str(5 +last_vertex_id) + " " + str( 12 +last_vertex_id) + " " + str( 10 +last_vertex_id) )
		f.write("\n")
		f.write( str(5 +last_vertex_id) + " " + str( 9 +last_vertex_id) + " " + str( 12 +last_vertex_id) )
		f.write("\n")
		f.write( str(4 +last_vertex_id) + " " + str( 12 +last_vertex_id) + " " + str( 9 +last_vertex_id) )
		f.write("\n")
		f.write( str(4 +last_vertex_id) + " " + str( 8 +last_vertex_id) + " " + str( 12 +last_vertex_id) )
		f.write("\n")
		f.write( str(3 +last_vertex_id) + " " + str( 12 +last_vertex_id) + " " + str( 8 +last_vertex_id) )
		f.write("\n")
		f.write( str(3 +last_vertex_id) + " " + str( 7 +last_vertex_id) + " " + str( 12 +last_vertex_id) )
		f.write("\n")
		f.write( str(2 +last_vertex_id) + " " + str( 12 +last_vertex_id) + " " + str( 7 +last_vertex_id) )
		f.write("\n")
		f.write( str(2 +last_vertex_id) + " " + str( 6 +last_vertex_id) + " " + str( 12 +last_vertex_id) )
		f.write("\n")
		f.write( str(1 +last_vertex_id) + " " + str( 12 +last_vertex_id) + " " + str( 6 +last_vertex_id) )







