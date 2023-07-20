#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define MAX_PLAYERS 4
#define MAX_CARDS 108 //76 de numeros (19 de cada cor) + 8 compre duas + 8 inverte jogo + 8 pula o proximo + 4 coringa (escolhe cor) + 4 coringa compra quatro e escolhe cor = 108

// define cores das cartas
typedef enum {
    AZUL,
    VERDE,
    VERMELHO,
    AMARELO,
    CORINGA
} Color;

// define tipo
typedef enum {
    NUMERO,
    SKIP,
    REVERSE,
    COMPRA_DUAS,
    CORINGA_COR,
    CORINGA_COMPRA
} CardType;

// descreve a carta: cor, tipo e numero (não confundir NUMERO(tipo) com number(o numero da carta em si))
typedef struct {
    Color color;
    CardType type;
    int number;
} Card;

// define nome do player, score, mão e número de cartas
typedef struct {
    int name;
    int score;
    Card hand[MAX_CARDS];
    int numCards;
} Player;

// define a struct do jogo
typedef struct {
    Player players[MAX_PLAYERS];
    int numPlayers;
    Card drawPile[MAX_CARDS];
    int numDrawPile;
    Card discardPile[MAX_CARDS];
    int numDiscardPile;
    Color currentColor;
    bool clockwise;
    int currentPlayer;
} UnoGame;

// definindo algumas funções usadas
void initializeGame(UnoGame* game, int numPlayers);
void shuffleDeck(Card* deck, int numCards);
void dealCards(UnoGame* game, int numCards);
void playGame(UnoGame* game);
void printCard(Card card);
void printPlayerHand(Player* player);
bool isCardPlayable(Card card, Card topDiscard, Color currentColor);
void drawCard(UnoGame* game, int playerIndex, int numCards);
void discardCard(UnoGame* game, int playerIndex, int cardIndex);
void applyCardEffects(UnoGame* game, Card card);
Color chooseColor();
Card deck[MAX_CARDS];

int main() {
    srand(time(NULL)); // usa o relógio do computador como um seed de sorteio

    UnoGame game;
    int numPlayers;

    printf("Bem-vindos ao UNO!\n");
    printf("Digite o numero de jogadores (2-4): ");
    scanf("%d", &numPlayers);

    initializeGame(&game, numPlayers);
    playGame(&game);

    return 0;
}

void initializeGame(UnoGame* game, int numPlayers) {
    // Inicializar jogadores
    game->numPlayers = numPlayers;
    for (int i = 0; i < numPlayers; i++) {
        printf("Entre com o numero do jogador %d : ", i + 1);
        scanf("%d", &game->players[i].name);
        game->players[i].score = 0;
        game->players[i].numCards = 0;
    }
    // Inicializar baralho
    int numCards = 0;
    // numera as cartas numericas, de azul até amarelo.
    for (Color color = AZUL; color <= AMARELO; color++) {
        for (int number = 0; number <= 9; number++) {
            Card card = { color, NUMERO, number };  // cor, tipo, numero
            deck[numCards++] = card;
            if (number != 0) {
                deck[numCards++] = card;
            }
        }
        // inicializa cartas skip, reverse e compraDuas. inicializa duas cartas por cor (8 cartas de cada no total)
        Card skipCard = { color, SKIP, -1 }; // cor, tipo, numero
        deck[numCards++] = skipCard;
        deck[numCards++] = skipCard;

        Card reverseCard = { color, REVERSE, -2 }; // tava -1 antes, mas identificava reverse com mesmo numero de skip, entao troquei todos os numeros das cartas que n tem numero
        deck[numCards++] = reverseCard;
        deck[numCards++] = reverseCard;

        Card drawTwoCard = { color, COMPRA_DUAS, -3 };
        deck[numCards++] = drawTwoCard;
        deck[numCards++] = drawTwoCard;
    }
    // inicializa as cartas coringa_cor e coringa_compra, são inicializadas, assim como as skip reverse e drawtwo com número = -1
    for (int i = 0; i < 4; i++) {
        Card wildCard_color = { CORINGA, CORINGA_COR, -4 }; // cor, tipo, numero
        deck[numCards++] = wildCard_color;

        Card wildCard_drawFour = { CORINGA, CORINGA_COMPRA, -5 };
        deck[numCards++] = wildCard_drawFour;
    }
    // embaralha e dá as cartas
    shuffleDeck(deck, numCards);
    dealCards(game, numCards);

    // Inicialização do estado inicial do jogo
    game->numDrawPile = numCards - (numPlayers * 7);
    for (int i = 0; i < game->numDrawPile; i++) {
        game->drawPile[i] = deck[i];
    }

    game->numDiscardPile = numPlayers * 7;
    for (int i = 0; i < game->numDiscardPile; i++) {
        game->discardPile[i] = deck[game->numDrawPile + i];
    }

    game->currentColor = game->discardPile[game->numDiscardPile - 1].color;
    game->currentPlayer = 0;
    game->clockwise = true;
}

// função de embaralhar
void shuffleDeck(Card* deck, int numCards) {
    for (int i = 0; i < numCards - 1; i++) {
        int j = i + rand() % (numCards - i);
        Card temp = deck[j];
        deck[j] = deck[i];
        deck[i] = temp;
    }
}

// função de dar as cartas
void dealCards(UnoGame* game, int numCards) {
    int currentPlayer = 0;
    shuffleDeck(deck, numCards);

    for (int i = 0; i < game->numPlayers; i++) {
        for (int j = 0; j < 7; j++) {
            int cardIndex = (i * 7) + j;
            game->players[currentPlayer].hand[game->players[currentPlayer].numCards++] = deck[cardIndex];
        }
        currentPlayer++;
    }
}

void playGame(UnoGame* game) {
    bool gameOver = false; // jogo não terminou

    // loop principal
    while (!gameOver) {
        //obtem um ponteiro para o player atual
        Player* currentPlayer = &game->players[game->currentPlayer];

        printf("Eh a vez de %d jogar\n", currentPlayer->name);
        printf("A carta atual no topo da pilha eh: ");
        printCard(game->discardPile[game->numDiscardPile - 1]);
        printf("\n");

        //printf("Sua mao: ");
        printPlayerHand(currentPlayer);
        printf("\n");

        int cardIndex;
        do {
            printf("Escolha uma carta para jogar (0-%d): ", currentPlayer->numCards - 1);
            scanf("%d", &cardIndex);
            // se a carta for jogável...
        } while (!isCardPlayable(currentPlayer->hand[cardIndex], game->discardPile[game->numDiscardPile - 1], game->currentColor));

        Card card = currentPlayer->hand[cardIndex];
        discardCard(game, game->currentPlayer, cardIndex);
        applyCardEffects(game, card);

        if (currentPlayer->numCards == 0) {
            printf("%d ganhou!\n", currentPlayer->name);
            gameOver = true;
        }

        // se for coringa, precisa escolher a cor
        if (card.color == CORINGA) {
            game->currentColor = chooseColor();
        } else {
            game->currentColor = card.color;
        }

        // se nao for skip, se clockwise for True, vai pro próximo, se clockwise for false (anti-horario), vai pro anterior.
        if (card.type != SKIP) {
            int nextPlayer = game->clockwise ? 1 : -1;
            game->currentPlayer = (game->currentPlayer + nextPlayer + game->numPlayers) % game->numPlayers;
        }
    }
}

void printCard(Card card) {
    switch (card.color) {
        case AZUL:
            printf("Azul ");
            break;
        case VERDE:
            printf("Verde ");
            break;
        case VERMELHO:
            printf("Vermelho ");
            break;
        case AMARELO:
            printf("Amarelo ");
            break;
        case CORINGA:
            printf("Coringa ");
            break;
    }

    switch (card.type) {
        case NUMERO:
            printf("%d", card.number);
            break;
        case SKIP:
            printf("Pular");
            break;
        case REVERSE:
            printf("Inverter");
            break;
        case COMPRA_DUAS:
            printf("Comprar duas");
            break;
        case CORINGA_COR:
            printf("Escolher cor");
            break;
        case CORINGA_COMPRA:
            printf("Comprar quatro");
            break;
    }
}

// printa as informações do player a cada turno
void printPlayerHand(Player* player) {
    printf("Jogador: %d\n", player->name);
    //printf("Placar: %d\n", player->score);
    printf("Numero de cartas: %d\n", player->numCards);
    printf("Mao:\n");
    for (int i = 0; i < player->numCards; i++) {
        printf("%d. ", i);
        printCard(player->hand[i]);
        printf("\n");
    }
    printf("\n");
}

// a carta é jogável se:
bool isCardPlayable(Card card, Card topDiscard, Color currentColor) {
    return (card.color == topDiscard.color ||
            card.number == topDiscard.number ||
            card.color == CORINGA ||
            card.color == currentColor);
}

// compra carta (funciona só com +2 e +4, a compra por falta de cartas jogáveis não está funcional)
void drawCard(UnoGame* game, int playerIndex, int numCards) {
    Player* player = &game->players[playerIndex];
    for (int i = 0; i < numCards; i++) {
        player->hand[player->numCards++] = game->drawPile[--game->numDrawPile];
    }
}

// tira a carta jogada da mão do jogador e joga na pilha
void discardCard(UnoGame* game, int playerIndex, int cardIndex) {
    Player* player = &game->players[playerIndex];
    Card card = player->hand[cardIndex];

    for (int i = cardIndex; i < player->numCards - 1; i++) {
        player->hand[i] = player->hand[i + 1];
    }
    player->numCards--;

    game->discardPile[game->numDiscardPile++] = card;
}

// função que aplica os efeitos das cartas
void applyCardEffects(UnoGame* game, Card card) {
    if (card.type == REVERSE) {
        game->clockwise = !game->clockwise;
    } else if (card.type == COMPRA_DUAS) {
        drawCard(game, (game->currentPlayer + 1) % game->numPlayers, 2);
    } else if (card.type == CORINGA_COMPRA) {
        drawCard(game, (game->currentPlayer + 1) % game->numPlayers, 4);
        //game->currentColor = chooseColor();
    } else if (card.type == SKIP) {
        int nextPlayer = game->clockwise ? 2 : -2;
        game->currentPlayer = (game->currentPlayer + nextPlayer + game->numPlayers) % game->numPlayers;
    } else if (card.type == CORINGA_COR) {
        //game->currentColor = chooseColor();
    }
}

// escolhe cor
Color chooseColor() {
    int color;
    printf("Escolha uma cor (0-Azul, 1-Verde, 2-Vermelho, 3-Amarelo): ");
    scanf("%d", &color);
    return (Color)color;
}

//////////////RESOLVIDO//////////////////
// CARTA SKIP: FUNCIONANDO
// CARTA +4: FUNCIONANDO  ** NÃO ACUMULA
// CARTA ESCOLHE COR: FUNCIONANDO
//////////////FALTA RESOLVER/////////////
// FALTA IMPLEMENTAR A COMPRA QUANDO NÃO TEM JOGADA POSSÍVEL
// FALTA PRINTAR A COR ATUAL, POIS QUANDO A CARTA É CORINGA O JOGADOR NÃO SABE QUE COR FOI ESCOLHIDA
