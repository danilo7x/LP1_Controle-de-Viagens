#ifndef PASSAGEIRO_H
#define PASSAGEIRO_H

#include <string>
#include "Cidade.h"

// Uma pessoa que viaja. Quando esta em transito, localAtual fica nullptr
// e a informacao de onde ele esta vem da Viagem em andamento.
class Passageiro {
private:
    std::string nome;
    Cidade* localAtual; // nullptr quando esta em transito

public:
    Passageiro(std::string nome, Cidade* localAtual);

    std::string getNome() const;
    Cidade* getLocalAtual() const;
    void setLocalAtual(Cidade* local);
};

#endif
