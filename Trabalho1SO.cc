#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Page;
static const int N_QUADROS = 1024;

vector<int> PIDs;
vector<Page> Pagetable;
int RAM[N_QUADROS];
int QdAtual;

struct Page{																			// Struct que representa uma pagina da pagetable
	Page() : valido(0),presente(0),frame(0) {}
	Page(int p) : valido(1), presente(0),frame(p) {} 
	
	bool valido;
	bool presente;
	int frame;
	
};

int Find(int p) {																	// Essa função serve para determinar o estado do processo P, 2 para existe e está na ram,
	for ( auto& it : Pagetable ) {									// 1 para existe mas não está na RAM, e 0 para não existe.
		if (it.frame == p) {
			if (it.presente && it.valido)	return 2;
			else if (it.valido){
				it.presente = 1;
				return 1;	
			} 			
		}
	}
	return 0;
}

void TiraDaRam() {                                // Note, essa função não realmente retira da ram, so marca na pagetable que tal processo				   	
	int p = RAM[QdAtual%N_QUADROS]; 								// não está na RAM, quem realmente tira é a função que é chamada em seguida, PoeNaRam, que sobreescreve	
	if (QdAtual < N_QUADROS) return;	// <------       Serve para não tirar elementos quando a RAM ainda não estiver preenchida
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			it.presente = 0;
			return;
		}
	}
}

void PoeNaRam ( int p) {													// Faz exatamente o que se espera
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			RAM[QdAtual%N_QUADROS] = p;
			it.presente = 1;
			return;
		}
	}
}

void PoeNaRamLRU ( int p) {												// Faz exatamente o que se espera
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			it.presente = 1;
		}
	}
	//cout << "Entrando na RAM o " <<p << endl;
	for (int i = N_QUADROS-2 ; i >= 0 ; i--){
		//cout << i+1 << endl;
		RAM[i+1] = RAM[i];
	}
	RAM[0] = p;
}

void TiraDaRamLRU() {                                // Note, essa função não realmente retira da ram, so marca na pagetable que tal processo				   	
	int p = RAM[min(QdAtual,N_QUADROS-1)]; 								// não está na RAM, quem realmente tira é a função que é chamada em seguida, PoeNaRam, que sobreescreve	
	if (QdAtual < N_QUADROS) return;	// <------       Serve para não tirar elementos quando a RAM ainda não estiver preenchida
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			it.presente = 0;
			return;
		}
	}
}
void Usa(int p) {
	int PosProc=QdAtual;
	int aux;
	//cout << "Entrei em usa" << endl;
	for ( int i = 0 ; i < N_QUADROS ; i++) if (RAM[i] == p) {
		PosProc = i;
		//cout << "Encontrei o Processo na posição " << PosProc << endl;
		break;
	}
	for ( int i = PosProc ; i > 0 ; i--) {
		//cout << "Acessando posições " << i << " e " << i-1 << endl;
		aux = RAM[i];
		RAM[i] = RAM[i-1];
		RAM[i-1] = aux;
	} 
}

int FIFO (vector<int> Acessos) {
	QdAtual = 0;
	int PagFault = 0;
	int EstadoPag;
	for ( auto PID : Acessos ) {
		EstadoPag = Find(PID);
		if ( EstadoPag == 2) {}  // Caso ele esteja na RAM, não faça nada
		else if (EstadoPag == 1){ // Caso seja valido e não esteja na RAM, tire um elemento da RAM e coloque o atual
			TiraDaRam();
			PoeNaRam(PID);
			QdAtual++;
			PagFault++;
		}
		else {										// Caso não seja válido, crie a pagina, retire uma pagina da RAM e coloque a atual
			Pagetable.push_back(Page{PID});
			TiraDaRam();
			PoeNaRam(PID);
			QdAtual++;
			PagFault++;
		}
	}
	return PagFault;
}

int LRU (vector<int> Acessos) {
	QdAtual = 0;
	int PagFault = 0;
	int EstadoPag;
	for ( auto PID : Acessos ) {
			
		//cout << "Estado da RAM: ";
		//for ( int i = 0 ; i < N_QUADROS ; i++) cout << RAM[i] << " ";
		EstadoPag = Find(PID);
		if ( EstadoPag == 2) {    
			Usa(PID);
		}
		else if (EstadoPag == 1){  // Caso seja valido e não esteja na RAM, tire um elemento da RAM e coloque o atual
			TiraDaRamLRU();
			PoeNaRamLRU(PID);
			QdAtual++;
			PagFault++;
		}
		else {										// Caso não seja válido, crie a pagina, retire uma pagina da RAM e coloque a atual
			Pagetable.insert(Pagetable.begin(),Page{PID});
			TiraDaRamLRU();
			PoeNaRamLRU(PID);
			QdAtual++;
			PagFault++;
		}
	}
	return PagFault;
}


int main (int argc, char **argv) {
	

	int PID;
	while (!feof(stdin)){				// le os acessos e os guarda em um vector
		cin >> PID;
		if (feof(stdin)) break;
		PIDs.push_back(PID);
	}
	
	cout << FIFO(PIDs);
	cout << LRU(PIDs);
	
	return 0;
}
