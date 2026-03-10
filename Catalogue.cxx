// Custom Catalogue.cxx for Notepad2 -- only links the lexers that are
// actually compiled into the project (derived from lexlink.js).
// This replaces scintilla/src/Catalogue.cxx in the CMake build.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <vector>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "LexerModule.h"
#include "Catalogue.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

static std::vector<LexerModule *> lexerCatalogue;
static int nextLanguage = SCLEX_AUTOMATIC+1;

const LexerModule *Catalogue::Find(int language) {
	Scintilla_LinkLexers();
	for (std::vector<LexerModule *>::iterator it=lexerCatalogue.begin();
		it != lexerCatalogue.end(); ++it) {
		if ((*it)->GetLanguage() == language) {
			return *it;
		}
	}
	return 0;
}

const LexerModule *Catalogue::Find(const char *languageName) {
	Scintilla_LinkLexers();
	if (languageName) {
		for (std::vector<LexerModule *>::iterator it=lexerCatalogue.begin();
			it != lexerCatalogue.end(); ++it) {
			if ((*it)->languageName && (0 == strcmp((*it)->languageName, languageName))) {
				return *it;
			}
		}
	}
	return 0;
}

void Catalogue::AddLexerModule(LexerModule *plm) {
	if (plm->GetLanguage() == SCLEX_AUTOMATIC) {
		plm->language = nextLanguage;
		nextLanguage++;
	}
	lexerCatalogue.push_back(plm);
}

int wxForceScintillaLexers(void) {
	return Scintilla_LinkLexers();
}

#define LINK_LEXER(lexer) extern LexerModule lexer; Catalogue::AddLexerModule(&lexer);

int Scintilla_LinkLexers() {
	static int initialised = 0;
	if (initialised)
		return 0;
	initialised = 1;

	// Only the lexers compiled by Notepad2 (see lexlink.js)
	LINK_LEXER(lmAsm);
	LINK_LEXER(lmBatch);
	LINK_LEXER(lmConf);
	LINK_LEXER(lmCPP);
	LINK_LEXER(lmCss);
	LINK_LEXER(lmDiff);
	LINK_LEXER(lmHTML);
	LINK_LEXER(lmMake);
	LINK_LEXER(lmNull);
	LINK_LEXER(lmPascal);
	LINK_LEXER(lmPerl);
	LINK_LEXER(lmPowerShell);
	LINK_LEXER(lmProps);
	LINK_LEXER(lmPython);
	LINK_LEXER(lmSQL);
	LINK_LEXER(lmVB);
	LINK_LEXER(lmVBScript);
	LINK_LEXER(lmXML);

	return 1;
}
