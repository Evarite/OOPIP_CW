#include "DeliverySystem.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>

namespace DeliverySystem
{
	unsigned long Account::Hash(const std::string& password) //djb2
	{
		unsigned long hash = 5381;
		for (auto c : password)
		{
			hash = (hash << 5) + hash + c;
		}
		return hash;
	}

	Account::Account()
	{
		memset(nickname, '\0', sizeof(nickname));
		memset(firstName, '\0', sizeof(firstName));
		memset(lastName, '\0', sizeof(lastName));
		memset(phoneNumber, '\0', sizeof(phoneNumber));
		password = 0;

		type = Account::Type::User;
	}
	Account::Account(const std::string& nickname, const std::string& firstName, const std::string& lastName,
		const std::string& phoneNumber, const std::string& password)
	{
		strcpy_s(this->nickname, nickname.c_str());
		strcpy_s(this->firstName, firstName.c_str());
		strcpy_s(this->lastName, lastName.c_str());
		strcpy_s(this->phoneNumber, phoneNumber.c_str());

		this->password = Hash(password);

		type = Account::Type::User;
	}

	std::string Account::GetNickname() const
	{
		return std::string(nickname);
	}
	std::string Account::GetFirstName() const
	{
		return std::string(firstName);
	}
	std::string Account::GetLastName() const
	{
		return std::string(lastName);
	}
	std::string Account::GetPhoneNumber() const
	{
		return std::string(phoneNumber);
	}
	Account::Type Account::GetType() const
	{
		return type;
	}
	std::vector<Cargo*> Account::GetCargos() const
	{
		return cargos;
	}

	void Account::SetNickname(const std::string& nickname)
	{
		strcpy_s(this->nickname, nickname.c_str());
	}
	void Account::SetFirstName(const std::string& firstName)
	{
		strcpy_s(this->firstName, firstName.c_str());
	}
	void Account::SetLastName(const std::string& lastName)
	{
		strcpy_s(this->lastName, lastName.c_str());
	}
	void Account::SetPhoneNumber(const std::string& phoneNumber)
	{
		strcpy_s(this->phoneNumber, phoneNumber.c_str());
	}
	void Account::SetType(const Type& type)
	{
		this->type = type;
	}
	void Account::SetPassword(const std::string& password)
	{
		this->password = Hash(password);
	}
	void Account::AddCargo(Cargo* cargo)
	{
		cargos.push_back(cargo);
	}
	void Account::RemoveCargo(Cargo* cargo)
	{
		for (int i = 0; i < cargos.size(); i++)
		{
			if (cargos[i] == cargo)
				cargos.erase(cargos.begin() + i);
		}
	}

	Account* Account::Authorise(std::list<Account>& accounts, const std::list<Country>& countries)
	{
		std::cout << std::setw(40) << "\x1b[33;1m" << "Вітаем вас у мэнэджэры прамысловых перавозак!" << "\x1b[0m"
			<< std::endl;
		std::cout << std::setw(20) <<"\x1b[36;1m" <<
			"Для адмовы на любым этапе аўтарызацыі ці рэгістрацыі ўвядзіце N, каб вярнуцца ў пачатак"
			<< "\x1b[0m" << std::endl;

		int choice;

		authorisation_beginning:
		while(true)
		{
			std::cout << std::endl;
			std::cout << "1. Стварыць акаўнт" << std::endl;
			std::cout << "2. Увайсці ў акаўнт" << std::endl;
			std::cout << "3. Выхад" << std::endl;
			choice = GetIntWithinRange(1, 3);

			switch (choice)
			{
			case 1:
			{
				std::cout << std::endl << std::setw(55) << "\x1b[33;1m" << "Рэгістрацыя" << "\x1b[0m" << std::endl;

				std::string nickname;
				std::string password;
				std::string firstName;
				std::string lastName;
				std::string phoneCode;
				unsigned long long phoneNumber;

				while (true)
				{
					nickname = GetString("Увядзіце імя акаўнту\n", FORBIDDEN_NICKNAME_SYMBOLS, MIN_NAME_SIZE,
						NAME_SIZE - 1, { "N", "n" });

					if (nickname == "N" || nickname == "n")
					{
						goto authorisation_beginning;
					}
					else
					{
						bool isOccupied = false;
						for (const auto& account : accounts)
							if (account.GetNickname() == nickname)
							{
								std::cout << "\x1b[31;1m" << "Імя ўжо занята. Паспрабуйце яшчэ раз"
									<< "\x1b[0m" << std::endl;
								isOccupied = true;
								break;
							}
						if (isOccupied)
							continue;
					}

					while (true)
					{
						std::cout << std::endl << "Увядзіце пароль" << std::endl;
						password = GetPasswordWithAsterisks();

						if (password == "N")
							goto authorisation_beginning;

						if (password.size() < MIN_PASSWORD_SIZE)
						{
							std::cout << "\x1b[31;1m" << "Мінімальны памер пароля: " << MIN_PASSWORD_SIZE
								<< ". Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
							continue;
						}

						std::cout << std::endl << "Паўтарыце пароль" << std::endl;

						if (password != GetPasswordWithAsterisks())
						{
							std::cout << "\x1b[31;1m" << "Паролі не супадаюць. Паспрабуйце яшчэ раз"
								<< "\x1b[0m" << std::endl;
							continue;
						}

						break;
					}

					firstName = GetString("\nУвядзіце ваша імя\n", FORBIDDEN_NAME_SYMBOLS, 1,
						NAME_SIZE - 1, { "N", "n" });

					if (firstName == "N" || firstName == "n")
						goto authorisation_beginning;

					lastName = GetString("\nУвядзіце ваша прозвішча\n", FORBIDDEN_NAME_SYMBOLS, 1, NAME_SIZE - 1,
						{ "N", "n" });

					if (lastName == "N" || lastName == "n")
						goto authorisation_beginning;

					std::cout << std::endl;
					int i = 0;
					for (const auto& country : countries)
						std::cout << ++i << ". " << country.GetName() << std::endl;

					std::cout << "0. Маёй краіны тут няма" << std::endl;

					std::cout << std::endl << "Выбярыце вашу краіну: ";
					choice = GetIntWithinRange(0, countries.size());

					switch (choice)
					{
					case 0:
						std::cout << std::endl
							<< "Прыносім нашыя шкадаванні, але наш сервіс пакуль што не працуе ў вашай краіне"
							<< std::endl;
						goto authorisation_beginning;
						break;
					default:
						while (true)
						{
							auto country = countries.begin();
							std::advance(country, choice - 1);
							phoneCode = country->GetPhoneCode();

							std::cout << std::endl << "Увядзіце ваш нумар тэлефону" << std::endl
								<< phoneCode;
							std::cin >> phoneNumber;

							if (!std::cin.good())
							{
								std::cin.clear();
								std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
									<< std::endl << std::endl;
								std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
								continue;
							}

							break;
						}
					}

					std::cout << std::endl << "Праверце вашыя дадзеныя" << std::endl;
					std::cout << "Імя акаўнту: " << nickname << std::endl
						<< "Уласнае імя: " << firstName << std::endl
						<< "Прозвішча: " << lastName << std::endl
						<< "Нумар тэлефону: " << phoneCode << phoneNumber << std::endl;

					while (true)
					{
						std::cout << std::endl << "Ці ўведзеная інфармацыя з'яўляецца сапраўднай?" << std::endl
							<< "1. Так" << std::setw(20) << "2. Не" << std::endl;

						choice = GetIntWithinRange(1, 2);

						switch (choice)
						{
						case 1:
						{
							std::cout << "\x1b[2J\x1b[1;1H";
							std::cout << std::setw(55) << "\x1b[33;1m" << "Сардэчна запрашаем, "
								<< nickname << '!' << "\x1b[0m" << std::endl;
							accounts.emplace_back(nickname, firstName, lastName,
								phoneCode + std::to_string(phoneNumber), password);
							return &accounts.back();
						}
						case 2:
							goto authorisation_beginning;
							break;
						}
					}

					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}

				break;
			}
			case 2:
			{
				std::cout << std::endl << std::setw(55) << "\x1b[33;1m" << "Аўтарызацыя" << "\x1b[0m" << std::endl;
				while (true)
				{
					std::string name = GetString("Увядзіце імя акаўнту\n");

					if (name == "N")
						goto authorisation_beginning;

					for (auto& account : accounts)
					{
						if (name == account.GetNickname())
						{
							while (true)
							{
								std::cout << std::endl << "Увядзіце пароль" << std::endl;

								std::string password = GetPasswordWithAsterisks();

								if (password == "N")
									goto authorisation_beginning;

								if (Hash(password) == account.password)
								{
									std::cout << "\x1b[2J\x1b[1;1H";
									std::cout << std::setw(47) << "\x1b[33;1m" << "Сардэчна запрашаем, "
										<< account.nickname << '!' << "\x1b[0m" << std::endl;
									return &account;
								}
								else
								{
									std::cout << "\x1b[31;1m" << "Няверны пароль! Паспрабуйце яшчэ раз"
										<< "\x1b[0m" << std::endl;
									continue;
								}
							}
						}
					}

					std::cout << "\x1b[31;1m" << "Акаўнт з такім імём не знойдзены. Паспрабуйце яшчэ раз"
						<< "\x1b[0m" << std::endl << std::endl;
				}

				break;
			}
			case 3: default:
				std::cout << "Да пабачэння" << std::endl;
				return nullptr;
			}
		}
	}

	bool Account::operator==(const Account& other) const
	{
		return std::string(nickname) == other.GetNickname();
	}

	std::ostream& operator<<(std::ostream& os, const Account& obj)
	{
		os << "Імя акаўнта: " << obj.nickname << std::endl;
		os << "Імя карыстальніка: " << obj.firstName << std::endl;
		os << "Прозвішча: " << obj.lastName << std::endl;
		os << "Нумар тэлефона: " << obj.phoneNumber << std::endl;
		os << "Тып акаўнта: " << obj.type << std::endl;
		os << std::endl << "Колькасць бягучых даставак: " << obj.cargos.size();
		
		return os;
	}

	std::ostream& operator<<(std::ofstream& os, const Account& obj)
	{
		os.write(obj.nickname, NAME_SIZE);
		if (!os.good())
		{
			throw std::runtime_error("Не атрымалася выканаць запіс у файл");
		}
		os.write(obj.firstName, NAME_SIZE);
		os.write(obj.lastName, NAME_SIZE);
		os.write(obj.phoneNumber, NAME_SIZE);
		os.write(reinterpret_cast<const char*>(&obj.type), sizeof(obj.type));

		os.flush();

		return os;
	}

	std::istream& operator>>(std::ifstream& is, Account& obj)
	{
		is.read(obj.nickname, NAME_SIZE);
		if (!is.good())
		{
			return is;
		}
		is.read(obj.firstName, NAME_SIZE);
		is.read(obj.lastName, NAME_SIZE);
		is.read(obj.phoneNumber, NAME_SIZE);
		is.read(reinterpret_cast<char*>(&obj.type), sizeof(obj.type));

		return is;
	}
}