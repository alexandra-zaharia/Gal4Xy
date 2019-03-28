# Gal4Xy
A turn-based 4X strategy game where you play against the computer to conquer the galaxy

**This is a work in progress! The README will be updated when more functionality is added and when the game actually becomes playable.**

## Introduction

Gal4Xy is a turn-based 4X strategy game set in a small (10 by 10) galaxy. Every player starts out from their home planet. As you explore the galaxy, you colonize planets and the resources they generate are used to build ships. Ships allow you to travel to different parts of the galaxy, to defend your own colonies, and to crush your enemies. 

![Gal4Xy screenshot](https://github.com/alexandra-zaharia/Gal4Xy/blob/master/Gal4Xy_screenshot.png)

By default, you play against other AI players. For the moment, only a dumb AI has been implemented but in the future it will be possible to select smarter AI opponents. The game does not support multiplayer at the moment, but this functionality is in the making: you will be able to set up a Gal4Xy server on your machine and have friends join you over the Internet.

At the moment, Gal4Xy is technically only a 2.5X game, but it will become 4X at some point! 4X means _explore_, _expand_, _exploit_, and _exterminate_. The current version of Gal4Xy combines the exploration and expansion steps and features extermination, but not exploitation. :-)

## Running Gal4Xy

You will need `gcc`, `cmake`, and `make` to build and run the game. 

First, clone this repository and `cd` into it:

```
https://github.com/alexandra-zaharia/Gal4Xy.git
cd Gal4Xy
```

Then create a directory for building Gal4Xy, and build it:

```
mkdir build
cd build
cmake ..
make
```

You can now run Gal4Xy as follows:

```
./Gal4Xy
```

## What works
  * The galaxy and planets are initialized, and home planets are randomly chosen for the human and AI players.
  * By default, only two players are created, but the game can handle multiple players. 
  * The galaxy may be displayed for the human player.
  * The galaxy may be displayed in "cheat" mode (all planets are visible, all (incoming) fleets of all players are visible).
  * The main game loop.
  * At each turn:
      * For every player, resources are harvested on every one of her planets.
      * For every player, ships are built on every planet having enough resources.
  * The player may display her planets.
  * The player may display her fleets.
  * The player may move her fleets to a designated sector.
  * When a player explores a previously unexplored sector, a random amount of bonus resources are gathered and sent to the player's home planet. (The resource bonus is then set to zero, ensuring that only the first player in a sector receives a bonus.)
  * When a sector containing a planet without owner is explored, the player having sent the fleet colonizes that planet.
  * A dumb AI has been implemented. It remembers nothing of past events and can view only the sectors that it has explored. It aims to explore 100% of the galaxy while also performing a semblance of defending already owned planets.
  * Battles between two players: the player with the maximum total firepower (in-place fleet and incoming fleet) wins. If both players have the same total firepower in a given sector, the battle is at tie. 
  * Battles between more than two players: the order in which players fire on their adversaries is random. The first player (randomly chosen) fires on the second one, the second one fires on the third one, etc., up to the last player who fires on the first one, then the cycle repeats while the weakest players are eliminated from the battle. The last player standing wins the fight, although a battle between more than two players may also finish with a tie.

## TODO
  * [ ] (UI) Ensure that player symbols/colors are unique.
  * [ ] (UI) Take into account dark/light background for console text.
  * [x] (UI) Take all AI players into account (incoming fleet) when displaying the galaxy in cheat mode.
  * [ ] (Gameplay) Implement handling of incoming fleets:
      * [x] Exploration (sectors)
      * [x] Colonization (planets)
      * [x] Battles between two players
      * [x] Battles between more than two players
  * [ ] (Gameplay) Implement AI (AI does not cheat):
      * [x] Dumb AI: has no memory of past events
      * [ ] Average AI: remembers past events
      * [ ] Smart AI: remembers past events (and does stuff better than the average AI -- what and how precisely is to be determined)  
  * [ ] (Gameplay) More engaging exploration (exploration bonus strengthens closest colony)
  * [ ] Multiplayer mode 
  * [ ] (Gameplay) Let player choose when, where and how many ships to build
  * [ ] (Gameplay) Complexify colonization: add planet characteristics (temperature, gravity), assign a finite quantity of material resources to every planet and convert the existing resources to in-game currency (ship building will require both material resources and money)
  * [ ] (Gameplay) Complexify ship building by adding several ship types, suitable for exploration, colonization, attack, and defense
