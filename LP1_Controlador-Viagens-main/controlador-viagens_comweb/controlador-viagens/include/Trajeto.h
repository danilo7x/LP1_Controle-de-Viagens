#ifndef TRAJETO_H
#define TRAJETO_H

#include "Cidade.h"

// Uma aresta do grafo: liga uma cidade de origem a uma de destino,
// com um tipo (Aquatico/Terrestre) e uma distancia em km.
// Tratamos o trajeto como bidirecional na hora de rotear.
class Trajeto {
private:
    Cidade* origem;
    Cidade* destino;
    char tipo;     // 'A' = Aquatico, 'T' = Terrestre
    int distancia; // km

public:
    Trajeto(Cidade* origem, Cidade* destino, char tipo, int distancia);

    Cidade* getOrigem() const;
    Cidade* getDestino() const;
    char getTipo() const;
    int getDistancia() const;
};

#endif
