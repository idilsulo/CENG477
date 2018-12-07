
# author: Gorkem Ozer
# more info: https://github.com/arifgorkemozer/3dgeometricshapes/


import sys

if len(sys.argv) < 8:
	print "Usage: python box_maker.py <start_x>\n\t\t\t<start_y>\n\t\t\t<start_z>\n\t\t\t<length>\n\t\t\t<width>\n\t\t\t<height>\n\t\t\t<last_vertex_id_3d_space>"

else:
	
	x1 = (float)(sys.argv[1])
	y1 = (float)(sys.argv[2])
	z1 = (float)(sys.argv[3])

	l = (int) (sys.argv[4])
	w = (int) (sys.argv[5])
	h = (int) (sys.argv[6])
	last_vertex_id = (int) (sys.argv[7])


	first = (x1, y1, z1)
	points = [first]

	#2nd
	points.append( (first[0] , first[1] - h , first[2]) )

	#3rd
	points.append( (first[0]+l , first[1] - h , first[2]) )

	#4th
	points.append( (first[0]+l , first[1] , first[2]) )

	#5th
	points.append( (first[0]+l , first[1] , first[2]-w) )

	#6th
	points.append( (first[0]+l , first[1] - h , first[2]-w) )

	#7th
	points.append( (first[0] , first[1] , first[2]-w) )

	#8th
	points.append( (first[0] , first[1]-h , first[2]-w) )

	print "------"
	print "colors are:"
	print "255 0 0"
	print "255 0 0"
	print "0 255 0"
	print "0 255 0"
	print "0 0 255"
	print "0 0 255"
	print "0 255 255"
	print "0 255 255"

	print "------"
	print "8 vertices"
	print "coordinates are:" 

	for elem in points:
		print elem[0], elem[1], elem[2]

	print "------"
	print "12 triangles"
	print "triangles are:"

	print 1+last_vertex_id, 2 + last_vertex_id, 3 + last_vertex_id
	print 1+last_vertex_id, 3 + last_vertex_id, 4 + last_vertex_id
	
	print 8+last_vertex_id, 3 + last_vertex_id, 2 + last_vertex_id
	print 6+last_vertex_id, 3 + last_vertex_id, 8 + last_vertex_id

	print 4+last_vertex_id, 3 + last_vertex_id, 6 + last_vertex_id
	print 4+last_vertex_id, 6 + last_vertex_id, 5 + last_vertex_id

	print 5+last_vertex_id, 6 + last_vertex_id, 8 + last_vertex_id
	print 5+last_vertex_id, 8 + last_vertex_id, 7 + last_vertex_id

	print 7+last_vertex_id, 1 + last_vertex_id, 4 + last_vertex_id
	print 7+last_vertex_id, 4 + last_vertex_id, 5 + last_vertex_id
	
	print 7+last_vertex_id, 2 + last_vertex_id, 1 + last_vertex_id
	print 7+last_vertex_id, 8 + last_vertex_id, 2 + last_vertex_id


	# write to a 3d scene file
	with open("box_scene.txt", 'w') as f:
		
		f.write("100 100 100")
		f.write("\n")
		f.write("1")
		f.write("\n")
		f.write("#Vertices")
		f.write("\n")
		f.write("8")
		f.write("\n")
		f.write("#Colors")
		f.write("\n")
		f.write( "255 0 0")
		f.write("\n")
		f.write( "255 0 0")
		f.write("\n")
		f.write( "0 255 0")
		f.write("\n")
		f.write( "0 255 0")
		f.write("\n")
		f.write( "0 0 255")
		f.write("\n")
		f.write( "0 0 255")
		f.write("\n")
		f.write( "0 255 255")
		f.write("\n")
		f.write( "0 255 255")
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
		f.write("12")
		f.write("\n")
		f.write( str(1+last_vertex_id) + " " + str(2 + last_vertex_id) + " " + str(3 + last_vertex_id) )
		f.write("\n")
		f.write( str(1+last_vertex_id) + " " + str(3 + last_vertex_id) + " " + str(4 + last_vertex_id) )
		f.write("\n")
		f.write( str(8+last_vertex_id) + " " + str(3 + last_vertex_id) + " " + str(2 + last_vertex_id) )
		f.write("\n")
		f.write( str(6+last_vertex_id) + " " + str(3 + last_vertex_id) + " " + str(8 + last_vertex_id) )
		f.write("\n")
		f.write( str(4+last_vertex_id) + " " + str(3 + last_vertex_id) + " " + str(6 + last_vertex_id) )
		f.write("\n")
		f.write( str(4+last_vertex_id) + " " + str(6 + last_vertex_id) + " " + str(5 + last_vertex_id) )
		f.write("\n")
		f.write( str(5+last_vertex_id) + " " + str(6 + last_vertex_id) + " " + str(8 + last_vertex_id) )
		f.write("\n")
		f.write( str(5+last_vertex_id) + " " + str(8 + last_vertex_id) + " " + str(7 + last_vertex_id) )
		f.write("\n")
		f.write( str(7+last_vertex_id) + " " + str(1 + last_vertex_id) + " " + str(4 + last_vertex_id) )
		f.write("\n")
		f.write( str(7+last_vertex_id) + " " + str(4 + last_vertex_id) + " " + str(5 + last_vertex_id) )
		f.write("\n")
		f.write( str(7+last_vertex_id) + " " + str(2 + last_vertex_id) + " " + str(1 + last_vertex_id) )
		f.write("\n")
		f.write( str(7+last_vertex_id) + " " + str(8 + last_vertex_id) + " " + str(2 + last_vertex_id) )
