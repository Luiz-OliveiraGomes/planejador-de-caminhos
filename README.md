# Planejador de Caminhos

**C++17 · Algoritmo A* · Grafos**

Aplicação de terminal em C++ que procura o caminho mais curto entre dois pontos de um mapa com o algoritmo A*. O programa lê os pontos e as rotas de arquivos de texto e usa a distância geográfica até o destino como heurística.

<p align="center">
  <img src="docs/img/trecho-br101.png" alt="Trecho de 7,8 km da BR-101 ligando os pontos 1 e 1A nos dados do projeto" width="880">
</p>

Mapa esquemático baseado em [pontos.txt](pontos.txt) e [rotas.txt](rotas.txt). O trecho destacado liga os dois pontos do exemplo de execução.

## Funcionalidades

- `pontos.txt` contém identificadores, nomes e coordenadas.
- `rotas.txt` descreve as ligações e o comprimento de cada trecho.
- A saída lista o trajeto, a distância total, o tempo de busca e as contagens de nós abertos e fechados.

## Como executar

Compile e execute na raiz do repositório, onde estão os arquivos do mapa.

```sh
g++ -std=c++17 planejador.cpp planejador-main.cpp -o planejador
./planejador
```

No Windows, execute `.\planejador.exe`. Escolha a opção `3` e informe, por exemplo, os identificadores `#1` e `#1A` para calcular um caminho entre o Viaduto de Ponta Negra e o Viaduto da Urbana, em Natal.

## Créditos

Projeto de C++ da UFRN proposto pelo professor Adelardo Adelino Dantas de Medeiros. O programa principal e os dados do mapa fazem parte do material do exercício.
