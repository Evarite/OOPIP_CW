#include "DeliverySystem.h"
#include <fstream>
#include <iostream>


namespace DeliverySystem 
{
    Application::Application()
    {
        account = nullptr;
        memset(message, '\0', MESSAGE_SIZE);
    }
    Application::Application(Account* account, const char* message) 
    {
        this->account = account;
        strcpy_s(this->message, message);
    }

    Account* Application::GetAccount() const
    { 
        return account; 
    }
    std::string Application::GetAppMessage() const
    {
        return std::string(message);
    }

    std::vector<std::string> Application::ToTableRow() const
    {
        return { account->GetNickname(), account->GetFirstName(), account->GetLastName(), account->GetPhoneNumber(),
            std::string(message) };
    }
    std::vector<std::string> Application::GetHeaders()
    {
        return { "Імя акаўнта", "Імя карыстальніка", "Прозвішча", "Нумар тэлефона", "Паведамленне" };
    }

    std::ostream& operator<<(std::ostream& os, const Application& obj) 
    {
        return os << "Імя акаўнта: " << obj.account->GetNickname() << "\n"
            << "Імя карыстальніка: " << obj.account->GetFirstName() << "\n"
            << "Прозвішча: " << obj.account->GetLastName() << "\n"
            << "Нумар тэлефона: " << obj.account->GetPhoneNumber() << "\n"
            << "Паведамленне: " << obj.message;
    }

    std::ostream& operator<<(std::ofstream& os, const Application& obj) 
    {
        os.write(obj.account->GetNickname().c_str(), NAME_SIZE);
        os.write(obj.message, MESSAGE_SIZE);

        return os;
    }

    std::istream& operator>>(std::ifstream& is, Application& obj) 
    {
        char nickname[NAME_SIZE];
        is.read(nickname, NAME_SIZE);
        if (!is.good())
            return is;
        is.read(obj.message, MESSAGE_SIZE);

        obj.account = Manager::FindAccount(nickname);

        return is;
    }
} 
