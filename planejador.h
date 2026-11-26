#ifndef _PLANEJADOR_H_
#define _PLANEJADOR_H_

#include <string>
#include <iostream>  /* istream, ostream */
#include <utility>   /* pair */
#include <stack>
#include <vector>
#include <list>      /* para os conjuntos aberto e fechado */

/* *************************
   * CLASSE IDPONTO        *
   ************************* */

///Identificador de um Ponto
///NAO DEVE SER MOFICADA
class IDPonto
{
private:
  std::string t;
public:
  ///Construtor default
  IDPonto(): t("") {}

  ///Atribuicao de string
  void set(std::string&& S);

  ///Teste de validade
  bool valid() const
  {
    return (t.size()>=2 && t[0]=='#');
  }

  ///Operadores de comparacao
  bool operator==(const IDPonto& ID) const
  {
    return t==ID.t;
  }

  bool operator!=(const IDPonto& ID) const
  {
    return !operator==(ID);
  }

  ///Impressao
  friend std::ostream& operator<<(std::ostream& X, const IDPonto& ID)
  {
    return X<<ID.t;
  }
};

/* *************************
   * CLASSE IDROTA         *
   ************************* */

///Identificador de uma Rota
///NAO DEVE SER MOFICADA
class IDRota
{
private:
  std::string t;
public:
  ///Construtor default
  IDRota(): t("") {}

  ///Atribuicao de string
  void set(std::string&& S);

  ///Teste de validade
  bool valid() const
  {
    return (t.size()>=2 && t[0]=='&');
  }

  ///Operadores de comparacao
  bool operator==(const IDRota& ID) const
  {
    return t==ID.t;
  }

  bool operator!=(const IDRota& ID) const
  {
    return !operator==(ID);
  }

  ///Impressao
  friend std::ostream& operator<<(std::ostream& X, const IDRota& ID)
  {
    return X<<ID.t;
  }
};

/* *************************
   * CLASSE PONTO          *
   ************************* */

///Um ponto no mapa
///Pode receber ACRESCIMOS
struct Ponto
{
  IDPonto id;
  std::string nome;
  double latitude;
  double longitude;

  ///Construtor default
  Ponto(): id(), nome(""), latitude(0.0), longitude(0.0) {}

  ///Teste de validade
  bool valid() const
  {
    return id.valid() && nome.size()>=2 &&
           latitude>=-90.0 && latitude<=90.00 &&
           longitude>=-180.0 && longitude<=180.0;
  }

  ///Impressao
  friend std::ostream& operator<<(std::ostream& X, const Ponto& P);

  ///Operadores de comparacao
  bool operator==(const IDPonto& ID) const
  {
    return id == ID;
  }

  bool operator==(const Ponto& P) const
  {
    return id == P.id;
  }

  ///Distancia entre dois pontos
  double distancia(const Ponto& P) const;
};

/* *************************
   * CLASSE ROTA           *
   ************************* */

///Uma rota no mapa
///Pode receber ACRESCIMOS
struct Rota
{
  IDRota id;
  std::string nome;
  IDPonto extremidade[2];
  double comprimento;

  ///Construtor default
  Rota(): id(), nome(""), extremidade(), comprimento(0.0) {}

  ///Teste de validade
  bool valid() const
  {
    return id.valid() && nome.size()>=2 && comprimento>0.0 &&
           extremidade[0].valid() && extremidade[1].valid();
  }

  ///Impressao
  friend std::ostream& operator<<(std::ostream& X, const Rota& R);

  ///Operadores de comparacao
  bool operator==(const IDRota& ID) const
  {
    return id == ID;
  }

  bool operator==(const Rota& R) const
  {
    return id == R.id;
  }

  ///Outra extremidade
  IDPonto outraExtremidade(const IDPonto& ID) const;
};

/* *************************
   * CLASSE CAMINHO        *
   ************************* */

///Um caminho encontrado entre dois pontos
///NAO DEVEM SER MOFICADAS
using Trecho = std::pair<IDRota,IDPonto>;
using Caminho = std::stack<Trecho>;

/* *************************
   * CLASSE NOH            *
   ************************* */

///Noh usado no algoritmo A*
///deve ser declarada e implementada
struct Noh
{
  IDPonto id_pt;
  IDRota id_rt;
  double g;
  double h;

  ///construtor default
  Noh();

  ///construtor com parametros
  Noh(const IDPonto& pt, const IDRota& rt, double custo_passado, double custo_futuro);

  ///custo total
  double f() const;

  ///operadores de comparacao (necessarios para os algoritmos stl)
  bool operator==(const IDPonto& ID) const;
  bool operator==(const Noh& N) const;
  bool operator<(const Noh& N) const;
};

/* *************************
   * CLASSE PLANEJADOR     *
   ************************* */

///Classe do planejador
///NAO DEVE SER MOFICADA
class Planejador
{
private:
  std::vector<Ponto> pontos;
  std::vector<Rota> rotas;

public:
  ///Construtor default
  Planejador(): pontos(), rotas() {}

  ///Construtor com arquivos
  Planejador(const std::string& arq_pontos,
             const std::string& arq_rotas): Planejador()
  {
    ler(arq_pontos,arq_rotas);
  }

  ///Destrutor
  ~Planejador()
  {
    clear();
  }

  ///Limpa o mapa
  void clear();

  ///Testa se o mapa esta vazio
  bool empty() const
  {
    return pontos.empty();
  }

  ///Leitura do mapa
  void ler(const std::string& arq_pontos,
           const std::string& arq_rotas);

  ///Consulta dimensoes do mapa
  size_t getNumPontos() const {return pontos.size();}
  size_t getNumRotas() const {return rotas.size();}

  ///Consulta ponto por indice
  Ponto getPonto(int I) const  {return pontos.at(I);}

  ///Consulta rota por indice
  Rota getRota(int I) const {return rotas.at(I);}

  ///Consulta ponto por id
  Ponto getPonto(const IDPonto& Id) const;

  ///Consulta rota por id
  Rota getRota(const IDRota& Id) const;

  ///Calcula caminho
  double calculaCaminho(const IDPonto& id_origem,
                        const IDPonto& id_destino,
                        Caminho& C, int& NumAberto, int& NumFechado);
};

#endif // _PLANEJADOR_H_
