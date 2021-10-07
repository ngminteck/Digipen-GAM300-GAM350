1. Object not loading correctly from SCENE file due to it loading from PREFAB
  FIX: remove / add the prefab name in editor for the particular object depending on needs
  Example: Waypoint was added to scene using waypoint editor from PREFAB therefore have 
  the reference from PREFAB which has no neighbours.
  
2. If logicManager scream "LogicManager::Reflect cannot find script~!\n" check if some SICREATE is not added 
  
3. If there is weird iterator expires problem like in a for loop,
-first probability is that the vector size has changed like some actions or functions
increment the vector size during the range for loop causing the iterator to expires
-quick solution
if you are sure that during the iteration the only change to the vector is increment,
we can change the range for loop to using a normal for loop.
else if the vector does decrease in size... all behaviour are undefine