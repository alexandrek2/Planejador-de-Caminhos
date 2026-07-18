\# Planejador de Rotas em C++ (Algoritmo A\*)



Programa em C++ que calcula o \*\*caminho mais curto entre dois pontos\*\* em um

mapa de rodovias, usando o algoritmo de busca \*\*A\\\*\*\* (A-estrela), desenvolvido

na disciplina de Programação Avançada.



O mapa é montado a partir de dados reais de rodovias do Rio Grande do Norte

(pontos como Natal, Mossoró, etc.), lidos de arquivos de texto.



\## Funcionalidades



\- Leitura de pontos e rotas a partir de arquivos `.txt`

\- Cálculo de distância entre coordenadas geográficas (fórmula de Haversine)

\- Cálculo do caminho mais curto entre dois pontos com o algoritmo A\*

\- Consulta de pontos e rotas cadastrados no mapa

\- Tratamento de exceções para arquivos ou parâmetros inválidos

\- Menu interativo via console



\## Estrutura do projeto



```

planejador.h        // Declaração das classes (IDPonto, IDRota, Ponto, Rota, Planejador)

planejador.cpp       // Implementação do Planejador e do algoritmo A\*

planejador-main.cpp  // Programa de demonstração (menu interativo)

teste.cpp            // Testes do projeto

pontos.txt            // Base de dados de pontos (id;nome;latitude;longitude)

rotas.txt             // Base de dados de rotas (id;nome;extremidade1;extremidade2;comprimento)

```



\## Como compilar



```bash

g++ -std=c++17 -o planejador planejador-main.cpp planejador.cpp

./planejador

```



> Os arquivos `pontos.txt` e `rotas.txt` precisam estar na mesma pasta do executável.



\## Conceitos de C++ e algoritmos aplicados



\- Algoritmo de busca heurística A\* (A-estrela)

\- Estruturas de dados: `stack`, `vector`, `pair`

\- Sobrecarga de operadores (`==`, `<<`)

\- Leitura e parsing de arquivos de texto

\- Tratamento de exceções (`try`/`catch`)

\- Cálculo de distância geográfica (fórmula de Haversine)



\## Autor



Alexandre Kennedy Rodrigues da Fonseca

