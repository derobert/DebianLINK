PKGS := $(patsubst ./%,%,$(shell find -maxdepth 1 -type d -not -name '.'))

all:
	for pkg in $(PKGS); do pushd "$$pkg" && dpkg-buildpackage -rfakeroot -uc -us || exit 1; popd; done

clean:
	rm -f *.deb *.tar.gz *.changes *.dsc *.upload
