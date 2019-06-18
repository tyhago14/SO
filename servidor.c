#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define SIZE 1000
#define SIZE2 100

char* Argumentos3[100][15];
char NovaLinha[1] = "\n";
char NovoEspaco[1] = " ";
int comp;
int chamada ;

void listar(char nomeficheiro[]);
int lerficheiro(char nomeficheiro[],void *buf);
int CarregaStruct();
void cancelarEvento();

void executar();
void executado();
void porexecutar();
void limparstruct();

int compararDataHora();

int main(int argc, char* argv[]){

	if(argc < 2){
		write(0,"ERRO NOS Argumentos\n",sizeof("ERRO NOS Argumentos\n"));
		return -1;
	}

	mkfifo("fifo1",0666);
	int LerFifo1 = open("fifo1",O_RDONLY);
	int ficheiro = open(argv[1],O_WRONLY|O_CREAT|O_APPEND,0666);
	char buf;

	if(LerFifo1 == -1){
		write(0,"ERRO ao abrir o fifo\n",sizeof("ERRO ao abrir o fifo\n"));
		return -1;
	}
	if(ficheiro == -1){
		write(0,"ERRO ao abrir o ficheiro\n",sizeof("ERRO ao abrir o ficheiro\n"));
		return -1;
	}

	int NRlinha = CarregaStruct(argv[1]);
	int empty = 1;


while (1) {

	if(NRlinha > 1)
	{	comp = compararDataHora();}

	if (empty == 0 && comp != -1)
	{
		executar(argv[1],comp);
		sleep(1);
		CarregaStruct(argv[1]);
		chamada = 1	;
	}

	int Valor1;
	char Buff2[50];
	char Buff3[50];
	char Buffcopia[52];
	char NovaLinha[1] = "\n";
	char NovoEspaco[1] = " ";
	while((Valor1 = read(LerFifo1,Buff2, sizeof Buff2 - 1)) > 0 )
	{
		Buff2[Valor1 - 1] = '\0';
		strncpy(Buff3,Buff2,50);

		char* token;
		char* rest = Buff3;
		int i = 0;
		char* Argumentos[15];
		while ((token = strtok_r(rest, " ", &rest)))
			 {
				 Argumentos[i] = token;
				 i++;
				}

		int NumArg = i;
		int erroArg = 1;

		if(strcmp(Argumentos[0],"-a")==0){
			write(0,"Adiciona eventos\n",sizeof("Adiciona eventos\n"));
			empty = 0;
			erroArg = 0;
			int ficheiro = open(argv[1],O_WRONLY|O_APPEND,0666);
			int a = 1;
			char nlinha[6];
			sprintf(nlinha,"%d-> ",NRlinha);
			char nlinha2[6];
			sprintf(nlinha2,"%d ",NRlinha);

			int EscreveFifo2;
			EscreveFifo2 = open("fifo2",O_WRONLY);
			write(EscreveFifo2,nlinha2,strlen(nlinha2));
			close(EscreveFifo2);

			write(ficheiro,nlinha,strlen(nlinha));
			for(a = 1; a < NumArg; a++)
			{
				write(ficheiro,Argumentos[a],strlen(Argumentos[a]));
				write(ficheiro,NovoEspaco,1);
			}
			write(ficheiro,NovaLinha,1);
			NRlinha++;
			CarregaStruct(argv[1]);
		}

		if(strcmp(Argumentos[0],"-l")==0){
			erroArg = 0;
			write(0,"Lista eventos\n",sizeof("Lista eventos\n"));
			listar(argv[1]);
			CarregaStruct(argv[1]);
		}

		if(strcmp(Argumentos[0],"-c")==0)
		{
			int nbytes;
			char buf2[10];
			int i = 0;
			char *ptr;
			long ret;
			erroArg = 0;
			write(0,"Cancela eventos\n",sizeof("Cancela eventos\n"));
			ret = strtol(Argumentos[1], &ptr, 10);
			int send = ret;
			cancelarEvento(argv[1],send);
			CarregaStruct(argv[1]);
		}

		if(erroArg == 1){
			write(0,"O Argumento não é valido\n",sizeof("O Argumento não é valido\n"));
		}
	}
}
	close(LerFifo1);
	close(ficheiro);
	return 0;
}

void cancelarEvento(char nomeficheiro[], int id)
{
	int Rficheiro = open(nomeficheiro,O_RDONLY,0666);
	int ficheiro;
	char nid[6];
	sprintf(nid,"%d->",id);
	int x = 0;
	int apagado = 0;

  int j = 0;

		while(Argumentos3[x][0] != NULL)
		{
			if(strcmp(Argumentos3[x][0],nid)==0)
			{write(0,"Existe esse evento\n",sizeof("Existe esse evento\n"));

				for(int i = 0;Argumentos3[i][0] != NULL;i++)
				{
					if(strcmp(Argumentos3[i][0],nid)!=0)
					{

						for(j = 0;Argumentos3[i][j] != NULL;j++)
						{
							ficheiro = open("aux.txt",O_WRONLY|O_CREAT|O_APPEND,0666);
							write(ficheiro,Argumentos3[i][j],strlen(Argumentos3[i][j]));
							write(ficheiro,NovoEspaco,strlen(NovoEspaco));
						}
						write(ficheiro,"\n",1);
						j = 0;

					}

					else
					{
						for(j = 0;Argumentos3[i][j] != NULL;j++)
						{
							Argumentos3[i][j] = NULL;
						}

						j = 0;
						apagado = 1;

					}
				}
			}

			x++;
		}

		if(apagado == 0)
  		{
			write(0,"Não existe esse evento \n",sizeof("Não existe esse evento \n"));

 		}
 		else
 		{
		remove(nomeficheiro);
		rename("aux.txt",nomeficheiro);
		remove("aux.txt" );
		limparstruct();
		}
}


void listar(char nomeficheiro[])
{
	char *buf = malloc(SIZE*sizeof(char));
	char *bufE = malloc(SIZE*sizeof(char));
	int EscreveFifo2;
	EscreveFifo2 = open("fifo2",O_WRONLY);
	if (EscreveFifo2 == -1)
	{
		write(0,"ERRO a abrir o fifo2 \n",sizeof("ERRO a abrir o fifo2 \n"));
	}

	char exemplo[30] = "Sem programas agendados\n";
	char agendadas[30] = "    AGENDADAS   \n";
	char executadas[30] = "    EXECUTADAS    \n";
	char exemplo2[30] = "Sem programas já executados\n";


	if(Argumentos3[0][0] == NULL)
	{
		write(EscreveFifo2,agendadas,strlen(agendadas));
		write(EscreveFifo2,exemplo,strlen(exemplo));
	}

	else
	{
			write(EscreveFifo2,agendadas,strlen(agendadas));
			int i = 0;
			int j = 0;
			for(int i = 0;Argumentos3[i][j] != NULL;i++)
			{
				for(int j = 0;Argumentos3[i][j] != NULL;j++)
				{
					write(EscreveFifo2,Argumentos3[i][j],strlen(Argumentos3[i][j]));
					write(EscreveFifo2,NovoEspaco,1);
				}
				write(EscreveFifo2,NovaLinha,1);
				j = 0;
			}
		}

			int e = lerficheiro("executados.txt",bufE);
			if( e != -1)
			{
				write(EscreveFifo2,executadas,strlen(executadas));
				write(EscreveFifo2,bufE,strlen(bufE));
			}
			else
			{
				write(EscreveFifo2,executadas,strlen(executadas));
				write(EscreveFifo2,exemplo2,strlen(exemplo2));
			}
			close(EscreveFifo2);

}


int lerficheiro(char nomeficheiro[],void*buf)
{
	int fildes = open(nomeficheiro,O_RDONLY,0666);
	if(fildes == -1)
	{
		return -1;
	}

	int readbytes = 0;
	int i = 0;
	int j = 0;
	int ii = 0;
	char *buf2 = (char*)buf;
	char buf3[SIZE];
	while((i = read(fildes,buf2,1))!=0)
	{
				buf3[j] = *buf2;
				buf2 += 1;
				j++;
	}
	char buf4[j];
	strncpy(buf4,buf3, j);
	memset(buf4,0,j);
	memset(buf4,0,SIZE);
	return 1;
}

int CarregaStruct(char nomeficheiro[]){

		memset(Argumentos3, 0, sizeof(Argumentos3[0][0]) * 10 * 10);
		char *buf = malloc(SIZE*sizeof(char));
		lerficheiro(nomeficheiro,buf);
		char* token;
		char* token2;
		char* rest = buf;
		int i = 0;
		int j = 0;
		char* Argumentos2[100];
		while ((token = strtok_r(rest, "\n", &rest)))
			 {
				   Argumentos2[i] = token;
				 		char* rest2 = Argumentos2[i];
						 while((token2 = strtok_r(rest2, " ", &rest2)))
						 {
							 Argumentos3[i][j]=token2;
							 j++;
						 }
				 j=0;
				 i++;
				}
			if(Argumentos3[0][0] != NULL)
			{
				write(0,"Dados da Estrutura Carregados\n",sizeof("Dados da Estrutura Carregados\n"));
			}
			else
			{
				write(0,"Ficheiro sem dados\n",sizeof("Ficheiro sem dados\n"));
			}
	return i+1;
}

void executar(char nomeficheiro[], int i)
{

		int c = 0;
		wait(0);

		if(fork() == 0)
		{
			char* Argumentos4[10];
			for(int j = 0;Argumentos3[i][j] != NULL;j++)
			{
				Argumentos4[j] = Argumentos3[i][j+3];
				Argumentos4[j] = Argumentos3[i][j+3];
			}
				executado(nomeficheiro,i);
				i=0;
				execvp(Argumentos4[0],Argumentos4);
				wait(0);
				_exit(0);
		}
		wait(&c);
}


void executado(char nomeficheiro[], int numerolinha)
{
	int ficheiro;
	int x = 0;
	int j = 0;
	write(0,"Executado\n",sizeof("Executado\n"));
	for(j = 0;Argumentos3[numerolinha][j] != NULL;j++)
	{
		ficheiro = open("executados.txt",O_WRONLY|O_CREAT|O_APPEND,0666);
		write(ficheiro,Argumentos3[numerolinha][j],strlen(Argumentos3[numerolinha][j]));
		write(ficheiro,NovoEspaco,strlen(NovoEspaco));
	}
	write(ficheiro,"\n",1);
	write(0,"\n",1);

	char nid[6];
	sprintf(nid, "%s",Argumentos3[numerolinha][0]);
	char *ptr;
	long ret;
	ret = strtol(nid, &ptr, 10);
	int id = ret;

	if (chamada != id)
	{
		cancelarEvento(nomeficheiro,id);
		chamada = id;
	}

}


void limparstruct()
{
	int i = 0;
	int j = 0;

	for(int i = 0;i != 100;i++)
	{
		for(int j = 0;j != 15;j++)
		{
			Argumentos3[i][j] = NULL;
		}
		j = 0;
	}
}

int compararDataHora()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char data[10];
	char hora[10];
	char *ptr;
	int ano,mes,dia,hr,min;
	int difano, difmes, difdia, difhr, difmin;

	for( int x = 0; Argumentos3[x][0] != NULL; x++ )
	{
		dia = strtol(Argumentos3[x][1], &ptr, 10);
		mes = strtol(ptr, &ptr, 10)*-1;
		ano = strtol(ptr, &ptr, 10)*-1;
		hr = strtol(Argumentos3[x][2], &ptr, 10);
		min = strtol(ptr+1, &ptr, 10);

		if(Argumentos3[x][1] == NULL || Argumentos3[x][2] == NULL)
		{
			 return -1;
		}

		if(dia == tm.tm_mday && mes == tm.tm_mon+1 && ano == tm.tm_year+1900 && hr == tm.tm_hour && min == tm.tm_min)

		{
			write(0,"Chegou a hora para executar o programa\n",sizeof("Chegou a hora para executar o programa\n"));
			return x;
		}
	}

	return -1;
}
