#include "DeliverySystem.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#define NOMINMAX
#include <Windows.h>
#include <conio.h>
#include <sstream>
//#include <magic_enum.hpp>

constexpr auto COUNTRIES = "Countries.dat";
constexpr auto ACCOUNTS = "Accounts.dat";
constexpr auto CARGOS = "Cargos.dat";
constexpr auto DELIVERIES = "Deliveries.dat";
constexpr auto LORRIES = "Lorries.dat";
constexpr auto TRAILERS = "Trailers.dat";
constexpr auto DRIVERS = "Drivers.dat";
constexpr auto APPLICATIONS = "Applications.dat";
constexpr auto REPORT = "Report.txt";

namespace DeliverySystem
{
	Account* Manager::FindAccount(const std::string& nickname)
	{
		for (auto& account : accounts)
		{
			if (nickname == account.GetNickname())
				return &account;
		}

		throw std::runtime_error("Акаўнт з імём " + nickname + " не зноёдзены");
	}

	Driver* Manager::FindDriver(const std::string& nickname)
	{
		for (auto& driver : drivers)
		{
			if (driver.GetAccount()->GetNickname() == nickname)
				return &driver;
		}

		throw std::runtime_error("Кіроўца з імём " + nickname + " не зноёдзены");
	}

	void Manager::Initialise()
	{
		std::ifstream countriesFile(COUNTRIES, std::ios::binary);
		Country countryBuf;
		while (countriesFile >> countryBuf)
			countries.push_back(countryBuf);
		countriesFile.close();

		std::ifstream accountsFile(ACCOUNTS, std::ios::binary);
		Account accBuf;
		while (accountsFile >> accBuf)
			accounts.push_back(accBuf);
		accountsFile.close();

		std::ifstream lorriesFile(LORRIES, std::ios::binary);
		Lorry lorryBuf;
		while (lorriesFile >> lorryBuf)
			lorries.push_back(lorryBuf);
		lorriesFile.close();

		std::ifstream driversFile(DRIVERS, std::ios::binary);
		Driver driverBuf;
		while (driversFile >> driverBuf)
		{
			drivers.push_back(driverBuf);
			driverBuf.GetLorry()->SetOwner(&drivers.back());
		}
		driversFile.close();

		std::ifstream cargosFile(CARGOS, std::ios::binary);
		Cargo cargoBuf;
		while (cargosFile >> cargoBuf)
		{
			cargos.push_back(cargoBuf);
			if (cargoBuf.GetClient() != nullptr)
				cargoBuf.GetClient()->AddCargo(&cargos.back());
		}
		cargosFile.close();

		std::ifstream trailersFile(TRAILERS, std::ios::binary);
		while (trailersFile.peek() != EOF)
		{
			Trailer::Type type;
			trailersFile.read(reinterpret_cast<char*>(&type), sizeof(type));

			std::unique_ptr<Trailer> trailer;
			switch (type)
			{
			case Trailer::Type::Car:
				trailer = std::make_unique<CarTrailer>();
				break;
			case Trailer::Type::Tank:
				trailer = std::make_unique<TankTrailer>();
				break;
			case Trailer::Type::Timber:
				trailer = std::make_unique<TimberTrailer>();
				break;
			case Trailer::Type::Lowboy:
				trailer = std::make_unique<LowboyTrailer>();
				break;
			case Trailer::Type::Tarp:
				trailer = std::make_unique<TarpTrailer>();
				break;
			case Trailer::Type::Refrigerated:
				trailer = std::make_unique<RefrigeratedTrailer>();
				break;
			default:
				throw std::runtime_error("Невядомы тып трэйлера");
			}
			trailer->InitialiseType(type);
			trailersFile >> *trailer;

			trailers.push_back(std::move(trailer));
		}
		trailersFile.close();

		std::ifstream deliveriesFile(DELIVERIES, std::ios::binary);
		Delivery deliveryBuf;
		while (deliveriesFile >> deliveryBuf)
		{
			deliveries.push_back(deliveryBuf);
			deliveryBuf.GetDriver()->SetDelivery(&deliveries.back());
			deliveryBuf.GetLorry()->SetDelivery(&deliveries.back());
			deliveryBuf.GetCargo()->SetDelivery(&deliveries.back());
			deliveryBuf.GetTrailer()->SetDelivery(&deliveries.back());
		}
		deliveriesFile.close();

		std::ifstream applicationsFile(APPLICATIONS, std::ios::binary);
		Application applicationBuf;
		while (applicationsFile >> applicationBuf)
			applications.push_back(applicationBuf);
		applicationsFile.close();
	}

	void Manager::Authorise()
	{
		account = Account::Authorise(accounts, countries);
	}

	void Manager::Run()
	{
		EnableAnsiColors();
		Initialise();
		Authorise();
		if (account == nullptr)
		{
			Manager::UpdateFiles();
			return;
		}

		Menu();
		UpdateFiles();
	}

	void Manager::UpdateFiles()
	{
		std::ofstream countriesFile(COUNTRIES, std::ios::binary);
		for (const auto& country : countries)
			countriesFile << country;
		countriesFile.close();

		std::ofstream accountsFile(ACCOUNTS, std::ios::binary);
		for (const auto& account : accounts)
			accountsFile << account;
		accountsFile.close();

		std::ofstream lorriesFile(LORRIES, std::ios::binary);
		for (const auto& lorry : lorries)
			lorriesFile << lorry;
		lorriesFile.close();

		std::ofstream driversFile(DRIVERS, std::ios::binary);
		for (const auto& driver : drivers)
			driversFile << driver;
		driversFile.close();

		std::ofstream cargosFile(CARGOS, std::ios::binary);
		for (const auto& cargo : cargos)
			cargosFile << cargo;
		cargosFile.close();

		std::ofstream trailersFile(TRAILERS, std::ios::binary);
		for (const auto& trailer : trailers)
			trailersFile << *trailer;
		trailersFile.close();

		std::ofstream deliveriesFile(DELIVERIES, std::ios::binary);
		for (const auto& delivery : deliveries)
			deliveriesFile << delivery;
		deliveriesFile.close();

		std::ofstream applicationsFile(APPLICATIONS, std::ios::binary);
		for (const auto& application : applications)
			applicationsFile << application;
		applicationsFile.close();
	}

	//All
	void Manager::EditAccount()
	{
		int choice = ShowMenuWithNavigation({
			"Імя акаўнту",
			"Пароль",
			"Уласнае імя",
			"Прозвішча",
			"Нумар тэлефону",
			"Выхад"
			}, "Што хаціце адрэдагаваць?");

		if (choice == 6) return;

		switch (choice)
		{
		case 1:
		{
			std::string nickname;

			while (true)
			{
				std::cout << "Увядзіце новае імя акаўнту\n";
				std::getline(std::cin, nickname);
				nickname = TrimWhitespace(nickname);

				if (nickname.size() < MIN_NAME_SIZE)
				{
					std::cout << "\x1b[31;1m" << "Мінімальны памер імя: " << MIN_NAME_SIZE
						<< ". Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
				}
				else if (nickname.size() > NAME_SIZE - 1)
				{
					std::cout << "\x1b[31;1m" << "Максімальны памер імя: " << NAME_SIZE - 1
						<< ". Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
				}
				else if (nickname.find_first_of(FORBIDDEN_NICKNAME_SYMBOLS) != std::string::npos)
				{
					std::cout << "\x1b[31;1m" << "Імя не можа мець наступныя сімвалы: "
						<< FORBIDDEN_NICKNAME_SYMBOLS << std::endl;
					std::cout << "Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;

					continue;
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

				break;
			}

			account->SetNickname(nickname);

			break;
		}
		case 2:
		{
			std::string password;
			while (true)
			{
				std::cout << std::endl << "Увядзіце новы пароль" << std::endl;
				password = GetPasswordWithAsterisks();

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

				account->SetPassword(password);

				break;
			}
			break;
		}
		case 3:
		{
			std::string firstName;
			while (true)
			{
				std::cout << std::endl << "Увядзіце ваша новае імя" << std::endl;
				std::getline(std::cin, firstName);

				if (firstName.size() > NAME_SIZE)
				{
					std::cout << "\x1b[31;1m" << "Максімальны памер імя: " << NAME_SIZE - 1
						<< ". Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
				}
				else if (firstName.find_first_of(FORBIDDEN_NAME_SYMBOLS) != std::string::npos)
				{
					std::cout << "\x1b[31;1m" << "Некарэктны фармат імя. Паспрабуйце яшчэ раз"
						<< "\x1b[0m" << std::endl;

					continue;
				}

				account->SetFirstName(firstName);

				break;
			}
			break;
		}
		case 4:
		{
			std::string lastName;
			while (true)
			{
				std::cout << std::endl << "Увядзіце ваша прозвішча" << std::endl;
				std::getline(std::cin, lastName);

				if (lastName.size() > NAME_SIZE)
				{
					std::cout << "\x1b[31;1m" << "Максімальны памер прозвішча: " << NAME_SIZE - 1
						<< ". Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
				}
				else if (lastName.find_first_of(FORBIDDEN_NAME_SYMBOLS) != std::string::npos)
				{
					std::cout << "\x1b[31;1m" << "Некарэктны фармат прозвишча. Паспрабуйце яшчэ раз"
						<< "\x1b[0m" << std::endl;

					continue;
				}

				account->SetLastName(lastName);

				break;
			}
			break;
		}
		case 5:
		{
			std::string phoneCode;
			unsigned long long phoneNumber;

			std::cout << std::endl;
			int i = 0;
			for (const auto& country : countries)
			{
				std::cout << ++i << ". " << country.GetName() << std::endl;
			}

			int countryChoice = GetIntWithinRange(1, countries.size(), "Выбярыце вашу краіну: ");

			while (true)
			{
				auto country = countries.begin();
				std::advance(country, countryChoice);
				phoneCode = country->GetPhoneCode();

				std::cout << std::endl << "Увядзіце ваш нумар тэлефону" << std::endl
					<< phoneCode;
				std::cin >> phoneNumber;

				if (!std::cin.good())
				{
					std::cin.clear();
					std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз"
						<< "\x1b[0m" << std::endl << std::endl;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				account->SetPhoneNumber(phoneCode + std::to_string(phoneNumber));

				break;
			}
		}
		}
	}
	void Manager::UpdateDistance()
	{
		std::cout << "\nАднаўляюцца адлегласці даставак...\n";

		std::vector<Delivery*> deliveriesCopy;
		for (auto& delivery : deliveries)
			deliveriesCopy.push_back(&delivery);

		for (auto& delivery : deliveriesCopy)
			delivery->UpdateDistance(deliveries, cargos);

		std::cout << "\nАднаўленне паспяхова скончана\n";
	}

	//User
	void Manager::UserRequestDelivery()
	{
		std::vector<Cargo*> availableCargos;
		std::vector<City*> availableCities;

		while (true)
		{
			int i = 0;
			TablePrinter table(Cargo::GetHeaders());
			for (auto& cargo : cargos)
			{
				if (cargo.GetCurrentDelivery() == nullptr)
				{
					table.AddRow(cargo);
					availableCargos.push_back(&cargo);
				}
			}
			std::cout << table << '\n';

			if (i == 0)
			{
				std::cout << "\x1b[31;1m" << "Няма даступных грузаў" << "\x1b[0m" << std::endl;
				break;
			}
			else
			{
				int choiceCargo = GetIntWithinRange(1, i, "Выбярыце груз: ");

				while (true)
				{
					int j = 0;

					TablePrinter cityTable(City::GetHeaders());
					std::cout << std::endl << std::endl;
					for (auto& country : countries)
					{
						for (auto& city : country.GetCitiesL())
						{
							cityTable.AddRow(city);
							availableCities.push_back(&city);
						}
					}
					std::cout << cityTable << '\n';
					if (j == 0)
					{
						std::cout << "\x1b[31;1m" << "Няма даступных гарадоў" << "\x1b[0m" << std::endl;
						break;
					}
					else
					{
						int choiceCity = GetIntWithinRange(1, j, "Выбярыце горад: ");

						availableCargos[choiceCargo - 1]->RequestDelivery(account, availableCities[choiceCity - 1]);

						std::cout << "Заказ сфарміраваны. Дастаўка пачнецца ў бліжайшы час." << std::endl;

						return;
					}
				}

				break;
			}
		}
	}
	void Manager::UserShowCargos()
	{
		if (cargos.empty())
		{
			std::cout << "Няма актыўных заказаў" << std::endl;
			return;
		}
		int i = 0;

		TablePrinter table(account->GetCargos());
		std::cout << table << '\n';
	}
	void Manager::ShowSupportedArea()
	{
		TablePrinter table(countries);
		std::cout << table << '\n';
	}
	void Manager::BecomeADriver()
	{
		if (!applications.empty() && applications.back().GetAccount() == account)
		{
			int choice = ShowMenuWithNavigation({ "Так", "Не" }, "Вы ўжо адправілі заяўку. Жадаеце выдаліць яе?");

			switch (choice)
			{
			case 1:
				applications.pop_back();
				std::cout << "\nЗаяўка паспяхова выдалена\n";
				return;
			case 2:
				std::cout << '\n';
				return;
			}
		}

		char message[MESSAGE_SIZE];
		std::cout << "Дадайце паведамленне да вашай заўцы\n";
		std::cin.getline(message, MESSAGE_SIZE);

		applications.emplace_back(account, message);

		std::cout << "\nВаша заяўка паспяхова сфарміравана\n";
	}
	void Manager::UserSort()
	{
		enum class SortContainer
		{
			Countries,
			Cities,
			Cargos,
			Deliveries
		};
		enum class SortOrder
		{
			Ascending,
			Descending
		};

		SortContainer container;
		int choice = ShowMenuWithNavigation({ "Спіс краін", "Спіс гарадоў", "Спіс грузаў", "Спіс даставак" },
			"Выбярыце, што вы жадаеце адсартаваць");

		container = static_cast<SortContainer>(choice - 1);

		switch (container)
		{
		case SortContainer::Countries:
		{
			enum class SortAttribute
			{
				Name,
				Code
			};
			SortAttribute attribute;

			choice = ShowMenuWithNavigation({ "Назва краіны", "Тэлефонны код краіны" }, "Выбярыце атрыбут сартавання");

			attribute = static_cast<SortAttribute>(choice - 1);

			SortOrder order;

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");

			order = static_cast<SortOrder>(choice - 1);

			countries.sort([attribute, order](const Country& a, const Country& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Name:
								return a.GetName() < b.GetName();
							case SortAttribute::Code:
								return a.GetPhoneCode() < b.GetPhoneCode();
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Cities:
		{
			enum class SortAttribute
			{
				Name,
				CountryAbbreviation
			};

			SortAttribute attribute;
			choice = ShowMenuWithNavigation({ "Назва горада", "Абрэвіатура краіны" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			SortOrder order;
			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			for (auto& country : countries)
			{
				country.GetCitiesL().sort([attribute, order](const City& a, const City& b)
					{
						auto compare = [attribute, &a, &b]() -> bool
							{
								switch (attribute)
								{
								case SortAttribute::Name:
									return a.GetName() < b.GetName();
								case SortAttribute::CountryAbbreviation:
									return a.GetCountryAbbreviation() < b.GetCountryAbbreviation();
								}
							};

						return order == SortOrder::Ascending ? compare() : !compare();
					}
				);
			}

			return;
		}
		case SortContainer::Cargos:
		{
			enum class SortAttribute
			{
				Name,
				Mass,
				CityFrom,
				CityTo,
				Type
			};

			SortAttribute attribute;
			choice = ShowMenuWithNavigation({ "Назва груза", "Маса груза", "Горад адпраўлення", "Горад прыбыцця",
				"Тып груза" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			SortOrder order;
			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			cargos.sort([attribute, order](const Cargo& a, const Cargo& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Name:
								return a.GetName() < b.GetName();
							case SortAttribute::Mass:
								return a.GetMass() < b.GetMass();
							case SortAttribute::CityFrom:
								return a.GetCityFrom()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::CityTo:
								if (a.GetCityTo() == nullptr)
									return true;
								else if (b.GetCityTo() == nullptr)
									return false;
								else
									return a.GetCityTo()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::Type:
								return static_cast<int>(a.GetType()) < static_cast<int>(b.GetType());
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			break;
		}
		case SortContainer::Deliveries:
		{
			enum class SortAttribute
			{
				DriverName,
				CargoName,
				CargoMass,
				CargoCityFrom,
				CargoCityTo,
				CargoType
			};

			SortAttribute attribute;
			choice = ShowMenuWithNavigation({ "Імя кіроўцы", "Назва груза", "Маса груза", "Горад адпраўлення",
				"Горад прыбыцця", "Тып груза" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			SortOrder order;
			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			deliveries.sort([attribute, order](const Delivery& a, const Delivery& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::DriverName:
								return a.GetDriver()->GetAccount()->GetNickname()
									< b.GetDriver()->GetAccount()->GetNickname();
							case SortAttribute::CargoName:
								return a.GetCargo()->GetName() < b.GetCargo()->GetName();
							case SortAttribute::CargoMass:
								return a.GetCargo()->GetMass() < b.GetCargo()->GetMass();
							case SortAttribute::CargoCityFrom:
								return a.GetCityFrom()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::CargoCityTo:
								if (a.GetCityTo() == nullptr)
									return true;
								else if (b.GetCityTo() == nullptr)
									return false;
								else
									return a.GetCityTo()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::CargoType:
								return static_cast<int>(a.GetCargo()->GetType())
									< static_cast<int>(b.GetCargo()->GetType());
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		}
	}

	//Driver
	void Manager::AcceptDelivery(Driver* driver)
	{
		std::vector<Trailer*> availableTrailers;
		std::vector<Cargo*> availableCargos;

		std::cout << "Даступныя грузы:\n";
		int i = 0;
		for (auto& cargo : cargos)
			if (cargo.GetClient() != nullptr && cargo.GetCurrentDelivery() == nullptr)
			{
				std::cout << ++i << ".\n" << cargo << "\n\n";
				availableCargos.push_back(&cargo);
			}

		if (availableCargos.empty())
		{
			std::cout << "Няма даступных грузаў. Звярніцеся да мадэратараў\n";
			return;
		}

		int choiceCargo = GetIntWithinRange(0, availableCargos.size(), "Выбярыце груз (0 для адмовы): ");

		if (choiceCargo == 0)
			return;

		std::cout << "Даступныя прычэпы для гэтага тыпу грузу:\n" << std::endl;
		int j = 0;
		for (auto& trailer : trailers)
		{
			if (trailer->GetCurrentDelivery() == nullptr && trailer->IsCargoSupported(availableCargos[choiceCargo - 1])
				&& trailer->GetMaxPayload() >= availableCargos[choiceCargo - 1]->GetMass())
			{
				availableTrailers.push_back(trailer.get());
				std::cout << ++j << ".\n" << *trailer << "\n\n";
			}
		}
		if (availableTrailers.empty())
		{
			std::cout << "Няма даступных прычэпаў. Звярніцеся да мадэратараў\n";
			return;
		}

		int choiceTrailer = GetIntWithinRange(0, availableTrailers.size(), "Выбярыце прычэп (0 для адмовы): ");

		if (choiceTrailer == 0)
			return;

		deliveries.emplace_back(driver, driver->GetLorry(), availableCargos[choiceCargo - 1],
			availableTrailers[choiceTrailer - 1]);

	}
	void Manager::DriverQuit(Driver* driver)
	{
		int choice = ShowMenuWithNavigation({ "Так", "Не" }, "Гэта дзеянне нельга будзе адмовіць. Вы ўпэўнены?");

		switch (choice)
		{
		case 1:
			driver->GetLorry()->SetOwner(nullptr);
			for (auto i = drivers.begin(); i != drivers.end();)
			{
				if (*driver == *i)
				{
					drivers.erase(i);
					break;
				}

				std::advance(i, 1);
			}

			account->SetType(Account::Type::User);
			std::cout << "Вы звольнены." << std::endl;
			return;
		case 2:
			return;
		}
	}
	void Manager::ShowCurrentDelivery(Driver* driver)
	{
		std::cout << *driver->GetCurrentDelivery() << '\n';
	}
	void Manager::DropDelivery(Driver* driver)
	{
		int choice = ShowMenuWithNavigation({ "Так", "Не" },
			"Гэта дзеянне нельга будзе адмовіць. За няўстойкі вы будзеце аштрафаваны\nВы ўпэўнены?");

		switch (choice)
		{
		case 1:
			driver->CancelDelivery(deliveries);
			return;
		case 2:
			return;
		}
	}
	void Manager::DriverSort()
		///Sorts cargo only
	{
		enum class SortOrder
		{
			Ascending,
			Descending
		};
		enum class SortAttribute
		{
			Name,
			Mass,
			CityFrom,
			CityTo,
			Type
		};

		SortAttribute attribute;
		SortOrder order;
		int choice = ShowMenuWithNavigation({ "Назва груза", "Маса груза", "Горад адпраўлення", "Горад прыбыцця",
			"Тып груза" }, "Выбярыце атрыбут сартавання");
		attribute = static_cast<SortAttribute>(choice - 1);

		choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
		order = static_cast<SortOrder>(choice - 1);

		cargos.sort([attribute, order](const Cargo& a, const Cargo& b)
			{
				auto compare = [attribute, &a, &b]() -> bool
					{
						switch (attribute)
						{
						case SortAttribute::Name:
							return a.GetName() < b.GetName();
						case SortAttribute::Mass:
							return a.GetMass() < b.GetMass();
						case SortAttribute::CityFrom:
							return a.GetCityFrom()->GetName() < b.GetCityFrom()->GetName();
						case SortAttribute::CityTo:
							if (a.GetCityTo() == nullptr)
								return true;
							else if (b.GetCityTo() == nullptr)
								return false;
							else
								return a.GetCityTo()->GetName() < b.GetCityFrom()->GetName();
						case SortAttribute::Type:
							return static_cast<int>(a.GetType()) < static_cast<int>(b.GetType());
						}
					};

				return order == SortOrder::Ascending ? compare() : !compare();
			}
		);
	}

	//Moderator
	void Manager::ShowAllUsers()
	{
		TablePrinter table(Account::GetHeaders());
		for (const auto& account : accounts)
			if (account.GetType() == Account::Type::User)
				table.AddRow(account);
	}
	void Manager::ShowAllDrivers()
	{
		TablePrinter table(Account::GetHeaders());
		for (const auto& account : accounts)
			if (account.GetType() == Account::Type::Driver)
				table.AddRow(account);
	}
	void Manager::CargosList()
	{
		std::cout << "\nСпіс грузаў:\n";

		TablePrinter cargosT(cargos);
		std::cout << cargosT << '\n';

		int choice = ShowMenuWithNavigation({ "Дадаць груз", "Выдаліць груз", "Выхад" }, "Выбярыце пункт меню");

		switch (choice)
		{
		case 1:
		{
			std::string name;
			float mass;
			int typeChoice;
			City* from = nullptr;

			std::cout << "Увядзіце назву грузу: ";
			std::getline(std::cin, name);

			mass = GetFloat("Увядзіце масу грузу: ");

			typeChoice = ShowMenuWithNavigation({ "Драўніна", "Ежа", "Аўтамабілі", "Паліва", "Хімічныя рэчыва",
				"Малако", "Жвір, друз", "Крупы", "Пясок", "Бетон", "Сталёвыя канструкцыі", "Цэгла",
				"Прамысловае абсталяванне", "Будаўнічая тэхніка", "Кантэйнеры", "Выбуховыя рэчы",
				"Таксічныя матэрыялы", "Замарожаныя прадукты", "Медыкаменты" }, "Выбярыце тып грузу");

			std::cout << "\nДаступныя гарады:\n";
			int j = 0;
			std::vector<City*> availableCities;
			for (auto& country : countries)
			{
				for (auto& city : country.GetCitiesL())
				{
					std::cout << ++j << ". " << city.GetName() << "\t" << city.GetCountryAbbreviation() << std::endl;
					availableCities.push_back(&city);
				}
			}

			if (availableCities.empty())
			{
				std::cout << "\x1b[31;1m" << "Няма даступных гарадоў" << "\x1b[0m" << "\n";
				return;
			}

			std::cout << "Выбярыце горад адпраўлення (0 для адмовы): ";
			int cityChoice = GetIntWithinRange(0, availableCities.size());
			if (cityChoice == 0)
				return;

			from = availableCities[cityChoice - 1];

			Cargo::Type selectedType = static_cast<Cargo::Type>(typeChoice - 1);
			cargos.emplace_back(name, mass, selectedType, from, cargos);
			std::cout << "Груз паспяхова дададзены!\n";
			break;
		}
		case 2:
		{
			int cargoChoice;
			std::cout << "Увядзіце нумар груза для выдалення: ";
			cargoChoice = GetIntWithinRange(1, cargos.size());

			auto cargo = cargos.begin();
			std::advance(cargo, cargoChoice - 1);

			cargos.erase(cargo);
			std::cout << "Груз паспяхова выдалены!\n";

			break;
		}
		case 3:
			return;
		}
	}
	void Manager::TrailersList()
	{
		std::cout << "\nСпіс прычэпаў:\n";
		TablePrinter table(trailers);
		std::cout << table << '\n';

		int choice = ShowMenuWithNavigation({ "Дадаць прычэп", "Выдаліць прычэп", "Выхад" }, "Меню");

		switch (choice)
		{
		case 1:
		{
			float length, maxPayload;
			int typeChoice;

			length = GetFloat("Увядзіце даўжыню прычэпа: ");
			maxPayload = GetFloat("Увядзіце максімальную нагрузку: ");

			typeChoice = ShowMenuWithNavigation({ "Аўтавоз", "Цыстэрна", "Лесавоз", "Трал", "Тэнтавы", "Рэфрыжэратар" },
				"Меню");

			std::unique_ptr<Trailer> newTrailer;
			switch (typeChoice)
			{
			case 1:
				newTrailer = std::make_unique<CarTrailer>(length, maxPayload, trailers);
				break;
			case 2:
				newTrailer = std::make_unique<TankTrailer>(length, maxPayload, trailers);
				break;
			case 3:
				newTrailer = std::make_unique<TimberTrailer>(length, maxPayload, trailers);
				break;
			case 4:
				newTrailer = std::make_unique<LowboyTrailer>(length, maxPayload, trailers);
				break;
			case 5:
				newTrailer = std::make_unique<TarpTrailer>(length, maxPayload, trailers);
				break;
			case 6:
				newTrailer = std::make_unique<RefrigeratedTrailer>(length, maxPayload, trailers);
				break;
			}

			trailers.push_back(std::move(newTrailer));
			std::cout << "Прычэп паспяхова дададзены!\n";
			break;
		}
		case 2:
		{
			int trailerChoice = GetIntWithinRange(1, trailers.size(), "Увядзіце нумар прычэпа для выдалення: ");

			auto trailer = trailers.begin();
			std::advance(trailer, trailerChoice - 1);

			trailers.erase(trailer);
			std::cout << "Прычэп паспяхова выдалены!\n";

			break;
		}
		case 3:
			return;
		}
	}
	void Manager::ConsiderJobApplications()
	{
		if (applications.empty())
		{
			std::cout << "Няма заявак да разгляду\n";
			return;
		}

		int choiceApp;
		int choice;

		TablePrinter table(applications);
		std::cout << table << '\n';

		choiceApp = GetIntWithinRange(1, applications.size(), "\nВыбярыце заяўку да разгляду: ");

		choice = ShowMenuWithNavigation({ "Так", "Не" }, "Падцвердзіць заяўку?");

		switch (choice)
		{
		case 1:
		{
			if (lorries.empty())
			{
				std::cout << "\x1b[31;1m"
					<< "Немагчыма падцвердзіць заяўку, бо адсутнічаюць свабодныя грузавікі\n"
					<< "\x1b[0m";
				break;
			}

			std::vector<Lorry*> availableLorries;
			std::cout << "\nСвабодныя грузавікі:\n";
			TablePrinter lorriesTable(Lorry::GetHeaders());
			for (auto& lorry : lorries)
			{
				if (lorry.GetOwner() == nullptr)
				{
					lorriesTable.AddRow(lorry);
					availableLorries.push_back(&lorry);
				}
			}
			std::cout << lorriesTable << '\n';

			if (availableLorries.empty())
			{
				std::cout << "\x1b[31;1m"
					<< "Немагчыма падцвердзіць заяўку, бо адсутнічаюць свабодныя грузавікі\n"
					<< "\x1b[0m";
				break;
			}

			choice = GetIntWithinRange(1, availableLorries.size(), "Выбярыце грузавік для кіроўцы: ");

			auto application = applications.begin();
			std::advance(application, choiceApp - 1);

			drivers.emplace_back(application->GetAccount(), availableLorries[choice - 1]);

			applications.erase(application);

			break;
		}
		case 2:
			auto application = applications.begin();
			std::advance(application, choiceApp - 1);
			applications.erase(application);
			std::cout << "Заяўка паспяхова адмоўлена\n";
			break;
		}

		if (applications.empty())
			return;

		choice = ShowMenuWithNavigation({ "Так", "Не" }, "Жадаеце працягнуць?");

		switch (choice)
		{
		case 1:
			break;
		case 2:
			return;
		}
	}
	void Manager::ModAdmQuit()
	{
		int choice = ShowMenuWithNavigation({ "Так", "Не" }, "Гэта дзеянне нельга будзе адмовіць. Вы ўпэўнены?");

		switch (choice)
		{
		case 1:
			account->SetType(Account::Type::User);
			std::cout << "Вы звольнены." << std::endl;
			return;
		case 2:
			return;
		}
	}
	void Manager::ModSort()
	{
		enum class SortContainer
		{
			Accounts,
			Drivers,
			Cargos,
			Trailers
		};
		enum class SortOrder
		{
			Ascending,
			Descending
		};

		SortContainer container;
		SortOrder order;

		int choice = ShowMenuWithNavigation({ "Спіс акаўнтаў", "Спіс кіроўцаў", "Спіс грузаў", "Спіс прычэпаў" },
			"Выбярыце, што вы жадаеце адсартаваць");

		container = static_cast<SortContainer>(choice - 1);

		switch (container)
		{
		case SortContainer::Accounts:
		{
			enum class SortAttribute
			{
				Nickname,
				Firstname,
				Lastname
			};
			SortAttribute attribute;

			choice = ShowMenuWithNavigation({ "Імя акаўнту", "Уласнае імя", "Прозвішча" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			accounts.sort([attribute, order](const Account& a, const Account& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Nickname:
								return a.GetNickname() < b.GetNickname();
							case SortAttribute::Firstname:
								return a.GetFirstName() < b.GetFirstName();
							case SortAttribute::Lastname:
								return a.GetLastName() < b.GetLastName();
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Drivers:
		{
			enum class SortAttribute
			{
				Nickname,
				Firstname,
				Lastname,
				DeliveryCityFrom,
				DeliveryCityTo
			};
			SortAttribute attribute;

			choice = ShowMenuWithNavigation({ "Імя акаўнту", "Уласнае імя", "Прозвішча", "Горад адпраўлення",
				"Горад прыбыцця" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			drivers.sort([attribute, order](const Driver& a, const Driver& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Nickname:
								return a.GetAccount()->GetNickname() < b.GetAccount()->GetNickname();
							case SortAttribute::Firstname:
								return a.GetAccount()->GetFirstName() < b.GetAccount()->GetFirstName();
							case SortAttribute::Lastname:
								return a.GetAccount()->GetLastName() < b.GetAccount()->GetLastName();
							case SortAttribute::DeliveryCityFrom:
								if (a.GetCurrentDelivery() == nullptr)
									return true;
								else if (b.GetCurrentDelivery() == nullptr)
									return false;
								else
									return a.GetCurrentDelivery()->GetCityFrom()->GetName() <
									b.GetCurrentDelivery()->GetCityFrom()->GetName();
							case SortAttribute::DeliveryCityTo:
								if (a.GetCurrentDelivery() == nullptr)
									return true;
								else if (b.GetCurrentDelivery() == nullptr)
									return false;
								else
									return a.GetCurrentDelivery()->GetCityTo()->GetName() <
									b.GetCurrentDelivery()->GetCityTo()->GetName();
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Cargos:
		{
			enum class SortAttribute
			{
				Name,
				Mass,
				CityFrom,
				CityTo,
				Type
			};
			SortAttribute attribute;

			choice = ShowMenuWithNavigation({ "Назва груза", "Маса груза", "Горад адпраўлення", "Горад прыбыцця",
				"Тып груза" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			cargos.sort([attribute, order](const Cargo& a, const Cargo& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Name:
								return a.GetName() < b.GetName();
							case SortAttribute::Mass:
								return a.GetMass() < b.GetMass();
							case SortAttribute::CityFrom:
								return a.GetCityFrom()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::CityTo:
								if (a.GetCityTo() == nullptr)
									return true;
								else if (b.GetCityTo() == nullptr)
									return false;
								else
									return a.GetCityTo()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::Type:
								return static_cast<int>(a.GetType()) < static_cast<int>(b.GetType());
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Trailers:
		{
			enum class SortAttribute
			{
				Length,
				MaxPayload,
				CityFrom,
				CityTo,
				Type
			};
			SortAttribute attribute;

			choice = ShowMenuWithNavigation({ "Даўжыня", "Максімальная загрузка", "Горад адпраўлення", "Горад прыбыцця",
				"Тып прычепу" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			trailers.sort([attribute, order](const std::unique_ptr<Trailer>& a, const std::unique_ptr<Trailer>& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Length:
								return a->GetLength() < b->GetLength();
							case SortAttribute::MaxPayload:
								return a->GetMaxPayload() < b->GetMaxPayload();
							case SortAttribute::CityFrom:
								if (a->GetCurrentDelivery() == nullptr)
									return true;
								else if (b->GetCurrentDelivery() == nullptr)
									return false;
								else
									return a->GetCurrentDelivery()->GetCityFrom()->GetName()
									< b->GetCurrentDelivery()->GetCityFrom()->GetName();
							case SortAttribute::CityTo:
								if (a->GetCurrentDelivery() == nullptr)
									return true;
								else if (b->GetCurrentDelivery() == nullptr)
									return false;
								else
									return a->GetCurrentDelivery()->GetCityTo()->GetName()
									< b->GetCurrentDelivery()->GetCityTo()->GetName();
							case SortAttribute::Type:
								return static_cast<int>(a->GetType()) < static_cast<int>(b->GetType());
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		}
	}

	//Admin
	void Manager::ShowAllAccounts()
	{
		TablePrinter table(accounts);
		std::cout << table << '\n';
	}
	void Manager::EditAccounts()
	{
		std::cout << "\nСпіс уліковых запісаў:\n";
		TablePrinter table(accounts);
		std::cout << table << '\n';

		int choice = ShowMenuWithNavigation({ "Змяніць тып акаўнту", "Выдаліць акаўнт", "Выхад" }, "Выбярыце пункт меню");

		switch (choice)
		{
		case 1:
		{
			int accountChoice, typeChoice;
			accountChoice = GetIntWithinRange(1, accounts.size(), "Увядзіце нумар акаўнту для змены: ");

			auto account = accounts.begin();
			std::advance(account, accountChoice - 1);

			typeChoice = ShowMenuWithNavigation({ "Карыстальнік", "Мадэратар", "Адміністратар" },
				"Выбярыце новы тып акаўнту");

			Account::Type newType;
			switch (typeChoice)
			{
			case 1:
				newType = Account::Type::User;
				break;
			case 2:
				newType = Account::Type::Moderator;
				break;
			case 3:
				newType = Account::Type::Admin;
				break;
			}

			account->SetType(newType);
			std::cout << "Тып акаўнту паспяхова зменены!\n";

			break;
		}
		case 2:
		{
			int accountChoice = GetIntWithinRange(1, accounts.size(), "Увядзіце нумар акаўнту для выдалення: ");

			auto account = accounts.begin();
			std::advance(account, accountChoice - 1);

			for (const auto& driver : drivers)
			{
				if (driver.GetAccount() == &*account)
				{
					std::cout << "\x1b[31;1m" << "Немагчыма выдаліць акаўнт, які выкарыстоўваецца кіроўцай!"
						<< "\x1b[0m" << std::endl;

					break;
				}
			}

			if (!account->GetCargos().empty())
			{
				std::cout << "\x1b[31;1m" << "Немагчыма выдаліць акаўнт, які мае актыўныя дастаўкі!"
					<< "\x1b[0m" << std::endl;

				break;
			}

			if (*account == *Manager::account)
			{
				int choice = ShowMenuWithNavigation({ "Так", "Не" }, "Вы ўпэўнены, што хаціце выдаліць уласны акаўнт?");

				if (choice == 2)
					return;

				accounts.erase(account);
				std::cout << "Акаўнт паспяхова выдалены\n";

				std::cout << "\nУвядзіце любае значэнне для працягнення\n";
				GetString("");

				std::cout << "\x1b[2J\x1b[1;1H";
				Manager::account = nullptr;

				return;
			}

			accounts.erase(account);
			std::cout << "Акаўнт паспяхова выдалены!\n";

			break;
		}
		case 3:
			return;
		}
	}
	void Manager::DriversList()
	{
		std::cout << "Спіс кіроўцаў:\n";
		TablePrinter table(drivers);
		std::cout << table << '\n';

		int choice = ShowMenuWithNavigation({ "Зволніць кіроўцу", "Выхад" }, "Выбярыце пункт меню");

		switch (choice)
		{
		case 1:
		{
			int driverChoice = GetIntWithinRange(0, drivers.size(),
				"Выбярыце кіроўцу да звальнення (0 для адмовы): ");
			if (driverChoice == 0)
				return;

			auto driver = drivers.begin();
			std::advance(driver, driverChoice - 1);

			if (driver->GetCurrentDelivery() != nullptr)
			{
				driver->GetCurrentDelivery()->StopDelivery(deliveries);
				driver->GetLorry()->SetOwner(nullptr);
				driver->GetAccount()->SetType(Account::Type::User);
				drivers.erase(driver);
			}

			break;
		}
		case 2:
			return;
		}
	}
	void Manager::LorriesList()
	{
		int choice;

		std::cout << "\nСпіс грузавікоў:\n";
		TablePrinter table(lorries);
		std::cout << table << '\n';

		choice = ShowMenuWithNavigation({ "Дадаць грузавік", "Выдаліць грузавік", "Выхад" }, "Выбярыце пункт меню");

		switch (choice)
		{
		case 1:
		{
			std::string make, model, registrationSigns;
			unsigned int mileage;
			float gasolineCost;
			int countryChoice, cityChoice;

			std::cout << "Увядзіце марку грузавіка: ";
			std::getline(std::cin, make);

			std::cout << "Увядзіце мадэль грузавіка: ";
			std::getline(std::cin, model);

			std::cout << "Увядзіце пробег: ";
			std::cin >> mileage;
			std::cin.ignore();

			gasolineCost = GetFloat("Увядзіце кошт паліва (на 100км): ");

			std::cout << "Даступныя краіны:\n";
			int j = 0;
			for (const auto& country : countries)
			{
				std::cout << ++j << ". " << country.GetName() << std::endl;
			}

			countryChoice = GetIntWithinRange(1, countries.size(), "Выбярыце краіну: ");

			auto country = countries.begin();
			std::advance(country, countryChoice - 1);

			std::cout << "Даступныя гарады:\n";
			TablePrinter table(country->GetCities());
			std::cout << table << '\n';

			std::cout << "Выбярыце горад: ";
			cityChoice = GetIntWithinRange(1, country->GetCities().size());

			auto city = country->GetCitiesL().begin();
			std::advance(city, cityChoice - 1);

			std::cout << "Увядзіце рэгістрацыйныя знакі: ";
			std::getline(std::cin, registrationSigns);

			lorries.emplace_back(make, model, mileage, *country, *city,
				registrationSigns, gasolineCost, lorries);
			std::cout << "Грузавік паспяхова дададзены!\n";
			break;
		}
		case 2:
		{
			int lorryChoice = GetIntWithinRange(1, lorries.size(), "Увядзіце нумар грузавіка для выдалення: ");

			auto lorry = lorries.begin();
			std::advance(lorry, lorryChoice - 1);

			if (lorry->GetOwner() != nullptr)
			{
				std::cout << "\x1b[31;1m" << "Немагчыма выдаліць грузавік, які выкарыстоўваецца кіроўцай!"
					<< "\x1b[0m" << std::endl;
				break;
			}

			lorries.erase(lorry);
			std::cout << "Грузавік паспяхова выдалены!\n";

			break;
		}
		case 3:
			return;
		}
	}
	void Manager::AreasList()
	{
		std::cout << "\nСпіс краін і гарадоў:\n";
		TablePrinter table(countries);
		std::cout << table << '\n';

		int choice = ShowMenuWithNavigation({ "Дадаць краіну", "Дадаць горад", "Выдаліць краіну",
			"Выдаліць горад", "Выхад" }, "Выбярыце пункт меню");

		switch (choice)
		{
		case 1:
		{
			std::string name, abbreviation, phoneCode;

			std::cout << "Увядзіце назву краіны: ";
			std::getline(std::cin, name);

			std::cout << "Увядзіце абрэвіятуру краіны: ";
			std::getline(std::cin, abbreviation);

			std::cout << "Увядзіце тэлефонны код краіны: ";
			std::getline(std::cin, phoneCode);

			countries.emplace_back(name, abbreviation, phoneCode);
			std::cout << "Краіна паспяхова дададзена!\n";
			break;
		}
		case 2:
		{
			if (countries.empty())
			{
				std::cout << "\x1b[31;1m" << "Спачатку дадайце краіну!" << "\x1b[0m" << std::endl;
				break;
			}

			std::string name, abbreviation;
			int x, y;
			int countryChoice;

			std::cout << "Даступныя краіны:\n";
			int j = 0;
			for (const auto& country : countries)
			{
				std::cout << ++j << ". " << country.GetName() << std::endl;
			}

			countryChoice = GetIntWithinRange(1, countries.size(), "Выбярыце краіну: ");

			std::cout << "Увядзіце назву горада: ";
			std::getline(std::cin, name);

			std::cout << "Увядзіце абрэвіятуру горада: ";
			std::getline(std::cin, abbreviation);

			x = GetInt("Увядзіце каардынату X: ");
			y = GetInt("Увядзіце каардынату Y: ");

			auto country = countries.begin();
			std::advance(country, countryChoice - 1);

			City(name, abbreviation, *country, x, y, countries);
			std::cout << "Горад паспяхова дададзены!\n";
			break;
		}
		case 3:
		{
			std::vector<Country*> availableCountries;
			for (auto& country : countries)
				if (!country.GetCities().empty())
					std::cout << "Немагчыма выдаліць краіну " << country.GetName()
					<< ", бо яна мае гарады\n";
				else
					availableCountries.push_back(&country);

			if (availableCountries.empty())
			{
				std::cout << "Няма даступных краін да выдалення\n";
				return;
			}

			int i = 0;
			std::cout << "\nДаступныя краіны да выдалення:";
			TablePrinter table(availableCountries);
			std::cout << table << '\n';

			int countryChoice = GetIntWithinRange(0, availableCountries.size(),
				"Увядзіце нумар краіны для выдалення (0 для адмовы): ");
			if (countryChoice == 0)
				return;

			int j = 0;
			for (const auto& country : countries)
			{
				j++;
				if (country == *availableCountries[countryChoice - 1])
				{
					auto it = countries.begin();
					std::advance(it, j);
					countries.erase(it);
				}
			}

			std::cout << "Краіна паспяхова выдалена!\n";

			break;
		}
		case 4:
		{
			int countryChoice, cityChoice;

			TablePrinter table(countries);
			std::cout << table << '\n';

			countryChoice = GetIntWithinRange(0, countries.size(), "Выбярыце краіну (0 для адмовы): ");
			if (countryChoice == 0)
				return;

			auto country = countries.begin();
			std::advance(country, countryChoice - 1);

			if (country->GetCities().empty())
			{
				std::cout << "\x1b[31;1m" << "У гэтай краіне няма гарадоў!" << "\x1b[0m" << std::endl;
				break;
			}

			std::vector<City*> availableCities;
			for (auto& city : country->GetCitiesL())
			{
				bool isUsed = false;
				for (const auto& cargo : cargos)
				{
					if (*cargo.GetCityFrom() == city || (cargo.GetCityTo() != nullptr && *cargo.GetCityTo() == city))
					{
						std::cout << "Немагчыма выдаліць горад " << city.GetName()
							<< ", бо ён існуе ў адным з грузаў\n";
						isUsed = true;
					}
				}

				if (!isUsed)
					availableCities.push_back(&city);
			}

			std::cout << '\n';

			if (availableCities.empty())
			{
				std::cout << "Дадзеная краіна не мае даступных гарадоў да выдалення\n";
				break;
			}

			std::cout << "Даступныя гарады да выдалення:\n";
			TablePrinter citiesTable(availableCities);
			std::cout << citiesTable << '\n';

			cityChoice = GetIntWithinRange(0, availableCities.size(),
				"Увядзіце нумар горада для выдалення (0 для адмовы): ");
			if (cityChoice == 0)
				return;

			country->RemoveCity(availableCities[cityChoice - 1]);
			std::cout << "Горад паспяхова выдалены!\n";

			break;
		}
		case 5:
			return;
		}
	}
	void Manager::DeliveriesList()
	{
		std::cout << "\nСпіс даставак:\n";
		TablePrinter table(deliveries);
		std::cout << table << '\n';

		int choice = ShowMenuWithNavigation({ "Выдаліць дастаўку", "Выхад" }, "Выбярыце пункт меню");

		switch (choice)
		{
		case 1:
		{
			int deliveryChoice = GetIntWithinRange(1, deliveries.size(), "Увядзіце нумар дастаўкі для выдалення: ");

			auto delivery = deliveries.begin();
			std::advance(delivery, deliveryChoice);

			delivery->StopDelivery(deliveries);

			deliveries.erase(delivery);
			std::cout << "Дастаўка паспяхова выдалена!\n";

			break;
		}
		case 2:
			return;
		}
	}
	void Manager::AdminSort()
	{
		enum class SortContainer
		{
			Countries,
			Cities,
			Accounts,
			Drivers,
			Lorries,
			Cargos,
			Deliveries,
			Trailers,
		};
		enum class SortOrder
		{
			Ascending,
			Descending
		};

		SortContainer container;
		SortOrder order;

		int choice = ShowMenuWithNavigation({ "Спіс краін", "Спіс гарадоў", "Спіс акаўнтаў", "Спіс кіроўцаў",
			"Спіс грузавікоў", "Спіс грузаў", "Спіс даставак", "Спіс прычэпаў" },
			"Выбярыце, што вы жадаеце адсартаваць");

		container = static_cast<SortContainer>(choice - 1);

		switch (container)
		{
		case SortContainer::Countries:
		{
			enum class SortAttribute
			{
				Name,
				Code
			};
			SortAttribute attribute;
			choice = ShowMenuWithNavigation({ "Назва краіны", "Тэлефонны код краіны" },
				"Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			countries.sort([attribute, order](const Country& a, const Country& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Name:
								return a.GetName() < b.GetName();
							case SortAttribute::Code:
								return a.GetPhoneCode() < b.GetPhoneCode();
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Cities:
		{
			enum class SortAttribute
			{
				Name,
				CountryAbbreviation
			};

			SortAttribute attribute;
			choice = ShowMenuWithNavigation({ "Назва горада", "Абрэвіатура краіны" },
				"Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			for (auto& country : countries)
			{
				country.GetCitiesL().sort([attribute, order](const City& a, const City& b)
					{
						auto compare = [attribute, &a, &b]() -> bool
							{
								switch (attribute)
								{
								case SortAttribute::Name:
									return a.GetName() < b.GetName();
								case SortAttribute::CountryAbbreviation:
									return a.GetCountryAbbreviation() < b.GetCountryAbbreviation();
								}
							};

						return order == SortOrder::Ascending ? compare() : !compare();
					}
				);
			}

			return;
		}
		case SortContainer::Accounts:
		{
			enum class SortAttribute
			{
				Nickname,
				Firstname,
				Lastname
			};
			SortAttribute attribute;

			choice = ShowMenuWithNavigation({ "Імя акаўнту", "Уласнае імя", "Прозвішча" },
				"Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			accounts.sort([attribute, order](const Account& a, const Account& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Nickname:
								return a.GetNickname() < b.GetNickname();
							case SortAttribute::Firstname:
								return a.GetFirstName() < b.GetFirstName();
							case SortAttribute::Lastname:
								return a.GetLastName() < b.GetLastName();
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Drivers:
		{
			enum class SortAttribute
			{
				Nickname,
				Firstname,
				Lastname,
				DeliveryCityFrom,
				DeliveryCityTo
			};
			SortAttribute attribute;

			choice = ShowMenuWithNavigation({ "Імя акаўнту", "Уласнае імя", "Прозвішча", "Горад адпраўлення",
				"Горад прыбыцця" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			drivers.sort([attribute, order](const Driver& a, const Driver& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Nickname:
								return a.GetAccount()->GetNickname() < b.GetAccount()->GetNickname();
							case SortAttribute::Firstname:
								return a.GetAccount()->GetFirstName() < b.GetAccount()->GetFirstName();
							case SortAttribute::Lastname:
								return a.GetAccount()->GetLastName() < b.GetAccount()->GetLastName();
							case SortAttribute::DeliveryCityFrom:
								if (a.GetCurrentDelivery() == nullptr)
									return true;
								else if (b.GetCurrentDelivery() == nullptr)
									return false;
								else
									return a.GetCurrentDelivery()->GetCityFrom()->GetName() <
									b.GetCurrentDelivery()->GetCityFrom()->GetName();
							case SortAttribute::DeliveryCityTo:
								if (a.GetCurrentDelivery() == nullptr)
									return true;
								else if (b.GetCurrentDelivery() == nullptr)
									return false;
								else
									return a.GetCurrentDelivery()->GetCityTo()->GetName() <
									b.GetCurrentDelivery()->GetCityTo()->GetName();
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Lorries:
		{
			enum class SortAttribute
			{
				Make,
				Mileage
			};

			choice = ShowMenuWithNavigation({ "Марка", "Прабег" }, "Выбярыце атрыбут сартавання");

			SortAttribute attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			lorries.sort([attribute, order](const Lorry& a, const Lorry& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Make:
								return a.GetMake() < b.GetMake();
							case SortAttribute::Mileage:
								return a.GetMileage() < b.GetMileage();
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Cargos:
		{
			enum class SortAttribute
			{
				Name,
				Mass,
				CityFrom,
				CityTo,
				Type
			};

			SortAttribute attribute;
			choice = ShowMenuWithNavigation({ "Назва груза", "Маса груза", "Горад адпраўлення", "Горад прыбыцця",
				"Тып груза" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			cargos.sort([attribute, order](const Cargo& a, const Cargo& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Name:
								return a.GetName() < b.GetName();
							case SortAttribute::Mass:
								return a.GetMass() < b.GetMass();
							case SortAttribute::CityFrom:
								return a.GetCityFrom()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::CityTo:
								if (a.GetCityTo() == nullptr)
									return true;
								else if (b.GetCityTo() == nullptr)
									return false;
								else
									return a.GetCityTo()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::Type:
								return static_cast<int>(a.GetType()) < static_cast<int>(b.GetType());
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Deliveries:
		{
			enum class SortAttribute
			{
				DriverName,
				CargoName,
				CargoMass,
				CargoCityFrom,
				CargoCityTo,
				CargoType
			};

			SortAttribute attribute;
			choice = ShowMenuWithNavigation({ "Імя кіроўцы", "Назва груза", "Маса груза", "Горад адпраўлення",
				"Горад прыбыцця", "Тып груза" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			deliveries.sort([attribute, order](const Delivery& a, const Delivery& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::DriverName:
								return a.GetDriver()->GetAccount()->GetNickname()
									< b.GetDriver()->GetAccount()->GetNickname();
							case SortAttribute::CargoName:
								return a.GetCargo()->GetName() < b.GetCargo()->GetName();
							case SortAttribute::CargoMass:
								return a.GetCargo()->GetMass() < b.GetCargo()->GetMass();
							case SortAttribute::CargoCityFrom:
								return a.GetCityFrom()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::CargoCityTo:
								if (a.GetCityTo() == nullptr)
									return true;
								else if (b.GetCityTo() == nullptr)
									return false;
								else
									return a.GetCityTo()->GetName() < b.GetCityFrom()->GetName();
							case SortAttribute::CargoType:
								return static_cast<int>(a.GetCargo()->GetType())
									< static_cast<int>(b.GetCargo()->GetType());
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		case SortContainer::Trailers:
		{
			enum class SortAttribute
			{
				Length,
				MaxPayload,
				CityFrom,
				CityTo,
				Type
			};
			SortAttribute attribute;

			choice = ShowMenuWithNavigation({ "Даўжыня", "Максімальная загрузка", "Горад адпраўлення",
				"Горад прыбыцця", "Тып прычепу" }, "Выбярыце атрыбут сартавання");
			attribute = static_cast<SortAttribute>(choice - 1);

			choice = ShowMenuWithNavigation({ "Па ўзрастанні", "Па змяншэнні" }, "Выбярыце парадак сартавання");
			order = static_cast<SortOrder>(choice - 1);

			trailers.sort([attribute, order](const std::unique_ptr<Trailer>& a, const std::unique_ptr<Trailer>& b)
				{
					auto compare = [attribute, &a, &b]() -> bool
						{
							switch (attribute)
							{
							case SortAttribute::Length:
								return a->GetLength() < b->GetLength();
							case SortAttribute::MaxPayload:
								return a->GetMaxPayload() < b->GetMaxPayload();
							case SortAttribute::CityFrom:
								if (a->GetCurrentDelivery() == nullptr)
									return true;
								else if (b->GetCurrentDelivery() == nullptr)
									return false;
								else
									return a->GetCurrentDelivery()->GetCityFrom()->GetName()
									< b->GetCurrentDelivery()->GetCityFrom()->GetName();
							case SortAttribute::CityTo:
								if (a->GetCurrentDelivery() == nullptr)
									return true;
								else if (b->GetCurrentDelivery() == nullptr)
									return false;
								else
									return a->GetCurrentDelivery()->GetCityTo()->GetName()
									< b->GetCurrentDelivery()->GetCityTo()->GetName();
							case SortAttribute::Type:
								return static_cast<int>(a->GetType()) < static_cast<int>(b->GetType());
							}
						};

					return order == SortOrder::Ascending ? compare() : !compare();
				}
			);

			return;
		}
		}
	}
	void Manager::Filter()
	{
		enum class Container
		{
			Account, //Deliveries count
			Lorry, //Mileage
			Cargo, //Mass
			Trailer //MaxPayload
		};
		enum class Mode
		{
			Greater,
			Less
		};

		int choice = ShowMenuWithNavigation({ "Акаўнты па колькасці даставак", "Грузавікі па прабегу",
			"Грузы па масе", "Прычэпы па максімальнай грузападымальнасці" },
			"Выбярыце, што вы хаціце адфільтраваць");
		Container container = static_cast<Container>(choice - 1);

		choice = ShowMenuWithNavigation({ "Паказваць больш за значэнне", "Паказваць менш за значэнне" },
			"Выбярыце рэжым фільтрацыі");
		Mode mode = static_cast<Mode>(choice - 1);

		int value, i = 0;

		switch (container)
		{
		case Container::Account:
			value = mode == Mode::Greater ? GetInt("Увядзіце мінімальную колькасць\n")
				: GetInt("Увядзіце максімальную колькасць\n");

			for (const auto& account : accounts)
				if (mode == Mode::Greater)
				{
					if (account.GetCargos().size() >= value)
						std::cout << ++i << ".\n" << account << "\n\n";
				}
				else
					if (account.GetCargos().size() <= value)
						std::cout << ++i << ".\n" << account << "\n\n";

			break;
		case Container::Lorry:
			value = mode == Mode::Greater ? GetInt("Увядзіце мінімальны прабег\n")
				: GetInt("Увядзіце максімальны прабег\n");

			for (const auto& lorry : lorries)
				if (mode == Mode::Greater)
				{
					if (lorry.GetMileage() >= value)
						std::cout << ++i << ".\n" << lorry << "\n\n";
				}
				else
					if (lorry.GetMileage() <= value)
						std::cout << ++i << ".\n" << lorry << "\n\n";
			break;
		case Container::Cargo:
			value = mode == Mode::Greater ? GetInt("Увядзіце мінімальную масу\n")
				: GetInt("Увядзіце максімальную масу\n");

			for (const auto& cargo : cargos)
				if (mode == Mode::Greater)
				{
					if (cargo.GetMass() >= value)
						std::cout << ++i << ".\n" << cargo << "\n\n";
				}
				else
					if (cargo.GetMass() <= value)
						std::cout << ++i << ".\n" << cargo << "\n\n";

			break;
		case Container::Trailer:
			value = mode == Mode::Greater ? GetInt("Увядзіце мінімальную грузападымальнасць\n")
				: GetInt("Увядзіце максімальную грузападымальнасць\n");

			for (const auto& trailer : trailers)
				if (mode == Mode::Greater)
				{
					if (trailer->GetMaxPayload() >= value)
						std::cout << ++i << ".\n" << *trailer << "\n\n";
				}
				else
					if (trailer->GetMaxPayload() <= value)
						std::cout << ++i << ".\n" << *trailer << "\n\n";

			break;
		}
	}
	void Manager::Search()
	{
		enum class Container
		{
			Account,
			Driver,
			Lorry,
			Cargo,
			Country,
			City,
			Trailer,
			Delivery
		};

		int choice = ShowMenuWithNavigation({ "Акаўнт", "Кіроўца", "Грузавік", "Груз", "Краіна", "Горад",
			"Прычэп", "Дастаўка" }, "Выбярыце, што вы шукаеце");
		Container container = static_cast<Container>(choice - 1);

		switch (container)
		{
		case Container::Account:
		{
			std::string nickname = GetString("Увядзіце імя акаўнту:\n");

			for (const auto& account : accounts)
				if (account.GetNickname() == nickname)
				{
					std::cout << "\nАкаўнт знойдзены!\n\n" << account << '\n';
					return;
				}

			std::cout << "\nАкаўнт не знойдзены\n";

			break;
		}
		case Container::Driver:
		{
			std::string nickname = GetString("Увядзіце імя акаўнту кіроўцы:\n");

			for (const auto& driver : drivers)
				if (driver.GetAccount()->GetNickname() == nickname)
				{
					std::cout << "\nКіроўца знойдзены!\n\n" << driver << '\n';
					return;
				}

			std::cout << "\nКіроўца не знойдзены\n";

			break;
		}
		case Container::Lorry:
		{
			int id = GetInt("Увядзіце айдзі грузавіку: ");

			for (const auto& lorry : lorries)
				if (lorry.GetID() == id)
				{
					std::cout << "\nГрузавік знойдзены!\n\n" << lorry << '\n';
					return;
				}

			std::cout << "\nГрузавік не знойдзены\n";

			break;
		}
		case Container::Cargo:
		{
			int id = GetInt("Увядзіце айдзі грузу: ");

			for (const auto& cargo : cargos)
				if (cargo.GetID() == id)
				{
					std::cout << "\nГруз знойдзены!\n\n" << cargo << '\n';
					return;
				}

			std::cout << "\nГруз не знойдзены\n";

			break;
		}
		case Container::Country:
		{
			enum class SearchField
			{
				Name,
				Abbreviation,
				PhoneCode
			};

			choice = ShowMenuWithNavigation({ "Назва краіны", "Абрэвіятура краіны", "Тэлефонны код краіны" },
				"Выбярыце поле, па якому будзе адбывацца пошук");
			SearchField field = static_cast<SearchField>(choice - 1);

			switch (field)
			{
			case SearchField::Name:
			{
				std::string name = GetString("Увядзіце назву краіны:\n");

				for (const auto& country : countries)
					if (country.GetName() == name)
					{
						std::cout << "\nКраіна знойдзена!\n\n" << country << '\n';
						return;
					}

				std::cout << "Краіна не знойдзена\n";

				break;
			}
			case SearchField::Abbreviation:
			{
				std::string abbreviation = GetString("Увядзіце абрэвіятуру краіны:\n", 2, 2);

				for (const auto& country : countries)
					if (country.GetAbbreviation() == abbreviation)
					{
						std::cout << "\nКраіна знойдзена!\n\n" << country << '\n';
						return;
					}

				std::cout << "Краіна не знойдзена\n";

				break;
			}
			case SearchField::PhoneCode:
			{
				std::string phoneCode = "+" + GetString("Увядзіце тэлефонны код краіны:\n+", 1, 3);

				for (const auto& country : countries)
					if (country.GetPhoneCode() == phoneCode)
					{
						std::cout << "\nКраіна знойдзена!\n\n" << country << '\n';
						return;
					}

				std::cout << "Краіна не знойдзена\n";

				break;
			}
			}

			break;
		}
		case Container::City:
		{
			std::string name = GetString("Увядзіце назву горада:\n");
			int i = 0;

			for (const auto& country : countries)
				for (const auto& city : country.GetCities())
					if (city.GetName() == name)
					{
						if (i == 0)
							std::cout << "\nГорад знойдзены!\n";

						std::cout << '\n' << ++i << ".\n" << city << '\n';
					}

			if (i == 0)
				std::cout << "Горад не знойдзены";

			break;
		}
		case Container::Trailer:
		{
			int id = GetInt("Увядзіце айдзі прычэпу: ");

			for (const auto& trailer : trailers)
				if (trailer->GetID() == id)
				{
					std::cout << "\nПрычэп знойдзены!\n\n" << *trailer << '\n';
					return;
				}

			std::cout << "\nПрычэп не знойдзены\n";

			break;
		}
		case Container::Delivery:
		{
			enum class SearchField
			{
				DriverName,
				LorryID,
				CargoID,
				TrailerID
			};

			choice = ShowMenuWithNavigation({ "Імя кіроўцы", "Айдзі грузавіку", "Айдзі грузу", "Айдзі прычэпу" },
				"Выбярыце поле, па якому будзе адбывацца пошук");
			SearchField field = static_cast<SearchField>(choice - 1);

			switch (field)
			{
			case SearchField::DriverName:
			{
				std::string nickname = GetString("Увядзіце імя акаўнту кіроўцы:\n");

				for (const auto& delivery : deliveries)
					if (delivery.GetDriver()->GetAccount()->GetNickname() == nickname)
					{
						std::cout << "\nДастаўка знойдзена!\n\n" << delivery << '\n';
						return;
					}

				std::cout << "\nДастаўка не знойдзена\n";

				break;
			}
			case SearchField::LorryID:
			{
				int id = GetInt("Увядзіце айдзі грузавіку: ");

				for (const auto& delivery : deliveries)
					if (delivery.GetLorry()->GetID() == id)
					{
						std::cout << "\nДастўка знойдзена!\n\n" << delivery << '\n';
						return;
					}

				std::cout << "\nДастаўка не знойдзена\n";

				break;
			}
			case SearchField::CargoID:
			{
				int id = GetInt("Увядзіце айдзі грузу: ");

				for (const auto& delivery : deliveries)
					if (delivery.GetCargo()->GetID() == id)
					{
						std::cout << "\nДастаўка знойдзена!\n\n" << delivery << '\n';
						return;
					}

				std::cout << "\nДастаўка не знойдзена\n";

				break;
			}
			case SearchField::TrailerID:
			{
				int id = GetInt("Увядзіце айдзі прычэпу: ");

				for (const auto& delivery : deliveries)
					if (delivery.GetTrailer()->GetID() == id)
					{
						std::cout << "\nДастаўка знойдзена!\n\n" << delivery << '\n';
						return;
					}

				std::cout << "\nДастаўка не знойдзена\n";

				break;
			}
			}

			break;
		}
		}
	}
	void Manager::Report()
	{
		std::cout << "\nДаклад аб працы праграмы захоўваецца ў файл: " << REPORT << ".\n";

		std::ofstream report(REPORT);
		if (!report.is_open())
			throw(std::exception("Памылка пры адкрыцці файла"));
		else
		{
			report << "Краіны і гарады:\n";
			int i = 0;
			for (const auto& country : countries)
			{
				report << ++i << ".\t" << country.GetName() << '\t' << country.GetAbbreviation() << '\t'
					<< country.GetPhoneCode();
				report << "\nСпіс гарадоў:\n";

				int j = 0;
				for (const auto& city : country.GetCities())
					report << '\t' << ++j << ".\t" << city.GetName() << '\t' << city.GetAbbreviation() << '\n';

				report << "\n\n";
			}

			report << "*****************************************\n\n";

			report << "Акаўнты:\n";
			i = 0;
			for (const auto& account : accounts)
			{
				report << ++i << ".\t" << account.GetNickname() << '\n'
					<< account.GetFirstName() << ' ' << account.GetLastName() << '\t' << account.GetPhoneNumber()
					<< '\n' << account.GetType() << '\n';
				report << "Спіс грузаў:\n";
				int j = 0;
				for (const auto& cargo : account.GetCargos())
				{
					report << '\t' << ++j << ".\t" << cargo->GetName() << '\t' << cargo->GetID() << "\n\t"
						<< "З: " << cargo->GetCityFrom()->GetName() << '\t'
						<< cargo->GetCityFrom()->GetCountryAbbreviation() << "\n\t";
					if (cargo->GetCityTo() != nullptr)
						report << "Да: " << cargo->GetCityTo()->GetName() << '\t'
						<< cargo->GetCityTo()->GetCountryAbbreviation();
					report << "\n\t" << cargo->GetMass() << " кг"
						<< "\n\t" << cargo->GetType() << "\n\n";
				}
				if (j == 0)
					report << "Няма\n\n";
			}

			report << "*****************************************\n\n";

			report << "Кіроўцы:\n";
			i = 0;
			for (const auto& driver : drivers)
			{
				report << ++i << ".\t" << driver.GetAccount()->GetNickname() << '\t'
					<< driver.GetAccount()->GetFirstName() << ' ' << driver.GetAccount()->GetLastName() << '\n';
				report << "Грузавік кіроўцы: " << driver.GetLorry()->GetMake() << ' ' << driver.GetLorry()->GetModel()
					<< '\n';
				driver.GetCurrentDelivery() == nullptr ? report << "\nНяма задання\n\n" :
					report << "Заданне: " << driver.GetCurrentDelivery()->GetCargo()->GetName() << '\t'
					<< driver.GetCurrentDelivery()->GetCityFrom()->GetName() << " - "
					<< driver.GetCurrentDelivery()->GetCityTo()->GetName() << "\n\n";
			}

			report << "*****************************************\n\n";

			report << "Грузавікі:\n";
			i = 0;
			for (const auto& lorry : lorries)
			{
				report << ++i << ".\t" << lorry.GetMake() << ' ' << lorry.GetModel() << '\t'
					<< lorry.GetID() << '\n';
				lorry.GetCurrentDelivery() == nullptr ? report << "\nНяма задання\n\n" :
					report << "Заданне: " << lorry.GetCurrentDelivery()->GetCargo()->GetName() << '\t'
					<< lorry.GetCurrentDelivery()->GetCityFrom()->GetName() << " - "
					<< lorry.GetCurrentDelivery()->GetCityTo()->GetName() << "\n\n";
			}

			report << "*****************************************\n\n";

			report << "Прычэпы:\n";
			i = 0;
			for (const auto& trailer : trailers)
			{
				report << ++i << ".\t" << trailer->GetTypeString() << '\t' << trailer->GetID()
					<< "\nДаўжыня: " << trailer->GetLength()
					<< "\nМаксімальная грузападымальнасць: " << trailer->GetMaxPayload();
				trailer->GetCurrentDelivery() == nullptr ? report << "\nНяма задання\n\n" :
					report << "Заданне: " << trailer->GetCurrentDelivery()->GetCargo()->GetName() << '\t'
					<< trailer->GetCurrentDelivery()->GetCityFrom()->GetName() << " - "
					<< trailer->GetCurrentDelivery()->GetCityTo()->GetName() << "\n\n";
			}

			report << "*****************************************\n\n";

			report << "Грузы:\n";
			i = 0;
			for (const auto& cargo : cargos)
			{
				report << ++i << ".\t" << cargo.GetName() << '\t' << cargo.GetID() << '\t'
					<< cargo.GetMass() << " кг" << '\n';
				report << "З: " << cargo.GetCityFrom()->GetName();
				if (cargo.GetCityTo() != nullptr)
				{
					report << "Да: " << cargo.GetCityTo()->GetName() << '\n'
						<< "Заказчык: " << cargo.GetClient()->GetNickname() << '\n';

					if (cargo.GetCurrentDelivery() != nullptr)
					{
						report << "Дастаўляецца\n\n";
					}
					else
						report << "Не дастаўляецца\n\n";
				}
				else
					report << "Не заказаны\n\n";
			}

			report << "*****************************************\n\n";

			report << "Заяўкі на працу:\n";
			i = 0;
			for (const auto& application : applications)
			{
				report << ++i << ".\t" << application.GetAccount()->GetNickname() << '\n'
					<< application.GetAccount()->GetFirstName() << ' ' << application.GetAccount()->GetLastName()
					<< '\n' << application.GetAppMessage() << "\n\n";
			}
		}

		report.close();

		std::cout << "Даклад паспяхова сфарміраваны\n";
	}

	void Manager::Menu()
	{
	menu_begin:
		int choice;

		switch (account->GetType())
		{
		case Account::Type::User:
		{
			std::vector<std::string> menuItems =
			{
				"Замовіць дастаўку",
				"Праглядзець актыўныя заказы",
				"Рэдагаваць асабістыя дадзеныя",
				"Праглядзець даступныя гарады і краіны",
				"Аднавіць дастаўкі",
				"Сартаванне",
				"Стань кіроўцай!",
				"Выхад"
			};

			while (true)
			{
				choice = ShowMenuWithNavigation(menuItems, "Меню:");
				std::cout << std::endl;

				switch (choice)
				{
				case 1:
					UserRequestDelivery();
					break;
				case 2:
					UserShowCargos();
					break;
				case 3:
					EditAccount();
					break;
				case 4:
					ShowSupportedArea();
					break;
				case 5:
					UpdateDistance();
					break;
				case 6:
					UserSort();
					break;
				case 7:
					BecomeADriver();
					break;
				case 8:
					return;
				}
			}
			break;
		}
		case Account::Type::Driver:
		{
			Driver* driver = FindDriver(account->GetNickname());

			while (true) {
				if (driver->GetCurrentDelivery() == nullptr)
				{
					std::vector<std::string> menuItems =
					{
						"Прыняць замову",
						"Сартаванне грузаў",
						"Рэдагаваць асабістыя дадзеныя",
						"Аднавіць дастаўкі",
						"Звольніцца",
						"Выхад"
					};

					choice = ShowMenuWithNavigation(menuItems, "Меню:");
					std::cout << std::endl;

					switch (choice)
					{
					case 1:
						AcceptDelivery(driver);
						break;
					case 2:
						DriverSort();
						break;
					case 3:
						EditAccount();
						break;
					case 4:
						UpdateDistance();
						break;
					case 5:
						DriverQuit(driver);
						goto menu_begin;
					case 6:
						return;
					}
				}
				else
				{
					std::vector<std::string> menuItems =
					{
						"Праглядзець бягучую замову",
						"Аднавіць дастаўкі",
						"Адмовіцца ад замовы",
						"Рэдагаваць асабістыя дадзеныя",
						"Выхад"
					};

					choice = ShowMenuWithNavigation(menuItems, "Меню:");
					std::cout << std::endl;

					switch (choice) {
					case 1:
						ShowCurrentDelivery(driver);
						break;
					case 2:
						UpdateDistance();
						break;
					case 3:
						DropDelivery(driver);
						break;
					case 4:
						EditAccount();
						break;
					case 5:
						return;
					}
				}
			}
			break;
		}
		case Account::Type::Moderator:
		{
			std::vector<std::string> menuItems =
			{
				"Рэдагаваць асабістыя дадзеныя",
				"Праглядзець усіх карыстальнікаў",
				"Праглядзець усіх кіроўцаў",
				"Спіс грузаў",
				"Спіс прычэпаў",
				"Разглядзець заяўкі на працу",
				"Аднавіць дастаўкі",
				"Сартаванне",
				"Звольніцца",
				"Выхад"
			};

			while (true)
			{
				choice = ShowMenuWithNavigation(menuItems, "Меню:");
				std::cout << std::endl;

				switch (choice)
				{
				case 1:
					EditAccount();
					break;
				case 2:
					ShowAllUsers();
					break;
				case 3:
					ShowAllDrivers();
					break;
				case 4:
					CargosList();
					break;
				case 5:
					TrailersList();
					break;
				case 6:
					ConsiderJobApplications();
					break;
				case 7:
					UpdateDistance();
					break;
				case 8:
					ModSort();
					break;
				case 9:
					ModAdmQuit();
					goto menu_begin;
				case 10:
					return;
				}
			}
			break;
		}
		case Account::Type::Admin:
		{
			std::vector<std::string> menuItems =
			{
				"Рэдагаваць асабістыя дадзеныя",
				"Праглядзець усі ўліковыя запісы",
				"Кіраванне ўліковымі запісамі",
				"Спіс кіроўцаў",
				"Спіс грузаў",
				"Спіс грузавікоў",
				"Спіс прычэпаў",
				"Спіс гарадоў і краін",
				"Спіс даставак",
				"Разглядзець заяўкі на працу",
				"Аднавіць дастаўкі",
				"Сартаванне",
				"Фільтр",
				"Пошук",
				"Звольніцца",
				"Даклад у файл",
				"Выхад"
			};

			while (true) {
				choice = ShowMenuWithNavigation(menuItems, "Меню:");
				std::cout << std::endl;

				switch (choice) {
				case 1:
					EditAccount();
					break;
				case 2:
					ShowAllAccounts();
					break;
				case 3:
					EditAccounts();

					if (account == nullptr)
						if ((account = Account::Authorise(accounts, countries)) == nullptr)
							return;

					goto menu_begin;
				case 4:
					DriversList();
					break;
				case 5:
					CargosList();
					break;
				case 6:
					LorriesList();
					break;
				case 7:
					TrailersList();
					break;
				case 8:
					AreasList();
					break;
				case 9:
					DeliveriesList();
					break;
				case 10:
					ConsiderJobApplications();
					break;
				case 11:
					UpdateDistance();
					break;
				case 12:
					AdminSort();
					break;
				case 13:
					Filter();
					break;
				case 14:
					Search();
					break;
				case 15:
					ModAdmQuit();
					goto menu_begin;
				case 16:
					Report();
					break;
				case 17:
					return;
				}
			}
			break;
		}
		}
	}
}