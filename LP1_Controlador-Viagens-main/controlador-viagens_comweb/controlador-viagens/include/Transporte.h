#ifndef TRANSPORTE_H
#define TRANSPORTE_H

#include <string>
#include "Cidade.h"

// Um meio de transporte (barco, onibus, etc).
// Tem um tipo que precisa casar com o tipo do trajeto (Aquatico so anda
// em trajeto Aquatico, e idem para Terrestre).
class Transporte {
private:
    std::string nome;
    char tipo;                     // 'A' = Aquatico, 'T' = Terrestre
    int capacidade;                // numero maximo de passageiros
    int velocidade;                // km/h
    int distanciaEntreDescansos;   // km percorridos antes de precisar descansar
    int tempoDeDescanso;           // horas paradas a cada descanso
    Cidade* localAtual;            // nullptr quando esta em transito

public:
    Transporte(std::string nome, char tipo, int capacidade, int velocidade,
               int distanciaEntreDescansos, int tempoDeDescanso, Cidade* localAtual);

    std::string getNome() const;
    char getTipo() const;
    int getCapacidade() const;
    int getVelocidade() const;
    int getDistanciaEntreDescansos() const;
    int getTempoDescanso() const;
    Cidade* getLocalAtual() const;
    void setLocalAtual(Cidade* local);
};

#endif
