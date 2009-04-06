all: bv


bv: bv.c tools.c bn.c ec.c
	gcc $^ -o $@ -lcrypto -std=c99

clean:
	-@rm bv 2&>/dev/null; exit 0


tools.c:
	@echo "get $@ from segher's git"; exit 1
	

bn.c:
	@echo "get $@ from segher's git"; exit 1

ec.c:
	@echo "get $@ from segher's git"; exit 1
