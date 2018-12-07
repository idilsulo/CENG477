
# author: Gorkem Ozer
# more info: https://github.com/arifgorkemozer/3dgeometricshapes/

import sys

if len(sys.argv) < 2:
	print "Usage: python camera_maker.py <generated_camera_file_name>"

else:
	generated_file_name = sys.argv[1]

	if not generated_file_name.endswith(".txt"):
		generated_file_name = generated_file_name + ".txt"

	with open(generated_file_name, 'w') as f:
		f.write("2")
		f.write("\n")
		f.write("#Camera 1")
		f.write("\n")		
		f.write("0 0 100")
		f.write("\n")
		f.write("0 0 -1")
		f.write("\n")
		f.write("0 1 0")
		f.write("\n")
		f.write("-1 1 -1 1")
		f.write("\n")
		f.write("2 1000")
		f.write("\n")
		f.write("1000 1000")
		f.write("\n")
		f.write(generated_file_name + "_camera_1.ppm")
		f.write("\n")
		f.write("#Camera 2")
		f.write("\n")
		f.write("0 100 0")
		f.write("\n")
		f.write("0 -1 0")
		f.write("\n")
		f.write("0 0 -1")
		f.write("\n")
		f.write("-1 1 -1 1")
		f.write("\n")
		f.write("2 1000")
		f.write("\n")
		f.write("1000 1000")
		f.write("\n")
		f.write(generated_file_name + "_camera_2.ppm")
