#ifndef __H_MODULE_TRUEFONT__
#define __H_MODULE_TRUEFONT__

#include <unordered_map>
#include <SDL_ttf.h>

#include "Module.h"

class ModuleTrueFont : public Module
{
public:
	ModuleTrueFont();
	virtual ~ModuleTrueFont();

	bool Init();
    bool CleanUp();

	/* Doesn't actually load the fonts, but verifies if the font is usable and stores it as a usable font for UI_LABEL components */
    bool RegisterFont(const char* name, const char* path);

	const std::vector<std::string>& GetAvailableFonts() const;
	TTF_Font* GetFont(const std::string& name, int size);
	bool ReleaseFont(TTF_Font* font);

private:
	struct SizeInfo {
		unsigned int numRefs = 0;
		TTF_Font* font = nullptr;
	};
	struct FontInfo {
		std::string path;
		std::unordered_map<int, SizeInfo> loadedSizes;
	};

	std::vector<std::string> loadedFontsNames;
	std::unordered_map<std::string, FontInfo> fontsNamesToFontInfo;

};

#endif __H_MODULE_TRUEFONT__
