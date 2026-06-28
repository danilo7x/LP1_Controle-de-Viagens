#include "Trajeto.h"

Trajeto::Trajeto(Cidade* origem, Cidade* destino, char tipo, int distancia)
    : origem(origem), destino(destino), tipo(tipo), distancia(distancia) {}

Cidade* Trajeto::getOrigem() const { return origem; }
Cidade* Trajeto::getDestino() const { return destino; }
char Trajeto::getTipo() const { return tipo; }
int Trajeto::getDistancia() const { return distancia; }
