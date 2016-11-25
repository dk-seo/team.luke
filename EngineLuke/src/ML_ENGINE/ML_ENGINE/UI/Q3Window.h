#pragma once
class Q3Window
{
public:
	Q3Window();
	~Q3Window();

	bool IsActive() const { return _active; }
	void SetActive(bool active) { _active = active; }

	void Update();

private:
	bool _active;
};
