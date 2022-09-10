MajVer:=0
MinVer:=0
PatVer:=1

all:
	MajVer=$(MajVer) MinVer=$(MinVer) PatVer=$(PatVer) scons -j 4

release:
	MajVer=$(MajVer) MinVer=$(MinVer) PatVer=$(PatVer) scons -j 4 mode=release

gen-assets:
	python3 tools/create_assets.py

clean:
	scons -c
