PKG_CONFIG_PKGS := ecore ecore-evas ecore-x evas
CPPFLAGS := -Wall -Werror $(shell pkg-config --cflags $(PKG_CONFIG_PKGS))
CFLAGS := --std=gnu99 -g
LDFLAGS := $(shell pkg-config --libs $(PKG_CONFIG_PKGS))

all: bin/nl-launcher bin/nl-launcher.eet

clean: 
	rm -Rf bin/ obj/

bin/nl-launcher: obj/main.o
	@mkdir -p bin
	$(CC) $(LDFLAGS) -o "$@" "$<"

bin/nl-launcher.eet: obj/arrow.png obj/movie.png obj/tv.png
	eet -i "$@" arrow obj/arrow.png 0
	eet -i "$@" movie obj/movie.png 0
	eet -i "$@" tv obj/tv.png 0

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o "$@" "$<"

obj/%.png: img-in/%.svg
	inkscape -C -e "$@" "$<"
