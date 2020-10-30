//======================================================================================================================================
//This is fired whenever the sight perception is updated with new information
//This function consideres current target, all sighted enemies,
//and chooses to either switch to a new target, or keep the old one
//======================================================================================================================================
void AAIBaseController::EvaluateThreatTargets()
{
  UBlackboardComponent* bb = GetBlackboard();
  bIsSightDirty = false;
  bNewTarget = false;
  
  //If we don't have any targetd, or the old target is dead
  if (!bHasTarget || !currentTarget->GetIsAlive())
  {
	//Run thru all valid enemies in memory, choose a new target based on distance
    FVector const pos = controlledCharacter->GetActorLocation();
    float dis2 = FLT_MAX;
    ABaseCharacter *sighted = nullptr;
    for (auto &i : sightMap)
    {
      if (i.Value.character == nullptr)
      {
        continue;
      }
      //UWorld const *world = GetWorld();
      //DrawDebugSphere(world, i.Value.sightedLocation, 100.f, 20, FColor::Green, true, 0.f);
      float const d2 = FVector::DistSquared(i.Value.sightedLocation, pos);
      if (d2 < dis2 && i.Value.bVisible && i.Value.character->GetIsAlive())
      {
        sighted = i.Value.character;
        dis2 = d2;
      }
    }
	
	//If here, we have no current target, but potentially found a new one 
	//either set a valid target, or clear relavent values
    if(sighted)
    {
      bb->SetValueAsObject(focusActorKey, sighted);
      bHasTarget = true;
      currentTarget = sighted;
      bNewTarget = true;
    }
    else
    {
      bb->SetValueAsObject(focusActorKey, nullptr);
      bHasTarget = false;
      currentTarget = nullptr;
      bNewTarget = true;
    }
  }
  
  //If we have a target already, check other sighted enemies if something is more threatening
  else if(bHasTarget && currentTarget)
  {
	//Grab distance & direction to current target
    FVector const targetPos = currentTarget->GetActorLocation();
    FVector const pos = controlledCharacter->GetActorLocation();
    FVector selfDir = (pos - targetPos);
    selfDir.Normalize();
    FVector targetDir = currentTarget->GetControlRotation().Vector();
	float currD2 = FVector::DistSquared(currentTarget->GetActorLocation(), pos);
	
	//Check if our current target is ignorant of us (if our sighted target is not looking at us)
    bool bTargetIsIgnorant = FVector::DotProduct(selfDir, targetDir) > 0.7f;
	
  
	//for all others sighted
	for (auto &i : sightMap)
    {
      //skip current target & invalids
      if((i.Value.character == currentTarget) || (!i.Value.character) )
      {
        continue;
      }
      float const d2 = FVector::DistSquared(i.Value.sightedLocation, pos);
	  FVector const canidateDir = i.Value.character->GetControlRotation().Vector();
      bool bCanidateIgnorant = FVector::DotProduct(selfDir, canidateDir) > 0.7f;

      //is this target focusing on self? and does it have a line of fire?
      //we only change targets if our target is ignoring us, and someone else is looking at us
      if(!bTargetIsIgnorant && bCanidateIgnorant && i.Value.bVisible)
      {
        bb->SetValueAsObject(focusActorKey, i.Value.character);
        bHasTarget = true;
        currentTarget = i.Value.character;
        bNewTarget = true;
		return; //break from loop
      }
      //if the current target is focusing on us, then we will only change if a closer, and therefore deadlier, target is shootable
      if (d2 < currD2 && i.Value.bVisible && i.Value.character->GetIsAlive())
      {
        bb->SetValueAsObject(focusActorKey, i.Value.character);
        bHasTarget = true;
        currentTarget = i.Value.character;
        bNewTarget = true;
        return; //break from loop
      }
    }
  }
}