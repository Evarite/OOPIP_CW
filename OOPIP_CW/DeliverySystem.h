#pragma once
#include <iosfwd>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <list>


namespace DeliverySystem 
{
    //+ 1 is for '\0'
    inline constexpr auto COUNTRY_ABBREVIATION_SIZE = 2 + 1;
    inline constexpr auto NAME_SIZE = 20 + 1;
    inline constexpr auto MIN_NAME_SIZE = 3;
    inline constexpr auto CODE_SIZE = 4 + 1;
    inline constexpr auto PHONE_SIZE_MIN = 9;
    inline constexpr auto PHONE_SIZE_MAX = 11;
    inline constexpr auto PHONE_SIZE = PHONE_SIZE_MAX + CODE_SIZE;
    inline constexpr auto REGISTRATION_CITY_ABBR_SIZE = 3 + 1;
    inline constexpr auto REGISTRATION_SIGNS_SIZE = 6;
    inline constexpr auto REGISTRATION_NUMBER_SIZE = REGISTRATION_CITY_ABBR_SIZE + COUNTRY_ABBREVIATION_SIZE + 
        REGISTRATION_SIGNS_SIZE + 1 - 1; //+1 for '-'
    inline constexpr auto MIN_ID = 1111111;
    inline constexpr auto MAX_ID = 9999999;
    inline constexpr auto MESSAGE_SIZE = 255 + 1;
    inline constexpr auto FORBIDDEN_NICKNAME_SYMBOLS = "\\\'\",/";
    inline constexpr auto FORBIDDEN_NAME_SYMBOLS = "1234567890!=+_/*\\\'\"?.,|~@#№$;%^&:";
    inline constexpr auto MIN_PASSWORD_SIZE = 6;
    inline constexpr auto VEC_RESERVE = 100;

    // Forward declaring for the relations
    class Account;
    class Country;
    class Lorry;
    class Cargo;
    class Driver;
    class Trailer;
    class Manager;

    class City 
    {
        char name[NAME_SIZE];
        char abbreviation[REGISTRATION_CITY_ABBR_SIZE];
        char countryAbbreviation[COUNTRY_ABBREVIATION_SIZE];
        
        int x;
        int y;
        
        unsigned int id;
        
    public:
        City();
        City(const std::string &name, const std::string &abbreviation,
             Country &country, int x, int y, const std::list<Country> &countries);
        
        std::string GetName() const;
        std::string GetAbbreviation() const;
        std::string GetCountryAbbreviation() const;
        unsigned int GetID() const;
        
        static unsigned int CalculateDistance(const City &city1, const City &city2);
        
        bool operator==(const City& obj) const;

        friend std::ostream &operator<<(std::ostream &os, const City &obj);
        friend std::ostream &operator<<(std::ofstream &os, const City &obj);
        friend std::istream &operator>>(std::ifstream &is, City &obj);
    };

    class Delivery 
    {
        Driver *driver;
        Lorry *lorry;
        Cargo *cargo;
        Trailer *trailer;
        unsigned int remainingDistance;

    public:
        Delivery();
        Delivery(Driver *driver, Lorry *lorry, Cargo *cargo, Trailer *trailer);

        Driver *GetDriver() const;
        Cargo *GetCargo() const;
        Lorry *GetLorry() const;
        City *GetCityFrom() const;
        City *GetCityTo() const;
        Trailer *GetTrailer() const;
        unsigned int GetRemainingDistance() const;

        void StopDelivery(std::list<Delivery>& deliveries);
        void UpdateDistance(std::list<Delivery>& deliveries);

        bool operator==(const Delivery& obj) const;

        friend std::ostream &operator<<(std::ostream &os, const Delivery &obj);
        friend std::ostream &operator<<(std::ofstream &os, const Delivery &obj);
        friend std::istream &operator>>(std::ifstream &is, Delivery &obj);
    };

    class Country 
    {
        char name[NAME_SIZE];
        char abbreviation[COUNTRY_ABBREVIATION_SIZE];
        char phoneCode[CODE_SIZE];

        std::list<City> cities;

    public:
        Country();
        Country(const std::string &name, const std::string &abbreviation, 
            const std::string &phoneCode);
        Country(const std::string &name, const std::string &abbreviation,
            const std::string &phoneCode, const City &city);
        Country(const std::string &name, const std::string &abbreviation,
            const std::string &phoneCode, const std::list<City> &cities);

        std::string GetName() const;
        std::string GetPhoneCode() const;
        std::string GetAbbreviation() const;
        const std::list<City> &GetCities() const;
        std::list<City> &GetCitiesL();

        void AddCity(const City &city);
        void RemoveCity(unsigned int index);
        void RemoveCity(City* city);

        bool operator==(const Country& obj);

        friend std::ostream &operator<<(std::ostream &os, const Country &obj);
        friend std::ostream &operator<<(std::ofstream &os, const Country &obj);
        friend std::istream &operator>>(std::ifstream &is, Country &obj);
    };

    class Account 
    {
    public:
        enum class Type
        {
            User,
            Driver,
            Moderator,
            Admin
        };

    private:
        char nickname[NAME_SIZE];
        char firstName[NAME_SIZE];
        char lastName[NAME_SIZE];
        char phoneNumber[PHONE_SIZE];
        unsigned long password;
    
        Type type;
    
        std::vector<Cargo *> cargos;
    
        static unsigned long Hash(const std::string &password);

    public:
        Account();
        Account(const std::string &nickname, const std::string &firstName, 
            const std::string &lastName, const std::string &phoneNumber,
            const std::string &password);

        std::string GetNickname() const;
        std::string GetFirstName() const;
        std::string GetLastName() const;
        std::string GetPhoneNumber() const;
        Type GetType() const;
        std::vector<Cargo *> GetCargos();

        void SetNickname(const std::string &nickname);
        void SetFirstName(const std::string &firstName);
        void SetLastName(const std::string &lastName);
        void SetPhoneNumber(const std::string &phoneNumber);
        void SetType(const Type &type);
        void SetPassword(const std::string &password);
        void AddCargo(Cargo *cargo);
        void RemoveCargo(Cargo *cargo);
        void ReplaceCargo(Cargo* cargo);

        static Account *Authorise(std::list<Account> &accounts, const std::list<Country> &countries);

        bool operator==(const Account &obj);
        friend std::ostream &operator<<(std::ostream &os, const Account &obj);
        friend std::ostream &operator<<(std::ofstream &os, const Account &obj);
        friend std::istream &operator>>(std::ifstream &is, Account &obj);
    };

    class Driver
    {
        Account *account;
        Lorry *lorry;
        Delivery *currentDelivery;

        void StopDelivery();

        friend void Delivery::StopDelivery(std::list<Delivery>& deliveries);
        friend Delivery::Delivery(Driver *driver, Lorry *lorry, Cargo *cargo, Trailer *trailer);

    public:
        Driver();
        Driver(Account *account, Lorry *lorry);

        Account *GetAccount() const;
        Lorry *GetLorry() const;
        Delivery *GetCurrentDelivery() const;

        void CancelDelivery(std::list<Delivery>& deliveries);
        void Fire(std::list<Delivery>& deliveries);
        void SetDelivery(Delivery* delivery);

        bool operator==(Driver &obj);

        friend std::ostream &operator<<(std::ostream &os, const Driver &obj);
        friend std::ostream &operator<<(std::ofstream &os, const Driver &obj);
        friend std::istream &operator>>(std::ifstream &is, Driver &obj);
    };

    class Lorry
    {
        unsigned int id;
        char make[NAME_SIZE];
        char model[NAME_SIZE];
        char registrationNumber[REGISTRATION_NUMBER_SIZE];

        unsigned int mileage;
        float gasolineCost; // In USD, cost per 100km

        Driver *owner;
        Delivery *currentDelivery;

        friend Delivery::Delivery(Driver *driver, Lorry *lorry, Cargo *cargo, Trailer *trailer);

        void StopDelivery();
        friend void Delivery::StopDelivery(std::list<Delivery>& deliveries);

    public:
        Lorry();
        Lorry(const std::string &make, const std::string &model, unsigned int mileage,
            const Country &country, const City &city,
            const std::string &registrationSigns, float gasolineCost,
            const std::list<Lorry> &lorries);
        Lorry(const std::string &make, const std::string &model, unsigned int mileage,
            const Country &country, const City &city,
            const std::string &registrationSigns, float gasolineCost, Driver *owner,
            const std::list<Lorry> &lorries);

        std::string GetMake() const;
        std::string GetModel() const;
        std::string GetRegistrationNumber() const;
        unsigned int GetMileage() const;
        Delivery *GetCurrentDelivery() const;
        Driver *GetOwner() const;
        unsigned int GetID() const;

        void SetOwner(Driver *driver);

        void AddMileage(unsigned int newMileage);

        float CalculateGasolineCost(unsigned int distance);

        void SetDelivery(Delivery* delivery);

        friend std::ostream &operator<<(std::ostream &os, const Lorry &obj);
        friend std::ostream &operator<<(std::ofstream &os, const Lorry &obj);
        friend std::istream &operator>>(std::ifstream &is, Lorry &obj);
    };

    class Cargo 
    {
    public:
        enum class Type 
        {
            Wood,      // 🪵 Для лесоматериалов - TimberTrailer
            Food,      // 🍎 Для пищевых продуктов - RefrigeratedTrailer 
            Cars,      // 🚗 Для автомобилей - CarTrailerы
            Fuel,      // ⛽ Топливо - TankTrailer 
            Chemicals, // 🧪 Химические вещества - TankTrailer 
            Milk,     // 🥛 Молочная продукция - RefrigeratedTrailer 
            Gravel,   // 🪨 Щебень, гравий - TarpTrailer 
            Grain,    // 🌾 Зерно - TarpTrailer 
            Sand,     // 🏖️ Песок - TarpTrailer 
            Concrete, // 🏗️ Бетон - TankTrailer 
            Steel,    // 🔩 Стальные конструкции - LowboyTrailer 
            Bricks,   // 🧱 Кирпичи - TarpTrailer
            Machinery,    // 🏭 Промышленное оборудование - LowboyTrailer 
            Construction, // 🚧 Строительная техника - LowboyTrailer
            Containers,   // 📦 Контейнеры - TarpTrailer 
            Explosives,   // 💣 Взрывчатые вещества - TankTrailer 
            Toxic,        // ☣️ Токсичные материалы - TankTrailer 
            Refrigerated, // ❄️ Замороженные продукты - RefrigeratedTrailer
            Pharmaceuticals // 💊 Медикаменты - RefrigeratedTrailer 
        };

    private:
        unsigned int id;
        char name[NAME_SIZE];
        float mass;
        Type type;
        Account *client;
        City *from;
        City *to;
        Delivery *currentDelivery;

        friend Delivery::Delivery(Driver *driver, Lorry *lorry, Cargo *cargo, Trailer *trailer);

        void StopDelivery();
        friend void Delivery::StopDelivery(std::list<Delivery>& deliveries);

    public:
        Cargo();
        Cargo(const std::string &name, float mass, const Type &type, City *from,
            const std::list<Cargo> &cargos);

        std::string GetName() const;
        float GetMass() const;
        Type GetType() const;
        City *GetCityFrom() const;
        City *GetCityTo() const;
        unsigned int GetID() const;
        Delivery *GetCurrentDelivery() const;
        Account *GetClient() const;
        void SetDelivery(Delivery* delivery);

        void RequestDelivery(Account *client, City *cityTo);

        bool operator==(Cargo *other);

        friend std::ostream &operator<<(std::ostream &os, const Cargo &obj);
        friend std::ostream &operator<<(std::ofstream &os, const Cargo &obj);
        friend std::istream &operator>>(std::ifstream &os, Cargo &obj);
    };

    class Trailer 
    {
    public:
        enum class Type 
        { 
            Car, 
            Tank, 
            Timber, 
            Lowboy, 
            Tarp, 
            Refrigerated 
        };

    protected:
        unsigned int id;

        float length;
        float maxPayload;
        Delivery *currentDelivery;
        Type type;

        friend Delivery::Delivery(Driver *driver, Lorry *lorry, Cargo *cargo, Trailer *trailer);

        void StopDelivery();
        friend void Delivery::StopDelivery(std::list<Delivery>& deliveries);

    public:
        Trailer();
        Trailer(float length, float maxPayload, const std::list<std::unique_ptr<Trailer>> &trailers);
        virtual ~Trailer() = default;

        float GetLength() const;
        float GetMaxPayload() const;
        Delivery *GetCurrentDelivery() const;
        unsigned int GetID() const;
        virtual Type GetType() const;
        void SetDelivery(Delivery *delivery);

        virtual unsigned short GetSpeedLimit() const = 0;
        virtual std::vector<Cargo::Type> GetSupportedCargoTypes() const = 0;
        virtual std::string GetTypeString() const = 0;

        bool IsCargoSupported(Cargo *cargo);

        friend std::ostream &operator<<(std::ostream &os, const Trailer &obj);
        friend std::ostream &operator<<(std::ofstream &os, const Trailer &obj);
        friend std::istream &operator>>(std::ifstream &is, Trailer &obj);
    };

    class CarTrailer : public Trailer 
    {
        static unsigned short speedLimit;

    public:
        CarTrailer();
        CarTrailer(float length, float maxPayload,
            const std::list<std::unique_ptr<Trailer>> &trailers);

        unsigned short GetSpeedLimit() const override;

        std::vector<Cargo::Type> GetSupportedCargoTypes() const override;

        std::string GetTypeString() const override;

        friend std::ostream &operator<<(std::ostream &os, const CarTrailer &obj);
    };

    class TankTrailer : public Trailer 
    {
        static unsigned short speedLimit;

    public:
        TankTrailer();
        TankTrailer(float length, float maxPayload,
            const std::list<std::unique_ptr<Trailer>> &trailers);

        unsigned short GetSpeedLimit() const override;

        std::vector<Cargo::Type> GetSupportedCargoTypes() const override;

        std::string GetTypeString() const override;

        friend std::ostream &operator<<(std::ostream &os, const TankTrailer &obj);
    };

    class TimberTrailer : public Trailer 
    {
        static unsigned short speedLimit;

    public:
        TimberTrailer();
        TimberTrailer(float length, float maxPayload,
            const std::list<std::unique_ptr<Trailer>> &trailers);

        unsigned short GetSpeedLimit() const override;

        std::vector<Cargo::Type> GetSupportedCargoTypes() const override;

        std::string GetTypeString() const override;

        friend std::ostream &operator<<(std::ostream &os, const TimberTrailer &obj);
    };

    class LowboyTrailer : public Trailer 
    {
        static unsigned short speedLimit;

    public:
        LowboyTrailer();
        LowboyTrailer(float length, float maxPayload,
            const std::list<std::unique_ptr<Trailer>> &trailers);

        unsigned short GetSpeedLimit() const override;

        std::vector<Cargo::Type> GetSupportedCargoTypes() const override;

        std::string GetTypeString() const override;

        friend std::ostream &operator<<(std::ostream &os, const LowboyTrailer &obj);
    };

    class TarpTrailer : public Trailer 
    {
        static unsigned short speedLimit;

    public:
        TarpTrailer();
        TarpTrailer(float length, float maxPayload,
            const std::list<std::unique_ptr<Trailer>> &trailers);

        unsigned short GetSpeedLimit() const override;

        std::vector<Cargo::Type> GetSupportedCargoTypes() const override;

        std::string GetTypeString() const override;

        friend std::ostream &operator<<(std::ostream &os, const TarpTrailer &obj);
    };

    class RefrigeratedTrailer : public Trailer {
        static unsigned short speedLimit;

    public:
        RefrigeratedTrailer();
        RefrigeratedTrailer(float length, float maxPayload,
            const std::list<std::unique_ptr<Trailer>> &trailers);

        unsigned short GetSpeedLimit() const override;

        std::vector<Cargo::Type> GetSupportedCargoTypes() const override;

        std::string GetTypeString() const override;

        friend std::ostream &operator<<(std::ostream &os, 
            const RefrigeratedTrailer &obj);
    };

    class Application 
    {
        Account* account;
        char message[MESSAGE_SIZE];

    public:
        Application();
        Application(Account *account, const char *message);

        Account *GetAccount() const;

        friend std::ostream &operator<<(std::ofstream &os, const Application &obj);
        friend std::ostream &operator<<(std::ostream &os, const Application &obj);
        friend std::istream &operator>>(std::ifstream &is, Application &obj);
    };

    template <typename T>
    concept FindType = std::is_same_v<T, Cargo> || std::is_same_v<T, Lorry> || 
        std::is_same_v<T, Trailer> || std::is_same_v<T, City>;

    class Manager // Яны не прыдумалі стацік класы, як жа я люблю с++ (не)!!!!!
    {
    public:
        static inline std::list<Country> countries{};
        static inline std::list<Account> accounts{};
        static inline std::list<Driver> drivers{};
        static inline std::list<Lorry> lorries{};
        static inline std::list<Cargo> cargos{};
        static inline std::list<Delivery> deliveries{};
        static inline std::list<std::unique_ptr<Trailer>> trailers{};
        static inline std::list<Application> applications{};
        static inline Account *account = nullptr;

        static void Initialise();
        static void UpdateFiles();
        static void Authorise();
        static void Menu();

        // All
        static void EditAccount();
        static void UpdateDistance();

        // User
        static void UserRequestDelivery();
        static void UserShowCargos();
        static void ShowSupportedArea();
        static void BecomeADriver(); 
        static void UserSort();

        // Driver
        static void AcceptDelivery(Driver *driver);
        static void DriverQuit(Driver *driver);
        static void ShowCurrentDelivery(Driver *driver);
        static void DropDelivery(Driver *driver);
        static void DriverSort();

        // Moderator
        static void ShowAllUsers();
        static void ShowAllDrivers();
        static void CargosList();
        static void TrailersList();
        static void ConsiderJobApplications();
        static void ModAdmQuit();
        static void ModSort();

        // Admin
        static void ShowAllAccounts();
        static void EditAccounts();
        static void DriversList();
        static void LorriesList();
        static void AreasList();
        static void DeliveriesList();
        static void AdminSort();

    public:
        template <FindType T> static void* FindWithID(unsigned int id) 
        {
            if (id == 0)
                return nullptr;

            if constexpr (std::is_same_v<T, Cargo>) 
            {
                for (auto &cargo : cargos)
                {
                    if (id == cargo.GetID())
                        return static_cast<void *>(&cargo);
                }
            } 
            else if constexpr (std::is_same_v<T, Lorry>) 
            {
                for (auto &lorry : lorries) 
                {
                    if (id == lorry.GetID())
                        return static_cast<void *>(&lorry);
                }
            } 
            else if constexpr (std::is_same_v<T, Trailer>) 
            {
                for (auto &trailer : trailers) 
                {
                    if (id == trailer->GetID())
                        return static_cast<void *>(trailer.get());
                }
            } 
            else if constexpr (std::is_same_v<T, City>) 
            {
                for (auto &country : countries) 
                {
                    auto &cities = country.GetCitiesL();
                    for (auto &city : cities) 
                    {
                        if (id == city.GetID()) 
                        {
                            return static_cast<void *>(&city);
                        }
                    }
                }
            }

            throw std::runtime_error("Не ўдалося знайсці аб'ект з ID " + id);
        }

        static Account *FindAccount(const std::string &nickname);
        static Driver *FindDriver(const std::string &nickname);

        static void Run();
    };

    class IDGenerator 
    {
    private:
        static std::random_device rd;
        static std::mt19937 gen;
        static std::uniform_int_distribution<unsigned int> dis;

    public:
        static unsigned int Generate();
    };

    // Functions
    std::string TrimWhitespace(const std::string &str);
    std::string GetPasswordWithAsterisks();
    int GetInt(const std::string& message = "Ваш выбар: ");
    int GetIntWithinRange(int left, int right, const std::string& message = "Ваш выбар: ");
    float GetFloat(const std::string& message = "Ваш выбар: ");
} 