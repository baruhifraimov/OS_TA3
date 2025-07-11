all:
	make -C LVL10
	make -C LVL9
	make -C LVL8
	make -C LVL7
	make -C LVL6
	make -C LVL5
	make -C LVL4
	make -C LVL3
	make -C LVL2
	make -C LVL1

clean:
	make -C LVL10 clean
	make -C LVL9 clean
	make -C LVL8 clean
	make -C LVL7 clean
	make -C LVL6 clean
	make -C LVL5 clean
	make -C LVL4 clean
	make -C LVL3 clean
	make -C LVL2 clean
	make -C LVL1 clean

.PHONY: all clean