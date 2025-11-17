//------------------------------------------------------------------------------------------------
//void OnControlledEntityChangedServer(int playerId, IEntity from, IEntity to);
//typedef func OnControlledEntityChangedServer;
//typedef ScriptInvokerBase<OnControlledEntityChangedServer> OnControlledEntityChangedServerInvoker;
//
//------------------------------------------------------------------------------------------------
//modded class SCR_PlayerController : PlayerController
//{
//	ref OnControlledEntityChangedServerInvoker m_Seize_OnControlledEntityChangedServer = new OnControlledEntityChangedServerInvoker();
//	
//	//------------------------------------------------------------------------------------------------
//	override void OnControlledEntityChanged(IEntity from, IEntity to)
//	{
//		super.OnControlledEntityChanged(from, to);
//		
//		if (!Replication.IsRunning() || Replication.IsServer())
//			m_Seize_OnControlledEntityChangedServer.Invoke(GetPlayerId(), from, to);
//	}
//}