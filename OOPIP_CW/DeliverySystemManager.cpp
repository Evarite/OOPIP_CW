#include "DeliverySystem.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

constexpr auto COUNTRIES = "Countries.bin";
constexpr auto ACCOUNTS = "Accounts.bin";
constexpr auto CARGOS = "Cargos.bin";
constexpr auto DELIVERIES = "Deliveries.bin";
constexpr auto LORRIES = "Lorries.bin";
constexpr auto TRAILERS = "Trailers.bin";
constexpr auto DRIVERS = "Drivers.bin";
constexpr auto APPLICATIONS = "Applications.bin";

namespace DeliverySystem
{
	Account* Manager::FindAccount(const std::string& nickname)
	{
		for (size_t i = 0; i < accounts.size(); i++)
		{
			if (nickname == accounts[i].GetNickname())
				return &accounts[i];
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
			driverBuf.GetLorry()->SetOwner(&driverBuf);
		}
		driversFile.close();

		std::ifstream cargosFile(CARGOS, std::ios::binary);
		Cargo cargoBuf;
		while (cargosFile >> cargoBuf)
			cargos.push_back(cargoBuf);
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
			trailersFile >> *trailer;

			trailers.push_back(std::move(trailer));
		}
		trailersFile.close();

		std::ifstream deliveriesFile(DELIVERIES, std::ios::binary);
		Delivery deliveryBuf;
		while (deliveriesFile >> deliveryBuf)
			deliveries.push_back(deliveryBuf);
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
		int choice;
		std::cout << "Што хаціце адрэдагаваць?\n"
			<< "1. Імя акаўнту\n2. Пароль\n3. Уласнае імя\n4. Прозвішча\n5. Нумар тэлефону\n0. Выхад\n"
			<< "Ваш выбар: ";
		choice = GetIntWithinRange(0, 5);

		switch (choice)
		{
		case 0:
			return;
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
			for (size_t i = 0; i < countries.size(); i++)
			{
				std::cout << i + 1 << ". " << countries[i].GetName() << std::endl;
			}

			choice = GetIntWithinRange(1, countries.size(), "Выбярыце вашу краіну: ");

			while (true)
			{
				phoneCode = countries[choice - 1].GetPhoneCode();

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

		for (auto& delivery : deliveries)
			delivery.UpdateDistance(deliveries);

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
			for (auto& cargo : cargos)
			{
				if (cargo.GetCurrentDelivery() == nullptr)
				{
					std::cout << ++i << ". " << cargo << std::endl;
					availableCargos.push_back(&cargo);
				}
			}
			if (i == 0)
			{
				std::cout << "\x1b[31;1m" << "Няма даступных грузаў" << "\x1b[0m" << std::endl;
				break;
			}
			else
			{
				int choiceCargo = GetIntWithinRange(0, i, "Выбярыце груз: ");

				while (true)
				{
					int j = 0;

					std::cout << std::endl << std::endl;
					for (auto& country : countries)
					{
						for (auto& city : country.GetCitiesL())
						{
							std::cout << ++j << ". " << city << std::endl;
							availableCities.push_back(&city);
						}
					}
					if (j == 0)
					{
						std::cout << "\x1b[31;1m" << "Няма даступных гарадоў" << "\x1b[0m" << std::endl;
						break;
					}
					else
					{
						int choiceCity = GetIntWithinRange(0, j, "Выбярыце горад: ");

						availableCargos[i - 1]->RequestDelivery(account, availableCities[j - 1]);

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
		for (auto& cargo : account->GetCargos())
			std::cout << ++i << ". " << *cargo << std::endl << std::endl;
	}
	void Manager::ShowSupportedArea()
	{
		int i = 0;
		for (const auto& country : countries)
		{
			std::cout << ++i << '.' << std::endl << country << std::endl << std::endl;
		}
	}
	void Manager::BecomeADriver()
	{
		if (applications.back().GetAccount() == account)
		{
			int choice;
			std::cout << "Вы ўжо адправілі заяўку. Жадаеце выдаліць яе?\n"
				<< "1. Так\t2. Не\n";

			choice = GetIntWithinRange(1, 2);

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
		int choice;
		std::cout << "Выбярыце, што вы жадаеце адсартаваць\n"
			<< "1. Спіс краін\n"
			<< "2. Спіс гарадоў\n"
			<< "3. Спіс грузаў\n"
			<< "4. Спіс даставак\n";

		choice = GetIntWithinRange(1, 4);

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
			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Назва краіны\n"
				<< "2. Тэлефонны код краіны\n";

			choice = GetIntWithinRange(1, 2);
			attribute = static_cast<SortAttribute>(choice - 1);

			SortOrder order;
			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(countries.begin(), countries.end(),
				[attribute, order](const Country& a, const Country& b)
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
			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Назва горада\n"
				<< "2. Абрэвіатура краіны\n";

			choice = GetIntWithinRange(1, 2);
			attribute = static_cast<SortAttribute>(choice - 1);

			SortOrder order;
			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			for (auto& country : countries)
			{
				std::sort(country.GetCitiesL().begin(), country.GetCitiesL().end(),
					[attribute, order](const City& a, const City& b)
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
			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Назва груза\n"
				<< "2. Маса груза\n"
				<< "3. Горад адпраўлення\n"
				<< "4. Горад прыбыцця\n"
				<< "5. Тып груза\n";

			choice = GetIntWithinRange(1, 5);
			attribute = static_cast<SortAttribute>(choice - 1);

			SortOrder order;
			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(cargos.begin(), cargos.end(),
				[attribute, order](const Cargo& a, const Cargo& b)
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
			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Імя кіроўцы\n"
				<< "2. Назва груза\n"
				<< "3. Маса груза\n"
				<< "4. Горад адпраўлення\n"
				<< "5. Горад прыбыцця\n"
				<< "6. Тып груза\n";

			choice = GetIntWithinRange(1, 6);
			attribute = static_cast<SortAttribute>(choice - 1);

			SortOrder order;
			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(deliveries.begin(), deliveries.end(),
				[attribute, order](const Delivery& a, const Delivery& b)
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
			if (trailer->GetCurrentDelivery() == nullptr && trailer->IsCargoSupported(availableCargos[choiceCargo - 1]))
			{
				availableTrailers.push_back(trailer.get());
				std::cout << ++j << ".\n" << trailer << "\n\n";
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
		std::cout << "Гэта дзеянне нельга будзе адмовіць.\nВы ўпэўнены?\n1. Так\t2. Не" << std::endl;
		int choice = GetIntWithinRange(1, 2);

		switch (choice)
		{
		case 1:
			driver->GetLorry()->SetOwner(nullptr);
			for (size_t i = 0; i < drivers.size(); i++)
			{
				if (*driver == drivers[i])
					drivers.erase(drivers.begin() + i);
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
		std::cout << driver->GetCurrentDelivery() << '\n';
	}
	void Manager::DropDelivery(Driver* driver)
	{
		std::cout << "Гэта дзеянне нельга будзе адмовіць. За няўстойкі вы будзеце аштрафаваны\n"
			<< "Вы ўпэўнены? \n1.Так\t2.Не" << std::endl;
		int choice = GetIntWithinRange(1, 2);

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

		std::cout << "Выбярыце атрыбут сартавання:\n"
			<< "1. Назва груза\n"
			<< "2. Маса груза\n"
			<< "3. Горад адпраўлення\n"
			<< "4. Горад прыбыцця\n"
			<< "5. Тып груза\n";

		int choice = GetIntWithinRange(1, 5);
		attribute = static_cast<SortAttribute>(choice - 1);

		std::cout << "Выбярыце парадак сартавання:\n"
			<< "1. Па ўзрастанні\n"
			<< "2. Па змяншэнні\n";

		choice = GetIntWithinRange(1, 2);
		order = static_cast<SortOrder>(choice - 1);

		std::sort(cargos.begin(), cargos.end(),
			[attribute, order](const Cargo& a, const Cargo& b)
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
		int i = 0;
		for (const auto& account : accounts)
			if (account.GetType() == Account::Type::User)
				std::cout << ++i << ".\n" << account << std::endl << std::endl;

		if (i == 0)
			std::cout << "Няма карыстальнікаў да праглядзення" << std::endl;
	}
	void Manager::ShowAllDrivers()
	{
		int i = 0;
		for (const auto& account : accounts)
			if (account.GetType() == Account::Type::Driver)
				std::cout << ++i << ".\n" << account << std::endl << std::endl;

		if (i == 0)
			std::cout << "Няма карыстальнікаў да праглядзення" << std::endl;
	}
	void Manager::CargosList()
	{
		std::cout << "\nСпіс грузаў:\n";

		int i = 0;
		for (const auto& cargo : cargos)
			std::cout << ++i << '\n' << cargo << "\n\n";

		std::cout << "\nВыбярыце пункт меню:\n1. Дадаць груз\n2. Выдаліць груз\n3. Выхад\n";
		int choice = GetIntWithinRange(1, 3);

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

			std::cout << "Увядзіце масу грузу: ";
			std::cin >> mass;
			std::cin.ignore();

			std::cout << "Выбярыце тып грузу:\n";
			std::cout << "1. Драўніна" << std::endl
				<< "2. Ежа" << std::endl
				<< "3. Аўтамабілі" << std::endl
				<< "4. Паліва" << std::endl
				<< "5. Хімічныя рэчыва" << std::endl
				<< "6. Малако" << std::endl
				<< "7. Жвір, друз" << std::endl
				<< "8. Крупы" << std::endl
				<< "9. Пясок" << std::endl
				<< "10. Бетон" << std::endl
				<< "11. Сталёвыя канструкцыі" << std::endl
				<< "12. Цэгла" << std::endl
				<< "13. Прамысловае абсталяванне" << std::endl
				<< "14. Будаўнічая тэхніка" << std::endl
				<< "15. Кантэйнеры" << std::endl
				<< "16. Выбуховыя рэчы" << std::endl
				<< "17. Таксічныя матэрыялы" << std::endl
				<< "18. Замарожаныя прадукты" << std::endl
				<< "19. Медыкаменты" << std::endl;

			std::cin >> typeChoice;
			std::cin.ignore();

			std::cout << "Даступныя гарады:\n";
			int j = 0;
			std::vector<City*> availableCities;
			for (auto& country : countries)
			{
				for (auto& city : country.GetCitiesL())
				{
					std::cout << ++j << ". " << city.GetName() << std::endl;
					availableCities.push_back(&city);
				}
			}

			int cityChoice;
			std::cout << "Выбярыце горад адпраўлення: ";
			std::cin >> cityChoice;
			std::cin.ignore();

			if (cityChoice > 0 && cityChoice <= availableCities.size())
			{
				from = availableCities[cityChoice - 1];
			}

			Cargo::Type selectedType = static_cast<Cargo::Type>(typeChoice - 1);
			cargos.emplace_back(name, mass, selectedType, from, cargos);
			std::cout << "Груз паспяхова дададзены!\n";
			break;
		}
		case 2:
		{
			int cargoChoice;
			std::cout << "Увядзіце нумар груза для выдалення: ";
			std::cin >> cargoChoice;
			std::cin.ignore();

			if (cargoChoice > 0 && cargoChoice <= cargos.size())
			{
				cargos.erase(cargos.begin() + cargoChoice - 1);
				std::cout << "Груз паспяхова выдалены!\n";
			}
			else
			{
				std::cout << "\x1b[31;1m" << "Няверны нумар груза!" << "\x1b[0m" << std::endl;
			}
			break;
		}
		case 3:
			return;
		}
	}
	void Manager::TrailersList()
	{
		std::cout << "\nСпіс прычэпаў:\n";
		int i = 0;
		for (const auto& trailer : trailers)
			std::cout << ++i << '\n' << *trailer << '\n';

		std::cout << "\nВыбярыце пункт меню:\n1. Дадаць прычэп\n2. Выдаліць прычэп\n3. Выхад\n";
		int choice = GetIntWithinRange(1, 3);

		switch (choice)
		{
		case 1:
		{
			float length, maxPayload;
			int typeChoice;

			length = GetFloat("Увядзіце даўжыню прычэпа: ");
			maxPayload = GetFloat("Увядзіце максімальную нагрузку: ");

			std::cout << "Выбярыце тып прычэпа:\n";
			std::cout << "1. Аўтавоз\n2. Цыстэрна\n3. Лесавоз\n4. Трал\n5. Тэнтавы\n6. Рэфрыжэратар\n";
			typeChoice = GetIntWithinRange(1, 6);

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
			int trailerChoice = GetIntWithinRange(0, trailers.size(), "Увядзіце нумар прычэпа для выдалення: ");

			trailers.erase(trailers.begin() + trailerChoice - 1);
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

		int i = 0;
		for (const auto& application : applications)
			std::cout << ++i << ". " << application << '\n';

		choiceApp = GetIntWithinRange(1, i, "\nВыбярыце заяўку да разгляду: ");

		std::cout << "Падцвердзіць заяўку?\n1. Так\t2. Не\n";
		choice = GetIntWithinRange(1, 2);

		switch (choice)
		{
		case 1:
			applications[choiceApp - 1].GetAccount()->SetType(Account::Type::Driver);
			applications.erase(applications.begin() + choiceApp - 1);
			std::cout << "Заяўка паспяхова прынята\n";
			break;
		case 2:
			applications.erase(applications.begin() + choiceApp - 1);
			std::cout << "Заяўка паспяхова адмоўлена\n";
			break;
		}

		if (applications.empty())
			return;

		std::cout << "Жадаеце працягнуць?\n1. Так\t2. Не\n";

		choice = GetIntWithinRange(1, 2);

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
		std::cout << "Гэта дзеянне нельга будзе адмовіць.\nВы ўпэўнены?\n1. Так\t2. Не\n" << std::endl;
		int choice = GetIntWithinRange(1, 2);

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

		std::cout << "Выбярыце, што вы жадаеце адсартаваць\n"
			<< "1. Спіс акаўнтаў\n"
			<< "2. Спіс кіроўцаў\n"
			<< "3. Спіс грузаў\n"
			<< "4. Спіс прычэпаў\n";
		int choice = GetIntWithinRange(1, 4);

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

			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Імя акаўнту\n"
				<< "2. Уласнае імя\n"
				<< "3. Прозвішча\n";
			choice = GetIntWithinRange(1, 3);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";
			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(accounts.begin(), accounts.end(),
				[attribute, order](const Account& a, const Account& b)
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

			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Імя акаўнту\n"
				<< "2. Уласнае імя\n"
				<< "3. Прозвішча\n"
				<< "4. Горад адпраўлення\n"
				<< "5. Горад прыбыцця\n";
			choice = GetIntWithinRange(1, 5);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";
			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(drivers.begin(), drivers.end(),
				[attribute, order](const Driver& a, const Driver& b)
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

			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Назва груза\n"
				<< "2. Маса груза\n"
				<< "3. Горад адпраўлення\n"
				<< "4. Горад прыбыцця\n"
				<< "5. Тып груза\n";

			int choice = GetIntWithinRange(1, 5);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(cargos.begin(), cargos.end(),
				[attribute, order](const Cargo& a, const Cargo& b)
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

			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Даўжыня\n"
				<< "2. Максімальная загрузка\n"
				<< "3. Горад адпраўлення\n"
				<< "4. Горад прыбыцця\n"
				<< "5. Тып прычепу\n";
			choice = GetIntWithinRange(1, 5);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";
			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(trailers.begin(), trailers.end(),
				[attribute, order](const std::unique_ptr<Trailer>& a, const std::unique_ptr<Trailer>& b)
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
		int i = 0;
		for (const auto& account : accounts)
			std::cout << ++i << ".\n" << account << "\n\n";
	}
	void Manager::EditAccounts()
	{
		std::cout << "\nСпіс уліковых запісаў:\n";
		int i = 0;
		for (const auto& acc : accounts)
			std::cout << ++i << '\n' << acc << '\n';

		std::cout << "\nВыбярыце пункт меню:\n1. Змяніць тып акаўнту\n2. Выдаліць акаўнт\n3. Выхад\n";
		int choice = GetIntWithinRange(1, 3);

		switch (choice)
		{
		case 1:
		{
			int accountChoice, typeChoice;
			accountChoice = GetIntWithinRange(1, accounts.size(), "Увядзіце нумар акаўнту для змены: ");

			Account& selectedAccount = accounts[accountChoice - 1];

			std::cout << "Выбярыце новы тып акаўнту:\n";
			std::cout << "1. User\n2. Driver\n3. Moderator\n4. Admin\n";
			typeChoice = GetIntWithinRange(1, 4);

			Account::Type newType;
			switch (typeChoice)
			{
			case 1:
				newType = Account::Type::User;
				break;
			case 2:
				newType = Account::Type::Driver;
				break;
			case 3:
				newType = Account::Type::Moderator;
				break;
			case 4:
				newType = Account::Type::Admin;
				break;
			}

			selectedAccount.SetType(newType);
			std::cout << "Тып акаўнту паспяхова зменены!\n";

			break;
		}
		case 2:
		{
			int accountChoice = GetIntWithinRange(1, accounts.size(), "Увядзіце нумар акаўнту для выдалення: ");

			Account* accountToRemove = &accounts[accountChoice - 1];

			for (const auto& driver : drivers)
			{
				if (driver.GetAccount() == accountToRemove)
				{
					std::cout << "\x1b[31;1m" << "Немагчыма выдаліць акаўнт, які выкарыстоўваецца кіроўцай!"
						<< "\x1b[0m" << std::endl;

					break;
				}
			}

			accounts.erase(accounts.begin() + accountChoice - 1);
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
		int i = 0;
		for (const auto& driver : drivers)
			std::cout << ++i << ".\n" << driver << "\n\n";

		std::cout << "Выбярыце пункт меню:\n1. Зволніць кіроўцу\n2. Выхад\n";
		int choice = GetIntWithinRange(1, 2);

		switch (choice)
		{
		case 1:
		{
			int driverChoice = GetIntWithinRange(0, drivers.size(), 
				"Выбярыце кіроўцу да звальнення (0 для адмовы): ");
			if (driverChoice == 0)
				return;

			if (drivers[driverChoice - 1].GetCurrentDelivery() != nullptr)
			{
				drivers[driverChoice - 1].GetCurrentDelivery()->StopDelivery(deliveries);
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
		int i = 0;
		for (const auto& lorry : lorries)
			std::cout << ++i << '\n' << lorry << '\n';

		std::cout << "\nВыбярыце пункт меню:\n1. Дадаць грузавік\n2. Выдаліць грузавік\n3. Выхад\n";
		choice = GetIntWithinRange(1, 3);

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

			countryChoice = GetIntWithinRange(0, countries.size(), "Выбярыце краіну: ");

			Country& selectedCountry = countries[countryChoice - 1];

			std::cout << "Даступныя гарады:\n";
			int k = 0;
			for (const auto& city : selectedCountry.GetCities())
			{
				std::cout << ++k << ". " << city.GetName() << std::endl;
			}

			std::cout << "Выбярыце горад: ";
			cityChoice = GetIntWithinRange(0, selectedCountry.GetCities().size());

			City& selectedCity = selectedCountry.GetCitiesL()[cityChoice - 1];

			std::cout << "Увядзіце рэгістрацыйныя знакі: ";
			std::getline(std::cin, registrationSigns);

			lorries.emplace_back(make, model, mileage, selectedCountry, selectedCity,
				registrationSigns, gasolineCost, lorries);
			std::cout << "Грузавік паспяхова дададзены!\n";
			break;
		}
		case 2:
		{
			int lorryChoice = GetIntWithinRange(0, lorries.size(), "Увядзіце нумар грузавіка для выдалення: ");

			Lorry& lorryToRemove = lorries[lorryChoice - 1];
			if (lorryToRemove.GetOwner() != nullptr)
			{
				std::cout << "\x1b[31;1m" << "Немагчыма выдаліць грузавік, які выкарыстоўваецца кіроўцай!"
					<< "\x1b[0m" << std::endl;
				break;
			}

			lorries.erase(lorries.begin() + lorryChoice - 1);
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
		int i = 0;
		for (const auto& country : countries)
		{
			std::cout << ++i << ". " << country << std::endl;
		}

		std::cout << "\nВыбярыце пункт меню:\n1. Дадаць краіну\n2. Дадаць горад\n"
			<< "3. Выдаліць краіну\n4. Выдаліць горад\n5. Выхад\n";
		int choice = GetIntWithinRange(1, 5);

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

			countryChoice = GetIntWithinRange(0, countries.size(), "Выбярыце краіну: ");

			std::cout << "Увядзіце назву горада: ";
			std::getline(std::cin, name);

			std::cout << "Увядзіце абрэвіятуру горада: ";
			std::getline(std::cin, abbreviation);

			x = GetInt("Увядзіце каардынату X: ");
			y = GetInt("Увядзіце каардынату Y: ");

			City(name, abbreviation, countries[countryChoice - 1], x, y, countries);
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
			for (auto& country : availableCountries)
				std::cout << ++i << '\n' << *country << "\n\n";

			int countryChoice = GetIntWithinRange(0, countries.size(),
				"Увядзіце нумар краіны для выдалення (0 для адмовы): ");
			if (countryChoice == 0)
				return;

			int j = 0;
			for(const auto& country : countries)
			{
				j++;
				if (country == *availableCountries[countryChoice - 1])
				{
					countries.erase(countries.begin() + j);
				}
			}

			std::cout << "Краіна паспяхова выдалена!\n";

			break;
		}
		case 4:
		{
			int countryChoice, cityChoice;

			countryChoice = GetIntWithinRange(0, countries.size(), "Выбярыце краіну (0 для адмовы): ");
			if (countryChoice == 0)
				return;

			Country& selectedCountry = countries[countryChoice - 1];

			if (selectedCountry.GetCities().empty())
			{
				std::cout << "\x1b[31;1m" << "У гэтай краіне няма гарадоў!" << "\x1b[0m" << std::endl;
				break;
			}

			std::vector<City*> availableCities;
			for (auto& city : selectedCountry.GetCitiesL())
				for (const auto& cargo : cargos)
					if (*cargo.GetCityFrom() == city || (cargo.GetCityTo() != nullptr && *cargo.GetCityTo() == city))
					{
						std::cout << "Немагчыма выдаліць горад " << city.GetName()
							<< ", бо ён існуе ў адным з грузаў\n";
					}
					else
						availableCities.push_back(&city);

			std::cout << '\n';

			if (availableCities.empty())
			{
				std::cout << "Дадзеная краіна не мае даступных гарадоў да выдалення\n";
				break;
			}

			std::cout << "Даступныя гарады да выдалення:\n";
			int j = 0;
			for (const auto& city : availableCities)
			{
				std::cout << ++j << ". " << city->GetName() << std::endl;
			}

			cityChoice = GetIntWithinRange(0, availableCities.size(),
				"Увядзіце нумар горада для выдалення (0 для адмовы): ");
			if (cityChoice == 0)
				return;

			selectedCountry.RemoveCity(availableCities[cityChoice - 1]);
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
		int i = 0;
		for (const auto& delivery : deliveries)
			std::cout << ++i << '\n' << delivery << '\n';

		std::cout << "\nВыбярыце пункт меню:\n1. Выдаліць дастаўку\n2. Выхад\n";
		int choice = GetIntWithinRange(1, 2);

		switch (choice)
		{
		case 1:
		{
			int deliveryChoice = GetIntWithinRange(0, deliveries.size(), "Увядзіце нумар дастаўкі для выдалення: ");

			deliveries[deliveryChoice - 1].StopDelivery(deliveries);

			deliveries.erase(deliveries.begin() + deliveryChoice - 1);
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

		std::cout << "Выбярыце, што вы жадаеце адсартаваць\n"
			<< "1. Спіс краін\n"
			<< "2. Спіс гарадоў\n"
			<< "3. Спіс акаўнтаў\n"
			<< "4. Спіс кіроўцаў\n"
			<< "5. Спіс грузавікоў\n"
			<< "6. Спіс грузаў\n"
			<< "7. Спіс даставак\n"
			<< "8. Спіс прычэпаў\n";
		int choice = GetIntWithinRange(1, 8);

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
			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Назва краіны\n"
				<< "2. Тэлефонны код краіны\n";

			choice = GetIntWithinRange(1, 2);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(countries.begin(), countries.end(),
				[attribute, order](const Country& a, const Country& b)
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
			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Назва горада\n"
				<< "2. Абрэвіатура краіны\n";

			choice = GetIntWithinRange(1, 2);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			for (auto& country : countries)
			{
				std::sort(country.GetCitiesL().begin(), country.GetCitiesL().end(),
					[attribute, order](const City& a, const City& b)
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

			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Імя акаўнту\n"
				<< "2. Уласнае імя\n"
				<< "3. Прозвішча\n";
			choice = GetIntWithinRange(1, 3);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";
			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(accounts.begin(), accounts.end(),
				[attribute, order](const Account& a, const Account& b)
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

			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Імя акаўнту\n"
				<< "2. Уласнае імя\n"
				<< "3. Прозвішча\n"
				<< "4. Горад адпраўлення\n"
				<< "5. Горад прыбыцця\n";
			choice = GetIntWithinRange(1, 5);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";
			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(drivers.begin(), drivers.end(),
				[attribute, order](const Driver& a, const Driver& b)
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

			std::cout << "Выбярыце атрыбут сартавання\n"
				<< "1. Марка\n"
				<< "2. Прабег\n";
			choice = GetIntWithinRange(1, 2);

			SortAttribute attribute = static_cast<SortAttribute>(choice - 1);
			
			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(lorries.begin(), lorries.end(),
				[attribute, order](const Lorry& a, const Lorry& b)
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
			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Назва груза\n"
				<< "2. Маса груза\n"
				<< "3. Горад адпраўлення\n"
				<< "4. Горад прыбыцця\n"
				<< "5. Тып груза\n";

			choice = GetIntWithinRange(1, 5);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(cargos.begin(), cargos.end(),
				[attribute, order](const Cargo& a, const Cargo& b)
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
			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Імя кіроўцы\n"
				<< "2. Назва груза\n"
				<< "3. Маса груза\n"
				<< "4. Горад адпраўлення\n"
				<< "5. Горад прыбыцця\n"
				<< "6. Тып груза\n";

			choice = GetIntWithinRange(1, 6);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";

			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(deliveries.begin(), deliveries.end(),
				[attribute, order](const Delivery& a, const Delivery& b)
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

			std::cout << "Выбярыце атрыбут сартавання:\n"
				<< "1. Даўжыня\n"
				<< "2. Максімальная загрузка\n"
				<< "3. Горад адпраўлення\n"
				<< "4. Горад прыбыцця\n"
				<< "5. Тып прычепу\n";
			choice = GetIntWithinRange(1, 5);
			attribute = static_cast<SortAttribute>(choice - 1);

			std::cout << "Выбярыце парадак сартавання:\n"
				<< "1. Па ўзрастанні\n"
				<< "2. Па змяншэнні\n";
			choice = GetIntWithinRange(1, 2);
			order = static_cast<SortOrder>(choice - 1);

			std::sort(trailers.begin(), trailers.end(),
				[attribute, order](const std::unique_ptr<Trailer>& a, const std::unique_ptr<Trailer>& b)
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

	void Manager::Menu()
	{
	menu_begin:

		int choice;

		switch (account->GetType())
		{
		case Account::Type::User:
		{
			while(true)
			{
				std::cout << std::endl << std::setw(20) << "\x1b[33;1m" << "Меню:" << "\x1b[0m" << std::endl;
				std::cout << "1. Заказаць дастаўку" << std::endl
					<< "2. Праглядзець актыўныя заказы" << std::endl
					<< "3. Рэдагаваць асабістыя дадзеныя" << std::endl
					<< "4. Праглядзець даступныя гарады і краіны" << std::endl
					<< "5. Аднавіць дастаўкі" << std::endl
					<< "6. Сартаванне" << std::endl
					<< "7. Стань кіроўцай!" << std::endl
					<< "8. Выхад" << std::endl;

				choice = GetIntWithinRange(1, 8, "Выбярыце пункт меню: ");

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
			while (true)
			{
				if (driver->GetCurrentDelivery() == nullptr)
				{
					std::cout << std::endl << std::setw(20) << "\x1b[33;1m" << "Меню:" << "\x1b[0m" << std::endl;
					std::cout << "1. Прыняць заказ" << std::endl
						<< "2. Сартаванне грузаў" << std::endl
						<< "3. Рэдагаваць асабістыя дадзеныя" << std::endl
						<< "4. Звольніцца" << std::endl
						<< "5. Выхад" << std::endl;

					choice = GetIntWithinRange(1, 4, "Выбярыце пункт меню: ");

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
						DriverQuit(driver);
						goto menu_begin;
					case 5:
						return;
					}
				}
				else
				{
					std::cout << std::endl << std::setw(20) << "\x1b[33;1m" << "Меню:" << "\x1b[0m" << std::endl;
					std::cout << "1. Праглядзець бягучы заказ" << std::endl
						<< "2. Аднавіць дастаўкі" << std::endl
						<< "3. Адмовіцца ад заказу" << std::endl
						<< "4. Рэдагаваць асабістыя дадзеныя" << std::endl
						<< "5. Выхад" << std::endl;

					choice = GetIntWithinRange(1, 5, "Выбярыце пункт меню: ");

					std::cout << std::endl;

					switch (choice)
					{
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
			while (true)
			{
				std::cout << std::endl << std::setw(20) << "\x1b[33;1m" << "Меню:" << "\x1b[0m" << std::endl;
				std::cout << "1. Рэдагаваць асабістыя дадзеныя" << std::endl
					<< "2. Праглядзець усіх карыстальнікаў" << std::endl
					<< "3. Праглядзець усіх кіроўцаў" << std::endl
					<< "4. Спіс грузаў" << std::endl
					<< "5. Спіс прычэпаў" << std::endl
					<< "6. Разглядзець заяўкі на працу" << std::endl
					<< "7. Звольніцца" << std::endl
					<< "8. Выхад" << std::endl;
				
				choice = GetIntWithinRange(1, 8, "Выбярыце пункт меню: ");

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
					ModAdmQuit();
					goto menu_begin;
				case 8:
					return;
				}
			}

			break;
		}
		case Account::Type::Admin:
		{
			while (true)
			{
				std::cout << std::endl << std::setw(20) << "\x1b[33;1m" << "Меню:" << "\x1b[0m" << std::endl;
				std::cout << "1. Рэдагаваць асабістыя дадзеныя" << std::endl
					<< "2. Праглядзець усі уліковыя запісы" << std::endl
					<< "3. Кіраванне ўліковымі запісамі" << std::endl
					<< "4. Спіс кіроўцаў" << std::endl
					<< "5. Спіс грузаў" << std::endl
					<< "6. Спіс грузавікоў" << std::endl
					<< "7. Спіс прычэпаў" << std::endl
					<< "8. Спіс гарадоў і краін" << std::endl
					<< "9. Спіс даставак" << std::endl
					<< "10. Разглядзець заяўкі на працу" << std::endl
					<< "11. Аднавіць дастаўкі" << std::endl
					<< "12. Звольніцца" << std::endl
					<< "13. Выхад" << std::endl;

				choice = GetIntWithinRange(1, 13, "Выбярыце пункт меню: ");

				std::cout << std::endl;

				switch (choice)
				{
				case 1:
					EditAccount();
					break;
				case 2:
					ShowAllAccounts();
					break;
				case 3:
					EditAccounts();
					break;
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
					ModAdmQuit();
					goto menu_begin;
				case 13:
					return;
				}
			}

			break;
		}
		}
	}
}