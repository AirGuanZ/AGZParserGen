CC = clang++
CC_INCLUDE_FLAGS = -I ./Include/
CC_FLAGS = -std=c++11 -O2 $(CC_INCLUDE_FLAGS) -Werror -Wall

CPP_SRC_FILES = $(shell find ./Sample/ -name "*.cpp")
CPP_OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_SRC_FILES))
CPP_DPT_FILES = $(patsubst %.cpp, %.d, $(CPP_SRC_FILES))

DST = ./Build/ParserGen

$(DST): $(CPP_OBJ_FILES)
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) $(CC_FLAGS) -c $< -o $@

%.d: %.cpp
	@set -e; \
	rm -f $@; \
	$(CC) -MM $(CC_FLAGS) $< $(CC_INCLUDE_FLAGS) > $@.$$$$.dtmp; \
	sed 's,\(.*\)\.o\:,$*\.o $*\.d\:,g' < $@.$$$$.dtmp > $@; \
	rm -f $@.$$$$.dtmp

-include $(CPP_DPT_FILES)

clean:
	rm -f $(DST)
	rm -f $(CPP_OBJ_FILES)
	rm -f $(CPP_DPT_FILES)
	rm -f $(shell find ./Sample/ -name "*.dtmp")

run:
	make
	$(DST)
