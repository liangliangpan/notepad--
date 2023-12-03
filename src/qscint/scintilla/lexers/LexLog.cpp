#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <regex>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "StringCopy.h"
#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "CharacterCategory.h"
#include "LexerModule.h"
#include "OptionSet.h"
#include "SubStyles.h"
#include "DefaultLexer.h"
#include "LexAccessor.h"

#include "UniConversion.h"

using namespace Scintilla;

//const char styleSubable[] = { SCE_P_IDENTIFIER, 0 };

//Default = 0,//����
//Ascii = 1,//Ӣ��
//Keyword = 2, //�ؼ��֣�ֻ����TXTΪĸ���

static LexicalClass lexicalClasses[] = {
	// Lexer Python SCLEX_PYTHON SCE_P_:
	0, "SCE_LOG_DEFAULT", "default", "default",
	1, "SCE_LOG_NUM", "Num", "Num",
	2, "SCE_LOG_DATE", "Date", "Date",
	3, "SCE_LOG_KEYWORD", "keyword", "keyword",
};

enum LOG_STATUS { 
	SCE_LOG_DEFAULT=0, 
	SCE_LOG_NUM , 
	SCE_LOG_DATE, 
	SCE_LOG_KEYWORD, 
	SCE_LOG_IDENTIFIER,
	SCE_LOG_HEX
};

// Options used for LexerPython
struct OptionsTxt {
	bool num;
	bool data;
	bool keyword;

	OptionsTxt() {
		num = true;
		data = true;
		keyword = true;
	}

};


struct OptionSetTxt : public OptionSet<OptionsTxt> {
	OptionSetTxt() {
		DefineProperty("lexer.log.num", &OptionsTxt::num,
			"text Unicode string");

		DefineProperty("lexer.txt.data", &OptionsTxt::data,
			"text literals ascii string");

		DefineProperty("lexer.txt.keyword", &OptionsTxt::keyword,
			"text keyword");
	}
};

class LexLOG :public DefaultLexer
{
	WordList keywords;
	OptionsTxt options;
	OptionSetTxt osTxt;
	CharacterSet setWord;
	CharacterSet setWordStart;
	CharacterSet hexWord;
public:
	explicit LexLOG() :
		DefaultLexer(lexicalClasses, ELEMENTS(lexicalClasses)),
		setWord(CharacterSet::setAlphaNum, "._", 0x80, true),//��ĸ�����֡��»��ߡ�. ���ʵ��ַ�����
		setWordStart(CharacterSet::setAlpha, "_", 0x80, true),
		hexWord(CharacterSet::setDigits, "abcdefABCDEF", 0x80, true)
	{
		
	}
	virtual ~LexLOG() {}

	void SCI_METHOD Release() override {
		delete this;
	}
	int SCI_METHOD Version() const override {
		return lvSubStyles;
	}
	const char *SCI_METHOD PropertyNames() override {
		return osTxt.PropertyNames();
	}
	int SCI_METHOD PropertyType(const char *name) override {
		return osTxt.PropertyType(name);
	}
	const char *SCI_METHOD DescribeProperty(const char *name) override {
		return osTxt.DescribeProperty(name);
	}
	Sci_Position SCI_METHOD PropertySet(const char *key, const char *val) override;

	const char *SCI_METHOD DescribeWordListSets() override {
		return "";
	}
	Sci_Position SCI_METHOD WordListSet(int n, const char *wl) override;

	void SCI_METHOD Lex(Sci_PositionU startPos, Sci_Position length, int initStyle, IDocument *pAccess) override;
	void SCI_METHOD Fold(Sci_PositionU startPos, Sci_Position length, int initStyle, IDocument *pAccess) override;
	void *SCI_METHOD PrivateCall(int, void *) override {
		return 0;
	}
	int SCI_METHOD LineEndTypesSupported() override {
		return SC_LINE_END_TYPE_UNICODE;
	}

	int SCI_METHOD PrimaryStyleFromStyle(int style) override {
		return style;
	}

	int SCI_METHOD DistanceToSecondaryStyles() override {
		return 0;
	}

	static ILexer *LexerFactoryLog() {
		return new LexLOG();
	}
};

Sci_Position SCI_METHOD LexLOG::PropertySet(const char *key, const char *val) {
	if (osTxt.PropertySet(&options, key, val)) {
		return 0;
	}
	return -1;
}

Sci_Position SCI_METHOD LexLOG::WordListSet(int n, const char *wl) {

	WordList *wordListN = 0;
	switch (n) {
	case 0:
		wordListN = &keywords;
		break;
	}
	Sci_Position firstModification = -1;
	if (wordListN) {
		WordList wlNew;
		wlNew.Set(wl);
		if (*wordListN != wlNew) {
			wordListN->Set(wl);
			firstModification = 0;
		}
	}
	return firstModification;
}

const int indicatorWhitespace = 1;

inline bool IsAAsciiChar(int ch) {
	return (ch < 0x80);
}

inline bool IsAWordStart(int ch, bool unicodeIdentifiers) {
	if (ch < 0x80)
		return (isalpha(ch) || ch == '_');

	if (!unicodeIdentifiers)
		return false;

	return IsXidStart(ch);
}

//�Ƿ��ǵ��ʷָ���ţ�����ascii�е����ֺ���ĸ������ַ������� . @ \t �ȣ�����Ϊһ�����ʵķָ���š�
inline bool IsWordSplitChar(int ch) noexcept {
	return (ch < 0x80) && !isalnum(ch);
}

inline bool IsLineEol(int ch) noexcept {
	return ch == '\n' || ch == '\r';
}

//��ȡ�У�����ȡ128���ַ���
static std::string GetLineContents(LexAccessor& styler, Sci_Position start, const int len=128) {
	std::string lineContent;
	Sci_Position i = 0;
	char ch = styler.SafeGetCharAt(start, '\n');
	const Sci_Position endLine = styler.LineEnd(styler.GetLine(start));
	while (((start + i) < endLine) && ( i < len)) {
		const char chNext = styler.SafeGetCharAt(start + i + 1, '\n');
		lineContent += ch;
		i++;
		ch = chNext;
	}
	return lineContent;
}



//ֻʶ�����ĺ�Ӣ�����ֵ��ʵ�״̬
void SCI_METHOD LexLOG::Lex(Sci_PositionU startPos, Sci_Position length, int initStyle, IDocument *pAccess) {
	Accessor styler(pAccess, NULL);

	const Sci_Position endPos = startPos + length;

	StyleContext sc(startPos, endPos - startPos, initStyle, styler);

	std::regex patternData ("\\d{4}[-/]\\d{2}[-/]\\d{2} \\d{2}:\\d{2}:\\d{2}[.:]\\d{1,3}");

	Sci_Position startIndicator = sc.currentPos;

	bool isDataMatch = false;

	for (; sc.More();) {
		//��������п�ͷ�����ȡ64�ֽڣ�ʶ������е����ڡ�
		if (sc.atLineStart) {
			std::string lineContens = GetLineContents(styler, sc.currentPos,64);
			//ƥ�����������ʱ��ֵ
			std::smatch result;
			std::string::const_iterator iterStart = lineContens.begin();
			std::string::const_iterator iterEnd = lineContens.end();

			//ƥ�䵽����
			if (std::regex_search(iterStart, iterEnd, result, patternData))
			{
				int start = result[0].first - iterStart;
				int end = result[0].second - iterStart;

				int i = 0;
				while (i < start)
				{
					sc.Forward();
					++i;
				}
				sc.SetState(SCE_LOG_DATE);
				while (i < end)
				{
					sc.Forward();
					++i;
				}
				sc.SetState(SCE_LOG_DEFAULT);
			}
		}

		/*if (!sc.atLineEnd && !setWordStart.Contains(sc.chPrev) && setWordStart.Contains(sc.ch)) {
			sc.SetState(SCE_LOG_DEFAULT);
			sc.ChangeState(SCE_LOG_IDENTIFIER);
		}*/
		switch (sc.state)
		{
		case SCE_LOG_HEX:
		{
			if (!hexWord.Contains(sc.ch))
			{
				sc.ChangeState(SCE_LOG_NUM);//16���ƺ�10����һ���ĸ�ʽ
				sc.SetState(SCE_LOG_DEFAULT);
			}
		}
			break;
			case SCE_LOG_NUM:
			{
				if (!IsADigit(sc.ch))
				{
					sc.SetState(SCE_LOG_DEFAULT);
				}
			}
			break;

			case SCE_LOG_DEFAULT:
			{
				//ע��˳��Ҫ���ж����֡�
				if (sc.Match('0','x'))
				{
					sc.SetState(SCE_LOG_HEX);
					sc.Forward();
				}
				else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
					//�����ǰ�ַ������֣�������.���֣���ֱ�����õ�ǰ״̬Ϊ���֡�
					sc.SetState(SCE_LOG_NUM);
				}
				else if (!sc.atLineEnd && setWordStart.Contains(sc.ch)) {
					//���������β�����ҵ�ǰ�ַ���һ�����ʵĿ�ͷ��ʶ�ַ���������ʶ��ʶ��״̬��
					sc.SetState(SCE_LOG_IDENTIFIER);
				}
			}
			break;
			case SCE_LOG_IDENTIFIER:
			{
				if (sc.atLineStart || sc.atLineEnd || !setWord.Contains(sc.ch))
				{
					char s[1000];
					//ȡ����ǰ��ʶ����ע���ʶ�����ᳬ��1000������Ԥ�ƣ���������ɵ��ȡ��һ������1000���ַ���������
					sc.GetCurrentLowered(s, sizeof(s));

					if (keywords.InList(s)) {
						sc.ChangeState(SCE_LOG_KEYWORD);
					}
					else
					{
						sc.ChangeState(SCE_LOG_DEFAULT);
					}

					sc.SetState(SCE_LOG_DEFAULT);
				}
			}
		}

		sc.Forward();
	}
	
	styler.IndicatorFill(startIndicator, sc.currentPos, indicatorWhitespace, 0);
	sc.Complete();

}

static bool IsCommentLine(Sci_Position line, Accessor &styler) {
	Sci_Position pos = styler.LineStart(line);
	const Sci_Position eol_pos = styler.LineStart(line + 1) - 1;
	for (Sci_Position i = pos; i < eol_pos; i++) {
		const char ch = styler[i];
		if (ch == '#')
			return true;
		else if (ch != ' ' && ch != '\t')
			return false;
	}
	return false;
}

static bool IsQuoteLine(Sci_Position line, const Accessor &styler) {
	const int style = styler.StyleAt(styler.LineStart(line)) & 31;
	return false;
}


//�������κ��۵�
void SCI_METHOD LexLOG::Fold(Sci_PositionU startPos, Sci_Position length, int /*initStyle - unused*/, IDocument *pAccess) {
	return;
}

static const char *const txtWordListDesc[] = {
	0
};

LexerModule lmLog(SCLEX_LOG, LexLOG::LexerFactoryLog, "log", txtWordListDesc);
