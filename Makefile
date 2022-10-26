
usage:
	@echo "Usage:"
	@echo -e "\tmake [build|run|clean]"

build: src/*
	-mkdir -p obj
	g++ -Wall -Wextra -pedantic -std=c++20 -g src/* -o obj/main.out

run: build
	./obj/main.out

clean:
	-rm -rf obj

