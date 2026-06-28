#ifndef CIDADE_H
#define CIDADE_H

#include <string>

// Representa uma cidade do mapa. Os trajetos ligam cidades entre si,
// formando o "grafo" sobre o qual as viagens acontecem.
class Cidade {
private:
    std::string nome;
    int vezesVisitada; // usado no relatorio de cidades mais visitadas

public:
    Cidade(std::string nome);

    std::string getNome() const;

    int getVezesVisitada() const;
    void registrarVisita();              // incrementa o contador
    void setVezesVisitada(int v);        // usado ao carregar de arquivo
};

#endif
