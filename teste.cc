#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Page;

vector<int> PIDs;
vector<Page> Pagetable;
int RAM[64];
int QdAtual;
int N_QUADROS;

struct Page{																			// Struct que representa uma pagina da pagetable
	Page() : valido(0),presente(0),frame(0) {}
	Page(int p) : valido(1), presente(0),frame(p) {} 

	bool valido;
	bool presente;
	int frame;
	
};

void reset() {
	for ( auto& a : Pagetable ){
		a.valido = 0;
		a.frame = 0;
		a.presente = 0;
	}
	for (int i = 0 ; i < N_QUADROS ; i++) RAM[i] = 0;
	QdAtual = 0;
}

pair<int,int> MaxPair(pair<int,int> a , pair<int,int> b){
	return a.second >= b.second ? a : b;
}

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

int TiraDaRamOPT(int Acessos) {                                // Note, essa função não realmente retira da ram, so marca na pagetable que tal processo				   	
	pair<int,int> ultimo{0,0};
	long unsigned int j;							// não está na RAM, quem realmente tira é a função que é chamada em seguida, PoeNaRam, que sobreescreve	
	if (QdAtual < N_QUADROS) return QdAtual;	// <------       Serve para não tirar elementos quando a RAM ainda não estiver preenchida
	for (int i = 0 ; i < min(QdAtual,N_QUADROS) ; i++) {
		j = Acessos;
		//cout << "posição da ram: " << i << " e " << RAM[i] << " o j em " << PIDs[j] << endl;
		while (PIDs[j] != RAM[i] && j <= PIDs.size()) {
			j++;
			//cout << " O j está em " << j << " e o tamanho do vector em " << PIDs.size() << endl;
		}
		if ( j >= PIDs.size()) {
			cout << "Não achei outra ocorrencia de "<< RAM[i] << " vou sair." << endl;
			ultimo.first = i;
			break;
		} 
		ultimo = MaxPair(ultimo,pair<int,int>{i,j}); 
		//cout << "O Ultimo é o "<< RAM[ultimo.first] << " na posição " << ultimo.second << endl;
	}
	for ( auto& it : Pagetable ){
		if ( RAM[ultimo.first] == it.frame ) {
			it.presente = 0;
			break;
		}
	}
	return ultimo.first;
}

void PoeNaRamOPT( int p, int pos) {													// Faz exatamente o que se espera
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			//cout << "Vou botar na posição " << pos << endl;
			RAM[pos] = p;
			it.presente = 1;
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

void PoeNaRamLRU ( int p) {													// Faz exatamente o que se espera
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			RAM[min(QdAtual,N_QUADROS-1)] = p;
			it.presente = 1;
			return;
		}
	}
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
	int PosProc;
	int aux;
	//cout << "Entrei em usa" << endl;
	for ( int i = 0 ; i < N_QUADROS ; i++) if (RAM[i] == p) {
		PosProc = i;
		//cout << "Encontrei o Processo na posição " << PosProc;
	}
	for ( int i = PosProc ; i > 0 ; i--) {
		//cout << "Acessando posições " << i << " e " << i-1 << endl;
		aux = RAM[i];
		RAM[i] = RAM[i-1];
		RAM[i-1] = aux;
	} 
}

int FIFO (vector<int> Acessos) {
	reset();
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
	reset();
	int PagFault = 0;
	int EstadoPag;
	for ( auto PID : Acessos ) {
		EstadoPag = Find(PID);
		if ( EstadoPag == 2) Usa(PID);
		
		else if (EstadoPag == 1){  // Caso seja valido e não esteja na RAM, tire um elemento da RAM e coloque o atual
			TiraDaRamLRU();
			PoeNaRamLRU(PID);
			QdAtual++;
			PagFault++;
		}
		else {										// Caso não seja válido, crie a pagina, retire uma pagina da RAM e coloque a atual
			Pagetable.push_back(Page{PID});
			TiraDaRamLRU();
			PoeNaRamLRU(PID);
			QdAtual++;
			PagFault++;
		}
	}
	return PagFault;
}

int OPT (vector<int> Acessos) {
	reset();
	int AcessoAtual = 0;
	int PagFault = 0;
	int EstadoPag;
	int pos;
	for ( auto PID : Acessos ) {
		EstadoPag = Find(PID);
		if ( EstadoPag == 2) AcessoAtual++;
		
		else if (EstadoPag == 1){  // Caso seja valido e não esteja na RAM, tire um elemento da RAM e coloque o atual
			pos = TiraDaRamOPT(AcessoAtual);
			PoeNaRamOPT(PID,pos);
			QdAtual++;
			PagFault++;
			AcessoAtual++;
		}
		else {										// Caso não seja válido, crie a pagina, retire uma pagina da RAM e coloque a atual
			Pagetable.push_back(Page{PID});
			pos = TiraDaRamOPT(AcessoAtual);
			PoeNaRamOPT(PID,pos);
			QdAtual++;
			PagFault++;
			AcessoAtual++;
		}
	}
	return PagFault;
}

int main (int argc, char **argv) {
	
	if (argc == 1) cout << "Uso incorreto, use: " << argv[0] << " Número de Quadros " << endl;
	N_QUADROS = atoi(argv[1]);
	//RAM = (int *) malloc(sizeof(int)*N_QUADROS);
	int PID;
	while (!feof(stdin)){				// le os acessos e os guarda em um vector
		cin >> PID;
		if (feof(stdin)) break;
		PIDs.push_back(PID);
	}
	cout << endl;
	cout << atoi(argv[1]) << " quadros, \t " << PIDs.size() << " refs: FIFO:\t "<< LRU(PIDs) << " PFs, LRU: \t" << LRU(PIDs) << " PFs, OPT: \t "<< OPT(PIDs) << " PFs" << endl; 
	
	return 0;
}
