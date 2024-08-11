#include "Game/PGameEngine.h"
// Note on smart pointers:
// - Shared pointer: Shares ownership across all references.
// - Unique pointer: Does not share ownership with anything.
// - Weak pointer: Has no ownership over any references.

int main(int argc, char* argv[])
{
	int result = 0;
	if (!PGameEngine::GetGameEngine()->Run())
	{
		result = -1;
	}
	PGameEngine::DestroyEngine();
	return result;
}