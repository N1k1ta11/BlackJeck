#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
using namespace std;

class Card {
public:
	enum Meaning {
		Ace = 1,
		two,
		free,
		four,
		five,
		six,
		seven,
		eight,
		nine,
		ten,
		Jack,
		Queen,
		King
	};

	enum Suit {
		Clubs,
		Dimonds,
		Heart,
		Spades
	};

	friend ostream& operator<<(ostream& os, const Card& aCard);

	Card(Meaning m = Ace, Suit s = Spades, bool p = true);

	int GetValue() const; //returns the value of card

	void Flip(); //flipping card

private:
	Meaning m_meaning;
	Suit m_suit;
	bool m_position;
};

Card::Card(Meaning m, Suit s, bool p) : m_meaning(m), m_suit(s), m_position(p) {}

int Card::GetValue() const {
	int value = 0;
	if (m_position) {
		value = m_meaning;

		if (value > 10) {
			value = 10;
		}
	}

	return value;
}

void Card::Flip() {
	m_position = !(m_position);
}

class Hand {
protected:
	vector<Card*>m_card;

public:
	Hand();

	virtual ~Hand();

	void Add(Card* carts);

	void Clear();

	int GetTotal() const;

};

Hand::Hand() {
	m_card.reserve(7);
}

Hand::~Hand() {
	Clear();
}

void Hand::Add(Card* carts) {
	m_card.push_back(carts);
}

void Hand::Clear() {
	vector<Card*>::iterator iter = m_card.begin();

	for (iter = m_card.begin(); iter != m_card.end(); ++iter) {
		delete* iter;
		*iter = 0;
	}
	m_card.clear();
}

int Hand::GetTotal() const {
	if (m_card.empty()) {
		return 0;
	}

	if (m_card[0]->GetValue() == 0) {
		return 0;
	}

	int amount = 0;
	vector<Card*>::const_iterator iter;
	for (iter = m_card.begin(); iter != m_card.end(); ++iter) {
		amount += (*iter)->GetValue();
	}

	bool constAce = false;
	for (iter = m_card.begin(); iter != m_card.end(); ++iter) {
		if ((*iter)->GetValue() == Card::Ace) {
			constAce = true;
		}
	}

	if (constAce && amount <= 11) {
		amount += 10;
	}

	return amount;
}


class GenericPlayer : public Hand
{
	friend ostream& operator<<(ostream& os, const GenericPlayer& genericPlayer);

public:
	GenericPlayer(const string& name = "");

	virtual ~GenericPlayer();

	virtual bool IsHitting() const = 0;

	bool IsBoosted()const;

	void Bust() const;

protected:
	string m_name;
};

GenericPlayer::GenericPlayer(const string& name) : m_name(name) {}

GenericPlayer::~GenericPlayer() {}

bool GenericPlayer::IsBoosted() const {
	return (GetTotal() > 21);
}

void GenericPlayer::Bust() const {
	cout << "The player " << m_name << " has too many points\n";
}



class Deck : public Hand {

public:

	Deck();

	virtual ~Deck();

	void Populate();

	void Shuffle();

	void Deal(Hand& aHand);

	void AddItionalCards(GenericPlayer& aGenericPlayer);
};

Deck::Deck() {
	m_card.reserve(52);
	Populate();
}

Deck::~Deck() {}

void Deck::Populate() {
	Clear();
	for (int s = Card::Clubs; s <= Card::Spades; ++s) {
		for (int m = Card::Ace; m <= Card::King; ++m) {
			Add(new Card(static_cast<Card::Meaning>(m), static_cast<Card::Suit>(s)));
		}
	}
}

void Deck::Shuffle() {
	random_shuffle(m_card.begin(), m_card.end());
}

void Deck::Deal(Hand& aHand) {
	if (m_card.empty()) {
		cout << "Deck is empty\n";
	}
	else {
		aHand.Add(m_card.back()); //we take a card from the end of the deck
		m_card.pop_back(); //then we remove this card from the deck
	}
}

void Deck::AddItionalCards(GenericPlayer& aGenericPlayer) {
	cout << endl;
	while (!(aGenericPlayer.IsBoosted()) && aGenericPlayer.IsHitting()) {
		Deal(aGenericPlayer);
		cout << aGenericPlayer << endl;

		if (aGenericPlayer.IsBoosted()) {
			aGenericPlayer.Bust();
		}
	}
}


class Player : public GenericPlayer {
public:
	Player(const string& name = "");

	virtual ~Player();

	virtual bool IsHitting() const;


	void Win() const;

	void Lose() const;

	void Push() const;
};

Player::Player(const string& name) :GenericPlayer(name) {}
Player::~Player() {}

bool Player::IsHitting() const {
	cout << m_name << ", do you want to take a card from the deck? (Y / N)\n";
	char answer;
	cin >> answer;
	return (answer == 'Y' || answer == 'y');
}

void Player::Win() const {
	cout << m_name << " win\n";
}

void Player::Lose()const {
	cout << m_name << " lose\n";
}

void Player::Push()const {
	cout << m_name << " played in a draw\n";
}




class Diler : public GenericPlayer {
public:
	Diler(const string& name = "Diler");

	virtual ~Diler();

	virtual bool IsHitting() const;

	void FlipFirstCart();
};

Diler::Diler(const string& name) : GenericPlayer(name) {}
Diler::~Diler() {};

bool Diler::IsHitting()const {
	return (GetTotal() <= 16);
}

void Diler::FlipFirstCart() {
	if (m_card.empty()) {
		cout << "No cards";
	}
	else {
		m_card[0]->Flip();
	}
}

class Game {
public:
	Game(const vector<string>& names);

	~Game();

	void Play();

private:
	Deck m_deck;
	Diler m_diler;
	vector<Player> m_player;
};

Game::Game(const vector<string>& names) {
	vector<string>::const_iterator playerName;
	for (playerName = names.begin(); playerName != names.end(); ++playerName) {
		m_player.push_back(Player(*playerName));
	}
	srand(static_cast<unsigned int>(time(0)));
	m_deck.Populate();
	m_deck.Shuffle();
}

Game::~Game() {}

void Game::Play() {
	vector<Player>::iterator pPlayer;

	for (int i = 0; i < 2; ++i) {
		for (pPlayer = m_player.begin(); pPlayer != m_player.end(); ++pPlayer) {
			m_deck.Deal(*pPlayer);
		}
		m_deck.Deal(m_diler);
	}

	m_diler.FlipFirstCart();

	for (pPlayer = m_player.begin(); pPlayer != m_player.end(); ++pPlayer) {
		cout << *pPlayer << endl;
	}
	cout << m_diler << endl;

	for (pPlayer = m_player.begin(); pPlayer != m_player.end(); ++pPlayer) {
		m_deck.AddItionalCards(*pPlayer);
	}

	m_diler.FlipFirstCart();//Looked first cart diler
	cout << endl << m_diler;

	m_deck.AddItionalCards(m_diler);

	if (m_diler.IsBoosted()) {
		for (pPlayer = m_player.begin(); pPlayer != m_player.end(); ++pPlayer) {
			if (!(pPlayer->IsBoosted())) {
				pPlayer->Win();
			}
		}
	}
	else {
		for (pPlayer = m_player.begin(); pPlayer != m_player.end(); ++pPlayer) {
			if (!(pPlayer->IsBoosted())) {
				if (pPlayer->GetTotal() > m_diler.GetTotal()) {
					pPlayer->Win();
				}
				else if (pPlayer->GetTotal() < m_diler.GetTotal()) {
					pPlayer->Lose();
				}
				else
				{
					pPlayer->Push();
				}
			}
		}
	}

	for (pPlayer = m_player.begin(); pPlayer != m_player.end(); ++pPlayer) {//Clearing hands players
		pPlayer->Clear();
	}
	m_diler.Clear(); //Clearing Diler
}

ostream& operator<<(ostream& os, const Card& aCard) {
	const string meaning[]{ "zero" ,"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
	const string suit[]{ "C", "D", "H", "S" };

	if (aCard.m_position) {
		os << meaning[aCard.m_meaning] << suit[aCard.m_suit];
	}

	else {
		os << "XX";
	}

	return os;
}

ostream& operator<<(ostream& os, const GenericPlayer& genericPlayer) {
	os << genericPlayer.m_name << "\t";
	vector<Card*>::const_iterator pCard;
	if (genericPlayer.m_card.empty()) {
		os << "You don't have cards\n";
	}
	else {
		for (pCard = genericPlayer.m_card.begin(); pCard != genericPlayer.m_card.end(); ++pCard) {
			os << *(*pCard) << "\t";
		}

		if (genericPlayer.GetTotal() > 0) {
			cout << "Card amount:" << genericPlayer.GetTotal() << endl;
		}
	}
	return os;
}

int main()
{
	cout << "\t\t\t\t Welcom to BLACKJACK!!!\n";
	int cntPlayer = 0;
	while (cntPlayer < 1 || cntPlayer>7 && true) {
		cout << "How many players? (1-7): ";
		cin >> cntPlayer;
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(1, '\n');
			continue;
		}
	}
	vector<string> names;
	string name;
	for (int i = 0; i < cntPlayer; i++) {
		cout << "Enter name: ";
		cin >> name;
		names.push_back(name);
	}
	cout << endl;
	Game aGame(names);
	char again = 'y';
	while (again != 'n' && again != 'N') {
		aGame.Play();
		cout << "\nDo you want to play again? (Y/N): ";
		cin >> again;
	}

	return 0;
}