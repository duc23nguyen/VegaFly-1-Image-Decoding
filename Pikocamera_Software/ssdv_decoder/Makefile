# Rishav (2022/05/17)

target = main
build_dir = build

cpp_sources = \

c_sources = \
main.c \
ssdv/rs8.c \
ssdv/ssdv.c

includes = \
-Issdv

libs = 
lib_dir =

c_compiler = gcc
cpp_compiler = g++

linker_flags = $(lib_dir) $(libs) -g
complier_flags = -g -O3 -Wall $(includes)

objects += $(addprefix $(build_dir)/,$(notdir $(c_sources:.c=.o)))
vpath %.c $(sort $(dir $(c_sources)))

objects += $(addprefix $(build_dir)/,$(notdir $(cpp_sources:.cpp=.o)))
vpath %.cpp $(sort $(dir $(cpp_sources)))

all: clean $(build_dir)/$(target)
	$(build_dir)/$(target)

$(build_dir)/$(target): $(objects) Makefile
	$(cpp_compiler) $(objects) $(linker_flags) -o $@

$(build_dir)/%.o: %.c Makefile | $(build_dir)
	$(c_compiler) -c $(complier_flags) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(build_dir)/%.o: %.cpp Makefile | $(build_dir)
	$(cpp_compiler) -c $(complier_flags) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(build_dir):
	if [ ! -d "$(build_dir)" ]; then mkdir -p $(build_dir); fi

clean:
	rm -r build
	mkdir build

make run: all
	$(build_dir)/$(target)
