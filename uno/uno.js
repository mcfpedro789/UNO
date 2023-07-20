const colors = ["vermelho", "azul", "verde", "amarelo"];
const numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
const specialCards = ["skip", "reverse", "drawTwo", "wild", "wildDrawFour"];

let deck = [];
let playedCards = [];
let player1Hand = [];
let player2Hand = [];

let currentPlayer = 1;
let isGameActive = false;

function createDeck() {
    for (let color of colors) {
        // Adiciona cartas de número no baralho
        for (let number of numbers) {
            deck.push({ color: color, value: number });
            if (number !== 0) {
                deck.push({ color: color, value: number });
            }
        }

        // Adiciona cartas especiais no baralho
        for (let card of specialCards) {
            if (card === "wild") {
                deck.push({ color: "wild", value: card });
            } else if (card === "wildDrawFour") {
                deck.push({ color: "wild", value: card });
            } else {
                deck.push({ color: color, value: card });
                deck.push({ color: color, value: card });
            }
        }
    }
}



function shuffleDeck() {
    for (let i = deck.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [deck[i], deck[j]] = [deck[j], deck[i]];
    }
}

function dealHands() {
    player1Hand = deck.splice(0, 7);
    player2Hand = deck.splice(0, 7);
}

function initializeGame() {
    createDeck();
    shuffleDeck();
    dealHands();
    playedCards.push(deck.shift());

    isGameActive = true;
    currentPlayer = 1;

    updateUI();
    updateMessage(`Turno do jogador ${currentPlayer}`);
    enablePlayerActions();
}

function updateUI() {
    const player1HandElement = document.getElementById("player1-hand");
    const player2HandElement = document.getElementById("player2-hand");
    const playedCardElement = document.getElementById("played-card");

    // Limpa cartas anteriores
    player1HandElement.innerHTML = "";
    player2HandElement.innerHTML = "";

    // Mão do player 1
    for (let i = 0; i < player1Hand.length; i++) {
        const card = player1Hand[i];
        const cardElement = createCardElement(card, i, 1);
        player1HandElement.appendChild(cardElement);
    }

    // Mão do player 2
    for (let i = 0; i < player2Hand.length; i++) {
        const card = player2Hand[i];
        const cardElement = createCardElement(card, i, 2);
        player2HandElement.appendChild(cardElement);
    }

    // Carta jogada
    playedCardElement.innerHTML = "";
    const topCard = playedCards[playedCards.length - 1];
    const cardElement = createCardElement(topCard, -1, null);
    playedCardElement.appendChild(cardElement);

    // Add/remove class to indicate facedown cards for player's hand
    if (currentPlayer === 1) {
        player2HandElement.classList.add("facedown");
        player1HandElement.classList.remove("facedown");
    } else {
        player1HandElement.classList.add("facedown");
        player2HandElement.classList.remove("facedown");
    }
}

function createCardElement(card, index, player) {
    const cardElement = document.createElement("div");
    cardElement.className = "card";


    
    if (card) {
        if (card.value === "wild") {
            cardElement.style.backgroundImage = `url('cards/wildDrawFour.png')`;
        } else if (card.value === "wildDrawFour") {
            cardElement.style.backgroundImage = `url('cards/wild.png')`;
        } else {
            cardElement.style.backgroundImage = `url('cards/${card.color}_${card.value}.png')`;
        }
    } else {
        cardElement.classList.add("back");
    }

    if (player === currentPlayer) {
        cardElement.addEventListener("click", () => {
            if (isGameActive) {
                if (isValidMove(card)) {
                    makeMove(index);
                } else {
                    alert("Jogada inválida!");
                }
            }
        });
    }

    return cardElement;
}


function isValidMove(card) {
    const topCard = playedCards[playedCards.length - 1];

    if (card.color === "wild") {
        return true;
    } else if (topCard.color === card.color || topCard.value === card.value) {
        return true;
    }

    return false;
}

function makeMove(index) {
    let currentPlayerHand;
    if (currentPlayer === 1) {
        currentPlayerHand = player1Hand;
    } else {
        currentPlayerHand = player2Hand;
    }

    const card = currentPlayerHand.splice(index, 1)[0];
    playedCards.push(card);

    if (currentPlayerHand.length === 0) {
        endGame(`Jogador ${currentPlayer} venceu!`);
        return;
    }

    if (card.value === "reverse") {
        reverseGame();
    } else if (card.value === "skip") {
        skipTurn();
    } else if (card.value === "drawTwo") {
        drawCards(2, currentPlayer === 1 ? 2 : 1);
        skipTurn();
    } else if (card.value === "wild") {
        handleWildCard();
    } else if (card.value === "wildDrawFour") {
        handleWildDrawFourCard();
    }

    nextPlayerTurn();
}

function handleWildCard() {
    let selectedColor = prompt("Escolha a cor (vermelho, azul, verde ou amarelo):");
    selectedColor = selectedColor.toLowerCase();

    while (!colors.includes(selectedColor)) {
        alert("Cor inválida! Escolha entre vermelho, azul, verde ou amarelo.");
        selectedColor = prompt("Escolha a cor (vermelho, azul, verde ou amarelo):");
        selectedColor = selectedColor.toLowerCase();
    }

    const topCard = playedCards[playedCards.length - 1];
    topCard.color = selectedColor;
}


function handleWildDrawFourCard() {
    let selectedColor = prompt("Escolha a cor (vermelho, azul, verde ou amarelo):");
    selectedColor = selectedColor.toLowerCase();

    while (!colors.includes(selectedColor)) {
        alert("Cor inválida! Escolha entre vermelho, azul, verde ou amarelo.");
        selectedColor = prompt("Escolha a cor (vermelho, azul, verde ou amarelo):");
        selectedColor = selectedColor.toLowerCase();
    }

    const topCard = playedCards[playedCards.length - 1];
    topCard.color = selectedColor;
    drawCards(4, currentPlayer === 1 ? 2 : 1);
}


function nextPlayerTurn() {
    currentPlayer = currentPlayer === 1 ? 2 : 1;
    updateUI();
    updateMessage(`Turno do jogador ${currentPlayer}`);
}

function reverseGame() {
    player1Hand.reverse();
    player2Hand.reverse();
}

function skipTurn() {
    nextPlayerTurn();
}

function drawCards(numCards, player) {
    let targetHand;
    if (player === 1) {
        targetHand = player1Hand;
    } else {
        targetHand = player2Hand;
    }

    for (let i = 0; i < numCards; i++) {
        targetHand.push(deck.shift());
    }
}

function updateMessage(message) {
    const messageElement = document.getElementById("message");
    messageElement.textContent = message;
}

function enablePlayerActions() {
    const drawCardBtn = document.getElementById("draw-card-btn");
    drawCardBtn.disabled = false;
    drawCardBtn.addEventListener("click", drawCard);
}

function disablePlayerActions() {
    const drawCardBtn = document.getElementById("draw-card-btn");
    drawCardBtn.disabled = true;
    drawCardBtn.removeEventListener("click", drawCard);
}

function drawCard() {
    if (isGameActive) {
        drawCards(1, currentPlayer);
        nextPlayerTurn();
    }
}

function playCard() {
    if (isGameActive) {
        const selectedCardIndex = parseInt(prompt(`Jogador ${currentPlayer}, insira o índice da carta que deseja jogar:`));
        if (!isNaN(selectedCardIndex) && selectedCardIndex >= 0) {
            let currentPlayerHand;
            if (currentPlayer === 1) {
                currentPlayerHand = player1Hand;
            } else {
                currentPlayerHand = player2Hand;
            }

            if (selectedCardIndex < currentPlayerHand.length) {
                const selectedCard = currentPlayerHand[selectedCardIndex];
                if (isValidMove(selectedCard)) {
                    makeMove(selectedCardIndex);
                } else {
                    alert("Jogada inválida!");
                }
            } else {
                alert("Índice de carta inválido!");
            }
        } else {
            alert("Índice de carta inválido!");
        }
    }
}

function playSelectedCard() {
    if (isGameActive) {
        const cardIndexInput = document.getElementById("card-index-input");
        const selectedCardIndex = parseInt(cardIndexInput.value);

        let currentPlayerHand;
        if (currentPlayer === 1) {
            currentPlayerHand = player1Hand;
        } else {
            currentPlayerHand = player2Hand;
        }

        if (!isNaN(selectedCardIndex) && selectedCardIndex >= 0 && selectedCardIndex < currentPlayerHand.length) {
            const selectedCard = currentPlayerHand[selectedCardIndex];
            if (isValidMove(selectedCard)) {
                makeMove(selectedCardIndex);
            } else {
                alert("Jogada inválida!");
            }
        } else {
            alert("Índice de carta inválido!");
        }

        cardIndexInput.value = "";
    }
}

function endGame(message) {
    isGameActive = false;
    updateMessage(message);
    disablePlayerActions();
}

document.addEventListener("DOMContentLoaded", function () {
    initializeGame();
});
