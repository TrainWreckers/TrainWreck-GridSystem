class TW_GridCoordArrayBuckets<Class T>
{
	int x;
	int y;
	
	private ref map<int, ref array<T>> buckets = new map<int, ref array<T>>();
	
	void TW_GridCoordArrayBuckets(int x, int y)
	{
		this.x = x;
		this.y = y;
	}
	
	//! Add item to a specific bucket
	void Add(T item, int bucket)
	{
		if(buckets.Contains(bucket))
			buckets.Get(bucket).Insert(item);
		else
		{
			buckets.Insert(bucket, {});
			buckets.Get(bucket).Insert(item);
		}
	}
	
	//! Remove item from specific bucket
	void RemoveItem(T item, int bucket)
	{
		if(!buckets.Contains(bucket))
			return;
		
		buckets.Get(bucket).RemoveItem(item);
	}
	
	//! Remove specific item, with unknown bucket (expensive)
	void RemoveItem(T item)
	{
		if(buckets.IsEmpty())
			return;
		
		foreach(int bucket, ref array<T> items : buckets)
		{
			if(!items.Contains(item))
				continue;
			
			items.RemoveItem(item);
			return;	
		}
	}
	
	//! Retrieve all items from specific bucket
	int GetDataFromBucket(int bucket, notnull out array<T> data)
	{
		if(!buckets.Contains(bucket))
			return;
		
		ref array<T> local = buckets.Get(bucket);
		int count = local.Count();
		
		for(int i = 0; i < count; i++)
			data.Insert(local.Get(i));
		
		return count;
	}
	
	// Retrieve all items associated within this coordinate
	int GetData(notnull out array<T> data)
	{
		int count = 0;
		foreach(int bucket, ref array<T> items : buckets)
		{
			foreach(T item : items)
			{
				data.Insert(item);
				count++;
			}
		}
		
		return count;
	}
}