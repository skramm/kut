
OBJ_DIR=build/obj

DEMO_SRC_FILES      = $(wildcard demo/*.cpp)
DEMO_TEST_SRC_FILES = $(wildcard demo/test/*.cpp)

OBJ_FILES      = $(patsubst demo/%.cpp,$(OBJ_DIR)/%.o,$(DEMO_SRC_FILES))
OBJ_TEST_FILES = $(patsubst demo/test/%.cpp,$(OBJ_DIR)/%.o,$(DEMO_TEST_SRC_FILES))

EXE_FILES       = $(patsubst demo/%.cpp,$(BUILD_DIR)/%,$(DEMO_SRC_FILES))


show:
	@echo "DEMO_SRC_FILES=$(DEMO_SRC_FILES)"
	@echo "DEMO_TEST_SRC_FILES=$(DEMO_TEST_SRC_FILES)"
	@echo "OBJ_FILES=$(OBJ_FILES)"
	@echo "OBJ_TEST_FILES=$(OBJ_TEST_FILES)"
	@echo "EXE_FILES=$(EXE_FILES)"

demo:
	@echo "done."

demo_apps: $(D)
	@echo "Done";

doc:
	@doxygen

install:
	cp kut.h /usr/local/include
	cp kut2.h /usr/local/include
	@echo "done."


