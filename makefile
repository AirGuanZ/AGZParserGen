CC = clang++
CC_INCLUDE_FLAGS = -I ./Include/
CC_FLAGS = -std=c++14 -O2 $(CC_INCLUDE_FLAGS) -Werror

CPP_SRC_FILES = $(shell find ./Src/ -name "*.cpp")
CPP_OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_SRC_FILES))
CPP_DPT_FILES = $(patsubst %.cpp, %.d, $(CPP_SRC_FILES))

DST = ./Build/ParserGen

$(DST) : $(CPP_OBJ_FILES)
	$(CC) $^ -o $@

%.o : %.cpp
	$(CC) $(CC_FLAGS) -c $< -o $@

%.d : %.cpp
	@set -e; \
	rm -f $@; \
	$(CC) -MM $< $(CC_INCLUDE_FLAGS) > $@.$$$$.dtmp; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$.dtmp > $@; \
	rm -f $@.$$$$.dtmp

-include $(CPP_OBJ_FILES:.o=.d)

clean :
	rm -f $(DST)
	rm -f $(CPP_OBJ_FILES)
	rm -f $(CPP_DPT_FILES)
	rm -f $(shell find ./Src/ -name "*.dtmp")

run :
	make
	$(DST)
