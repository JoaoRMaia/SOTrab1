#include <stdio.h>

int nthreads;

typedef struct Thread{
	int ing;
	int dur;
	int pri;
	int twait;
}thread;

typedef struct Resultado{
	float tm;
	float tw;
	int tc;
	int t;
}resultado;

void bsort(thread *lista){                 //ordena baseado no menor ingresso e como desempate a menor prioridade
	thread aux;
	for (int i = 0; i < nthreads ; i++){
		for (int j = 0 ; j < nthreads ; j++){
			if (lista[i].ing == lista[j].ing) {
				if (lista[i].pri < lista[j].pri){
					aux = lista[i];
					lista[i] = lista[j];
					lista[j] = aux;
				}
			}
			if (lista[i].ing < lista[j].ing) {
				aux = lista[i];
				lista[i] = lista[j];
				lista[j] = aux;
			}
		}
	}
}
int tTotal(thread *lista) {                // Retorna o tempo total gasto pelos processos
	int t = 0;
	for (int i = 0 ; i < nthreads ; i++) {
		t += lista[i].dur;
	}
	return t;
}

resultado FCFS( thread *threads) {
	resultado r;
	r.t = 0;
	bsort(threads);
	int tempototal = tTotal(threads);
	int i,j;
	for( i = 0 ; i < nthreads ; i++) {
		while (threads[i].dur > 0){
			threads[i].dur--;
			r.t++;
			for (j = i ; j < nthreads ; j++) {
				if (r.t >= threads[i].ing){
					threads[j].twait++;
				}
			}
		}
	}
	
	return r;
}

int main(int argc, char **argv)
{
	scanf("%d", &nthreads);
	thread  t[nthreads];
	
	return 0;
}

