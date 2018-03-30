#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib")
#include <algorithm>
#include "ModuleTrueFont.h"
#include "globals.h"


ModuleTrueFont::ModuleTrueFont()
{
}


ModuleTrueFont::~ModuleTrueFont()
{
}

bool ModuleTrueFont::Init()
{
	return TTF_Init() == 0;
}

bool ModuleTrueFont::CleanUp()
{
	for (std::unordered_map<std::string, FontInfo>::iterator it = fontsNamesToFontInfo.begin(); it != fontsNamesToFontInfo.end(); ++it)
	{
		std::unordered_map<int, SizeInfo>& loadedSizes = it->second.loadedSizes;
		for (std::unordered_map<int, SizeInfo>::iterator itSizes = loadedSizes.begin(); itSizes != loadedSizes.end(); ++itSizes)
		{
			if (itSizes->second.numRefs != 0)
			{
				LOGGER("WARNING: ModuleTrueFont - The font %s with size %i was not released by %u users", it->first.c_str(), itSizes->first, itSizes->second.numRefs);
			}
			TTF_CloseFont(itSizes->second.font);
		}
	}
	loadedFontsNames.clear();
	fontsNamesToFontInfo.clear();

    TTF_Quit();

    return true;
}

bool ModuleTrueFont::RegisterFont(const char* name, const char* path)
{
	if (fontsNamesToFontInfo.count(name) != 0)
		return false;

	bool pathAlreadyUsed = false;
	for (std::unordered_map<std::string, FontInfo>::iterator it = fontsNamesToFontInfo.begin(); it != fontsNamesToFontInfo.end(); ++it)
	{
		if (strcmp(it->second.path.c_str(), path) == 0)
		{
			pathAlreadyUsed = true;
			break;
		}
	}

	if (pathAlreadyUsed)
		return false;

	/* Now we try to load the font with a default size of 28 to see if it is a valid font */
	TTF_Font* testFont = TTF_OpenFont(path, 28);
	if (!testFont)
		return false;

	TTF_CloseFont(testFont);
	testFont = nullptr;

	/* Now we can safely register the font for future use */
	loadedFontsNames.push_back(name);
	FontInfo fontInfo;
	fontInfo.path = path;
	/* fontInfo.loadedSize is left empty */
	fontsNamesToFontInfo[name] = fontInfo;

	return true;
}

const std::vector<std::string>& ModuleTrueFont::GetAvailableFonts() const
{
	return loadedFontsNames;
}

TTF_Font* ModuleTrueFont::GetFont(const std::string& name, int size)
{
	/* Verify that a font with that name is available */
	std::vector<std::string>::iterator it = std::find(loadedFontsNames.begin(), loadedFontsNames.end(), name);
	if (it == loadedFontsNames.end())
		return nullptr;

	FontInfo& fontInfo = fontsNamesToFontInfo[name];

	/* If the size has already been loaded, count the new font use (numRefs) and return the font */
	if (fontInfo.loadedSizes.count(size) == 1)
	{
		SizeInfo& sizeInfo = fontInfo.loadedSizes[size];
		++sizeInfo.numRefs;
		return sizeInfo.font;
	}
	/* If not, create a new entry in loaded fonts */
	else
	{
		SizeInfo newSizeInfo;
		newSizeInfo.numRefs = 1;
		newSizeInfo.font = TTF_OpenFont(fontInfo.path.c_str(), size);
		assert(newSizeInfo.font);
		
		fontInfo.loadedSizes[size] = newSizeInfo;
		return newSizeInfo.font;
	}

	return nullptr;
}

bool ModuleTrueFont::ReleaseFont(TTF_Font* font)
{
	for (std::unordered_map<std::string, FontInfo>::iterator it = fontsNamesToFontInfo.begin(); it != fontsNamesToFontInfo.end(); ++it)
	{
		std::unordered_map<int, SizeInfo>& loadedSizes = it->second.loadedSizes;
		for (std::unordered_map<int, SizeInfo>::iterator itSizes = loadedSizes.begin(); itSizes != loadedSizes.end(); ++itSizes)
		{
			SizeInfo& sizeInfo = itSizes->second;
			if (sizeInfo.font == font)
			{
				--sizeInfo.numRefs;
				/* Close the Font and remove SizeInfo if no longer in use */
				if (sizeInfo.numRefs == 0)
				{
					TTF_CloseFont(font);
					loadedSizes.erase(itSizes);
				}
				return true;
			}
		}
	}
	return false;
}
