#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int altura_maxima;
int num_caixas;

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

    if((fscanf(in, "%d", &valor)) != EOF )
    {
        num_caixas = valor;
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
                caixas[l].largura = valor;

            if(k%3 == 1)
                caixas[l].altura = valor;

            if(k%3 == 2)
                caixas[l].profundidade = valor;

            caixas[l].numero = l+1;
            caixas[l].rotacao = 1;
            k++;
            if(k%3 == 0)
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
    return;
}

int compare (const void *a, const void * b)
{
    return ( (*(Caixa *)b).profundidade * (*(Caixa *)b).largura ) -
           ( (*(Caixa *)a).profundidade * (*(Caixa *)a).largura );
}

int** preencheMatriz(int linha, int coluna, int valor)
{
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
    int linha = num_caixas;

    for (int i=0; i<linha; i++)
        free(matriz[i]);

    free(matriz);

}

void escreveResultados(char* nome, int valor, double sec)
{
    FILE *resultado = fopen("resultado_bottomup.csv", "a+");

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

int ** preencheMatrizCaixasValidas(Caixa *caixas)
{
    int **grafo = malloc(num_caixas * sizeof(int *));
    for (int i=0; i<num_caixas; i++)
        grafo[i] = malloc(num_caixas * sizeof(int));

    for (int i=0; i<num_caixas; i++)
        for (int j=0; j<num_caixas; j++)
            grafo[i][j] = 0;

    for (int i=0; i<num_caixas; i++)
        for (int j=0; j<num_caixas; j++)
            if(caixas[i].largura >= caixas[j].largura && caixas[i].profundidade >= caixas[j].profundidade)
                grafo[i][j]=1;

    return grafo;

}

void escreveCaixas(char *arquivo, Caixa *caixas, int **soma)
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

    int valor = soma[num_caixas-1][altura_maxima];
    int i = num_caixas - 1;
    int j = altura_maxima;
    int total_caixas = 0;
    int tamanho_vetor = num_caixas;
    Caixa *resultado = malloc(sizeof(Caixa)*tamanho_vetor);


    while(valor>0 && i>0)

    {
        if(total_caixas > tamanho_vetor - 1)
        {
            tamanho_vetor = 2 * tamanho_vetor;
            resultado = realloc(resultado, sizeof(Caixa) * tamanho_vetor);
        }

        if(soma[i][j-1] > soma[i-1][j])
        {
            resultado[total_caixas] = caixas[i];
            total_caixas++;
            j = j - caixas[i].altura;
        }
        else
        {
            while(soma[i][j] == soma[i-1][j] && i>1)
            {
                i--;
            }
            (soma[i][j] == soma[0][j])?i--:i;
            resultado[total_caixas] = caixas[i];
            total_caixas++;
            j = j - caixas[i].altura;
        }
        valor = soma[i][j];
    }

    while(valor>0 && j>=0)
    {
        resultado[total_caixas] = caixas[i];
        total_caixas++;
        j = j - caixas[i].altura;
        valor = soma[i][j];
    }

    fprintf(out, "%d\n", soma[num_caixas-1][altura_maxima]);
    fprintf(out, "%d\n", total_caixas);

    for(int i=total_caixas-1; i>=0; i--)
    {
        //fprintf(out, "%d ", resultado[i].largura);
        //fprintf(out, "%d ", resultado[i].altura);
        //fprintf(out, "%d ", resultado[i].profundidade);
        //fprintf(out, "%d ", resultado[i].valor);
        fprintf(out, "%d ", resultado[i].numero);
        fprintf(out, "%d\n", resultado[i].rotacao);

    }

    free(resultado);
    return;
}

int determinaAnterior(int **m, int i, int capacidade_linha)
{
    int anterior;

    if(capacidade_linha==0)
    {
        int aux =i;
        while(aux > 1)
            aux--;
        anterior = aux;
    }
    else
    {
        if(m[i][capacidade_linha-1] > m[i-1][capacidade_linha])
            anterior = i;

        else
        {
            int aux =i;
            while(m[aux][capacidade_linha-1] <= m[aux-1][capacidade_linha] && aux>1)
                aux--;
            anterior = aux;
        }
    }
    return anterior;
}

int empilhaCaixas(int capacidade, Caixa *caixas, int **m)
{
    int m_linha;
    int capacidade_linha;
    int **grafo = preencheMatrizCaixasValidas(caixas);


    for (int i=0; i<num_caixas; i++)
    {
        for (int k=0; k<=capacidade; k++)
        {
            capacidade_linha = k - caixas[i].altura;

            if(i==0)
            {
                if(capacidade_linha >= 0)
                    m[i][k]= m[i][capacidade_linha]+caixas[i].valor;
            }
            else
            {
                if(capacidade_linha >= 0)
                {
                    int anterior = determinaAnterior(m,i,capacidade_linha);

                    if(grafo[anterior][i] == 1)
                        m[i][k]= max(m[i][capacidade_linha]+caixas[i].valor, m[i-1][k]);

                    else
                        m[i][k] = m[i-1][k];

                }
                else
                {
                    m[i][k] = m[i-1][k];
                }
            }
        }
    }
    freeMatriz(grafo);
    return m[num_caixas-1][altura_maxima];
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

    int **soma = preencheMatriz(num_caixas, altura_maxima + 1, 0);

    int valor = empilhaCaixas(altura_maxima, caixas, soma);

    escreveCaixas(argv[2], caixas, soma);

    clock_t end = clock();
    double sec=((double)end-start)/((double)CLOCKS_PER_SEC);

    escreveResultados(argv[1], valor, sec);

    freeMatriz(soma);
    free(caixas);

    return 0;
}
