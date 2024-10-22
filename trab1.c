/* Gabriele Sequeira 2310202 3WC */
/* Giovana Malaia 2312080 3Wc */

#include "codifica.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *convertBin(unsigned int numero, int tamanho) {
  char *s = malloc(sizeof(char) *(tamanho +1)); // aloca memória para a string de tamanho + 1 para o caractere nulo
  if (s == NULL) {
    printf("Erro na alocação\n");
    exit(1);
  }

  for (int i = tamanho - 1; i >= 0; i--) {
    s[i] = (numero & 1) ? '1' : '0'; // converte o último bit de 'numero' em '1' ou '0' e o coloca em 's'
    numero >>= 1; // desloca 'numero' para a direita
  }

  s[tamanho] = '\0'; // adiciona o caractere nulo de terminação à string
  return s; // retorna a string convertida
}

void compacta(FILE *arqTexto, FILE *arqBin, struct compactadora *v) {
  char s[360]; // armazenar os códigos binários dos caracteres
  s[0] = '\0'; // inicializa como uma string vazia
  char c; // armazenar os caracteres lidos do arquivo de texto
  int contador = 7; // acompanhar o bit atual em 'conteudo'
  unsigned char conteudo = 0; // armazenar os bytes compactados

  while ((c = fgetc(arqTexto)) != EOF) { // loop até o final do arquivo de texto
    for (int i = 0; i < 32;i++) { // loop para buscar o código correspondente na tabela
      if (c == v[i].simbolo) { // se o caractere atual corresponder a um símbolo na tabela
        strcat(s, convertBin(v[i].codigo,v[i].tamanho)); // concatena o código binário correspondente em 's'
        break;
      }
    }
  }
  strcat(s, convertBin(v[31].codigo,v[31].tamanho)); //adiciona o código EOF ao final de 's'
  printf("%s\n", s);

  for (int i = 0; s[i] != '\0';i++) { // loop através de 's' até o caractere nulo
    conteudo |= (s[i] == '1') << contador; // define o bit em 'conteudo' de acordo com o caractere em 's'
    contador--;
    if (contador < 0) { // se todos os bits em 'conteudo' forem definidos
      fwrite(&conteudo, sizeof(unsigned char), 1,arqBin); // escreve o 'conteudo' no arquivo binário
      contador = 7;   // reinicia o contador
      conteudo = 0;   // reinicia 'conteudo'
    }
  }
  if (contador != 7) // se houver bits não escritos em 'conteudo'
    fwrite(&conteudo, sizeof(unsigned char), 1,arqBin); // escreve os bits restantes no arquivo binário
}

void descompacta(FILE *arqBin, FILE *arqTexto, struct compactadora *v) {
  char s[360]; // armazenar os códigos binários lidos do arquivo binário
  unsigned char conteudo; // armazenar os bytes lidos do arquivo binário
  int atual = 0; // posição atual em 's'

  while (fread(&conteudo, sizeof(unsigned char), 1, arqBin) > 0) {
    for (int i = 7; i >= 0; i--) { // loop para extrair os bits de 'conteudo'
      char unidade = (conteudo >> i) & 1; // extrai o bit atual de 'conteudo'
      s[atual++] = (unidade == 0) ? '0' : '1'; // armazena o bit como '0' ou '1' em 's'
    }
  }
  s[atual] = '\0'; // adiciona o caractere nulo de terminação à string 's'

  int j = 0; // indice atual em 's'
  while (1) { // loop para decodificar 's'
    int verifica = 0; // indica se um código foi encontrado na tabela
    for (int i = 0; i < 32; i++) {
      if (strncmp(&s[j], convertBin(v[i].codigo, v[i].tamanho), v[i].tamanho) ==
          0) { // se o símbolo for EOF
        if (v[i].simbolo == 0) {
          return;
        }
        fputc(v[i].simbolo,
              arqTexto); // escreve o símbolo correspondente no arquivo de texto
        j += v[i].tamanho; // atualiza o índice em 's'
        verifica = 1; // indica que um código foi encontrado
        break;
      }
    }
    if (!verifica) { // se nenhum código foi encontrado
      printf("Código inválido encontrado em %d\n", j);
      return;
    }
  }
}
