tsubomi: tsubomi_mkary tsubomi_sass
	
tsubomi_mkary: tsubomi.h tsubomi.cc tsubomi_mkary.cc
	g++ tsubomi.cc tsubomi_mkary.cc -o tsubomi_mkary

tsubomi_sass: tsubomi.h tsubomi.cc tsubomi_sass.cc
	g++ tsubomi.cc tsubomi_sass.cc -o tsubomi_sass

clean:
	rm tsubomi_mkary
	rm tsubomi_sass
