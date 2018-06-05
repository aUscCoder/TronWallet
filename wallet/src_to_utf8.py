
import os

def is_utf8(str):
	utf8 = True
	try:
		str.decode('utf-8')
	except UnicodeDecodeError:
		utf8 = False
	return utf8

def conv(file):
	f = open(file, "r")
	c = f.read()
	f.close()
	
	if not is_utf8(c):
		print "converting %s" % file
		u = c.decode("gbk").encode('utf8')
		open(file, "w").write(u)
	else:
		print "%s is utf-8" % file

if __name__ == "__main__":
	
	s = []
	for dir_name, subdirlist, filelist in os.walk(os.path.join(os.getcwd(), "")):
		for file in filelist:
			if file.endswith(".cpp") or file.endswith(".hpp") or file.endswith(".h") or file.endswith(".cc"):
				try:
					conv(os.path.join(dir_name, file))
				except UnicodeDecodeError:
					s.append(os.path.join(dir_name, file))
	print s
			