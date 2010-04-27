SRC_I = tsubomi_indexer.cc tsubomi_mkary.cc tsubomi_sass.cc
SRC_S = tsubomi_searcher.cc tsubomi_mkary.cc tsubomi_sass.cc
HDR_I = tsubomi_indexer.h tsubomi_mmap.h tsubomi_defs.h tsubomi.h
HDR_S = tsubomi_searcher.h tsubomi_mmap.h tsubomi_defs.h tsubomi.h

tsubomi: tsubomi_mkary tsubomi_sass
	
tsubomi_mkary: $(SRC_I) $(HDR_I)
	g++ -O2 tsubomi_indexer.cc tsubomi_mkary.cc -o tsubomi_mkary

tsubomi_sass: $(SRC_S) $(HDR_S)
	g++ -O2 tsubomi_searcher.cc tsubomi_sass.cc -o tsubomi_sass

clean:
	rm tsubomi_mkary
	rm tsubomi_sass
