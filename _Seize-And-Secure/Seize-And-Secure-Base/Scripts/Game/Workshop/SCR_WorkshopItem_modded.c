modded class SCR_WorkshopItem
{
	//-----------------------------------------------------------------------------------------------
	override Revision GetLatestRevision()
	{		
		if (!m_Item)
		{
			Debug.Error("SCR_WorkshopItem: Item is null!");
			return null;
		}
		
		Revision rev = m_Item.GetLatestRevision();
		if (!rev)
		{
			Debug.Error("SCR_WorkshopItem: GetLatestRevision: Item: " + m_Item + " | Rev: " + rev + " | Local Rev: " + m_Item.GetLocalRevision() + " | Active Rev: " + m_Item.GetActiveRevision());
			Debug.Error("SCR_WorkshopItem: Item is present but LatestRevision is null!");			
			return null;
//			rev = m_Item.GetLocalRevision();
		}
			
		return rev;
	}
};

//---- REFACTOR NOTE END ----
