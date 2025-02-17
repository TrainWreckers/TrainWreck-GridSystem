class TW_GridUtils
{
	//! Get center position of grid square in world coordinates
	static void GetCenterOfGridSquare(vector position, out int x, out int y, int gridSize = 1000)
	{
		ToGrid(position, x, y, gridSize);
		
		x = (x * gridSize) + (gridSize / 2);
		y = (y * gridSize) + (gridSize / 2);
	}
	
	//! Add chunks in a radius around position to chunk set
	static void AddSurroundingGridSquares(notnull inout set<string> chunks, vector position, int radius = 1, int gridSize = 1000)
	{
		int x, y;
		ToGrid(position, x, y, gridSize);
		
		int xStart = x - radius;
		int xEnd = x + radius;
		int yStart = y - radius;
		int yEnd = y + radius;
		
		string text = "";
		
		for(int gx = xStart; gx <= xEnd; gx++)
		{
			for(int gy = yStart; gy <= yEnd; gy++)
			{
				text = string.Format("%1 %2", gx, gy);
				if(!chunks.Contains(text))
					chunks.Insert(text);
			}
		}				
	}
	
	//! Convert position to grid coordinates (text)
	static string ToGridText(vector position, int gridSize = 1000)
	{
		int x, y;
		ToGrid(position, x, y, gridSize);
		return string.Format("%1 %2", x, y);				
	}
	
	//! Convert World position to grid based coordinates	
	static void ToGrid(vector position, out int x, out int y, int gridSize = 1000)
	{
		if(gridSize < 10)
			gridSize = 1000;
		
		x = position[0] / gridSize;
		y = position[2] / gridSize;
	}
	
	//! Extract X and Y from Grid text
	static void FromGridString(string grid, out int x, out int y)
	{
		ref array<int> nums = SCR_StringHelper.GetIntsFromString(grid, " ");
		if(nums.Count() >= 2)
		{
			x = nums[0];
			y = nums[1];
		}
	}
};