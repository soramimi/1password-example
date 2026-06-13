
main:
	cd libapikey && make
	cd main && make

run:
	OP_ACCOUNT_NAME="John Doe" && cd main && make run

clean:
	cd libapikey && make clean
	cd main && make clean
	
.PHONY: main run clean

