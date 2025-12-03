#include "DeliverySystem.h"
#include <iostream>
#include <fstream>
#include <iomanip>

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
		while (true)
		{
			std::cout << "Што хаціце адрэдагаваць?\n"
				<< "1. Імя акаўнту\n2. Пароль\n3. Уласнае імя\n4. Прозвішча\n5. Нумар тэлефону\n0. Выхад\n"
				<< "Ваш выбар: ";
			while (true)
			{
				std::cin >> choice;

				if (!std::cin.good())
				{
					std::cin.clear();
					std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
						<< std::endl << std::endl;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

					while (true)
					{
						std::cout << std::endl;
						for (size_t i = 0; i < countries.size(); i++)
						{
							std::cout << i + 1 << ". " << countries[i].GetName() << std::endl;
						}

						std::cout << std::endl << "Выбярыце вашу краіну: ";
						std::cin >> choice;

						if (!std::cin.good())
						{
							std::cin.clear();
							std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
								<< std::endl << std::endl;
							std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
							continue;
						}
						else if (choice > countries.size() || choice < 1)
						{
							std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m"
								<< std::endl;
							continue;
						}

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

							break;
						}

						account->SetPhoneNumber(phoneCode + std::to_string(phoneNumber));

						break;
					}

					break;
				}
				default:
					std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
				}
			}
		}
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
				int choiceCargo;
				std::cout << "Выбярыце груз: ";
				std::cin >> choiceCargo;
				if (!std::cin.good())
				{
					std::cin.clear();
					std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
						<< std::endl << std::endl;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}
				else if (choiceCargo > i || choiceCargo < 0)
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
				}

				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
						int choiceCity;
						std::cout << "Выбярыце горал: ";
						std::cin >> choiceCity;
						if (!std::cin.good())
						{
							std::cin.clear();
							std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
								<< std::endl << std::endl;
							std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
							continue;
						}
						else if (choiceCity > j || choiceCity < 0)
						{
							std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m"
								<< std::endl;
							continue;
						}

						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

						availableCargos[i]->RequestDelivery(account, availableCities[j]);

						std::cout << "Заказ сфарміраваны. Дастаўка пачнецца ў бліжайшы час." << std::endl;
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
		for (const auto& cargo : account->GetCargos())
			std::cout << ++i << ". " << cargo << std::endl << std::endl;
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

			while (true)
			{
				std::cin >> choice;
				if (!std::cin.good())
				{
					std::cin.clear();
					std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
						<< std::endl << std::endl;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				switch (choice)
				{
				case 1:
					applications.pop_back();
					std::cout << "\nЗаяўка паспяхова выдалена\n";
					return;
				case 2:
					std::cout << '\n';
					return;
				default:
					std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
				}
			}
		}

		char message[MESSAGE_SIZE];
		std::cout << "Дадайце паведамленне да вашай заўцы\n";
		std::cin.getline(message, MESSAGE_SIZE);

		applications.emplace_back(account, message);

		std::cout << "\nВаша заяўка паспяхова сфарміравана\n";
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

		int choiceCargo;
		while (true)
		{
			std::cout << "Выбярыце груз (0 для адмовы): ";
			std::cin >> choiceCargo;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (choiceCargo > availableCargos.size() || choiceCargo < 0)
			{
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
				continue;
			}
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

			int choiceTrailer;
			std::cout << "Выбярыце прычэп (0 для адмовы): ";
			std::cin >> choiceTrailer;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (choiceTrailer > availableCargos.size() || choiceTrailer < 0)
			{
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
				continue;
			}

			if (choiceTrailer == 0)
				return;

			deliveries.emplace_back(driver, driver->GetLorry(), availableCargos[choiceCargo - 1],
				availableTrailers[choiceTrailer - 1]);
		}
	}
	void Manager::DriverQuit(Driver* driver)
	{
		int choice;
		std::cout << "Гэта дзеянне нельга будзе адмовіць.\nВы ўпэўнены?\n1. Так\t2. Не" << std::endl;
		std::cout << "Ваш выбар: ";

		while (true)
		{
			std::cin >> choice;
			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
				continue;
			}
		}
	}
	void Manager::ShowCurrentDelivery(Driver* driver)
	{
		std::cout << driver->GetCurrentDelivery() << '\n';
	}
	/*In progress*/ void Manager::DropDelivery(Driver* driver)
	{
		std::cout << "Гэта дзеянне нельга будзе адмовіць. За няўстойкі вы будзеце аштрафаваны\n"
			<< "Вы ўпэўнены? \n1.Так\t2.Не" << std::endl;
		std::cout << "Ваш выбар: ";

		int choice;
		std::cin >> choice;
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
		int choice;
		while (true)
		{
			std::cout << "\nСпіс грузаў:\n";

			int i = 0;
			for (const auto& cargo : cargos)
				std::cout << ++i << '\n' << cargo << "\n\n";

			std::cout << "\nВыбярыце пункт меню:\n1. Дадаць груз\n2. Выдаліць груз\n3. Выхад\n";
			std::cout << "Ваш выбар: ";
			std::cin >> choice;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
			}
		}
	}
	void Manager::TrailersList()
	{
		int choice;
		while (true)
		{
			std::cout << "\nСпіс прычэпаў:\n";
			int i = 0;
			for (const auto& trailer : trailers)
				std::cout << ++i << '\n' << *trailer << '\n';

			std::cout << "\nВыбярыце пункт меню:\n1. Дадаць прычэп\n2. Выдаліць прычэп\n3. Выхад\n";
			std::cout << "Ваш выбар: ";
			std::cin >> choice;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			switch (choice)
			{
			case 1:
			{
				float length, maxPayload;
				int typeChoice;

				std::cout << "Увядзіце даўжыню прычэпа: ";
				std::cin >> length;
				std::cout << "Увядзіце максімальную нагрузку: ";
				std::cin >> maxPayload;

				std::cout << "Выбярыце тып прычэпа:\n";
				std::cout << "1. Аўтавоз\n2. Цыстэрна\n3. Лесавоз\n4. Трал\n5. Тэнтавы\n6. Рэфрыжэратар\n";
				std::cin >> typeChoice;
				std::cin.ignore();

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
				default:
					std::cout << "\x1b[31;1m" << "Няверны тып прычэпа!" << "\x1b[0m" << std::endl;
					continue;
				}

				trailers.push_back(std::move(newTrailer));
				std::cout << "Прычэп паспяхова дададзены!\n";
				break;
			}
			case 2:
			{
				int trailerChoice;
				std::cout << "Увядзіце нумар прычэпа для выдалення: ";
				std::cin >> trailerChoice;
				std::cin.ignore();

				if (trailerChoice > 0 && trailerChoice <= trailers.size())
				{
					trailers.erase(trailers.begin() + trailerChoice - 1);
					std::cout << "Прычэп паспяхова выдалены!\n";
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны нумар прычэпа!" << "\x1b[0m" << std::endl;
				}
				break;
			}
			case 3:
				return;
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
			}
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

		while (true)
		{
			int i = 0;
			for (const auto& application : applications)
				std::cout << ++i << ". " << application << '\n';

			while (true)
			{
				std::cout << "Выбярыце заяўку да разгляду: ";
				std::cin >> choiceApp;

				if (!std::cin.good())
				{
					std::cin.clear();
					std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
						<< std::endl << std::endl;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				while (true)
				{
					std::cout << "Падцвердзіць заяўку?\n1. Так\t2. Не";
					std::cin >> choice;

					if (!std::cin.good())
					{
						std::cin.clear();
						std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
							<< std::endl << std::endl;
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						continue;
					}

					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
					default:
						std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
						continue;
					}

					if (applications.empty())
						return;

					std::cout << "Жадаеце працягнуць?\n1. Так\t2. Не\n";

					while (true)
					{
						std::cin >> choice;
						if (!std::cin.good())
						{
							std::cin.clear();
							std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
								<< std::endl << std::endl;
							std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
							continue;
						}

						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

						switch (choice)
						{
						case 1:
							break;
						case 2:
							return;
						default:
							std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
							continue;
						}
					}

					break;
				}
				break;
			}
		}
	}
	void Manager::ModAdmQuit()
	{
		int choice;
		std::cout << "Гэта дзеянне нельга будзе адмовіць.\nВы ўпэўнены?\n1. Так\t2. Не" << std::endl;
		std::cout << "Ваш выбар: ";

		while (true)
		{
			std::cin >> choice;
			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			switch (choice)
			{
			case 1:
				account->SetType(Account::Type::User);
				std::cout << "Вы звольнены." << std::endl;
				return;
			case 2:
				return;
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
				continue;
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
		int choice;
		while (true)
		{
			std::cout << "\nСпіс уліковых запісаў:\n";
			int i = 0;
			for (const auto& acc : accounts)
				std::cout << ++i << '\n' << acc << '\n';

			std::cout << "\nВыбярыце пункт меню:\n1. Змяніць тып акаўнту\n2. Выдаліць акаўнт\n3. Выхад\n";
			std::cout << "Ваш выбар: ";
			std::cin >> choice;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			switch (choice)
			{
			case 1:
			{
				int accountChoice, typeChoice;
				std::cout << "Увядзіце нумар акаўнту для змены: ";
				std::cin >> accountChoice;
				std::cin.ignore();

				if (accountChoice > 0 && accountChoice <= accounts.size())
				{
					Account& selectedAccount = accounts[accountChoice - 1];

					std::cout << "Выбярыце новы тып акаўнту:\n";
					std::cout << "1. User\n2. Driver\n3. Moderator\n4. Admin\n";
					std::cin >> typeChoice;
					std::cin.ignore();

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
					default:
						std::cout << "\x1b[31;1m" << "Няверны тып акаўнту!" << "\x1b[0m" << std::endl;
						continue;
					}

					selectedAccount.SetType(newType);
					std::cout << "Тып акаўнту паспяхова зменены!\n";
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны нумар акаўнту!" << "\x1b[0m" << std::endl;
				}
				break;
			}
			case 2:
			{
				int accountChoice;
				std::cout << "Увядзіце нумар акаўнту для выдалення: ";
				std::cin >> accountChoice;
				std::cin.ignore();

				if (accountChoice > 0 && accountChoice <= accounts.size())
				{
					Account& accountToRemove = accounts[accountChoice - 1];

					for (const auto& driver : drivers)
					{
						if (driver.GetAccount() == &accountToRemove)
						{
							std::cout << "\x1b[31;1m" << "Немагчыма выдаліць акаўнт, які выкарыстоўваецца кіроўцам!"
								<< "\x1b[0m" << std::endl;
							break;
						}
					}

					accounts.erase(accounts.begin() + accountChoice - 1);
					std::cout << "Акаўнт паспяхова выдалены!\n";
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны нумар акаўнту!" << "\x1b[0m" << std::endl;
				}
				break;
			}
			case 3:
				return;
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
			}
		}
	}
	void Manager::DriversList()
	{
		int choice;
		while (true)
		{
			std::cout << "\nСпіс кіроўцаў:\n";
			int i = 0;
			for (const auto& driver : drivers)
				std::cout << ++i << '\n' << driver << '\n';

			std::cout << "\nВыбярыце пункт меню:\n1. Дадаць кіроўцу\n2. Выдаліць кіроўцу\n3. Выхад\n";
			std::cout << "Ваш выбар: ";
			std::cin >> choice;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			switch (choice)
			{
			case 1:
			{
				Account* freeAccount = nullptr;
				std::vector<Account*> availableAccounts;

				std::cout << "Даступныя аккаунты:\n";
				int j = 0;
				for (auto& account : accounts)
				{
					if (account.GetType() == Account::Type::User)
					{
						bool isDriver = false;
						for (const auto& driver : drivers)
						{
							if (driver.GetAccount() == &account)
							{
								isDriver = true;
								break;
							}
						}

						if (!isDriver)
						{
							std::cout << ++j << ". " << account.GetNickname() << std::endl;
							availableAccounts.push_back(&account);
						}
					}
				}

				if (availableAccounts.empty())
				{
					std::cout << "Няма даступных аккаунтаў для прызначэння кіроўцам!\n";
					break;
				}

				int accountChoice;
				std::cout << "Выбярыце аккаунт: ";
				std::cin >> accountChoice;
				std::cin.ignore();

				if (accountChoice > 0 && accountChoice <= availableAccounts.size())
				{
					freeAccount = availableAccounts[accountChoice - 1];
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар аккаунта!" << "\x1b[0m" << std::endl;
					break;
				}

				Lorry* freeLorry = nullptr;
				std::vector<Lorry*> availableLorries;

				std::cout << "Даступныя грузавікі:\n";
				int k = 0;
				for (auto& lorry : lorries)
				{
					if (lorry.GetOwner() == nullptr)
					{
						std::cout << ++k << ". " << lorry.GetMake() << " " << lorry.GetModel() << std::endl;
						availableLorries.push_back(&lorry);
					}
				}

				if (availableLorries.empty())
				{
					std::cout << "Няма даступных грузавікоў!\n";
					break;
				}

				int lorryChoice;
				std::cout << "Выбярыце грузавік: ";
				std::cin >> lorryChoice;
				std::cin.ignore();

				if (lorryChoice > 0 && lorryChoice <= availableLorries.size())
				{
					freeLorry = availableLorries[lorryChoice - 1];
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар грузавіка!" << "\x1b[0m" << std::endl;
					break;
				}

				drivers.emplace_back(freeAccount, freeLorry);
				freeAccount->SetType(Account::Type::Driver);
				freeLorry->SetOwner(&drivers.back());

				std::cout << "Кіроўца паспяхова дададзены!\n";
				break;
			}
			case 2:
			{
				int driverChoice;
				std::cout << "Увядзіце нумар кіроўцы для выдалення: ";
				std::cin >> driverChoice;
				std::cin.ignore();

				if (driverChoice > 0 && driverChoice <= drivers.size())
				{
					Driver& driverToRemove = drivers[driverChoice - 1];

					if (driverToRemove.GetLorry())
					{
						driverToRemove.GetLorry()->SetOwner(nullptr);
					}

					if (driverToRemove.GetAccount())
					{
						driverToRemove.GetAccount()->SetType(Account::Type::User);
					}

					drivers.erase(drivers.begin() + driverChoice - 1);
					std::cout << "Кіроўца паспяхова выдалены!\n";
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны нумар кіроўцы!" << "\x1b[0m" << std::endl;
				}
				break;
			}
			case 3:
				return;
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
			}
		}
	}
	void Manager::LorriesList()
	{
		int choice;
		while (true)
		{
			std::cout << "\nСпіс грузавікоў:\n";
			int i = 0;
			for (const auto& lorry : lorries)
				std::cout << ++i << '\n' << lorry << '\n';

			std::cout << "\nВыбярыце пункт меню:\n1. Дадаць грузавік\n2. Выдаліць грузавік\n3. Выхад\n";
			std::cout << "Ваш выбар: ";
			std::cin >> choice;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
			
				std::cout << "Увядзіце кошт паліва (на 100км): ";
				std::cin >> gasolineCost;
				std::cin.ignore();

				std::cout << "Даступныя краіны:\n";
				int j = 0;
				for (const auto& country : countries)
				{
					std::cout << ++j << ". " << country.GetName() << std::endl;
				}

				std::cout << "Выбярыце краіну: ";
				std::cin >> countryChoice;
				std::cin.ignore();

				if (countryChoice <= 0 || countryChoice > countries.size())
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар краіны!" << "\x1b[0m" << std::endl;
					break;
				}

				Country& selectedCountry = countries[countryChoice - 1];
			
				std::cout << "Даступныя гарады:\n";
				int k = 0;
				for (const auto& city : selectedCountry.GetCities())
				{
					std::cout << ++k << ". " << city.GetName() << std::endl;
				}

				std::cout << "Выбярыце горад: ";
				std::cin >> cityChoice;
				std::cin.ignore();

				if (cityChoice <= 0 || cityChoice > selectedCountry.GetCities().size())
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар горада!" << "\x1b[0m" << std::endl;
					break;
				}

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
				int lorryChoice;
				std::cout << "Увядзіце нумар грузавіка для выдалення: ";
				std::cin >> lorryChoice;
				std::cin.ignore();

				if (lorryChoice > 0 && lorryChoice <= lorries.size())
				{
					Lorry& lorryToRemove = lorries[lorryChoice - 1];
					if (lorryToRemove.GetOwner() != nullptr)
					{
						std::cout << "\x1b[31;1m" << "Немагчыма выдаліць грузавік, які выкарыстоўваецца кіроўцам!" 
								  << "\x1b[0m" << std::endl;
						break;
					}

					lorries.erase(lorries.begin() + lorryChoice - 1);
					std::cout << "Грузавік паспяхова выдалены!\n";
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны нумар грузавіка!" << "\x1b[0m" << std::endl;
				}
				break;
			}
			case 3:
				return;
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
			}
		}
	}
	void Manager::AreasList()
	{
		int choice;
		while (true)
		{
			std::cout << "\nСпіс краін і гарадоў:\n";
			int i = 0;
			for (const auto& country : countries)
			{
				std::cout << ++i << ". " << country << std::endl;
				for (const auto& city : country.GetCities())
				{
					std::cout << "   - " << city << std::endl;
				}
			}

			std::cout << "\nВыбярыце пункт меню:\n1. Дадаць краіну\n2. Дадаць горад\n3. Выдаліць краіну\n4. Выдаліць горад\n5. Выхад\n";
			std::cout << "Ваш выбар: ";
			std::cin >> choice;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

				std::cout << "Выбярыце краіну: ";
				std::cin >> countryChoice;
				std::cin.ignore();

				if (countryChoice <= 0 || countryChoice > countries.size())
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар краіны!" << "\x1b[0m" << std::endl;
					break;
				}

				std::cout << "Увядзіце назву горада: ";
				std::getline(std::cin, name);

				std::cout << "Увядзіце абрэвіятуру горада: ";
				std::getline(std::cin, abbreviation);

				std::cout << "Увядзіце каардынату X: ";
				std::cin >> x;

				std::cout << "Увядзіце каардынату Y: ";
				std::cin >> y;
				std::cin.ignore();

				countries[countryChoice - 1].AddCity(City(name, abbreviation, countries[countryChoice - 1], x, y, countries));
				std::cout << "Горад паспяхова дададзены!\n";
				break;
			}
			case 3:
			{
				int countryChoice;
				std::cout << "Увядзіце нумар краіны для выдалення: ";
				std::cin >> countryChoice;
				std::cin.ignore();

				if (countryChoice > 0 && countryChoice <= countries.size())
				{
					if (!countries[countryChoice - 1].GetCities().empty())
					{
						std::cout << "\x1b[31;1m" << "Немагчыма выдаліць краіну, якая мае гарады!" << "\x1b[0m" << std::endl;
						break;
					}

					countries.erase(countries.begin() + countryChoice - 1);
					std::cout << "Краіна паспяхова выдалена!\n";
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны нумар краіны!" << "\x1b[0m" << std::endl;
				}
				break;
			}
			case 4:
			{
				int countryChoice, cityChoice;

				std::cout << "Даступныя краіны:\n";
				int j = 0;
				for (const auto& country : countries)
				{
					std::cout << ++j << ". " << country.GetName() << std::endl;
				}

				std::cout << "Выбярыце краіну: ";
				std::cin >> countryChoice;
				std::cin.ignore();

				if (countryChoice <= 0 || countryChoice > countries.size())
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар краіны!" << "\x1b[0m" << std::endl;
					break;
				}

				Country& selectedCountry = countries[countryChoice - 1];

				if (selectedCountry.GetCities().empty())
				{
					std::cout << "\x1b[31;1m" << "У гэтай краіны няма гарадоў!" << "\x1b[0m" << std::endl;
					break;
				}

				std::cout << "Даступныя гарады:\n";
				int k = 0;
				for (const auto& city : selectedCountry.GetCities())
				{
					std::cout << ++k << ". " << city.GetName() << std::endl;
				}

				std::cout << "Увядзіце нумар горада для выдалення: ";
				std::cin >> cityChoice;
				std::cin.ignore();

				if (cityChoice > 0 && cityChoice <= selectedCountry.GetCities().size())
				{
					selectedCountry.RemoveCity(cityChoice - 1);
					std::cout << "Горад паспяхова выдалены!\n";
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны нумар горада!" << "\x1b[0m" << std::endl;
				}
				break;
			}
			case 5:
				return;
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
			}
		}
	}
	void Manager::DeliveriesList()
	{
		int choice;
		while (true)
		{
			std::cout << "\nСпіс даставак:\n";
			int i = 0;
			for (const auto& delivery : deliveries)
				std::cout << ++i << '\n' << delivery << '\n';

			std::cout << "\nВыбярыце пункт меню:\n1. Дадаць дастаўку\n2. Выдаліць дастаўку\n3. Выхад\n";
			std::cout << "Ваш выбар: ";
			std::cin >> choice;

			if (!std::cin.good())
			{
				std::cin.clear();
				std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
					<< std::endl << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			switch (choice)
			{
			case 1:
			{
				Driver* freeDriver = nullptr;
				std::vector<Driver*> availableDrivers;

				std::cout << "Даступныя кіроўцы:\n";
				int j = 0;
				for (auto& driver : drivers)
				{
					if (driver.GetCurrentDelivery() == nullptr)
					{
						std::cout << ++j << ". " << driver.GetAccount()->GetNickname() << std::endl;
						availableDrivers.push_back(&driver);
					}
				}

				if (availableDrivers.empty())
				{
					std::cout << "Няма даступных кіроўцаў!\n";
					break;
				}

				int driverChoice;
				std::cout << "Выбярыце кіроўцу: ";
				std::cin >> driverChoice;
				std::cin.ignore();

				if (driverChoice > 0 && driverChoice <= availableDrivers.size())
				{
					freeDriver = availableDrivers[driverChoice - 1];
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар кіроўцы!" << "\x1b[0m" << std::endl;
					break;
				}

				Cargo* freeCargo = nullptr;
				std::vector<Cargo*> availableCargos;

				std::cout << "Даступныя грузы:\n";
				int k = 0;
				for (auto& cargo : cargos)
				{
					if (cargo.GetCurrentDelivery() == nullptr && cargo.GetClient() != nullptr)
					{
						std::cout << ++k << ". " << cargo.GetName() << std::endl;
						availableCargos.push_back(&cargo);
					}
				}

				if (availableCargos.empty())
				{
					std::cout << "Няма даступных грузаў!\n";
					break;
				}

				int cargoChoice;
				std::cout << "Выбярыце груз: ";
				std::cin >> cargoChoice;
				std::cin.ignore();

				if (cargoChoice > 0 && cargoChoice <= availableCargos.size())
				{
					freeCargo = availableCargos[cargoChoice - 1];
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар груза!" << "\x1b[0m" << std::endl;
					break;
				}

				Trailer* suitableTrailer = nullptr;
				std::vector<Trailer*> availableTrailers;

				std::cout << "Даступныя прычэпы:\n";
				int l = 0;
				for (auto& trailer : trailers)
				{
					if (trailer->GetCurrentDelivery() == nullptr && trailer->IsCargoSupported(freeCargo))
					{
						std::cout << ++l << ". " << trailer->GetTypeString() << std::endl;
						availableTrailers.push_back(trailer.get());
					}
				}

				if (availableTrailers.empty())
				{
					std::cout << "Няма даступных прычэпаў для гэтага груза!\n";
					break;
				}

				int trailerChoice;
				std::cout << "Выбярыце прычэп: ";
				std::cin >> trailerChoice;
				std::cin.ignore();

				if (trailerChoice > 0 && trailerChoice <= availableTrailers.size())
				{
					suitableTrailer = availableTrailers[trailerChoice - 1];
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны выбар прычэпа!" << "\x1b[0m" << std::endl;
					break;
				}

				deliveries.emplace_back(freeDriver, freeDriver->GetLorry(), freeCargo, suitableTrailer);
				std::cout << "Дастаўка паспяхова створана!\n";
				break;
			}
			case 2:
			{
				int deliveryChoice;
				std::cout << "Увядзіце нумар дастаўкі для выдалення: ";
				std::cin >> deliveryChoice;
				std::cin.ignore();

				if (deliveryChoice > 0 && deliveryChoice <= deliveries.size())
				{
					deliveries[deliveryChoice - 1].StopDelivery();
					deliveries.erase(deliveries.begin() + deliveryChoice - 1);
					std::cout << "Дастаўка паспяхова выдалена!\n";
				}
				else
				{
					std::cout << "\x1b[31;1m" << "Няверны нумар дастаўкі!" << "\x1b[0m" << std::endl;
				}
				break;
			}
			case 3:
				return;
			default:
				std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
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
			while (true)
			{
				std::cout << std::endl << std::setw(20) << "\x1b[33;1m" << "Меню:" << "\x1b[0m" << std::endl;
				std::cout << "1. Заказаць дастаўку" << std::endl
					<< "2. Праглядзець актыўныя заказы" << std::endl
					<< "3. Рэдагаваць асабістыя дадзеныя" << std::endl
					<< "4. Праглядзець даступныя гарады і краіны" << std::endl
					<< "5. Стань кіроўцай!" << std::endl
					<< "6. Выхад" << std::endl;
				std::cout << "Выбярыце пункт меню: ";
				std::cin >> choice;
				if (!std::cin.good())
				{
					std::cin.clear();
					std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
						<< std::endl << std::endl;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
					BecomeADriver();
					break;
				case 6:
					return;
				default:
					std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
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
						<< "2. Рэдагаваць асабістыя дадзеныя" << std::endl
						<< "3. Звольніцца" << std::endl
						<< "4. Выхад" << std::endl;
					std::cout << "Выбярыце пункт меню: ";
					std::cin >> choice;
					if (!std::cin.good())
					{
						std::cin.clear();
						std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
							<< std::endl << std::endl;
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						continue;
					}

					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << std::endl;

					switch (choice)
					{
					case 1:
						AcceptDelivery(driver);
						break;
					case 2:
						EditAccount();
						break;
					case 3:
						DriverQuit(driver);
						goto menu_begin;
					case 4:
						return;
					default:
						std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
						continue;
					}
				}
				else
				{
					std::cout << std::endl << std::setw(20) << "\x1b[33;1m" << "Меню:" << "\x1b[0m" << std::endl;
					std::cout << "1. Праглядзець бягучы заказ" << std::endl
						<< "2. Адмовіцца ад заказу" << std::endl
						<< "3. Рэдагаваць асабістыя дадзеныя" << std::endl
						<< "4. Выхад" << std::endl;
					std::cout << "Выбярыце пункт меню: ";
					std::cin >> choice;
					if (!std::cin.good())
					{
						std::cin.clear();
						std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
							<< std::endl << std::endl;
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						continue;
					}

					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << std::endl;

					switch (choice)
					{
					case 1:
						ShowCurrentDelivery(driver);
						break;
					case 2:
						DropDelivery(driver);
						break;
					case 3:
						EditAccount();
						break;
					case 4:
						return;
					default:
						std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
						continue;
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
				std::cout << "Выбярыце пункт меню: ";
				std::cin >> choice;
				if (!std::cin.good())
				{
					std::cin.clear();
					std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
						<< std::endl << std::endl;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
				default:
					std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
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
					<< "11. Звольніцца" << std::endl
					<< "12. Выхад" << std::endl;
				std::cout << "Выбярыце пункт меню: ";
				std::cin >> choice;
				if (!std::cin.good())
				{
					std::cin.clear();
					std::cout << "\x1b[31;1m" << "Памылка ўводу. Паспрабуйце яшчэ раз" << "\x1b[0m"
						<< std::endl << std::endl;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
					ModAdmQuit();
					goto menu_begin;
				case 12:
					return;
				default:
					std::cout << "\x1b[31;1m" << "Няверны выбар. Паспрабуйце яшчэ раз" << "\x1b[0m" << std::endl;
					continue;
				}
			}

			break;
		}
		}
	}
}