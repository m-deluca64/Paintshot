# Paintshot
Paintshot is a Paintball like FPS developed in Unreal Engine 4. My main contribution to this project was the FPS AI. 

This repo contains a few snippets:
- AI Controller Tick & Sight.cpp
  - This shows off the core tick function & the sight update function. These are the 2 core areas of updates from within the AI Controller
- EvaluateThreats.cpp
  - EvaulateThreats contains the code for choosing a target enemy to focus on. This considers all sighted enemies and judges their threat level. 
- UpdateTrees.cpp
  - This holds the main decision tree (essentially a simple state machine) which chooses various behavior trees to run based on enemy/ally considerations.
- CheckInWithTeammates.cpp
  - This contains the main check for any agent's asking their teammates if they need help for flanking.
