#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int altura_maxima;
int num_caixas;

typedef struct caixa
{
    int altura, largura, profundidade, valor, rotacao;
} Caixa;

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
    return ( (*(Caixa *)b).profundidade * (*(Caixa *)b).largura ) -
           ( (*(Caixa *)a).profundidade * (*(Caixa *)a).largura );
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
    printf("\n");

    for(int i=0; i<=num_caixas; i++)
    {
        for(int j=0; j<=altura_maxima; j++)
            fprintf(tabela, "%d, ", soma[i][j]);
        fprintf(tabela,"\n");
    }
    printf("\n");
}

void imprimeCaixas(Caixa *caixas)
{
    printf("\n\n%s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t\n\n", "Indice", "Largura", "Altura",  "Profundidade", "Valor", "Rotacao", "Area");
    for(int i=0; i<num_caixas; i++)
    {
        printf("i==%d\t%10d\t%10d\t%10d\t%10d\t%10d\t%10d\t", i, caixas[i].largura, caixas[i].altura, caixas[i].profundidade, caixas[i].valor, caixas[i].rotacao, caixas[i].largura*caixas[i].profundidade  );
        printf("\n");
    }
    printf("\n");
}

int max (int x, int y)
{
    return (x > y)? x : y;
}


void empilhaCaixas(Caixa *caixas, int soma[num_caixas+1][altura_maxima+1])
{
    Caixa atual = {INT_MAX, INT_MAX, INT_MAX, 0, 0};
    Caixa candidata;
    Caixa aux;

    printf("\n\n%11s\t%11s\t%11s\t%11s\t%11s\t%11s\t\n\n", "Largura", "Altura",  "Profundidade", "Valor", "Rotacao", "Area");
    for (int i = 0; i <= num_caixas; i++ )
    {
        for (int j = 0; j <= altura_maxima; j++ )
        {
            if(i==0||j==0)
                soma[i][j]=0;
            else if (caixas[i-1].altura <= j)
            {
                soma[i][j] =  max(caixas[i-1].valor + soma[i][j-caixas[i-1].altura],  soma[i-1][j]);
                if(soma[i][j]!=soma[i-1][j] || soma[i][j]!=soma[i][j-1] )
                {
                    if(soma[i][j-1]>soma[i-1][j])
                    {
                        candidata = caixas[i-1];
                    }
                    if(soma[i][j-1]<soma[i-1][j])
                    {
                        int k = i;
                        k--;
                        while(soma[k][j-1]<soma[k-1][j])
                        {
                            k--;
                        }

                        candidata = caixas[k-1];

                    }
                    aux = atual;
                    atual = candidata;
                }
                if(candidata.largura > atual.largura && candidata.profundidade > atual.profundidade)
                {
                    soma[i][j]=soma[i-1][j];
                    atual = aux;
                }
            }
            else
                soma[i][j] = soma[i-1][j];
        }
        printf("%11d\t%11d\t%11d\t%11d\t%11d\t%11d\t\n", atual.largura, atual.altura, atual.profundidade, atual.valor, atual.rotacao, atual.largura*atual.profundidade);
    }

    printf("\n");
}

void escreveCaixas(char* arquivo, Caixa *caixas, int soma[num_caixas+1][altura_maxima+1] )
{
    FILE *out = fopen(arquivo, "a+");


    if(out == NULL)
        printf("Erro, nao foi possivel abrir o arquivo\n");
    else
    {
        int valor = soma[num_caixas][altura_maxima];
        int i = num_caixas;
        int j = altura_maxima;
        int total_caixas = 0;
        int capacidade = num_caixas;
        Caixa candidata;
        Caixa *resultado = (Caixa*) malloc(sizeof(Caixa)*capacidade);

        fprintf(out, "%d\n", valor);

        while(valor!=0)
        {
            if(total_caixas>capacidade-1)
            {
                capacidade = 2*capacidade;
                resultado = (Caixa*) realloc(resultado,sizeof(Caixa)*capacidade);
            }
            if(soma[i][j-1]>soma[i-1][j])
            {
                candidata = caixas[i-1];
                resultado[total_caixas] = caixas[i-1];
                j=j-caixas[i-1].altura;
                valor=soma[i][j];
                total_caixas++;
            }
            else
            {
                while(soma[i][j-1]<=soma[i-1][j])
                {
                    i--;
                    valor = soma[i][j];
                }
            }
        }
        fprintf(out, "%d\n", total_caixas);
        for(int i=0; i<total_caixas; i++)
        {
            fprintf(out, "%d ", resultado[i].largura);
            fprintf(out, "%d ", resultado[i].altura);
            fprintf(out, "%d ", resultado[i].profundidade);
            fprintf(out, "%d ", resultado[i].valor);
            fprintf(out, "%d\n", resultado[i].rotacao);
        }
    }


    fclose(out);
    return;
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

        printf("\nValores Originais:\n");
        imprimeCaixas(caixas);

        geraRotacao(num_caixas, caixas);
        num_caixas=2*num_caixas;

        printf("\nAntes de ordenar:\n");
        imprimeCaixas(caixas);

        qsort (caixas, num_caixas, sizeof(caixas[0]), compare);

        printf("\nDepois de ordenar:\n");
        imprimeCaixas(caixas);

        int soma[num_caixas+1][altura_maxima+1];
        empilhaCaixas(caixas, soma);

        gravaTabela(soma);

        escreveCaixas(argv[2], caixas, soma);

        printf("Numero de caixas: %d\n", num_caixas/2);
        printf("Altura maxima: %d\n", altura_maxima);
        printf("Soma: %d", soma[num_caixas][altura_maxima]);

        free(caixas);

    }
    return 0;
}
