#include <iostream>
#include <string>

using namespace std;

namespace constants
{
    const int CAMELS = 5;
}

typedef struct player {
  int position;
  bool hasMoved;
  player *above = NULL;
  player *below = NULL;

  player() : hasMoved(false) 
  {	
  }
} player;

player * players = new player[constants::CAMELS];

float scores[constants::CAMELS];
int nonMovedPlayers = constants::CAMELS;

void displayCamelInfo(player * camel);
void loop(int inversedChance);
void movePlayerWithCarriedPlayers(player * camel, int steps);
void setup();

int main()
{
	int i=0;

	setup();

	loop(1);

	cout << "Winning chances:\n";
	for(i=0; i < constants::CAMELS; ++i)
	{
		cout << i << ": " << scores[i] << endl;
	}


	delete[] players;



	return 0;
}

void displayCamelInfo(player * camel)
{
	cout << "Position: " << camel->position << ". Moved: " << camel->hasMoved << endl;
	return;
}

void loop(int inversedChance)
{
	int i, j, newInversedChance, max = 0, winner;

	// Check if there is still a move to do
	if(nonMovedPlayers > 0)
	{
		// Update inversed chance, since we haven't have a winner yet
		newInversedChance = 3*nonMovedPlayers*inversedChance;

		// Loop over all players
		for(i=0; i < constants::CAMELS; ++i)
		{
			// Skip moved players
			if(players[i].hasMoved)
				continue;

			// Loop over possible dice rolls
			for(j=1; j < 4; ++j)
			{
				// Move player ...
				movePlayerWithCarriedPlayers( &players[i], j);
				players[i].hasMoved = true;
				--nonMovedPlayers;

				// continue
				loop(newInversedChance);


				// Put player back where he was
				movePlayerWithCarriedPlayers( &players[i], -j);
				players[i].hasMoved = false;
				++nonMovedPlayers;
			}
		}
	}

	// If there is no move to do, find the winner and increase his chances!
	else
	{
		for(i=0; i < constants::CAMELS; ++i)
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

		// Increase winning chances
		scores[winner] += 1/(float)inversedChance;
	}
}

void movePlayerWithCarriedPlayers(player * camel, int steps)
{
	int i, pos = steps + camel->position;

	// [1] Find a new camel to jump on;
	player * jumpOn = NULL;

	for(i=0; i < constants::CAMELS; ++i)
	{
		if(players[i].position == pos && players[i].above == NULL)
		{
			jumpOn = &players[i];
			break;
		}
	}


	// [2] Move all camels with the one on top
	player * pointer = camel;

	while(pointer != NULL) {
		pointer->position = pos;
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
	int i, j, pos;

	cout << "Enter positions from back to front" << endl << endl;

	for(i=0; i < constants::CAMELS; ++i)
	{
		cout << "Position camel #" << i << endl;
		cin >> pos;

		players[i].position = pos;

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
}