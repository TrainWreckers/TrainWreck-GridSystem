class TW_GridManagerBase
{
	protected ref GridSettings m_GridSettings;
	
	private ref ScriptInvoker<ref GridSettings> m_OnGridSettingsChanged = new ScriptInvoker<ref GridSettings>();
	
	//! Allow being notified of when grid settings changed
	ScriptInvoker<ref GridSettings> GetOnGridSettingsChanged() { return m_OnGridSettingsChanged; }
	
	void ChangeSettings(GridSettings settings)
	{
		m_GridSettings = settings;
		OnSettingsChanged();
		
		if(m_OnGridSettingsChanged)
			m_OnGridSettingsChanged.Invoke(settings);
	}
	
	protected void OnSettingsChanged();
	
	int GetSizeInMeters()
	{
		if(!m_GridSettings)
		{
			Print("TrainWreck: TW_GridManagerBase -> GridSettings null. Defaulting size in meters to 1000...", LogLevel.WARNING);
			return 1000;
		}
		
		return m_GridSettings.SizeInMeters;
	}
	
	int GetDistanceInChunks()
	{
		if(!m_GridSettings)
		{
			Print("TrainWreck: TW_GridManagerBase -> GridSettings null. Defaulting distance in chunks to 3...", LogLevel.WARNING);
			return 3;
		}
		
		return m_GridSettings.DistanceInChunks;
	}
};