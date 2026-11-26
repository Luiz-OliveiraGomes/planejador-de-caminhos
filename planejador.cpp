#include <stdexcept>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <list>
#include <cctype>
#include "planejador.h"

using namespace std;

/* *************************
   * CLASSE IDPONTO        *
   ************************* */

void IDPonto::set(string&& S)
{
  t = move(S);
  if (!valid()) t.clear();
}

/* *************************
   * CLASSE IDROTA         *
   ************************* */

void IDRota::set(string&& S)
{
  t = move(S);
  if (!valid()) t.clear();
}

/* *************************
   * CLASSE PONTO          *
   ************************* */

ostream& operator<<(ostream& X, const Ponto& P)
{
  X << P.id << '\t' << P.nome << " (" << P.latitude << ',' << P.longitude << ')';
  return X;
}

double Ponto::distancia(const Ponto& P) const
{
  if (!valid() || !P.valid())
    throw invalid_argument("distancia: ponto(s) invalido(s)");

  if (id == P.id) return 0.0;

  static const double MY_PI = 3.14159265358979323846;
  static const double R_EARTH = 6371.0;

  //converte graus para radianos
  double lat1 = MY_PI * latitude / 180.0;
  double lat2 = MY_PI * P.latitude / 180.0;
  double lon1 = MY_PI * longitude / 180.0;
  double lon2 = MY_PI * P.longitude / 180.0;

  double sin_dlat2 = sin((lat2-lat1)/2.0);
  double sin_dlon2 = sin((lon2-lon1)/2.0);

  //formula de haversine
  double a = sin_dlat2*sin_dlat2 +
             cos(lat1)*cos(lat2)*sin_dlon2*sin_dlon2;

  return 2.0 * R_EARTH * atan2(sqrt(a), sqrt(1.0-a));
}

/* *************************
   * CLASSE ROTA           *
   ************************* */

ostream& operator<<(ostream& X, const Rota& R)
{
  X << R.id << '\t' << R.nome << '\t' << R.comprimento << "km"
    << " [" << R.extremidade[0] << ',' << R.extremidade[1] << ']';
  return X;
}

IDPonto Rota::outraExtremidade(const IDPonto& ID) const
{
  if (extremidade[0] == ID) return extremidade[1];
  if (extremidade[1] == ID) return extremidade[0];

  throw invalid_argument("outraExtremidade: invalid IDPonto parameter");
}

/* *************************
   * CLASSE NOH            *
   ************************* */

Noh::Noh(): id_pt(), id_rt(), g(0.0), h(0.0) {}

Noh::Noh(const IDPonto& pt, const IDRota& rt, double custo_passado, double custo_futuro)
  : id_pt(pt), id_rt(rt), g(custo_passado), h(custo_futuro) {}

//custo total do algoritmo A*
double Noh::f() const
{
  return g + h;
}

bool Noh::operator==(const IDPonto& ID) const
{
  return id_pt == ID;
}

bool Noh::operator==(const Noh& N) const
{
  return id_pt == N.id_pt;
}

bool Noh::operator<(const Noh& N) const
{
  return f() < N.f();
}

/* *************************
   * CLASSE PLANEJADOR     *
   ************************* */

void Planejador::clear()
{
  pontos.clear();
  rotas.clear();
}

inline void trim(string& S)
{
  //remove espacos no final da string
  while (!S.empty() && isspace(S.back())) S.pop_back();
}

void Planejador::ler(const string& arq_pontos,
                     const string& arq_rotas)
{
  vector<Ponto> pontos_tmp;
  vector<Rota> rotas_tmp;

  try
  {
    ifstream in(arq_pontos);
    if (!in.is_open()) throw 1;

    string cabecalho;
    getline(in >> ws, cabecalho);
    trim(cabecalho);

    if (!in || cabecalho != "ID;Nome;Latitude;Longitude") throw 2;

    while (in >> ws && !in.eof())
    {
      Ponto P;
      string S;
      char sep;

      getline(in >> ws, S, ';');
      trim(S);
      if (!in || S.empty()) throw 3;
      P.id.set(move(S));

      getline(in >> ws, P.nome, ';');
      trim(P.nome);
      if (!in || P.nome.empty()) throw 4;

      in >> ws >> P.latitude;
      if (!in) throw 5;

      in >> ws >> sep;
      if (!in || sep != ';') throw 6;

      in >> ws >> P.longitude;
      if (!in) throw 7;

      if (!P.valid()) throw 8;
      //nao deixa pontos repetidos
      if (find(pontos_tmp.begin(), pontos_tmp.end(), P.id) != pontos_tmp.end())
        throw 9;

      pontos_tmp.push_back(P);
    }

    if (pontos_tmp.empty()) throw 10;
  }
  catch (int i)
  {
    string msg = "Erro " + to_string(i) +
                 " na leitura do arquivo de pontos " + arq_pontos;
    throw ios_base::failure(msg);
  }

  try
  {
    ifstream in(arq_rotas);
    if (!in.is_open()) throw 1;

    string cabecalho;
    getline(in >> ws, cabecalho);
    trim(cabecalho);

    if (!in || cabecalho != "ID;Nome;Extremidade 1;Extremidade 2;Comprimento")
      throw 2;

    while (in >> ws && !in.eof())
    {
      Rota R;
      string S;

      getline(in >> ws, S, ';');
      trim(S);
      if (!in || S.empty()) throw 3;
      R.id.set(move(S));

      getline(in >> ws, R.nome, ';');
      trim(R.nome);
      if (!in || R.nome.empty()) throw 4;

      getline(in >> ws, S, ';');
      trim(S);
      if (!in || S.empty()) throw 5;
      R.extremidade[0].set(move(S));

      getline(in >> ws, S, ';');
      trim(S);
      if (!in || S.empty()) throw 6;
      R.extremidade[1].set(move(S));

      in >> ws >> R.comprimento;
      if (!in) throw 7;

      if (!R.valid()) throw 8;

      if (find(pontos_tmp.begin(), pontos_tmp.end(), R.extremidade[0]) == pontos_tmp.end())
        throw 9;

      if (find(pontos_tmp.begin(), pontos_tmp.end(), R.extremidade[1]) == pontos_tmp.end())
        throw 10;
      //nao deixa rotas repetidas
      if (find(rotas_tmp.begin(), rotas_tmp.end(), R.id) != rotas_tmp.end())
        throw 11;

      rotas_tmp.push_back(R);
    }

    if (rotas_tmp.empty()) throw 12;
  }
  catch (int i)
  {
    string msg = "Erro " + to_string(i) +
                 " na leitura do arquivo de rotas " + arq_rotas;
    throw ios_base::failure(msg);
  }

  pontos = move(pontos_tmp);
  rotas = move(rotas_tmp);
}

Ponto Planejador::getPonto(const IDPonto& Id) const
{
  auto p = find(pontos.begin(), pontos.end(), Id);

  if (p != pontos.end())
    return *p;

  throw invalid_argument("getPonto: invalid IDPonto parameter");
}

Rota Planejador::getRota(const IDRota& Id) const
{
  auto r = find(rotas.begin(), rotas.end(), Id);

  if (r != rotas.end())
    return *r;

  throw invalid_argument("getRota: invalid IDRota parameter");
}

double Planejador::calculaCaminho(const IDPonto& id_origem,
                                  const IDPonto& id_destino,
                                  Caminho& C,
                                  int& NumAberto,
                                  int& NumFechado)
{
  try
  {
    if (empty()) throw 1;

    Ponto pt_origem;
    Ponto pt_destino;

    try
    {
      pt_origem = getPonto(id_origem);
      pt_destino = getPonto(id_destino);
    }
    catch (...)
    {
      throw 2;
    }

    list<Noh> Aberto;
    list<Noh> Fechado;
    //aberto guarda os nohs que ainda vao ser analisados
    //fechado guarda os nohs ja visitados
    Noh atual(id_origem, IDRota(), 0.0, pt_origem.distancia(pt_destino));

    Aberto.push_back(atual);

    do
    {
      atual = Aberto.front();
      Aberto.pop_front();

      Fechado.push_back(atual);

      if (atual.id_pt != id_destino)
      {
        auto rota_suc = rotas.begin();

        while (rota_suc != rotas.end())
        {
          //procura uma rota ligada ao ponto atual
          rota_suc = find_if(rota_suc, rotas.end(),
                             [atual](const Rota& R)
                             {
                               return R.extremidade[0] == atual.id_pt ||
                                      R.extremidade[1] == atual.id_pt;
                             });
          if (rota_suc != rotas.end())
          {
            Noh suc;

            suc.id_pt = rota_suc->outraExtremidade(atual.id_pt);
            suc.id_rt = rota_suc->id;
            suc.g = atual.g + rota_suc->comprimento;

            Ponto pt_suc = getPonto(suc.id_pt);
            //calcula a heuristica ate o destino
            suc.h = pt_suc.distancia(pt_destino);

            bool eh_inedito = true;

            auto oldF = find(Fechado.begin(), Fechado.end(), suc.id_pt);

            if (oldF != Fechado.end())
            {
              eh_inedito = false;
            }
            else
            {
              auto oldA = find(Aberto.begin(), Aberto.end(), suc.id_pt);

              if (oldA != Aberto.end())
              {
                if (suc.f() < oldA->f())
                {
                  Aberto.erase(oldA);
                }
                else
                {
                  eh_inedito = false;
                }
              }
            }

            if (eh_inedito)
            {
              auto big = upper_bound(Aberto.begin(), Aberto.end(), suc);
              Aberto.insert(big, suc);
            }

            ++rota_suc;
          }
        }
      }
    }
    while (!Aberto.empty() && atual.id_pt != id_destino);

    NumAberto = Aberto.size();
    NumFechado = Fechado.size();

    C = Caminho();

    if (atual.id_pt != id_destino)
      return -1.0;

    double Compr = atual.g;

    while (atual.id_rt.valid())
    {
      C.push(Trecho(atual.id_rt, atual.id_pt));

      Rota rota_ant = getRota(atual.id_rt);
      IDPonto id_pt_ant = rota_ant.outraExtremidade(atual.id_pt);

      auto ant = find(Fechado.begin(), Fechado.end(), id_pt_ant);

      if (ant == Fechado.end())
        throw 3;

      atual = *ant;
    }

    C.push(Trecho(IDRota(), atual.id_pt));

    return Compr;
  }
  catch (int i)
  {
    string msg_err = "Erro " + to_string(i) + " no calculo do caminho";
    throw invalid_argument(msg_err);
  }
}
