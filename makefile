all: clean

roda: compila
	./simula-vm 64 < referencias.txt

compila: 
	g++ -Wall -o "simula-vm" "Trabalho1SO.cc" -std=c++17

clean: roda
	rm simula-vm
