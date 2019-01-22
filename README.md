# Gal4Xy
A turn-based 4X strategy game where you play against the computer to conquer the galaxy

**This is a work in progress! The README will be updated when more functionality is added and when the game actually becomes playable.**

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

## TODO
  * [ ] Ensure that player symbols/colors are unique.
  * [ ] Take into account dark/light background for console text.
  * [x] Take all AI players into account (incoming fleet) when displaying the galaxy in cheat mode.
  * [ ] Implement handling of incoming fleets:
      * [x] Exploration (sectors)
      * [x] Colonization (planets)
      * [x] Battles between two players
      * [ ] Battles between more than two players
  * [ ] Implement AI (AI does not cheat):
      * [x] Dumb AI: has no memory of past events
      * [ ] Average AI: remembers past events
      * [ ] Smart AI: remembers past events (and does stuff better than the average AI -- what and how precisely is to be determined)  
