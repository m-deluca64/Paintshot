//======================================================================================================================================
//Check in with team is the fallback routine for an agent without anything to do
//If they have a teammate who is requesting help, they will initiate a flanking behavior
//Otherwise, they will scout the field, and check back in after 10 seconds of scouting

//Check in with team is initially caled from UpdateTrees()
//======================================================================================================================================
void AAIBaseController::CheckInWithTeam()
{
  //Get an array of teammates, and loop thru checking if anyone needs help
  //GetTeammates() returns an array of alive/valid teammates, excluding self 
  auto teamArray = GetTeammates();
  for(auto ally : teamArray)
  {
    auto *aic = Cast<AAIBaseController>(ally->GetController());
	//AAIBaseController::WantsHelp simply returns bHasTarget && !b
    if(aic->WantsHelp())
    {
	  //grab the ally's target, set it as our own
      auto bb = GetBlackboard();
      bb->SetValueAsObject(focusActorKey, aic->currentTarget);
      bHasTarget = true;
      currentTarget = aic->currentTarget;
	  
	  //switch to the flanking behavior tree
      SwitchTasks(controlledCharacter->flankTree);
      return;
    }
  }
  
  //If no other teammates want help scout for 10 seconds.
  //This will be aborted however if sight perception sees any enemies 
  SwitchTasks(controlledCharacter->scoutTree);
  GetWorldTimerManager().SetTimer(ScoutTimerHandle, this, &AAIBaseController::CheckInWithTeam,  10.f, false);
}