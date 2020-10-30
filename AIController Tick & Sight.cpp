//======================================================================================================================================
//The tick function in the AI Controller handles
// -Propagating desired actions to the pawn (mostly rotation, firing weapon, etc)
// -Lerping to a desired rotation adding in "hiccups" which adds to the realism
//		e.g. the aim is not 100% perfect and they can miss shots due to wandering aim
// -A bus between sensory information, and actions based on sensory information
// 		simply put, if the sight has been updated, or the agent has killed their target, they re evaluate which behaviors to run
//======================================================================================================================================
void AAIBaseController::Tick(float delta)
{
  Super::Tick(delta);

  //set pawn rotation & update the aiming vector to match pawn
  ReadPawnInput();
  
  //Lerp towards our current target, if available
  AimAtTarget();

  //If sight perception is dirty, re evaluate threats and possibly choose new target
  //UpdateTrees handles switching behavior trees over a reaction time ([0.2,0.4] seconds)
  if(bIsSightDirty)
  {
    ABaseCharacter* oldtarget = currentTarget;
    EvaluateThreatTargets();
    if(oldtarget != currentTarget && bNewTarget)
    {
      bNewTarget = false;
      UpdateTrees(); //see UpdateTrees.cpp
    }
    else if(!currentTarget)
    {
      UpdateTrees(); //see UpdateTrees.cpp
    }
  }
  //If sight hasn't been updated, make sure current target is still alive
  else if(bHasTarget && !currentTarget->GetIsAlive())
  {
    bNewTarget = false;
    bHasTarget = false;
    bIsSightDirty = true;
    currentTarget = nullptr;
  }
}

//======================================================================================================================================
//UpdateSight is called whenever the Sight Perception Component registers another player (enemy or ally)
//======================================================================================================================================
void AAIBaseController::UpdateSight(const FSightInfo& info)
{
  if (bMatchOver) { return; }
  ABaseCharacter *other = info.character;
  ABaseNPC *self = GetControlledNPC();

  if(!self->EnemyTags.HasAny(other->SelfTags))
  {
    //this is an ally, or at least, not an enemy
    return;
  }

  //start a timer that will wipe current target after a random interval
  //This acts as short term memory - specifically when sight is obstructed by cover, the agent won't forget their target
  if(!info.bVisible && info.character == currentTarget)
  {
    std::random_device rd;
    std::uniform_real_distribution<float> dist(4.2f, 6.0f);
    GetWorldTimerManager().ClearTimer(MemoryTimerHandle);
    GetWorldTimerManager().SetTimer(MemoryTimerHandle, this, &AAIBaseController::RememberSighted,  dist(rd), false);
    rememberInfo = info;
  }

  //set the dirty flag, ad this info to the sightmap
  //if the actor has already been added to the sightmap, this simply updates the information stored
  bIsSightDirty = true;
  sightMap.Add(info.character, info);
}