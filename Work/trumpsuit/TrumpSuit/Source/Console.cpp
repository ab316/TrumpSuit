#include "Stdafx.h"
#include "Card.h"
#include "Console.h"


void Console::DisplayMessage(std::string str)
{
	std::cout << str;
}


void Console::DisplayError(std::string str)
{
	std::cout << "ERROR: " << str << std::endl;
}


void Console::DisplayWarning(std::string str)
{
	std::cout << "WARNING: " << str << std::endl;
}


void Console::DisplayCard(CCard* pCard)
{
	switch (pCard->GetValue())
	{
	case VALUE_2:
	case VALUE_3:
	case VALUE_4:
	case VALUE_5:
	case VALUE_6:
	case VALUE_7:
	case VALUE_8:
	case VALUE_9:
	case VALUE_10:
		std::cout << (unsigned int)pCard->GetValue() + 2;
		break;
		
	case VALUE_J:
		std::cout << "jack";
		break;

	case VALUE_Q:
		std::cout << "queen";
		break;

	case VALUE_K:
		std::cout << "king";
		break;

	case VALUE_A:
		std::cout << "Ace";
		break;
	}

	std::cout << " of ";

	switch (pCard->GetSuit())
	{
	case SUIT_SPADE:
		std::cout << "spade";
		break;

	case SUIT_CLUB:
		std::cout << "club";
		break;

	case SUIT_DIAMOND:
		std::cout << "diamond";
		break;

	case SUIT_HEARTS:
		std::cout << "hearts";
		break;
	}

	std::cout << std::endl;
}


void Console::NewLine()
{
	std::cout << std::endl;
}


int Console::ReadInteger()
{
	int a;
	std::cin >> a;
	
	return a;
}


std::string	Console::ReadString()
{
	std::string str;
	std::cin >> str;

	return str;
}