# Te Apruma! 🩺

> *Saúde na comunidade*

Jogo casual em C desenvolvido para a disciplina **Programação Imperativa e Funcional (PIF) — 2026.2**, na CESAR School.

## Sobre o Jogo

Você é um **agente comunitário** que circula pela comunidade resolvendo situações de risco à saúde da população local: cachorros sem controle, doenças ligadas ao contato com água e locais contaminados, contato irresponsável com capivaras, entre outras. O objetivo é agir rápido, alertar os moradores (*"Te apruma, não pra tu ver!"*) e manter a saúde da comunidade em dia.

O jogo nasceu de um hackathon sobre mudanças climáticas, que discutiu o papel social das comunidades, as mais afetadas ambiental e financeiramente. A ideia é transformar esse tema em algo lúdico, sem culpar os moradores: a comunidade é a protagonista, e o jogo mostra que muitos riscos vêm da falta de infraestrutura.

## Fases

- **Fase 1 - Rua Limpa**: Pegue as sacolas de lixo antes que os cachorros caramelos consigam pegá-las e espalhar o lixo.
- **Fase 2 - Canal**: Retire as pessoas que entram no canal, evitando a formação de multidões e riscos à saúde (como leptospirose e correnteza forte).
- **Fase 3 - Bueiros e Chuva Torrencial** (em desenvolvimento): Desentupa os bueiros antes que a rua alague.
- **Fase 4 - Evacuação na Encosta** (em desenvolvimento): Conduza os moradores até o abrigo antes de um deslizamento.
- **Fase 5 - Coleta Seletiva** (em desenvolvimento): Direcione os resíduos para a lixeira correta.

## Como Compilar e Rodar

Você vai precisar do **[Raylib](https://www.raylib.com/)** e do **[w64devkit](https://github.com/skeeto/w64devkit)** configurados no seu ambiente.

### Opção 1: Usando Make

No terminal, na raiz do projeto:

```powershell
make
make run
```


## Estrutura do Projeto

```text
te_apruma/
├── assets/        # Sprites, sons e fontes
├── include/       # Arquivos de cabeçalho (.h)
├── src/           # Código fonte (.c)
├── build/         # Executáveis compilados
├── Makefile       # Script de compilação
└── README.md
```

## Conceitos Aplicados (PIF)

- **Structs**: Jogador, Entidades (cachorros, sacolas, pessoas)
- **Ponteiros e Alocação Dinâmica**: Criação/remoção dinâmica de entidades com `malloc/free`
- **Listas Encadeadas**: Gerenciamento de entidades em tempo real
- **Matrizes**: Preparado para mapas em grade (Fase 4)
- **Arquivos**: Preparado para sistema de ranking de pontuações

## Equipe

Projeto desenvolvido para a disciplina Programação Imperativa e Funcional - PIF 2026.2, CESAR School.