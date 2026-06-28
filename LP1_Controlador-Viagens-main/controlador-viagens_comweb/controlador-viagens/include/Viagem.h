#ifndef VIAGEM_H
#define VIAGEM_H

#include <vector>
#include "Transporte.h"
#include "Passageiro.h"
#include "Cidade.h"

// Uma Viagem representa um trecho direto (origem -> destino) feito por um
// transporte levando passageiros. Viagens podem ser encadeadas pelo ponteiro
// 'proxima': numa rota A->X->Y->B, criamos 3 viagens ligadas. Somente quando
// a ULTIMA termina os passageiros/transporte sao movidos ao destino final.
class Viagem {
private:
    Transporte* transporte;
    std::vector<Passageiro*> passageiros;
    Cidade* origem;
    Cidade* destino;
    int distancia;          // km deste trecho
    Viagem* proxima;        // proximo trecho da rota (ou nullptr)

    double kmPercorridos;   // progresso dentro do trecho atual
    bool emAndamento;       // este trecho esta rodando agora?
    bool concluida;         // este trecho ja terminou?

    // estado do controle de descanso
    double kmDesdeDescanso; // km desde o ultimo descanso
    int horasDescansoRestante; // horas ainda paradas descansando

public:
    Viagem(Transporte* transporte, std::vector<Passageiro*> passageiros,
           Cidade* origem, Cidade* destino, int distancia);

    void iniciar();                 // marca como em andamento (ativa o trecho)
    bool avancarHoras(int horas);   // simula; retorna true se concluiu neste avanco

    Cidade* getOrigem() const;
    Cidade* getDestino() const;
    Transporte* getTransporte() const;
    const std::vector<Passageiro*>& getPassageiros() const;
    int getDistancia() const;

    bool isEmAndamento() const;
    bool isConcluida() const;

    void setProxima(Viagem* p);
    Viagem* getProxima() const;
};

#endif
