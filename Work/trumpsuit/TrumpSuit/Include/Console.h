#pragma once

class CCard;

class Console
{
public:
	static void			DisplayMessage(std::string msg);
	static void			DisplayError(std::string msg);
	static void			DisplayWarning(std::string msg);
	static void			DisplayCard(CCard* pCard);
	static void			NewLine();

	static int			ReadInteger();
	static std::string	ReadString();
};