#include <iostream>
#include <string>
#include <fstream>

using namespace std;

namespace constants
{
    const int CAMELS = 5;
    const int LENGTH_TRACK = 16;
}

typedef struct player {
  int position;
  bool hasMoved;
  player *above = NULL;
  player *below = NULL;

  player() : hasMoved(false) {}
} player;

player * players = new player[constants::CAMELS];

float scores[constants::CAMELS];
int nonMovedPlayers = constants::CAMELS;
int positionsChecked = 0;
int * board = new int[constants::LENGTH_TRACK];

void loop(int inversedChance);
void movePlayerWithCarriedPlayers(player * camel, int steps);
void setup();
int getWinner();

int main()
{
	int i=0;
	float sum=0;

	for(i=0; i<constants::CAMELS; ++i)
	{
		scores[i] = 0.0;
	}

	for(i=0; i<constants::LENGTH_TRACK; ++i)
	{
		board[i] = i;
	}

	setup();

	loop(1);

	cout << "Checked " << positionsChecked << " positions. Winning chances:\n";
	
	for(i=0; i < constants::CAMELS; ++i)
	{
		sum += scores[i];
		cout << i << ": " << scores[i] << endl;
	}

	cout << "Check sum: " << sum << endl;


	delete[] players;
	delete[] board;


	return 0;
}

void loop(int inversedChance)
{
	int i, j, newInversedChance, winner, to, from;

	++positionsChecked;

	// Check if there is still a move to do
	if(nonMovedPlayers > 0)
	{
		// Update inversed chance, since we haven't have a winner yet
		newInversedChance = 3*nonMovedPlayers*inversedChance;

		// Loop over all players
		for(i=0; i < constants::CAMELS; ++i)
		{
			from = players[i].position;
			// Skip moved players
			if(players[i].hasMoved)
				continue;

			// Loop over possible dice rolls
			for(j=1; j < 4; ++j)
			{
				
				// Calculate new spot
				to = players[i].position + j;
				if(to >= 0 && to < constants::LENGTH_TRACK)
				{
					to = board[to];
				}

				// Move player ...
				movePlayerWithCarriedPlayers( &players[i], to);
				players[i].hasMoved = true;
				--nonMovedPlayers;

				// Check if player has finished the race
				if(players[i].position > constants::LENGTH_TRACK)
				{
					// Increase winning chances
					winner = getWinner();
					scores[winner] += 1/(float)newInversedChance;
				} else {

					// Check next turn
					loop(newInversedChance);
				}

				// Put player back where he was
				movePlayerWithCarriedPlayers( &players[i], from);
				players[i].hasMoved = false;
				++nonMovedPlayers;
			}
		}
	}

	// If there is no move to do, find the winner and increase his chances!
	else
	{

		winner = getWinner();

		// Increase winning chances
		scores[winner] += 1/(float)inversedChance;
	}
}

int getWinner()
{
	int max = 0, i, winner=0;

	for(i=0; i<constants::CAMELS; ++i)
	{
		// Check if position is okay
		if(players[i].position >= max)
		{
			max = players[i].position;

			// Check if camel is on top
			if(players[i].above == NULL)
			{
				winner = i;
			}
		}
	}

	return winner;
}

void movePlayerWithCarriedPlayers(player * camel, int position)
{
	int i;

	// don't do anything when nothing is asked
	if(camel->position == position)
		return;

	// [1] Find a new camel to jump on;
	player * jumpOn = NULL;

	for(i=0; i < constants::CAMELS; ++i)
	{
		if(players[i].position == position && players[i].above == NULL)
		{
			jumpOn = &players[i];
			break;
		}
	}


	// [2] Move all camels with the one on top
	player * pointer = camel;

	while(pointer != NULL) {
		pointer->position = position;
		pointer = pointer->above;
	}

	// [3] Set connections
	if( camel->below != NULL )
	{
		camel->below->above = NULL;
	}

	camel->below = jumpOn;

	if( jumpOn != NULL)
	{
		jumpOn->above = camel;
	}
}

void setup()
{
	ifstream input;
	int i, j, s, pos, numPlusTiles, numMinTiles;
	char moved;

	input.open("config");

	// Camel positions
	for(i=0; i < constants::CAMELS; ++i)
	{
		input >> pos;
		input >> moved;
		
		pos = pos - 1;

		players[i].position = pos;

		if(moved == 'y')
		{
			players[i].hasMoved = true;
			--nonMovedPlayers;
		}

		// Stack camels!
		for(j=0; j < i; ++j)
		{
			if(players[j].position == pos && players[j].above == NULL)
			{
				players[j].above = &players[i];
				players[i].below = &players[j];
			}
		}
	}

	input >> numPlusTiles;
	input >> numMinTiles;

	for(i=0; i<numPlusTiles; ++i)
	{
		input >> s;
		if(s > 0 && s <= constants::LENGTH_TRACK)
		{
			board[s-1] = s;
			cout << s << " -> " << s+1 << endl;
		}
	}

	for(i=0; i<numMinTiles; ++i)
	{
		input >> s;
		if(s > 1 && s <= constants::LENGTH_TRACK)
		{
			board[s-1] = s-2;
			cout << s << " -> " << s-1 << endl;
		}
	}

	input.close();

	cout << endl;
}