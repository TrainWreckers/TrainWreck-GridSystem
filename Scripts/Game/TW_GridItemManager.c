class TW_GridItemManager<Class T> : TW_GridManagerBase
{
	private ref map<int, ref map<int, ref TW_GridCoordItem<T>>> grid = new map<int, ref map<int, ref TW_GridCoordItem<T>>>();
	private int GridSize;
	
	// Coordinates that are currently active
	private ref set<string> activeCoords = new set<string>();
	
	private int pointerIndex = -1;
	private int activeCoordsCount = 0;
	
	void TW_GridItemManager(GridSettings settings)
	{
		m_GridSettings = settings;
	}
	
	TW_GridCoordItem<T> GetCoord(int x, int y)
	{
		if(!HasCoord(x, y))
			return null;
		
		map<int, ref TW_GridCoordItem<T>> sub = grid.Get(x);
		return sub.Get(y);
	}
	
	//! Remove multiple chunks from active coordinates
	void RemoveCoords(notnull set<string> coords)
	{
		int x, y;
		foreach(string coord : coords)
		{
			TW_Util.FromGridString(coord, x, y);
			
			if(!HasCoord(x, y))
				continue;
			
			ref map<int, ref TW_GridCoordItem<T>> sub = grid.Get(x);
			sub.Remove(y);
			
			if(sub.Count() <= 0)
				sub.Remove(x);
		}
	}
	
	void RemoveByWorld(vector position, T item)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		
		if(!HasCoord(x, y))
			return;
		
		ref TW_GridCoordItem<T> coord = GetCoord(x, y);
		
		coord.SetItem(item);
	}
	
	//! Try to insert item via world position
	void InsertByWorld(vector position, T item)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		ref TW_GridCoordItem<T> coord;
		
		if(!HasCoord(x, y))
		{
			coord = new TW_GridCoordItem<T>(x, y, item);
			InsertCoord(coord);
		}
		else
		{
			coord = GetCoord(x, y);
			coord.SetItem(item);
		}
	}
	
	//! Remove sector from grid
	void RemoveCoord(vector position)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		if(!HasCoord(x, y))
			return;
		
		ref map<int, ref TW_GridCoordItem<T>> sub = grid.Get(x);
		sub.Remove(y);
		
		if(sub.Count() <= 0)
			sub.Remove(x);
	}
	
	//! Does the grid have coordinate
	bool HasCoord(int x, int y)
	{
		if(!grid.Contains(x))
			return false;
		
		map<int, ref TW_GridCoordItem<T>> sub = grid.Get(x);

		return sub.Contains(y);
	}
	
	bool HasPosition(vector worldPosition)
	{
		int x, y;
		TW_Util.ToGrid(worldPosition, x, y, GridSize);
		
		return HasCoord(x,y);
	}
	
	void InsertCoord(TW_GridCoordItem<T> coord)
	{
		// IF X coordinate doesn't exist we have to insert both x and y
		if(!grid.Contains(coord.x))
		{
			ref map<int, ref TW_GridCoordItem<T>> sub = new map<int, ref TW_GridCoordItem<T>>();
			sub.Insert(coord.y, coord);
			grid.Insert(coord.x, sub);
		}
		
		// IF X but no Y -- insert y and coord
		if(!grid.Get(coord.x).Contains(coord.y))
		{
			ref map<int, ref TW_GridCoordItem<T>> sub = grid.Get(coord.x);
			sub.Insert(coord.y, coord);
		}
	}
	
	int GetNeighborsAroundPosition(vector position, notnull out array<T> items, int radius = -1, bool includeCenter = true)
	{
		if(radius <= 0)
			radius = GetDistanceInChunks();
		
		int x, y;
		TW_GridUtils.ToGrid(position, x, y, GetSizeInMeters());
		
		int leftBounds = x - radius;
		int rightBounds = x + radius;
		int lowerBounds = y - radius;
		int upperBounds = y + radius;
		
		int count = 0;
		for(int gridX = leftBounds; gridX <= rightBounds; gridX++)
		{
			for(int gridY = lowerBounds; gridY <= upperBounds; gridY++)
			{
				if(gridX == x && gridY == y && !includeCenter)
					continue;
				
				if(HasCoord(gridX, gridY))
				{
					ref TW_GridCoordItem<T> coord = GetCoord(gridX, gridY);
					
					if(coord.GetItem())
					{
						items.Insert(coord.GetItem());
						count++;
					}					
				}				
			}			
		}
		
		return count;
	}
};