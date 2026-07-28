#pragma once

namespace InputBindingsStore
{
	// Resets to defaults, then overlays whatever the file supplies. A missing file is not
	// an error: it means defaults. A malformed file is logged and also means defaults.
	void Load();

	void Save();
}
