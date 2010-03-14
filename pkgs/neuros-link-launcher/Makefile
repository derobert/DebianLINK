PKG_CONFIG_PKGS := ecore ecore-evas ecore-x evas edje
CPPFLAGS := -Wall -Werror $(shell pkg-config --cflags $(PKG_CONFIG_PKGS))
CFLAGS := --std=gnu99 -g
LDFLAGS := $(shell pkg-config --libs $(PKG_CONFIG_PKGS))

all: bin/nl-launcher bin/nl-launcher.edj

clean: 
	rm -Rf bin/ obj/

bin/nl-launcher: obj/nl-launcher.o
	@mkdir -p bin
	$(CC) $(LDFLAGS) -o "$@" "$<"

bin/nl-launcher.edj: edje/nl-launcher.edc obj/arrow.png obj/arrow-flipv.png obj/movie.png obj/tv.png obj/wtf-hydrogen.png obj/xterm.png

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o "$@" "$<"

obj/%.png: img-in/%.svg
	inkscape -C -e "$@" "$<"

obj/%.png: img-in/%.xcf
	xcf2png -o "$@" "$<"

%-flipv.png: %.png
	convert "$<" -flip "$@"

bin/%.edj: edje/%.edc
	edje_cc -id obj "$<" "$@"
