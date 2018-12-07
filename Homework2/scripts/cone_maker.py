
# author: Gorkem Ozer
# more info: https://github.com/arifgorkemozer/3dgeometricshapes/

import math
import sys

if len(sys.argv) < 8:
	print "Usage: python cone_maker.py <cone_bottom_center_x>\n\t\t\t<cone_bottom_center_y>\n\t\t\t<cone_bottom_z_coord>\n\t\t\t<cone_top_distance>\n\t\t\t<cone_bottom_radius>\n\t\t\t<step_threshold>\n\t\t\t<last_vertex_id_in_3d_space>\n\t\t\t<color_primary: \"R|G|B\">\n\t\t\t<color_secondary: \"R|G|B\">\n\t\t\t<color_bottom_center: \"R|G|B\">\n\t\t\t<color_top: \"R|G|B\">"

else:
	
	center_x = (float)(sys.argv[1])
	center_y = (float)(sys.argv[2])
	cone_bottom_z_coord = (float)(sys.argv[3])
	cone_top_distance = (float)(sys.argv[4])

	radius = (float)(sys.argv[5])
	step_threshold = (float)(sys.argv[6])
	last_vertex_id = (int)(sys.argv[7])

	color_primary = None
	color_secondary = None
	color_cone_bottom_center = None
	color_cone_top = None

	if len(sys.argv) == 12:
		color_primary_values = sys.argv[8].split("|")
		color_secondary_values = sys.argv[9].split("|")
		color_bottom_values = sys.argv[10].split("|")
		color_top_values = sys.argv[11].split("|")

		cpv = [int(n) for n in color_primary_values]
		csv = [int(n) for n in color_secondary_values]
		cbv = [int(n) for n in color_bottom_values]
		ctv = [int(n) for n in color_top_values]

		color_primary = tuple(cpv)
		color_secondary = tuple(csv)
		color_cone_bottom_center = tuple(cbv)
		color_cone_top = tuple(ctv)

	else:
		
		color_primary = (255, 0, 0)
		color_secondary = (0, 0, 255)
		color_cone_bottom_center = (0, 255, 0)
		color_cone_top = (0, 255, 255)
	

	x = -radius

	points = []

	while x <= radius:
		points.append( (x, math.sqrt(radius**2 - x**2), cone_bottom_z_coord) )
		x += step_threshold

	points_inv = points[::-1]

	for elem in points_inv[1:]:

		points.append( (elem[0], -elem[1], elem[2]) )



	# add bottom center
	points.append( (center_x, center_y, cone_bottom_z_coord) )

	# add cone top
	points.append( (center_x, center_y, cone_bottom_z_coord - cone_top_distance) )


	triangles = []


	front_last_vertex_id = len(points)-2
	front_center_id = len(points)-1
	back_center_id = len(points)

	# cone bottom triangles (to cone bottom center)
	for i in range(1, front_last_vertex_id):
		triangles.append( (front_center_id +last_vertex_id, i+1 +last_vertex_id, i +last_vertex_id) )


	# cone side triangles (to cone top)
	for i in range(1, front_last_vertex_id):
		triangles.append( (back_center_id +last_vertex_id, i +last_vertex_id, i+1 +last_vertex_id ) )


	
	colors = []
	for i in range(1, front_last_vertex_id+1):
		if i % 2 == 1:
			colors.append( color_primary )
		else:
			colors.append( color_secondary )

	colors.append(color_cone_bottom_center)
	colors.append(color_cone_top)


	print "Colors:"
	for c in colors:
		print c[0], c[1], c[2]

	print "Positions:", len(points)
	for elem in points:
		print elem[0], elem[1], elem[2]

	print "Triangles:", len(triangles)
	for tri in triangles:
		print tri[0], tri[1], tri[2]

	print len(points), "points created"
	print len(triangles), "triangles created"

	# write to a 3d scene file
	with open("cone_scene.txt", 'w') as f:
		
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
		
		for c in colors:
			f.write( str(c[0]) + " " + str(c[1]) + " " + str(c[2]))
			f.write("\n")	


		f.write("#Positions")
		f.write("\n")

		for elem in points:
			f.write( str(elem[0]) + " " + str(elem[1]) + " " + str(elem[2]))
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
		f.write(str(len(triangles)))
		f.write("\n")

		for tri in triangles:
			f.write( str(tri[0]) + " " + str(tri[1]) + " " + str(tri[2]))
			f.write("\n")



