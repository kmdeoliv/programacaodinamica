#include <stdio.h>
#include <stdlib.h>
#include<string.h>
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

int *preencheVetor(int tamanho, int valor)
{
    int *vetor = malloc( tamanho * sizeof(int));

    for(int i=0; i<tamanho; i++)
        vetor[i] = valor;

    return vetor;
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

char* concatenar(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *remover(char* mystr)
{
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
        return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
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

void gravaTabela(int **soma, char* arquivo)
{
    if (!arquivo)
    {
        fprintf (stderr, "Erro: argumento invalido.\n");
        exit(EXIT_FAILURE);
    }

    FILE *tabela = fopen(arquivo, "a+");

    if(!tabela)
    {
        fprintf(stderr,"Erro: nao foi possivel abrir a tabela.\n");
        exit(EXIT_FAILURE);
    }

    int linha = num_caixas;
    int coluna = altura_maxima+1;

    for(int j=0; j<coluna; j++)
        fprintf(tabela, "%d,", j);
    fprintf(tabela,"\n");

    for(int i=0; i<linha; i++)
    {
        for(int j=0; j<coluna; j++)
            fprintf(tabela, "%d,", soma[i][j]);
        fprintf(tabela,"\n");
    }
    fclose(tabela);
    return;
}

void freeGrafo(int **grafo)
{
    for (int i=0; i<num_caixas; i++)
        free(grafo[i]);

    free(grafo);

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
    freeGrafo(grafo);
    return m[num_caixas-1][altura_maxima];
}

void imprimeCaixas(char *arquivo, Caixa *caixas)
{
    if (!arquivo)
    {
        fprintf (stderr, "Erro: argumento invalido.\n");
        exit(EXIT_FAILURE);
    }

    FILE *out = fopen(arquivo, "a+");

    if(!out)
    {
        fprintf(stderr,"Erro: nao foi possivel abrir o arquivo de caixas.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(out, "%12s ", "Largura");
    fprintf(out, "%12s ", "Altura");
    fprintf(out, "%12s ", "Profund.");
    fprintf(out, "%12s ", "Valor");
    fprintf(out, "%12s ", "Numero");
    fprintf(out, "%12s\n", "Rotacao");

    for(int i=0; i<num_caixas; i++)
    {
        fprintf(out, "%12d ", caixas[i].largura);
        fprintf(out, "%12d ", caixas[i].altura);
        fprintf(out, "%12d ", caixas[i].profundidade);
        fprintf(out, "%12d ", caixas[i].valor);
        fprintf(out, "%12d ", caixas[i].numero);
        fprintf(out, "%12d\n", caixas[i].rotacao);
    }
}


void escreveCaixas(char *arquivo, Caixa *caixas, int **soma)
{

    int valor = soma[num_caixas-1][altura_maxima];
    int i = num_caixas - 1;
    int j = altura_maxima;
    int total_caixas = 0;
    int tamanho_vetor = num_caixas;
    Caixa *resultado = malloc(sizeof(Caixa)*tamanho_vetor);


    while(valor>0 && j>0 && i>0)

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
            while(soma[i][j-1] <= soma[i-1][j] && i>1)
            {
                i--;
            }

            (soma[i][j-1] <= soma[i-1][j])?i--:i;
        }
        valor = soma[i][j];
    }

    printf("%d\n", soma[num_caixas-1][altura_maxima]);
    printf("%d\n", total_caixas);
    for(int i=total_caixas-1; i>=0; i--)
    {
        printf("%d ", resultado[i].largura);
        printf("%d ", resultado[i].altura);
        printf("%d ", resultado[i].profundidade);
        printf("%d ", resultado[i].valor);
        printf("%d ", resultado[i].numero);
        printf("%d\n", resultado[i].rotacao);
    }

    free(resultado);
    return;
}

void printCaixas(Caixa *caixas)
{
    printf("\n\n%s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t\n\n", "Indice", "Largura", "Altura",  "Profundidade", "Valor", "Rotacao", "Numero");
    for(int i=0; i<num_caixas; i++)
    {
        printf("i==%d\t%10d\t%10d\t%10d\t%10d\t%10d\t%10d\t", i, caixas[i].largura, caixas[i].altura, caixas[i].profundidade, caixas[i].valor, caixas[i].rotacao, caixas[i].numero );
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[] )
{
    if(argc != 3)
    {
        fprintf(stderr,"Argumentos necessarios: \"programa\" \"entrada\" \"saida\"");
        exit(EXIT_FAILURE);
    }
    Caixa *caixas = leCaixas(argv[1]);

    geraRotacao(num_caixas, caixas);
    num_caixas = 2 * num_caixas;

    char* nome = remover(argv[1]);
    nome = concatenar(nome, ".txt");

    qsort(caixas, num_caixas, sizeof(caixas[0]), compare);

     printCaixas(caixas);

    int **soma = preencheMatriz(num_caixas, altura_maxima + 1, 0);

    printf("Lucro: %d\n",empilhaCaixas(altura_maxima,caixas, soma));

    nome = remover(argv[1]);
    nome = concatenar(nome, ".csv");
    //gravaTabela(soma,nome);

    escreveCaixas(argv[2], caixas, soma);

    free(soma);
    free(caixas);

    return 0;
}
