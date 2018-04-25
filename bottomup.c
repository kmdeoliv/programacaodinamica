#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

int altura_maxima;
int num_caixas;

typedef struct caixa
{
    int altura, largura, profundidade, valor, rotacao;
} Caixa;

int max (int x, int y)
{
    return (x > y)? x : y;
}

Caixa* leCaixas(char* arquivo)
{

    FILE *in;
    int i=0;
    int j=0;
    int k=0;
    int l=0;
    int valor;
    Caixa *caixas;

    in = fopen(arquivo, "r");

    if(in == NULL)
        printf("Erro, nao foi possivel abrir o arquivo\n");
    else
    {
        if((fscanf(in, "%d", &valor))!=EOF )
        {
            num_caixas=valor;
            i++;
        }
        caixas = (Caixa*) malloc(sizeof(Caixa)*2*num_caixas);

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
                    caixas[l].largura = valor;
                if(k%3==1)
                    caixas[l].altura = valor;
                if(k%3==2)
                    caixas[l].profundidade = valor;
                caixas[l].rotacao = 1;
                k++;
                if(k%3==0)
                    l++;
            }
            i++;
        }
    }
    return caixas;
}

void imprimeTabela(int soma[num_caixas+1][altura_maxima+1])
{
    printf("%2s ", "  ");
    for(int j=0; j<=altura_maxima; j++)
        printf("%3d ", j);
    printf("\n");

    printf("%2s ", "  ");
    for(int j=0; j<=altura_maxima; j++)
        printf("%s ", "___");
    printf("\n");

    for(int i=0; i<=num_caixas; i++)
    {
        printf("%2d|", i);
        for(int j=0; j<=altura_maxima; j++)
            printf("%3d ", soma[i][j]);
        printf("\n");
    }
    printf("\n");
}

void gravaTabela(int soma[num_caixas+1][altura_maxima+1])
{
    FILE *tabela = fopen("tabela.csv", "a+");

    for(int j=0; j<=altura_maxima; j++)
        fprintf(tabela, "%d,", j);
    fprintf(tabela,"\n");

    for(int i=0; i<=num_caixas; i++)
    {
        for(int j=0; j<=altura_maxima; j++)
            fprintf(tabela, "%d,", soma[i][j]);
        fprintf(tabela,"\n");
    }

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
}

int compare (const void *a, const void * b)
{
    return ( (*(Caixa *)b).profundidade  ) -
           ( (*(Caixa *)a).profundidade  );
}

int podeEmpilhar(Caixa a, Caixa b)
{

    return (a.profundidade>=b.profundidade && a.largura>=b.largura)?1:0;
}

int empilhaCaixas(int capacidade, Caixa *caixas)
{

    int dp[capacidade+1];
    Caixa filtro = {930, 930, 930, 0, 0};
    memset(dp, 0, sizeof dp);
    Caixa aux = {930, 930, 930, 0, 0};


    for (int i=0; i<=capacidade; i++)
    {
        for (int j=0; j<=930; j++)
        {
            for(int k=0; k<=930; k++)
            {
                for (int l=0; l<num_caixas; l++)
                {
                    if(caixas[l].altura <= i&&  caixas[l].profundidade<=j && caixas[l].largura <= k)
                    {
                            dp[i] = max(dp[i], dp[i-caixas[l].altura]+caixas[l].valor);

                    }
                }
            }
        }
    }

    return dp[capacidade];


}

int main(int argc, char *argv[] )
{

    if(argc != 3)
    {
        printf("Argumentos necessarios: \"entrada\" \"saida\"");
    }
    else
    {
        Caixa *caixas = leCaixas(argv[1]);

        geraRotacao(num_caixas, caixas);
        num_caixas=2*num_caixas;

        qsort(caixas, num_caixas, sizeof(caixas[0]), compare);


        int soma[altura_maxima];
        for(int i=0; i<altura_maxima; i++)
            soma[i]= -1;


        printf("%d\n",empilhaCaixas(altura_maxima,caixas));

        //gravaTabela(soma);

        //escreveCaixas(argv[2], caixas, soma);

        printf("Numero de caixas: %d\n", num_caixas/2);
        printf("Altura maxima: %d\n", altura_maxima);

        free(caixas);

    }
    return 0;
}

