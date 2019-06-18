#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char* argv[]){


	if(strcmp(argv[1],"-a")==0 && (argv[2] == NULL || argv[3] == NULL))
	{
		write(0,"Insira a data e as horas corretamente\n",sizeof("Insira a data e as horas corretamente\n"));
		write(0,"\tDD-MM-AAAA HH:MM\n",sizeof("\tDD-MM-AAAA HH:MM\n"));
		return 0;
	}

	int EscreveFifo1;
	char Argumentos[100];
	strcpy(Argumentos, argv[1]);
	for(int i = 2; argv[i]!= NULL; i++)
	{
		strcat(Argumentos," ");
		strcat(Argumentos,argv[i]);
	}
	strcat(Argumentos,"\n");

	if(argc > 1)
	{EscreveFifo1 = open("fifo1",O_WRONLY);}
		else{
			write(0,"ERRO NOS Argumentos\n",sizeof("ERRO NOS Argumentos\n"));
			return -1;
		}

	if (EscreveFifo1 == -1) {
		write(0,"ERRO a abrir o pipe \n",sizeof("ERRO a abrir o pipe \n"));
		return -1;
	}

	write(EscreveFifo1,Argumentos,strlen(Argumentos));
	close(EscreveFifo1);

	int Valor2;
	char* Buff4[100];
	if(strcmp(argv[1],"-l")==0)
	{
		mkfifo("fifo2",0666);
		int LerFifo2 = open("fifo2",O_RDONLY);
		int ji = 1;

		while((Valor2 = read(LerFifo2,Buff4, sizeof Buff4)) > 0 )
		{
			Buff4[Valor2] = '\0';
			write(0,Buff4,Valor2);
			ji++;
		}

	}

	int Valor3;
	char* Buff5[1];
	char NovaLinha[1] = "\n";
	if(strcmp(argv[1],"-a")==0)
	{
		mkfifo("fifo2",0666);
		int LerFifo2 = open("fifo2",O_RDONLY);
		int x = 1;

		while((Valor3 = read(LerFifo2,Buff5, sizeof Buff5)) > 0 )
		{
			Buff5[Valor3] = '\0';
			write(0,Buff5,Valor3);
			write(0,NovaLinha,1);
			x++;
		}
	}
	return 0;
}
