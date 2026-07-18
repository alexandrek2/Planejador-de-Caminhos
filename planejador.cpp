#include <stdexcept>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <algorithm>
#include <list>
/* ACRESCENTE SE NECESSARIO */

#include "planejador.h"

using namespace std;

/* *************************
   * CLASSE IDPONTO        *
   ************************* */

/// Atribuicao de string
/// NAO DEVE SER MODIFICADA
void IDPonto::set(string&& S)
{
  t=move(S);
  if (!valid()) t.clear();
}

/* *************************
   * CLASSE IDROTA         *
   ************************* */

/// Atribuicao de string
/// NAO DEVE SER MODIFICADA
void IDRota::set(string&& S)
{
  t=move(S);
  if (!valid()) t.clear();
}

/* *************************
   * CLASSE PONTO          *
   ************************* */

/// Impressao em console
/// NAO DEVE SER MODIFICADA
ostream& operator<<(ostream& X, const Ponto& P)
{
  X << P.id << '\t' << P.nome << " (" <<P.latitude << ',' << P.longitude << ')';
  return X;
}

/// Distancia entre 2 pontos (formula de haversine)
/// NAO DEVE SER MODIFICADA
double Ponto::distancia(const Ponto& P) const
{
  // Gera excecao se pontos invalidos
  if (!valid() || !P.valid())
    throw invalid_argument("distancia: ponto(s) invalido(s)");

  // Tratar logo pontos identicos
  if (id == P.id) return 0.0;
  // Constantes
  static const double MY_PI = 3.14159265358979323846;
  static const double R_EARTH = 6371.0;
  // Conversao para radianos
  double lat1 = MY_PI*this->latitude/180.0;
  double lat2 = MY_PI*P.latitude/180.0;
  double lon1 = MY_PI*this->longitude/180.0;
  double lon2 = MY_PI*P.longitude/180.0;
  // Seno das diferencas
  double sin_dlat2 = sin((lat2-lat1)/2.0);
  double sin_dlon2 = sin((lon2-lon1)/2.0);
  // Quadrado do seno do angulo entre os pontos
  double sin2_ang = sin_dlat2*sin_dlat2 + cos(lat1)*cos(lat2)*sin_dlon2*sin_dlon2;
  // Em vez de utilizar a funcao arcosseno, asin(sqrt(sin2_ang)),
  // vou utilizar a funcao arcotangente, menos sensivel a erros numericos.
  // Distancia entre os pontos
  return 2.0*R_EARTH*atan2(sqrt(sin2_ang),sqrt(1-sin2_ang));
}

/* *************************
   * CLASSE ROTA           *
   ************************* */

/// Impressao em console
/// NAO DEVE SER MODIFICADA
ostream& operator<<(ostream& X, const Rota& R)
{
  X << R.id << '\t' << R.nome << '\t' << R.comprimento << "km"
    << " [" << R.extremidade[0] << ',' << R.extremidade[1] << ']';
  return X;
}

/// Retorna a outra extremidade da rota, a que nao eh o parametro.
/// Gera excecao se o parametro nao for uma das extremidades da rota.
/// NAO DEVE SER MODIFICADA
IDPonto Rota::outraExtremidade(const IDPonto& ID) const
{
  if (extremidade[0]==ID) return extremidade[1];
  if (extremidade[1]==ID) return extremidade[0];
  throw invalid_argument("outraExtremidade: invalid IDPonto parameter");
}

/* *************************
   * CLASSE PLANEJADOR     *
   ************************* */

/// Torna o mapa vazio
/// NAO DEVE SER MODIFICADA
void Planejador::clear()
{
  pontos.clear();
  rotas.clear();
}

/// Leh um mapa dos arquivos arq_pontos e arq_rotas.
/// Caso nao consiga ler dos arquivos, deixa o mapa inalterado e gera excecao ios_base::failure.
/// Deve receber ACRESCIMOS
void Planejador::ler(const std::string& arq_pontos,
                     const std::string& arq_rotas)
{



  // 1. Vetores temporários
  vector<Ponto> temp_pontos;
  vector<Rota> temp_rotas;


  string header;
  string s_temp;


  try
    {
        ifstream pontos(arq_pontos);
        if (!pontos.is_open())
            throw ios_base::failure("Erro 1 na leitura do arquivo de pontos" + arq_pontos);

        // 2) Leh e testa o cabecalho
        getline(pontos, header);
        if (pontos.fail() || header != "ID;Nome;Latitude;Longitude")
            throw ios_base::failure("Erro 2 na leitura do arquivo de pontos" + arq_pontos);

        pontos >> ws;

        while (!pontos.eof())
        {
            Ponto P_temp;

            // | 3.1) Leh a ID
            getline (pontos, s_temp, ';');
            if(pontos.fail() || s_temp.empty())
                throw ios_base::failure("Erro 3 na leitura do arquivo de pontos" + arq_pontos);
            P_temp.id.set(move(s_temp));

            // | 3.2) Leh o nome
            pontos >> ws;
            getline(pontos, P_temp.nome, ';' );
            if(pontos.fail() || P_temp.nome.empty())
                throw ios_base::failure("Erro 4 na leitura do arquivo de pontos" + arq_pontos);

            // | 3.3) Leh a latitude
            pontos >> P_temp.latitude;
            if (pontos.fail())
                throw ios_base::failure("Erro 5 na leitura do arquivo de pontos" + arq_pontos);


            pontos >> ws;
            getline(pontos, s_temp, ';');
            if(pontos.fail() || !s_temp.empty())
                throw ios_base::failure("Erro 6 na leitura do arquivo de pontos" + arq_pontos);

            pontos >> P_temp.longitude;
            if (pontos.fail())
                throw ios_base::failure("Erro 7 na leitura do arquivo de pontos" + arq_pontos);

            // | 3.5) Tirar os espaços
            pontos >> ws;

            // | 3.6) Testa se o Ponto... eh valido
            if (!P_temp.valid())
                throw ios_base::failure("Erro 8 na leitura do arquivo de pontos" + arq_pontos);

            // | 3.7) Testa se a ID já existe
            if (find(temp_pontos.begin(), temp_pontos.end(), P_temp) != temp_pontos.end())
                throw ios_base::failure("Erro 9 na leitura do arquivo de pontos" + arq_pontos);

            // | 3.8) Insere o Ponto lido
            temp_pontos.push_back(P_temp);
        }

        if(temp_pontos.empty())
            throw ios_base::failure("Erro 10 na leitura do arquivo de pontos" + arq_pontos);

        // 4) Fecha o arquivo de Pontos
        pontos.close();
    }
    catch (const ios_base::failure& e)
    {
        throw;
    }
    // --- FIM DA LEITURA DE PONTOS ---


    // --- INÍCIO DA LEITURA DE ROTAS ---
    ifstream rotas(arq_rotas);
    try
    {
        if (!rotas.is_open())
            throw ios_base::failure("Erro 1 na leitura do arquivo de rotas" + arq_rotas);

        // ### MUDANÇA AQUI ###
        // Tenta pular qualquer espaço em branco no início
        rotas >> ws;

        // Se, ao pular os espaços, chegamos ao FIM do arquivo,
        // significa que o arquivo estava vazio.
        if (rotas.eof())
        {
            // Lança o "Erro 1" como esperado
            throw ios_base::failure("Erro 1 na leitura do arquivo de rotas" + arq_rotas);
        }

        // 2) Leh e testa o cabecalho
        getline(rotas, header);

        if (rotas.fail() || header != "ID;Nome;Extremidade 1;Extremidade 2;Comprimento")
            throw ios_base::failure("Erro 2 na leitura do arquivo de rotas" + arq_rotas);

        rotas >> ws;

        // 3) Repita a leitura
        while(!rotas.eof())
        {
            Rota R_temp;
            Ponto ext0;
            Ponto ext1;

            // | 3.1) Leh a ID
            getline(rotas, s_temp, ';' );
            if(rotas.fail() || s_temp.empty())
                throw ios_base::failure("Erro 3 na leitura do arquivo de rotas" + arq_rotas);
            R_temp.id.set(move(s_temp));

            // | 3.2) leh o nome
            rotas >> ws;
            getline(rotas, R_temp.nome, ';' );
            if(rotas.fail() || R_temp.nome.empty())
                throw ios_base::failure("Erro 4 na leitura do arquivo de rotas" + arq_rotas);

            // | 3.3) leh a ID da extremidade[0]
            rotas >> ws;
            getline(rotas, s_temp, ';' );
            if(rotas.fail() || s_temp.empty())
                throw ios_base::failure("Erro 5 na leitura do arquivo de rotas" + arq_rotas);
            R_temp.extremidade[0].set(move(s_temp));

            // | 3.4) leh a ID da extremidade[1]
            rotas >> ws;
            getline(rotas, s_temp, ';' );
            if(rotas.fail() || s_temp.empty())
                throw ios_base::failure("Erro 6 na leitura do arquivo de rotas" + arq_rotas);
            R_temp.extremidade[1].set(move(s_temp));

            // | 3.5) Leh o comprimento )
            rotas >> R_temp.comprimento;
            if(rotas.fail())
                throw ios_base::failure("Erro 7 na leitura do arquivo de rotas" + arq_rotas);

            // | 3.6) Consome os separadores apos a Rota
            rotas >> ws;

            // | 3.7) Testa se a Rota com esses parametros lidos eh valida
            if(!R_temp.valid())
                throw ios_base::failure("Erro 8 na leitura do arquivo de rotas" + arq_rotas);

            // | 3.8) Testa que a Id da extremidade[0] corresponde a um ponto
            ext0.id = R_temp.extremidade[0];
          if(find(temp_pontos.begin(), temp_pontos.end(), R_temp.extremidade[0]) == temp_pontos.end())
            throw ios_base::failure("Erro 9 na leitura do arquivo de rotas" + arq_rotas);
            // | 3.9) Testa que a Id da extremidade[1] corresponde a um ponto
            ext1.id = R_temp.extremidade[1];
            if(find(temp_pontos.begin(), temp_pontos.end(), ext1) == temp_pontos.end())
                throw ios_base::failure("Erro 10 na leitura do arquivo de rotas" + arq_rotas);

            // | 3.10) Testa que nao existe Rota com a mesma ID
            if(find(temp_rotas.begin(), temp_rotas.end(), R_temp) != temp_rotas.end())
                throw ios_base::failure("Erro 11 na leitura do arquivo de rotas" + arq_rotas);

            // | 3.11) Insere a Rota lida no vetor temporario de Rotas
            temp_rotas.push_back(R_temp);
        };

        // 4) Se não foi lido nenhuma Rota, gera erro (codigo 12)
        if(temp_rotas.empty())
            throw ios_base::failure("Erro 12 na leitura do arquivo de rotas" + arq_rotas);

        // 5) Fecha o arquivo de Rotas
        rotas.close();
        // --- FIM DA LEITURA DE ROTAS ---
    }
    catch (const ios_base::failure& e)
    {
        throw;
    }

    this->pontos = std::move(temp_pontos);
    this->rotas = std::move(temp_rotas);
}

/// Retorna um Ponto do mapa, passando a id como parametro.
/// Se a id for inexistente, gera excecao.
/// Deve receber ACRESCIMOS

Ponto Planejador::getPonto(const IDPonto& Id) const
{
  // Procura um ponto que corresponde aa Id do parametro
  auto it = std::find(pontos.begin(), pontos.end(), Id);
  // Em caso de sucesso, retorna o ponto encontrado
  /* *********** /
  /  FALTA FAZER  /
  /  *********** */
  // O PDF manda testar se o iterador é diferente de .end()
  if (it != pontos.end())
  {
    // Se for diferente, encontramos. Retorna o Ponto (usando *it)
    return *it;
  }
  // Se nao encontrou, gera excecao
  throw invalid_argument("getPonto: invalid IDPonto parameter");
}

/// Retorna um Rota do mapa, passando a id como parametro.
/// Se a id for inexistente, gera excecao.
/// Deve receber ACRESCIMOS
Rota Planejador::getRota(const IDRota& Id) const
// Procura um ponto que corresponde aa Id do parametro
{

 auto it = std::find(rotas.begin(), rotas.end(), Id);
  // Em caso de sucesso, retorna o ponto encontrado
  /* *********** /
  /  FALTA FAZER  /
  /  *********** */
  // O PDF manda testar se o iterador é diferente de .end()
  if (it != rotas.end())
  {
    // Se for diferente, encontramos. Retorna o Ponto (usando *it)
    return *it;
  }
  // Se nao encontrou, gera excecao
  throw invalid_argument("getPonto: invalid IDPonto parameter");
}

/// *******************************************************************************
/// Calcula o caminho entre a origem e o destino do planejador usando o algoritmo A*
/// *******************************************************************************

/// Noh: os elementos dos conjuntos de busca do algoritmo A*.
/// Deve ser DECLARADA E IMPLEMENTADA inteiramente.
/* *************************
   * CLASSE NOH     *
   ************************* */
struct noh{
        IDPonto id_pt;
        IDRota id_rt;

        double g;
        double h;

        double f() const{
            return g + h;
        }

        bool operator<(const noh& aux) const{ return (this -> f() < aux.f()); }

        bool operator== (const noh& aux) const {return (this -> id_pt == aux.id_pt); }
        bool operator==(const IDPonto& ID) const { return id_pt == ID; }

    };


/// Calcula o caminho mais curto no mapa entre origem e destino, usando o algoritmo A*
/// Retorna o comprimento do caminho encontrado (<0 se nao existe caminho).
/// O parametro C retorna o caminho encontrado (vazio se nao existe caminho).
/// O parametro NumAberto retorna o numero de nos (>=0) em Aberto ao termino do algoritmo A*,
/// mesmo quando nao existe caminho.
/// O parametro NumFechado retorna o numero de nos (>=0) em Fechado ao termino do algoritmo A*,
/// mesmo quando nao existe caminho.
/// Em caso de parametros de entrada invalidos ou de erro no algoritmo, gera excecao.
/// Deve receber ACRESCIMOS.
double Planejador::calculaCaminho(const IDPonto& id_origem,
                                  const IDPonto& id_destino,
                                  Caminho& C, int& NumAberto, int& NumFechado)
{
  try
  {

    // Mapa vazio
    if (empty()) throw 1;

    Ponto pt_origem, pt_destino;
    // Calcula os pontos que correspondem a id_origem e id_destino.
    // Se algum nao existir, throw 2
    try
    {
      pt_origem = getPonto(id_origem);
      pt_destino = getPonto(id_destino);
    }
    catch(...)
    {
      throw 2;
    }

    /* *****************************  /
    /  IMPLEMENTACAO DO ALGORITMO A*  /
    /  ***************************** */
    noh current;
    current.id_pt = id_origem;
    current.id_rt = IDRota();

    current.g = (0.0);
    current.h = pt_origem.distancia(pt_destino);
    list <noh> opened;
    deque <noh> closed;
    opened.push_back(current);
    float compr;

    while(!opened.empty() && current.id_pt != id_destino){
        current = opened.front();
        opened.pop_front();
        closed.push_back(current);

        if(current.id_pt != id_destino)
        {
          auto itr = this->rotas.begin();
while (itr != this->rotas.end()) {
    // Agora usamos apenas find, comparando a rota com o IDPonto do noh atual
    itr = std::find(itr, this->rotas.end(), current.id_pt);

    if (itr != this->rotas.end()) {
        const Rota& outra_rota = *itr;

        noh suc;
        suc.id_pt = outra_rota.outraExtremidade(current.id_pt);
        suc.id_rt = outra_rota.id;
        suc.g = current.g + outra_rota.comprimento;
        suc.h = getPonto(suc.id_pt).distancia(pt_destino);

        bool eh_inedito = true;

        auto oldF = std::find(closed.begin(), closed.end(), suc);

        if(oldF != closed.end()){
            eh_inedito = false;
        }
        else{
            auto oldA = std::find(opened.begin(), opened.end(), suc);
            if (oldA != opened.end()){
                if(suc.f() < (*oldA).f()){
                    opened.erase(oldA);
                }
                else {
                    eh_inedito = false;
                }
            }
        }

        if(eh_inedito){
            auto big = std::upper_bound(opened.begin(), opened.end(), suc);
            opened.insert(big, suc);
        }

        ++itr; // Fundamental avançar o iterador para não criar loop infinito
    }
}

        }
    }
     NumAberto = opened.size();
     NumFechado = closed.size();



    // Substitua pelo caminho correto
    C = Caminho();


        if(current.id_pt != id_destino){
            compr=-1.0;
        }
        else{
            compr=current.g;
            while(current.id_rt.valid()){
                C.push(Trecho(current.id_rt, current.id_pt));
                Rota rota_ant = getRota(current.id_rt);

                IDPonto id_pt_ant = rota_ant.extremidade[0] != current.id_pt ? rota_ant.extremidade[0]:rota_ant.extremidade[1];

                noh j;
                j.id_pt = id_pt_ant;
                current = *find(closed.begin(), closed.end(), j);


            }
            C.push(Trecho(IDRota(),current.id_pt));

        }


    // Substitua pelo valor correto
    return compr;
  }
  catch(int i)
  {
    string msg_err = "Erro " + to_string(i) + " no calculo do caminho\n";
    throw invalid_argument(msg_err);
  }

  // Zera o caminho resultado em caso de erro
  C = Caminho();

  // Retorna -1 em caso de erro
  return -1.0;
}
