
all:
	@echo "available targets: doc - install"

doc:
	@doxygen

install:
	cp kut.h /usr/local/include
	@echo "done."


