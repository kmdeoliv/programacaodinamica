#include <stdio.h>
#include <stdlib.h>

int altura_maxima;
int num_caixas;
int max_dimensao;

typedef struct caixa
{
    int altura, largura, profundidade, valor, rotacao;
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


int ** preencheMatrizCaixasValidas(Caixa *caixas)
{
    int **grafo = (int **)malloc(num_caixas * sizeof(int *));
    for (int i=0; i<num_caixas; i++)
        grafo[i] = (int *)malloc(num_caixas * sizeof(int));

    for (int i = 0; i <  num_caixas; i++)
        for (int j = 0; j < num_caixas; j++)
            grafo[i][j] = 0;

    for (int i =0; i< num_caixas; i++)
        for (int j =0; j< num_caixas; j++)
            if(caixas[i].largura>= caixas[j].largura && caixas[i].profundidade >= caixas[j].profundidade)
                grafo[i][j]=1;

    return grafo;

}

void escreveCaixas(char *arquivo, Caixa *caixas, int *soma, int *indices  )
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
    int capacidade = altura_maxima;
    int indice = indices[capacidade];
    int total_caixas = 0;
    int valor = soma[altura_maxima];

    while(indice>-1)
    {
        if(total_caixas>tamanho_vetor-1)
        {
            tamanho_vetor = 2*tamanho_vetor;
            resultado = realloc(resultado,sizeof(Caixa)*tamanho_vetor);
        }
        resultado[total_caixas] = caixas[indice];
        total_caixas++;
        capacidade = capacidade-caixas[indice].altura;
        indice = indices[capacidade];
    }

    fprintf(out, "%d\n", valor);
    fprintf(out, "%d\n", total_caixas);
    for(int i=total_caixas-1; i>=0; i--)
    {
        fprintf(out, "%d ", resultado[i].largura);
        fprintf(out, "%d ", resultado[i].altura);
        fprintf(out, "%d ", resultado[i].profundidade);
        fprintf(out, "%d ", resultado[i].valor);
        fprintf(out, "%d\n", resultado[i].rotacao);
    }

    free(resultado);
    fclose(out);
    return;
}

int empilhaCaixas(int capacidade, int profundidade, int largura, Caixa *caixas, int *m, int *indices)
{
    int m_linha;
    int capacidade_linha;
    int **grafo = preencheMatrizCaixasValidas(caixas);

    for (int k=0; k<=capacidade; k++)
    {
        for (int i=0; i<num_caixas; i++)
        {
            capacidade_linha = k-caixas[i].altura;

            if(capacidade_linha>=0  )
            {
                m_linha = m[capacidade_linha]+caixas[i].valor;
                if(m_linha>=m[k])
                {

                    int aux=m[k];
                    int j=indices[k];

                    m[k] = m_linha;
                    indices[k] = i;

                    int ant = k-caixas[i].altura;

                    if(indices[ant]!=-1)
                    {
                        if(grafo[indices[ant]][i]==0)
                        {
                            m[k] = aux;
                            indices[k] = j;
                        }
                    }
                }
            }
        }
    }
    free(grafo);
    return m[capacidade];
}

int main(int argc, char *argv[] )
{
    if(argc != 3)
    {
        fprintf(stderr,"Argumentos necessarios: \"entrada\" \"saida\"");
        return 1;
    }
    else
    {
        Caixa *caixas = leCaixas(argv[1]);

        geraRotacao(num_caixas, caixas);
        num_caixas=2*num_caixas;

        qsort(caixas, num_caixas, sizeof(caixas[0]), compare);

        int *soma = malloc((altura_maxima+1) * sizeof(int));
        int *indices = malloc((altura_maxima+1) * sizeof(int));

        for(int i=0; i<=altura_maxima; i++)
        {
            soma[i]=0;
            indices[i]=-1;
        }

        printf("Lucro: %d\n",empilhaCaixas(altura_maxima,max_dimensao,max_dimensao,caixas, soma, indices));
        printf("Numero de caixas: %d\n", num_caixas/2);
        printf("Altura maxima: %d\n", altura_maxima);

        escreveCaixas(argv[2], caixas, soma, indices);

        free(soma);
        free(indices);
        free(caixas);
    }
    return 0;
}

