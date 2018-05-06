#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int altura_maxima;
int num_caixas;
int max_dimensao;
int indice_i;
int indice_j;

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

    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    int valor;
    Caixa *caixas;
    max_dimensao = 0;

    if((fscanf(in, "%d", &valor))!=EOF )
    {
        num_caixas=valor;
        i++;
    }

    caixas = malloc(sizeof(Caixa) * 2 * num_caixas);

    while( (fscanf(in, "%d", &valor)) != EOF )
    {
        if(i == 1)
            altura_maxima = valor;
        if(i >= 2 && i < num_caixas+2)
        {
            caixas[j].valor = valor;
            j++;
        }
        if(i >= num_caixas+2)
        {
            if(k%3 == 0)
            {
                max_dimensao = max(max_dimensao, valor);
                caixas[l].largura = valor;
            }

            if(k%3 == 1)
            {
                max_dimensao = max(max_dimensao, valor);
                caixas[l].altura = valor;
            }

            if(k%3 == 2)
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
    int index = num_caixas;
    for (int i=0; i<num_caixas; i++)
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

int **preencheMatriz(int valor)
{
    int linha = num_caixas+2;
    int coluna = altura_maxima+1;

    int **m = malloc(linha * sizeof(int *));

    for (int i=0; i<linha; i++)
        m[i] = malloc(coluna * sizeof(int));

    for(int i=0; i<linha; i++)
        for(int j=0; j<coluna; j++)
            m[i][j] = valor;
    return m;
}

void freeMatriz(int ** matriz)
{
    int linha = num_caixas+2;

    for (int i=0; i<linha; i++)
        free(matriz[i]);

    free(matriz);

}

void escreveResultados(char* nome, int valor, double sec)
{
    FILE *resultado = fopen("resultado_topdownm.csv", "a+");

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

void escreveCaixas(char* arquivo, Caixa *caixas, int **soma, int **indexes)
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

    int tamanho_vetor = num_caixas;
    Caixa *resultado = malloc(sizeof(Caixa)*tamanho_vetor);
    int i = indice_i;
    int j = indice_j;
    int total_caixas = 0;
    int valor = soma[i][j];
    int indice = indexes[i][j];
    int proximo_valor;

    resultado[total_caixas] = caixas[indice];
    total_caixas++;
    proximo_valor = valor - caixas[indice].valor;
    j = j - caixas[indice].altura;

    while(proximo_valor != 0)
    {
        if(total_caixas > tamanho_vetor - 1)
        {
            tamanho_vetor = 2 * tamanho_vetor;
            resultado = realloc(resultado, sizeof(Caixa) * tamanho_vetor);
        }
        if(valor == proximo_valor)
        {
            indice = indexes[i][j];
            resultado[total_caixas] = caixas[indice];
            total_caixas++;
            proximo_valor = valor - caixas[indice].valor;
            j = j - caixas[indice].altura;
            valor = soma[i][j];
        }
        else
        {
            while(valor != proximo_valor && i>0)
            {
                i--;
                valor = soma[i][j];
            }
            indice = indexes[i][j];
            resultado[total_caixas] = caixas[indice];
            total_caixas++;
            proximo_valor = valor - caixas[indice].valor;
            j = j - caixas[indice].altura;
            valor = soma[i][j];
        }
    }

    fprintf(out, "%d\n", soma[indice_i][indice_j]);
    fprintf(out, "%d\n", total_caixas);

    for(int i=0; i<total_caixas; i++)
    {
        //fprintf(out, "%d ", resultado[i].largura);
        //fprintf(out, "%d ", resultado[i].altura);
        //fprintf(out, "%d ", resultado[i].profundidade);
        //fprintf(out, "%d ", resultado[i].valor);
        fprintf(out, "%d ", resultado[i].numero);
        fprintf(out, "%d\n", resultado[i].rotacao);

    }

    free(resultado);
    fclose(out);
    return;
}

int empilhaCaixas(int capacidade, int profundidade, int largura, Caixa *caixas, int **m, int indice, int **indexes)
{
    int m_linha;
    int capacidade_linha;
    int profundidade_linha;
    int largura_linha;

    if(m[indice][capacidade] != 0)
    {
        return m[indice][capacidade];
    }

    for(int i=0; i<num_caixas; i++)
    {
        capacidade_linha = capacidade - caixas[i].altura;
        profundidade_linha = caixas[i].profundidade;
        largura_linha = caixas[i].largura;
        if(capacidade_linha >= 0)
        {
            if(profundidade_linha <= profundidade && largura_linha <= largura  )
            {
                m_linha = empilhaCaixas(capacidade_linha, profundidade_linha, largura_linha, caixas,m, i, indexes)+ caixas[i].valor;
                if (m_linha > m[indice][capacidade])
                {
                    m[indice][capacidade] = m_linha;
                    indexes[indice][capacidade] = i;
                }

            }

        }

    }

    indice_i = indice;
    indice_j = capacidade;
    return m[indice][capacidade];
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
    num_caixas = 2 * num_caixas;

    qsort(caixas, num_caixas, sizeof(caixas[0]), compare);

    int **m = preencheMatriz(0);
    int **indexes = preencheMatriz(-1);

    int valor = empilhaCaixas(altura_maxima, max_dimensao, max_dimensao, caixas, m, num_caixas, indexes);

    escreveCaixas(argv[2], caixas, m, indexes);

    clock_t end = clock();
    double sec=((double)end-start)/((double)CLOCKS_PER_SEC);

    escreveResultados(argv[1], valor, sec);

    freeMatriz(m);
    freeMatriz(indexes);
    free(caixas);

    return 0;
}
