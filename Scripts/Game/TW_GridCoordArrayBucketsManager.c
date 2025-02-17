//! Manage multiple things within a coordinate (envisioned to use Enums)
class TW_GridCoordArrayBucketsManager<Class T> : TW_GridManagerBase
{
	private ref map<int, ref map<int, ref TW_GridCoordArrayBuckets<T>>> grid = new map<int, ref map<int, ref TW_GridCoordArrayBuckets<T>>>();
	
	void TW_GridCoordArrayBucketsManager(GridSettings settings)
	{
		super(settings);
	}
	
	//! Retrieve bucket of items at coordinate
	TW_GridCoordArrayBuckets<T> GetCoord(int x, int y)
	{
		if(!HasCoord(x, y))
			return null;
		
		map<int, ref TW_GridCoordArrayBuckets<T>> sub = grid.Get(0);
		return sub.Get(y);
	}
	
	//! Remove multiple coordinates from being tracked
	void RemoveCoords(notnull set<string> coords)
	{
		int x, y;
		foreach(string coord : coords)
		{
			TW_GridUtils.FromGridString(coord, x, y);
			
			if(!HasCoord(x, y))
				continue;
			
			ref map<int, ref TW_GridCoordArrayBuckets<T>> sub = grid.Get(x);
			sub.Remove(y);
			
			if(sub.Count() <= 0)
				sub.Remove(x);
		}
	}
	
	//! Remove coordinate via world position
	void RemoveCoord(vector position)
	{
		int x, y;
		TW_GridUtils.ToGrid(position, x, y, GetSizeInMeters());
		
		if(!HasCoord(x, y))
			return;
		
		ref map<int, ref TW_GridCoordArrayBuckets<T>> sub = grid.Get(x);
		sub.Remove(y);
		
		if(sub.Count() <= 0)
			sub.Remove(x);
	}
	
	//! Try to insert item via world position
	void InsertByWorld(vector position, T item, int bucket)
	{
		int x, y;
		TW_GridUtils.ToGrid(position, x, y, GetSizeInMeters());
		
		ref TW_GridCoordArrayBuckets<T> coord;
		
		if(!HasCoord(x, y))
		{
			coord = new TW_GridCoordArrayBuckets<T>(x, y);
			InsertCoord(coord);
		}
		else
			coord = GetCoord(x, y);
		
		coord.Add(item);
	}
	
	//! Does the grid have coordinate
	bool HasCoord(int x, int y)
	{
		if(!grid.Contains(x))
			return false;
		
		map<int, ref TW_GridCoordArrayBuckets<T>> sub = grid.Get(x);
		
		return sub.Contains(y);
	}
	
	//! Has grid coord (converts position)
	bool HasCoord(vector worldPosition)
	{
		int x, y;
		TW_GridUtils.ToGrid(worldPosition, x, y, GetSizeInMeters());
		
		return HasCoord(x, y);
	}
	
	//! Retrieve coords around center
	int GetNeighbors(notnull out array<ref TW_GridCoordArrayBuckets<T>> items, int x, int y, int radius = -1, bool includeCenter = true)
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
	
	//! Insert a preinstantiated bucket of items
	void InsertCoord(TW_GridCoordArrayBuckets<T> coord)
	{
		if(!grid.Contains(coord.x))
		{
			ref map<int, ref TW_GridCoordArrayBuckets<T>> sub = new map<int, ref TW_GridCoordArrayBuckets<T>>();
			sub.Insert(coord.y, coord);
			grid.Insert(coord.x, sub);
		}
		
		// IF X but no Y -- insert y and coord
		if(!grid.Get(coord.x).Contains(coord.y))
		{
			ref map<int, ref TW_GridCoordArrayBuckets<T>> sub = grid.Get(coord.x);
			sub.Insert(coord.y, coord);
		}
	}
	
	//! Get all chunks around player position
	int GetChunksAround(notnull out array<ref TW_GridCoordArrayBuckets<T>> chunks, notnull set<string> textCoords, int radius = -1)
	{
		if(radius <= 0)
			radius = GetDistanceInChunks();
		
		ref set<string> completedCoords = new set<string>();
		
		int x, y, totalCount = 0;
		
		foreach(string textCoord : textCoords)
		{
			if(completedCoords.Contains(textCoord))
				continue;
			
			TW_GridUtils.FromGridString(textCoord, x, y);
			
			// If this coordinate has already been checked -- continue
			if(completedCoords.Contains(textCoord))
				continue;
			
			completedCoords.Insert(textCoord);
			
			if(HasCoord(x, y))
			{
				ref TW_GridCoordArrayBuckets<T> chunk = GetCoord(x, y);
				chunks.Insert(chunk);
				totalCount++;
			}
		}
		
		return totalCount;
	}
	
	//! Retrieve items from a specific bucket from neighboring chunks
	int GetNeighboringBucketItems(notnull out array<T> items, int x, int y, int radius = -1, bool includeCenter = true, int bucketType = 0)
	{
		if(radius <= 0)
			radius = GetDistanceInChunks();
		
		ref set<string> completedCoords = new set<string>();
		ref array<ref TW_GridCoordArrayBuckets<T>> neighbors = {};
		
		int chunks = GetNeighbors(neighbors, x, y, radius, includeCenter);
		
		if(chunks <= 0)
			return 0;
		
		int count = 0;
		
		foreach(ref TW_GridCoordArrayBuckets<T> chunk : neighbors)
		{
			count += chunk.GetDataFromBucket(bucketType, items);
		}
		
		return count;
	}
	
	//! Retrieve all items being tracked witthin this grid manager
	int GetAll(notnull inout array<T> items)
	{
		int count = 0;
		
		// map<int, ref map<int, ref TW_GridCoordArrayBuckets<T>>>
		foreach(int gridX, ref map<int, ref TW_GridCoordArrayBuckets<T>> yChunks : grid)
		{
			foreach(int gridY, ref TW_GridCoordArrayBuckets<T>> chunkData : yChunks)
			{
				count += chunkData.GetData(items);
			}
		}
		
		return count;
	}
};