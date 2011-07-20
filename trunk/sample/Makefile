CC  = /usr/bin/g++
BIN = /usr/local/bin/
LIB = /usr/local/lib/
INC = /usr/local/include/
UNAME = $(shell uname)

futaba: erika_match
erika_match: futaba_match.cc
	$(CC) -O2 -Wall -g futaba_match.cc -ltsubomi -o futaba_match

clean:
	- rm -f futaba_match

