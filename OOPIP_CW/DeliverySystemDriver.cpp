#include "DeliverySystem.h"
#include <iostream>
#include <fstream>

namespace DeliverySystem
{
	void Driver::SetDelivery(Delivery* delivery)
	{
		currentDelivery = delivery;
	}

	void Driver::StopDelivery()
	{
		currentDelivery = nullptr;
	}

	Driver::Driver()
	{
		account = nullptr;
		lorry = nullptr;
		currentDelivery = nullptr;
	}
	Driver::Driver(Account* account, Lorry* lorry) : account(account),
		lorry(lorry), currentDelivery(nullptr)
	{
		account->SetType(Account::Type::Driver);
		lorry->SetOwner(this);
	}

	Account* Driver::GetAccount() const
	{
		return account;
	}
	Lorry* Driver::GetLorry() const
	{
		return lorry;
	}
	Delivery* Driver::GetCurrentDelivery() const
	{
		return currentDelivery;
	}

	void Driver::CancelDelivery(std::list<Delivery>& deliveries)
	{
		if(currentDelivery != nullptr)
			currentDelivery->StopDelivery(deliveries);
	}
	void Driver::Fire(std::list<Delivery>& deliveries)
	{
		if (currentDelivery != nullptr)
			currentDelivery->StopDelivery(deliveries);
		lorry->SetOwner(nullptr);
		account->SetType(Account::Type::User);
	}

	std::vector<std::string> Driver::ToTableRow() const
	{
		return { account->GetNickname(), account->GetFirstName(), account->GetLastName(), account->GetPhoneNumber(),
			lorry->GetMake(), currentDelivery != nullptr ? currentDelivery->GetCargo()->GetName() + ": " + 
			currentDelivery->GetCityFrom()->GetName() + ' ' + currentDelivery->GetCityFrom()->GetCountryAbbreviation() +
			" - " + currentDelivery->GetCityTo()->GetName() + ' ' + currentDelivery->GetCityTo()->GetCountryAbbreviation()
			: "-"};
	}
	std::vector<std::string> Driver::GetHeaders()
	{
		return { "Імя акаўнта", "Імя кіроўцы", "Прозвішча", "Нумар тэлефона", "Грузавік", "Бягучае заданне"};
	}

	bool Driver::operator==(Driver& obj)
	{
		return account->GetNickname() == obj.GetAccount()->GetNickname();
	}

	std::ostream& operator<<(std::ostream& os, const Driver& obj)
	{
		os << *obj.account << '\n';
		os << "Грузавік: " << obj.lorry->GetMake() << ' ' << obj.lorry->GetModel();
		os << '\n';
		os << "Бягучае заданне: ";
		if (obj.currentDelivery == nullptr)
		{
			os << "Няма";
		}
		else
		{
			os << obj.currentDelivery->GetCityFrom()->GetName() << " - "
				<< obj.currentDelivery->GetCityTo()->GetName() << '\t'
				<< obj.currentDelivery->GetCargo()->GetName();
		}

		return os;
	}
	
	std::ostream& operator<<(std::ofstream& os, const Driver& obj)
	{
		unsigned int lorryID = obj.lorry != nullptr ? obj.lorry->GetID() : 0;

		os.write(obj.account->GetNickname().c_str(), NAME_SIZE);
		os.write(reinterpret_cast<const char*>(&lorryID), sizeof(unsigned int));

		os.flush();

		return os;
	}

	std::istream& operator>>(std::ifstream& is, Driver& obj)
	{
		char accountName[NAME_SIZE];
		unsigned int lorryID;

		is.read(accountName, NAME_SIZE);
		if (!is.good())
		{
			return is;
		}
		is.read(reinterpret_cast<char*>(&lorryID), sizeof(unsigned int));

		obj.account = Manager::FindAccount(std::string(accountName));
		obj.lorry = static_cast<Lorry*>(Manager::FindWithID<Lorry>(lorryID));

		return is;
	}
}