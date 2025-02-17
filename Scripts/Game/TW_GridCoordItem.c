class TW_GridCoordItem<Class T>
{
	int x;
	int y;
	
	private T item;
	
	void TW_GridCoordItem(int x, int y, T item)
	{
		this.x = x;
		this.y = y;
		this.item = item;
	}
	
	T GetItem() { return item; }
	void SetItem(T item) { this.item = item; }
}