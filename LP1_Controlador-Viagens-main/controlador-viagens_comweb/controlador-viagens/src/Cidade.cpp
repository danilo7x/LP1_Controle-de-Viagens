#include "Cidade.h"

Cidade::Cidade(std::string nome) : nome(nome), vezesVisitada(0) {}

std::string Cidade::getNome() const { return nome; }

int Cidade::getVezesVisitada() const { return vezesVisitada; }

void Cidade::registrarVisita() { vezesVisitada++; }

void Cidade::setVezesVisitada(int v) { vezesVisitada = v; }
