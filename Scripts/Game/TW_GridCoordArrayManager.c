class TW_GridCoordArrayManager<Class T> : TW_GridManagerBase
{
	private ref map<int, ref map<int, ref TW_GridCoordArray<T>>> grid = new map<int, ref map<int, ref TW_GridCoordArray<T>>>();	
	private int pointerIndex = -1;
	
	void TW_GridCoordArrayManager(GridSettings settings)
	{
		super(settings);
	}
	
	TW_GridCoordArray<T> GetCoord(int x, int y)
	{
		if(!HasCoord(x, y))
			return null;
		
		map<int, ref TW_GridCoordArray<T>> sub = grid.Get(x);
		return sub.Get(y);
	}
	
	//! Try to insert item via world position
	void InsertByWorld(vector position, T item)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GetSizeInMeters());
		
		ref TW_GridCoordArray<T> coord;
		
		if(!HasCoord(x, y))
		{
			coord = new TW_GridCoordArray<T>(x, y);
			InsertCoord(coord);
		}
		else
			coord = GetCoord(x, y);
		
		coord.Add(item);
	}
	
	// Remove item via world position
	void RemoveByWorld(vector position, T item)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GetSizeInMeters());
		
		
		if(!HasCoord(x, y))
			return;
		
		ref TW_GridCoordArray<T> coord = GetCoord(x, y);
		
		coord.RemoveItem(item);
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
			
			ref map<int, ref TW_GridCoordArray<T>> sub = grid.Get(x);
			sub.Remove(y);
			
			if(sub.Count() <= 0)
				sub.Remove(x);
		}
	}
	
	//! Remove sector from grid
	void RemoveCoord(vector position)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GetSizeInMeters());
		
		if(!HasCoord(x, y))
			return;
		
		ref map<int, ref TW_GridCoordArray<T>> sub = grid.Get(x);
		sub.Remove(y);
		
		if(sub.Count() <= 0)
			sub.Remove(x);
	}
	
	//! Does the grid have coordinate
	bool HasCoord(int x, int y)
	{
		if(!grid.Contains(x))
			return false;
		
		map<int, ref TW_GridCoordArray<T>> sub = grid.Get(x);

		return sub.Contains(y);
	}
	
	// Does the provided world position translate into a coordinate being tracked by this manager?
	bool HasPosition(vector worldPosition)
	{
		int x, y;
		TW_Util.ToGrid(worldPosition, x, y, GetSizeInMeters());
		
		return HasCoord(x,y);
	}
	
	//! Retrieve coords around center
	int GetNeighbors(notnull out array<ref TW_GridCoordArray<T>> items, int x, int y, int radius = -1, bool includeCenter = true)
	{
		if(radius <= 0)
			radius = GetDistanceInChunks();
		
		int leftBounds = x - radius;
		int rightBounds = x + radius;
		int upperBounds = y + radius;
		int lowerBounds = y - radius;
		int count = 0;
		
		for(int gridX = leftBounds; gridX <= rightBounds; gridX++)
		{
			for(int gridY = lowerBounds; gridY <= upperBounds; gridY++)
			{
				if(gridX == x && gridY == y && !includeCenter)
					continue;
				
				if(HasCoord(gridX, gridY))
				{
					items.Insert(GetCoord(gridX, gridY));
					count++;
				}
			}
		}
		
		return count;
	}
	
	//! Insert coordinate directly into manager
	void InsertCoord(TW_GridCoordArray<T> coord)
	{
		// IF X coordinate doesn't exist we have to insert both x and y
		if(!grid.Contains(coord.x))
		{
			ref map<int, ref TW_GridCoordArray<T>> sub = new map<int, ref TW_GridCoordArray<T>>();
			sub.Insert(coord.y, coord);
			grid.Insert(coord.x, sub);
		}
		
		// IF X but no Y -- insert y and coord
		if(!grid.Get(coord.x).Contains(coord.y))
		{
			ref map<int, ref TW_GridCoordArray<T>> sub = grid.Get(coord.x);
			sub.Insert(coord.y, coord);
		}
	}
	
	//! Get all chunks around player positions
	int GetChunksAround(notnull out array<ref TW_GridCoordArray<T>> chunks, notnull set<string> textCoords, int radius = -1)
	{
		if(radius <= 0)
			radius = GetDistanceInChunks();
		
		ref set<string> completedCoords = new set<string>();
		
		int x;
		int y;
		int totalCount = 0;
		
		foreach(string textCoord : textCoords)
		{
			if(completedCoords.Contains(textCoord))
				continue;
			
			TW_Util.FromGridString(textCoord, x, y);
					
			// If this coordinate has already been checked -- continue
			if(completedCoords.Contains(textCoord))
				continue;
					
			completedCoords.Insert(textCoord);
				
			if(HasCoord(x, y))
			{
				ref TW_GridCoordArray<T> chunk = GetCoord(x, y);
				chunks.Insert(chunk);
				totalCount++;
			}			
		}
		
		return totalCount;
	}
	
	//! Retrieve all registered items via specified grid coordinates, in a radius
	int GetNeighborsAround(notnull out array<T> data, notnull set<string> textCoords, int radius = -1)
	{
		if(radius <= 0)
			radius = GetDistanceInChunks();
		
		ref set<string> completedCoords = new set<string>();
		
		int x;
		int y;
		int totalCount = 0;
		
		foreach(string textCoord : textCoords)
		{
			if(completedCoords.Contains(textCoord))
				continue;
			
			TW_Util.FromGridString(textCoord, x, y);
			
			// If this coordinate has already been checked -- continue
			if(completedCoords.Contains(textCoord))
				continue;
					
			completedCoords.Insert(textCoord);
					
			if(HasCoord(x, y))
			{
				ref TW_GridCoordArray<T> chunk = GetCoord(x, y);
				totalCount += chunk.GetData(data);
			}
		}
		
		return totalCount;
	}	
	
	int GetNeighborsAroundPosition(vector position, notnull out array<ref TW_GridCoordArray<T>> items, int radius = -1, bool includeCenter = true)
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
					ref TW_GridCoordArray<T> coord = GetCoord(gridX, gridY);
					count += coord.GetData(items);
				}				
			}			
		}
		
		return count;
	}
	
	//! Retrieve all registered items around specific grid coordinate
	int GetNeighboringItems(notnull out array<T> items, int x, int y, int radius = -1, bool includeCenter = true)
	{
		if(radius <= 0)
			radius = GetDistanceInChunks();
		
		ref array<ref TW_GridCoordArray<T>> neighbors = {};
		int chunks = GetNeighbors(neighbors, x, y, radius, includeCenter);
		
		if(chunks < 0)
			return 0;
		
		int count = 0;
		foreach(ref TW_GridCoordArray<T> chunk : neighbors)
		{
			count += chunk.GetData(items);
		}
		
		return count;
	}
	
	//! Retrieve all items being tracked by this grid manager
	int GetAllItems(notnull out array<T> items)
	{
		int count = 0;
		
		foreach(int x, ref map<int, ref TW_GridCoordArray<T>> values : grid)
			foreach(int y, ref TW_GridCoordArray<T> value : values)
				count += value.GetData(items);
		
		return count;
	}	
};