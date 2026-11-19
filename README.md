# Te Apruma! 🩺

> *Saúde na comunidade*

Jogo casual em C, feito com a biblioteca **[Raylib](https://www.raylib.com/)**, desenvolvido para a disciplina **Programação Imperativa e Funcional (PIF) - 2026.2**, na CESAR School.

## Sobre o Jogo

Você é um **agente comunitário** que circula pela comunidade resolvendo situações de risco à saúde da população local: cachorros sem controle, doenças ligadas ao contato com água e locais contaminados, contato irresponsável com capivaras, entre outras. O objetivo é agir rápido, alertar os moradores (*"Te apruma, não pra tu ver!"*) e manter a saúde da comunidade em dia.

O jogo nasceu de um hackathon sobre mudanças climáticas, que discutiu o papel social das comunidades, as mais afetadas ambiental e financeiramente. A ideia é transformar esse tema em algo lúdico, sem culpar os moradores: a comunidade é a protagonista, e o jogo mostra que muitos riscos vêm da falta de infraestrutura.

## Vídeo do Jogo

▶️ **[Assistir ao vídeo do jogo](https://youtu.be/Z5ZM2SgRebY)**

## Fases

- **Fase 1 - Rua Limpa**: pegue as sacolas de lixo antes que os cachorros consigam pegá-las e espalhar o lixo. Cada sacola coletada vale 1 ponto; cada sacola que o cachorro come tira 1 ponto. Dura 30 segundos e você vence com pontuação maior que zero.
- **Fase 2 - Canal**: retire as pessoas que entram no canal, evitando a formação de multidões e riscos à saúde (como leptospirose e correnteza forte). Cada pessoa resgatada vale 1 ponto. Se 6 pessoas chegarem ao canal, a fase é perdida na hora. Dura 30 segundos e você vence se menos de 6 pessoas tiverem caído no canal.

A partida tem as duas fases em sequência. A dificuldade aumenta com o tempo: os cachorros ficam mais rápidos e aparecem mais.

## Controles

| Tela | Tecla / ação | Função |
|---|---|---|
| Menu | Setas ou W/S | Escolher opção |
| Menu | ENTER ou clique | Confirmar opção |
| Menu | R | Abrir o ranking |
| Fase | Setas ou W/A/S/D | Mover o Player |
| Fase | ESC ou P | Abrir o menu de pausa |
| Pausa | Setas ou W/S, ENTER ou clique | Escolher CONTINUAR, MENU PRINCIPAL ou SAIR |
| Pausa | ESC ou P | Voltar ao jogo |
| Fim de fase / Ranking / Game over | ENTER | Avançar ou voltar ao menu |

Abandonar a partida pelo menu de pausa **não** registra pontuação no ranking.

## Ranking

Cada partida gera **uma linha** no ranking, com:
- pontos das duas fases somados;
- sacolas coletadas na fase 1;
- pessoas resgatadas na fase 2.

Partidas perdidas na fase 1 também entram, com pessoas = 0. Só entram pontuações positivas, e o ranking guarda as 5 melhores.

As pontuações são salvas no arquivo `ranking.txt`, criado na pasta de onde o jogo é executado. O arquivo tem uma partida por linha, no formato `pontos sacolas pessoas`.

## Como Compilar

### Windows

O projeto usa o **[w64devkit](https://github.com/skeeto/w64devkit)** (que traz o `gcc` e o `make`) e a Raylib para Windows.

1. Instale o **w64devkit x64** (o compilador de 64 bits) e baixe a **Raylib 6.0 para MinGW-w64 (64 bits)**.
2. Na raiz do projeto, copie o modelo de configuração:
   ```powershell
   copy config.mk.example config.mk
   ```
3. Abra o `config.mk` e ajuste `CC` (caminho do `gcc.exe` do w64devkit) e `RAYLIB_PATH` (pasta que contém `include\raylib.h`). Use `/` no lugar de `\`.
4. No terminal, na raiz do projeto:
   ```powershell
   make
   ```

O executável é gerado em `build/teapruma.exe`. O `config.mk` é só da sua máquina e não vai para o Git. Se o `make` reclamar que a Raylib não foi encontrada ou que o compilador não é 64 bits, confira esses dois caminhos.

### Linux

1. Instale o `gcc`, o `make` e a Raylib (por exemplo, `sudo apt install build-essential libraylib-dev`). Se a versão do repositório for antiga, compile a Raylib pelo código-fonte.
2. Na raiz do projeto, rode:
   ```bash
   make
   ```

O `Makefile` detecta o Linux e usa o `pkg-config` para achar a Raylib. Não precisa de `config.mk`.

## Como Executar

Rode o jogo **sempre a partir da raiz do projeto**, porque os sprites, os sons e o `ranking.txt` são carregados por caminhos relativos.

- **Windows:** `make run`, ou `build\teapruma.exe` a partir da raiz.
- **Linux:** `./build/teapruma`, a partir da raiz.

Se o jogo abrir sem sprites ou sem som, confira se a pasta de execução é a raiz do projeto.

## Organização do Código

```text
te_apruma/
├── assets/
│   ├── audio/        # Música de fundo, vitória e game over (.wav)
│   └── sprites/      # Folhas de sprites 32x32 empilhadas na vertical (.png)
├── include/          # Cabeçalhos (.h): declarações de cada módulo
├── src/              # Código-fonte (.c): implementação de cada módulo
├── build/            # Arquivos gerados pela compilação
├── main.c            # Exemplo antigo de teste da Raylib (não é o ponto de entrada)
├── Makefile          # Compilação para Windows
└── README.md
```

O ponto de entrada do jogo é `src/main.c`. Ele chama `GameInit`, `GameRun` e `GameClose`, nessa ordem.

Cada módulo cuida de uma parte do jogo:

| Módulo | Responsabilidade | Principais funções |
|---|---|---|
| `game` | Janela, loop principal, menus e troca de telas | `GameInit`, `GameRun`, `GameClose` |
| `world` | Fase 1 (Rua Limpa): sacolas, cachorros e pontuação | `WorldInit`, `WorldReset`, `WorldUpdate`, `WorldDraw` |
| `canal` | Fase 2 (Canal): pessoas, água e resgates | `CanalInit`, `CanalReset`, `CanalUpdate`, `CanalDraw` |
| `player` | Ítalo, o jogador: movimento, animação e espelhamento | `PlayerInit`, `PlayerUpdate`, `PlayerDraw`, `PlayerGet` |
| `entity` | Entidades (sacolas, cachorros, pessoas): criação, animação e desenho | `EntityCreate`, `EntityUpdate`, `EntityDraw`, `EntityDestroy` |
| `list` | Lista encadeada genérica que guarda as entidades em cena | `ListCreate`, `ListPushBack`, `ListRemoveIf`, `ListForEach`, `ListDestroy` |
| `ranking` | Matriz de pontuações e leitura/escrita do `ranking.txt` | `RankingInit`, `RankingAdd`, `RankingGet`, `RankingCount` |
| `textures` | Carregamento e liberação das imagens | `TexturesLoad`, `TexturesUnload`, `TexturesGetPlayer` |
| `audio` | Música de fundo e efeitos sonoros | `AudioLoad`, `AudioUpdate`, `AudioPlayWin`, `AudioPlayGameOver`, `AudioUnload` |
| `config` | Constantes globais, como o tamanho da tela | `SCREEN_W`, `SCREEN_H`, `GROUND_Y` |

## Conceitos Aplicados (PIF)

- **Structs**: `Player`, `Entity`, `List`, `Node`, `World` e `Canal`.
- **Ponteiros**: `Entity*` e `List*`; callbacks genéricos recebem `void* context`.
- **Alocação dinâmica**: entidades e nós da lista são criados com `malloc` e liberados com `free`. A função `ListPushBack` informa se a inserção falhou, e nesse caso a entidade é destruída na hora, para não vazar memória.
- **Listas encadeadas**: a lista de entidades em cena (`src/list.c`), percorrida com `ListForEach` e limpa com `ListRemoveIf`.
- **Matrizes**: o ranking é uma matriz `ranking[RANKING_SIZE][RANKING_COLS]`, ordenada por insertion sort.
- **Arquivos**: o ranking é lido ao iniciar e regravado a cada partida, em `ranking.txt`.

## Equipe

Projeto desenvolvido para a disciplina Programação Imperativa e Funcional — PIF 2026.2, CESAR School.

- Arthur Vitorino Fernandes
- Kelwin Karan
- Maria Clara Pontes
