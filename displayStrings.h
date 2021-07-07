#ifndef DISPLAYSTRINGS_H
#define DISPLAYSTRINGS_H

const int scoreLen = 10;
uint8_t scoreBase[scoreLen] = {'S','c','o','r','e',' ','0','0',' ',' '};
uint8_t score[scoreLen] = {'S','c','o','r','e',' ','0','0',' ',' '};

const int twoPEndlessScoreWinnerLen = 17;
uint8_t TwoPEndlessScoreWinnerBase[twoPEndlessScoreWinnerLen] = {'W','i','n','n','e','r',' '};
uint8_t TwoPEndlessScoreWinnerRotator[twoPEndlessScoreWinnerLen] = {'W','i','n','n','e','r',' '};

const int TwoPVSWinnerLen = 8;
uint8_t TwoPVSWinnerBase[TwoPVSWinnerLen] = {' ','W','i','n','n','e','r',' '};
uint8_t TwoPVSWinnerRotator[TwoPVSWinnerLen] = {' ','W','i','n','n','e','r',' '};

const int twoPEndlessScoreLoserLen = 16;
uint8_t TwoPEndlessScoreLoserBase[twoPEndlessScoreLoserLen] = {'L','o','s','e','r',' '};
uint8_t TwoPEndlessScoreLoserRotator[twoPEndlessScoreLoserLen] = {'L','o','s','e','r',' '};

const int TwoPVSLoserLen = 7;
uint8_t TwoPVSLoserBase[TwoPVSLoserLen] = {' ','L','o','s','e','r',' '};
uint8_t TwoPVSLoserRotator[TwoPVSLoserLen] = {' ','L','o','s','e','r',' '};

const int OnePPrintLen = 9;
uint8_t OnePPrintBase[OnePPrintLen] = {' ','E','n','d','l','e','s','s',' '};
uint8_t OnePPrintRotator[OnePPrintLen] = {' ','E','n','d','l','e','s','s',' '};

const int TwoPPrintLen = 12;
uint8_t TwoPPrintBase[TwoPPrintLen] = {' ','2','P',' ','E','n','d','l','e','s','s',' '};
uint8_t TwoPPrintRotator[TwoPPrintLen] = {' ','2','P',' ','E','n','d','l','e','s','s',' '};

const int VSPrintLen = 10;
uint8_t VSPrintBase[VSPrintLen] = {' ','V','S',' ','M','o','d','e',' ',' '};
uint8_t VSPrintRotator[VSPrintLen] = {' ','V','S',' ','M','o','d','e',' ',' '};

const int IDSelPrintLen = 17;
uint8_t IDSelPrintBase[IDSelPrintLen] = {' ','P','1',' ','P','r','e','s','s',' ','c','e','n','t','e','r',' '};
uint8_t IDSelPrintRotator[IDSelPrintLen] = {' ','P','1',' ','P','r','e','s','s',' ','c','e','n','t','e','r',' '};

const int WaitingOnP1Len = 15;
uint8_t WaitingOnP1Base[WaitingOnP1Len] = {' ','W','a','i','t','i','n','g',' ','o','n',' ','P','1',' '};
uint8_t WaitingOnP1Rotator[WaitingOnP1Len] = {' ','W','a','i','t','i','n','g',' ','o','n',' ','P','1',' '};

const int WaitingOnP2Len = 15;
uint8_t WaitingOnP2Base[WaitingOnP1Len] = {' ','W','a','i','t','i','n','g',' ','o','n',' ','P','2',' '};
uint8_t WaitingOnP2Rotator[WaitingOnP1Len] = {' ','W','a','i','t','i','n','g',' ','o','n',' ','P','2',' '};

const int GenningPrintLen = 17;
uint8_t GenningPrintBase[GenningPrintLen] = {' ','E','n','t','e','r',' ','1','5',' ','i','n','p','u','t','s',' '};
uint8_t GenningPrintRotator[GenningPrintLen] = {' ','E','n','t','e','r',' ','1','5',' ','i','n','p','u','t','s',' '};

uint8_t CountDown3[6] = {'R','e','a','d','y',' '};
uint8_t CountDown2[6] = {'S','e','t',' ',' ',' '};
uint8_t CountDown1[6] = {'G','o',' ',' ',' ',' '};

#endif /* DISPLAYSTRINGS_H */