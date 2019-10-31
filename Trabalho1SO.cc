#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Page;
int N_QUADROS;

vector<int> PIDs;
vector<Page> Pagetable;
vector<int> RAM;
int QdAtual;

struct Page{												// Struct que representa uma pagina da pagetable
	Page() : valido(0),presente(0),frame(0) {}
	Page(int p) : valido(1), presente(0),frame(p) {} 
	
	bool valido;
	bool presente;
	int frame;
	
};

pair<int,int> MaxPair(pair<int,int> a , pair<int,int> b){   // Retorna o max() do segundo elemento da struct pair
	return a.second >= b.second ? a : b;
}

int Find(int p) {											// Essa função serve para determinar o estado do processo P,
	for ( auto& it : Pagetable ) {							// 2 para existe e está na ram,
		if (it.frame == p) {								// 1 para existe mas não está na RAM e 
			if (it.presente && it.valido)	return 2;		// 0 para não existe.
			else if (it.valido){
				it.presente = 1;
				return 1;	
			} 			
		}
	}
	return 0;
}

void TiraDaRam() {                               		// Note, essa função não realmente retira da ram, so marca na pagetable que tal processo				   	
	int p = RAM[QdAtual%N_QUADROS]; 					// não está na RAM, quem realmente tira é a função que é chamada em seguida, PoeNaRam, que sobreescreve	
	if (QdAtual < N_QUADROS) return;	// <------       Serve para não tirar elementos quando a RAM ainda não estiver preenchida
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			it.presente = 0;
			return;
		}
	}
}

int TiraDaRamOPT(int Acessos) {                     // Nessa função, nos buscamos a proxima ocorrência de cada elemento da RAM, e a posição do último				   	
	pair<int,int> ultimo{0,0};						// a ser referenciado é retornada para ser sobreescrita pela função PoeNaRamOPT
	long unsigned int j;							
	if (QdAtual < N_QUADROS) return QdAtual; 		// Serve para não tirar elementos quando a RAM ainda não estiver preenchida
	for (int i = 0 ; i < min(QdAtual,N_QUADROS) ; i++) {
		j = Acessos;
		while (PIDs[j] != RAM[i] && j <= PIDs.size()) j++;
		if ( j >= PIDs.size()) {
			ultimo.first = i;
			break;
		} 
		ultimo = MaxPair(ultimo,pair<int,int>{i,j}); 
	}
	for ( auto& it : Pagetable ){
		if ( RAM[ultimo.first] == it.frame ) {
			it.presente = 0;
			break;
		}
	}
	return ultimo.first;
}

void PoeNaRamOPT( int p, int pos) {					// Poe na RAM na posição retornada pela TiraDaRamOPT e marca como presente na pagetable
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			RAM[pos] = p;
			it.presente = 1;
			return;
		}
	}
}
void PoeNaRam ( int p) {							// Poe na ram utilizando-se do mod para ciclar entre os elementos
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			RAM[QdAtual%N_QUADROS] = p;
			it.presente = 1;
			return;
		}
	}
}

void PoeNaRamLRU ( int p) {							// Insere o elemento no começo para sinalizar que foi usado e remove
	for ( auto& it : Pagetable ){					// o último elemento, que no caso é que está há mais tempo ser ser acessado
		if ( p == it.frame ) {
			RAM.insert(RAM.begin(),p);
			RAM.pop_back();
			it.presente = 1;
			return;
		}
	}
}

void TiraDaRamLRU() {                               // Essa função apenas sinaliza na pagetable que o elemento não está mais presente na RAM,				   	
	int p = RAM[min(QdAtual,N_QUADROS-1)]; 			// quem de fato remove os elementos é a PoeNaRamLRU
	if (QdAtual < N_QUADROS) return;
	for ( auto& it : Pagetable ){
		if ( p == it.frame ) {
			it.presente = 0;
			return;
		}
	}
}
void Usa(int p) {									// Joga o elemento para o inicio do vetor, para sinalizar que foi usado recentemente
	for ( int i = 0 ; i < N_QUADROS ; i++) if (RAM[i] == p) {
		RAM.erase(RAM.begin()+i);
	}
	RAM.insert(RAM.begin(),p);
}

int FIFO (vector<int> Acessos) {                    // Função que retorna o número de page faults através do algoritmo FIFO
	QdAtual = 0;									// Ela cicla pelos elementos usando mod N_QUADROS
	int PagFault = 0;
	int EstadoPag;
	for ( auto PID : Acessos ) {
		EstadoPag = Find(PID);
		if ( EstadoPag == 2) {}			    	    // Caso ele esteja na RAM, não faça nada
		else if (EstadoPag == 1){					// Caso seja valido e não esteja na RAM, tire um elemento da RAM e coloque o atual
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

int LRU (vector<int> Acessos) {						// A lógica de implementação desse algoritmo foi a seguinte,
	QdAtual = 0;									// Criar um vetor com os elementos da ram, o elemento que está em último,
	int PagFault = 0;								// é o menos acessado, portanto é removido
	int EstadoPag;
	for ( auto PID : Acessos ) {
		EstadoPag = Find(PID);
		if ( EstadoPag == 2) Usa(PID);
		
		else if (EstadoPag == 1){  					// Caso seja valido e não esteja na RAM, tire um elemento da RAM e coloque o atual
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

int OPT (vector<int> Acessos) {						// Para encontrarmos o ótimo, vasculhamos para cada posição da RAM, a próxima referência
	QdAtual = 0;									// para aquele elemento, o elemento que tiver a última refêrencia ou não apresentar mais
	int AcessoAtual = 0;							// referências, é removido.
	int PagFault = 0;
	int EstadoPag;
	int pos;
	for ( auto PID : Acessos ) {
		EstadoPag = Find(PID);
		if ( EstadoPag == 2) AcessoAtual++;
		
		else if (EstadoPag == 1){ 					 // Caso seja valido e não esteja na RAM, tire um elemento da RAM e coloque o atual
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
	
	if (argc == 1) cout << "Uso incorreto, use: " << argv[0] << " Número de Quadros < input.txt" << endl;
	N_QUADROS = atoi(argv[1]);
	for (int i = 0 ; i < N_QUADROS ; i++ ) RAM.push_back(-1);
	int PID;
	while (!feof(stdin)){							// Le os acessos e os guarda em um vector
		cin >> PID;
		if (feof(stdin)) break;
		PIDs.push_back(PID);
	}
	
	cout << N_QUADROS << " quadros, "<< PIDs.size() << " refs: FIFO: "<< FIFO(PIDs) << " PFs, LRU: ";  
	Pagetable = vector<Page>();							// Limpa a pagetable
	for (int i = 0 ; i < N_QUADROS ; i++ ) RAM[i] = -1; // Limpa a ram
	cout << LRU(PIDs) << " PFs, OPT: ";
	Pagetable = vector<Page>();
	for (int i = 0 ; i < N_QUADROS ; i++ ) RAM[i] = -1;
	cout << OPT(PIDs) << " PFs " << endl;
	
	return 0;
}
