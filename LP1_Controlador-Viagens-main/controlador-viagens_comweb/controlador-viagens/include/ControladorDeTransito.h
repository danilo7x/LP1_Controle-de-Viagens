#ifndef CONTROLADOR_DE_TRANSITO_H
#define CONTROLADOR_DE_TRANSITO_H

#include <string>
#include <vector>
#include "Cidade.h"
#include "Trajeto.h"
#include "Transporte.h"
#include "Passageiro.h"
#include "Viagem.h"

// Classe central. Guarda todas as entidades e orquestra cadastros,
// roteamento (Dijkstra), simulacao do tempo e relatorios.
class ControladorDeTransito {
private:
    std::vector<Cidade*> cidades;
    std::vector<Trajeto*> trajetos;
    std::vector<Transporte*> transportes;
    std::vector<Passageiro*> passageiros;
    std::vector<Viagem*> viagens; // guarda as viagens-cabeca (inicio de cada rota)

    // helpers de busca por nome
    Cidade* buscarCidade(const std::string& nome) const;
    Transporte* buscarTransporte(const std::string& nome) const;
    Passageiro* buscarPassageiro(const std::string& nome) const;

    // Dijkstra: menor distancia total. Retorna a sequencia de cidades do
    // caminho (incluindo origem e destino) ou vetor vazio se nao houver rota.
    // 'tipoTransporte' restringe as arestas usaveis ('A' ou 'T').
    std::vector<Cidade*> melhorTrajeto(Cidade* origem, Cidade* destino,
                                       char tipoTransporte) const;

    // distancia direta entre duas cidades adjacentes para um tipo dado (-1 se nao houver)
    int distanciaDireta(Cidade* a, Cidade* b, char tipo) const;

public:
    ~ControladorDeTransito();

    // ---- Cadastros ----
    bool cadastrarCidade(const std::string& nome);
    bool cadastrarTrajeto(const std::string& nomeOrigem, const std::string& nomeDestino,
                          char tipo, int distancia);
    bool cadastrarTransporte(const std::string& nome, char tipo, int capacidade,
                             int velocidade, int distanciaEntreDescansos,
                             int tempoDeDescanso, const std::string& localAtual);
    bool cadastrarPassageiro(const std::string& nome, const std::string& localAtual);

    // ---- Viagens ----
    bool iniciarViagem(const std::string& nomeTransporte,
                       const std::vector<std::string>& nomesPassageiros,
                       const std::string& nomeOrigem, const std::string& nomeDestino);
    void avancarHoras(int horas);

    // ---- Relatorios ----
    void relatarPassageiros() const;
    void relatarTransportes() const;
    void relatarViagensEmAndamento() const;
    void relatarCidadesMaisVisitadas() const;
    void listarCidades() const;
    void listarTrajetos() const;

    // ---- Persistencia ----
    void salvar(const std::string& pasta) const;
    void carregar(const std::string& pasta);

    // acesso para o menu
    const std::vector<Cidade*>& getCidades() const { return cidades; }

    // ---- Suporte ao dashboard web ----
    // Gera todo o estado do sistema em JSON (string) para o front-end consumir.
    std::string estadoJson() const;
};

#endif
