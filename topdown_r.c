#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int altura_maxima;
int num_caixas;
int max_dimensao;

typedef struct caixa
{
    int altura, largura, profundidade, valor, rotacao, numero;
} Caixa;

int max (int x, int y)
{
    return (x > y)? x : y;
}

Caixa* leCaixas(char *arquivo)
{
    if (!arquivo)
    {
        fprintf (stderr, "Erro: argumento invalido.\n");
        exit(EXIT_FAILURE);
    }

    FILE *in = fopen(arquivo, "r");

    if(!in)
    {
        fprintf(stderr,"Erro: nao foi possivel abrir o arquivo de entrada.\n");
        exit(EXIT_FAILURE);
    }

    int i=0;
    int j=0;
    int k=0;
    int l=0;
    int valor;
    Caixa *caixas;
    max_dimensao=0;

    if((fscanf(in, "%d", &valor))!=EOF )
    {
        num_caixas=valor;
        i++;
    }

    caixas = malloc(sizeof(Caixa)*2*num_caixas);

    while( (fscanf(in, "%d", &valor))!=EOF )
    {
        if(i==1)
            altura_maxima = valor;
        if(i>=2 && i<num_caixas+2)
        {
            caixas[j].valor = valor;
            j++;
        }
        if(i>=num_caixas+2)
        {
            if(k%3==0)
            {
                max_dimensao = max(max_dimensao, valor);
                caixas[l].largura = valor;
            }

            if(k%3==1)
            {
                max_dimensao = max(max_dimensao, valor);
                caixas[l].altura = valor;
            }

            if(k%3==2)
            {
                max_dimensao = max(max_dimensao, valor);
                caixas[l].profundidade = valor;
            }

            caixas[l].rotacao = 1;
            caixas[l].numero = l+1;
            k++;
            if(k%3==0)
                l++;
        }
        i++;
    }

    fclose(in);
    return caixas;
}

void geraRotacao(int num_caixas, Caixa *caixas)
{
    int index=num_caixas;
    for (int i = 0; i < num_caixas; i++)
    {
        caixas[index].altura = caixas[i].largura;
        caixas[index].profundidade = caixas[i].profundidade;
        caixas[index].largura = caixas[i].altura;
        caixas[index].valor = caixas[i].valor;
        caixas[index].numero = caixas[i].numero;
        caixas[index].rotacao = 2;
        index++;
    }
}

int compare (const void *a, const void * b)
{
    return ( (*(Caixa *)b).profundidade * (*(Caixa *)b).largura ) -
           ( (*(Caixa *)a).profundidade * (*(Caixa *)a).largura );
}

void escreveResultados(char* nome, int valor, double sec)
{
    FILE *resultado = fopen("resultado_topdownr.csv", "a+");

    if(!resultado)
    {
        fprintf(stderr,"Erro: nao foi possivel abrir o arquivo de resultados.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(resultado, "%s,", nome );
    fprintf(resultado, "%d,", valor );
    fprintf(resultado, "%.4f", sec );
    fprintf(resultado, "\n");

    fclose(resultado);
    return;
}

void escreveCaixas(char* arquivo, int valor)
{
    if (!arquivo)
    {
        fprintf (stderr, "Erro: argumento invalido.\n");
        exit(EXIT_FAILURE);
    }

    FILE *out = fopen(arquivo, "a+");

    if(!out)
    {
        fprintf(stderr,"Erro: nao foi possivel abrir o arquivo de saida.\n");
        exit(EXIT_FAILURE);
    }


    fprintf(out, "%d\n", valor);

    fclose(out);
    return;
}

int empilhaCaixas(int capacidade, int profundidade, int largura, Caixa *caixas)
{
    int m=0;
    int m_linha;
    int capacidade_linha;
    int profundidade_linha;
    int largura_linha;

    for(int i=0; i<num_caixas; i++)
    {
        capacidade_linha = capacidade-caixas[i].altura;
        profundidade_linha = caixas[i].profundidade;
        largura_linha = caixas[i].largura;
        if(capacidade_linha >= 0)
            if(profundidade_linha<= profundidade && largura_linha <= largura  )
            {
                m_linha = empilhaCaixas(capacidade_linha, profundidade_linha, largura_linha, caixas)+ caixas[i].valor;
                if (m_linha > m)
                    m = m_linha;
            }

    }
    return m;
}

int main(int argc, char *argv[] )
{
    if(argc != 3)
    {
        fprintf(stderr,"Argumentos necessarios: \"programa\" \"entrada\" \"saida\"");
        exit(EXIT_FAILURE);
    }
    clock_t start = clock();

    Caixa *caixas = leCaixas(argv[1]);

    geraRotacao(num_caixas, caixas);
    num_caixas=2*num_caixas;

    qsort(caixas, num_caixas, sizeof(caixas[0]), compare);

    int valor = empilhaCaixas(altura_maxima,max_dimensao,max_dimensao,caixas);

    escreveCaixas(argv[2], valor);

    clock_t end = clock();
    double sec=((double)end-start)/((double)CLOCKS_PER_SEC);

    escreveResultados(argv[1], valor, sec);

    free(caixas);

    return 0;
}

