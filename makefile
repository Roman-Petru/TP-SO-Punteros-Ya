all: clean default

clean:
	$(MAKE) clean -C ./Utils/.
	$(MAKE) clean -C ./App/.
	$(MAKE) clean -C ./Cliente/.
	$(MAKE) clean -C ./Comanda/.
	$(MAKE) clean -C ./Restaurante/.
	$(MAKE) clean -C ./Sindicato/.

default:
	$(MAKE) -C ./Utils/.
	$(MAKE) -C ./App/.
	$(MAKE) -C ./Cliente/.
	$(MAKE) -C ./Comanda/.
	$(MAKE) -C ./Restaurante/.
	$(MAKE) -C ./Sindicato/.
