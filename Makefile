.PHONY: all clean experiments

all:
	$(MAKE) -C src

experiments:
	$(MAKE) -C experiments

clean:
	$(MAKE) -C src clean
	$(MAKE) -C experiments clean

