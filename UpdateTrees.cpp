//======================================================================================================================================
//======================================================================================================================================
/*
 * Can I see people?
 * Yes
 *  Should I hide or attack?
 *    Hide Tree
 *    Attack Tree
 * No
 *  Is it because I'm hiding?
 *    Yes
 *      Pop out after a timer interval
 *    No
 *      Ask if team needs help?
 */
void AAIBaseController::UpdateTrees()
{
  //Clear the scouting timer, we are gonna reeval now instead
  GetWorldTimerManager().ClearTimer(ScoutTimerHandle);
  
  auto teamArray = GetTeammates();
  
  
  if(bHasTarget && currentTarget)
  {
    
    if(!sightMap[currentTarget].bVisible)
    {
		//can't see the target cause were hiding
		//so keep hiding!
		//Short term sight memory will kick us out of our hiding state later
    }
    else
    {
      std::random_device rd;
      std::uniform_real_distribution<float> dist(0.f, 1.f);
      
	  //Consider current ammo when deciding if we want to hide or attack
      AProjectileWeapon *weapon = Cast<AProjectileWeapon>(currentTarget->Equipment->GetCurrentItem());
      int32 ammo = weapon->GetCurrentAmmoInClip();
      int32 max = weapon->GetMaxClipSize();
      float clipPercentage = static_cast<float>(ammo) / static_cast<float>(max);


	  //Add more weight to the hiding decision based on the current score of the game
      float hideWeight;
      int enemies = GetEnemies().size();
      int team = teamArray.size();
      if(team == enemies)
      {
        hideWeight = 0.7f;
      }
      else if(team > enemies)
      {
        hideWeight = 0.3f;
      }
      else
      {
		//If our team is losing, then we more than probably want to hide
        hideWeight = 1.01f;
      }

      //default to hiding if running not a full clip
	  //otherwise, roll the dice against our calculated hide percentage
      if(dist(rd) < hideWeight || clipPercentage < 0.6f)
      {
        SwitchTasks(controlledCharacter->hideTree);
      }
      else
      {
		//If our team is winning, we might as well start flanking the enemy as they can't cover every teammate
        if(team > enemies)
        {
          SwitchTasks(controlledCharacter->flankTree);
        }
        else
        {
          SwitchTasks(controlledCharacter->attackTree);
        }
      }
    }
  }
  //If the agent dosn't have any sighted enemies, ask if anyone needs help, and/or scout the area
  else
  {
    if(!teamArray.empty())
    {
      for(auto ally : teamArray)
      {
        auto *aic = Cast<AAIBaseController>(ally->GetController());
        if(aic->WantsHelp())
        {
          auto bb = GetBlackboard();
          bb->SetValueAsObject(focusActorKey, aic->currentTarget);
          bHasTarget = true;
          currentTarget = aic->currentTarget;
          SwitchTasks(controlledCharacter->flankTree);
          return;
        }
      }
    }
	
	//If we are here, then begin scouting, but check back in with team in 10 seconds, if we didn't find any enemies while scouting
    SwitchTasks(controlledCharacter->scoutTree);
    GetWorldTimerManager().SetTimer(ReactionTimerHandle, this, &AAIBaseController::CheckInWithTeam,  10.f, false);
  }
}