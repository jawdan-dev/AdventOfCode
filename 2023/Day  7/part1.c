// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 50
enum ReadState {
	ReadState_HandCards,
	ReadState_HandBet,
};
#include "../Common/FileBuffer.h"

enum CardType {
	CardType_2,
	CardType_3,
	CardType_4,
	CardType_5,
	CardType_6,
	CardType_7,
	CardType_8,
	CardType_9,
	CardType_10,
	CardType_Jack,
	CardType_Queen,
	CardType_King,
	CardType_Ace,
};
enum HandType {
	HandType_HighCard,
	HandType_OnePair,
	HandType_TwoPair,
	HandType_ThreeOfAKind,
	HandType_FullHouse,
	HandType_FourOfAKind,
	HandType_FiveOfAKind,
};

struct Card {
	char m_char;
	enum CardType m_type;
};
struct Hand {
	struct Card m_cards[5];
	enum HandType m_type;
	size_t m_betAmount;
};

const bool Card_isSame(const enum CardType a, const enum CardType b);
const enum CardType Card_getCardValue(const char cardChar);
const enum HandType Hand_getHandType(const struct Hand* const this);
void Hand_Constructor(struct Hand* const this, const char* cards);
const bool Hand_LessThan(const struct Hand* const this, const struct Hand* const other);

// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ ' ', ReadState_HandCards },
		{ '\n', ReadState_HandBet }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_HandBet);

	// Interpretation variables.
	struct Hand hands[1000];
	size_t handCount = 0;

	// Interpret file.
	while (FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Check for incoming seg error.
		if (handCount >= (sizeof(hands) / sizeof(*hands))) {
			fprintf(stderr, "Hand limit of %zu exceeded.", sizeof(hands) / sizeof(*hands));
			abort();
		}

		switch (fileBuffer.readState) {
			case ReadState_HandCards: {
				// Load the card information.
				Hand_Constructor(
					&hands[handCount],
					fileBuffer.readBuffer
				);
			} break;
			case ReadState_HandBet: {
				// Read bet amount.
				size_t betAmount;
				sscanf(fileBuffer.readBuffer, "%zu", &betAmount);

				// Assign.
				hands[handCount].m_betAmount = betAmount;

				// Increment counter.
				handCount++;
			} break;
		}
	}

	// Storage.
	struct Hand* sortedHands[sizeof(hands) / sizeof(*hands)];
	for (size_t i = 0; i < handCount; i++) sortedHands[i] = &hands[i];

	// Sort.
	size_t offset = 0;
	for (bool sorted = false; !sorted; offset++) {
		// Reset information.
		sorted = true;
		// Loop.
		for (size_t i = 1; i < handCount - offset; i++) {
			// Get target values.
			struct Hand
				** const a = &sortedHands[i - 1],
				** const b = &sortedHands[i];

			if (Hand_LessThan(*b, *a)) {
				// Swap.
				struct Hand* const c = *a;
				*a = *b;
				*b = c;
				// Update details.
				sorted = false;
			}
		}
	}

	// Calculate answer.
	size_t totalWinnings = 0;
	for (size_t i = 0; i < handCount; i++) {
		struct Hand* const hand = sortedHands[i];
		const size_t rank = i + 1;
		const size_t winnings = rank * hand->m_betAmount;

		// Add to total.
		totalWinnings += winnings;
	}

	// Output answer.
	printf("Output: %zu\n", totalWinnings);
	return 0;
}

const bool Card_isSame(const enum CardType a, const enum CardType b) {
	return a == b;
}
const enum CardType Card_getCardValue(const char cardChar) {
	switch (cardChar) {
		case 'A': return CardType_Ace;
		case '2': return CardType_2;
		case '3': return CardType_3;
		case '4': return CardType_4;
		case '5': return CardType_5;
		case '6': return CardType_6;
		case '7': return CardType_7;
		case '8': return CardType_8;
		case '9': return CardType_9;
		case 'T': return CardType_10;
		case 'J': return CardType_Jack;
		case 'Q': return CardType_Queen;
		case 'K': return CardType_King;
	}
	// Errr.
	fprintf(stderr, "Failed to determine CardType for %c.\n", cardChar);
	return 0;
}
const enum HandType Hand_getHandType(const struct Hand* const this) {
	// NOTE: This entire function assumes a deck size of 5.
	if ((sizeof(this->m_cards) / sizeof(*this->m_cards)) != 5) {
		fprintf(stderr, "Hand card count != 5, which is incompatible with the function 'Hand_getHandType(...)'.");
		return HandType_HighCard;
	}

	// Sort cards (for ease).
	enum CardType sortedCards[5];
	for (size_t i = 0; i < 5; i++) sortedCards[i] = this->m_cards[i].m_type;

	// Bubble sort <3. (Perfectly valid for 5 elements.)
	bool sorted = false;
	size_t offset = 0;
	while (!sorted) {
		sorted = true;
		for (size_t i = 1; i < 5 - offset; i++) {
			// Get target values.
			enum CardType
				* const a = &sortedCards[i - 1],
				* const b = &sortedCards[i];

			if (*a < *b) {
				// Swap.
				enum CardType c = *a;
				*a = *b;
				*b = c;
				// Update details.
				sorted = false;
			}
		}
		offset++;
	}

	// Five of a kind.
	if ((Card_isSame(sortedCards[0], sortedCards[1])) &&
		(Card_isSame(sortedCards[0], sortedCards[2])) &&
		(Card_isSame(sortedCards[0], sortedCards[3])) &&
		(Card_isSame(sortedCards[0], sortedCards[4]))) {
		return HandType_FiveOfAKind;
	}

	// Four of a kind.
	if ((Card_isSame(sortedCards[1], sortedCards[2]) && Card_isSame(sortedCards[1], sortedCards[3])) &&
		(Card_isSame(sortedCards[1], sortedCards[0]) || Card_isSame(sortedCards[1], sortedCards[4]))) {
		return HandType_FourOfAKind;
	}

	// Full house.
	if ((Card_isSame(sortedCards[0], sortedCards[1]) && Card_isSame(sortedCards[3], sortedCards[4])) &&
		(Card_isSame(sortedCards[2], sortedCards[1]) || Card_isSame(sortedCards[2], sortedCards[3]))) {
		return HandType_FullHouse;
	}

	// Three of a kind.
	if ((Card_isSame(sortedCards[2], sortedCards[0]) && Card_isSame(sortedCards[2], sortedCards[1])) ||
		(Card_isSame(sortedCards[2], sortedCards[1]) && Card_isSame(sortedCards[2], sortedCards[3])) ||
		(Card_isSame(sortedCards[2], sortedCards[3]) && Card_isSame(sortedCards[2], sortedCards[4]))) {
		return HandType_ThreeOfAKind;
	}

	// Pair counter.
	// NOTE: 'Three of a kind' should catch if more than 2 are concurrent.
	size_t pairCount = 0;
	enum CardType currentCard = sortedCards[0];
	for (size_t i = 1; i < 5; i++) {
		if (Card_isSame(sortedCards[i], currentCard)) pairCount++;
		currentCard = sortedCards[i];
	}

	// Handle pairs.
	if (pairCount == 2) {
		// Two pairs found.
		return HandType_TwoPair;
	} else if (pairCount == 1) {
		// One pairs found.
		return HandType_OnePair;
	}

	// Default to high card otherwise?
	if (pairCount != 0) {
		fprintf(
			stderr, "Unknown card type for %c %c %c %c %c.\n",
			this->m_cards[0].m_char, this->m_cards[1].m_char, this->m_cards[2].m_char,
			this->m_cards[3].m_char, this->m_cards[4].m_char
		);
		abort();
	}

	return HandType_HighCard;
}
void Hand_Constructor(struct Hand* const this, const char* cards) {
	// Constants.
	const size_t
		handCardCount = sizeof(this->m_cards) / sizeof(*this->m_cards),
		cardCount = strlen(cards);

	// Error check.
	if (cardCount < handCardCount) {
		fprintf(stderr, "Given hand cards string too short: '%s'.", cards);
		return;
	}

	// Initialize hand.
	this->m_betAmount = 0;
	this->m_type = HandType_HighCard;

	// Process cards.
	for (size_t i = 0; i < handCardCount; i++) {
		this->m_cards[i].m_char = cards[i];
		this->m_cards[i].m_type = Card_getCardValue(cards[i]);
	}

	// Get hand information.
	this->m_type = Hand_getHandType(this);
}
const bool Hand_LessThan(const struct Hand* const this, const struct Hand* const other) {
	// Check types.
	if (this->m_type != other->m_type)
		return this->m_type < other->m_type;

	// Compare unsorted cards.
	for (size_t i = 0; i < sizeof(this->m_cards) / sizeof(*this->m_cards) && i < sizeof(other->m_cards) / sizeof(*other->m_cards); i++)
		if (this->m_cards[i].m_type != other->m_cards[i].m_type)
			return this->m_cards[i].m_type < other->m_cards[i].m_type;

	// Otherwise, the same.
	return false;
}